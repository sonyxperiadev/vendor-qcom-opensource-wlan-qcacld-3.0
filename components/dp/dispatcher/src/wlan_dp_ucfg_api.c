/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * DOC: public API related to the DP called by north bound HDD/OSIF
 */

#include "wlan_dp_ucfg_api.h"
#include "wlan_ipa_ucfg_api.h"
#include "wlan_dp_main.h"
#include "wlan_dp_objmgr.h"
#include "cdp_txrx_cmn.h"
#include "cfg_ucfg_api.h"
#include "wlan_pmo_obj_mgmt_api.h"
#include "wlan_dp_objmgr.h"
#include "wlan_dp_bus_bandwidth.h"
#include "wlan_dp_periodic_sta_stats.h"
#include "wlan_dp_nud_tracking.h"

void ucfg_dp_update_inf_mac(struct wlan_objmgr_psoc *psoc,
			    struct qdf_mac_addr *cur_mac,
			    struct qdf_mac_addr *new_mac)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_psoc_context *dp_ctx;

	dp_info("MAC update");
	dp_ctx =  dp_psoc_get_priv(psoc);

	dp_intf = dp_get_intf_by_macaddr(dp_ctx, cur_mac);
	if (!dp_intf) {
		dp_err("DP interface not found addr:" QDF_MAC_ADDR_FMT,
		       QDF_MAC_ADDR_REF(cur_mac));
		return;
	}

	qdf_copy_macaddr(&dp_intf->mac_addr, new_mac);
}

QDF_STATUS
ucfg_dp_create_intf(struct wlan_objmgr_psoc *psoc,
		    struct qdf_mac_addr *intf_addr,
		    qdf_netdev_t ndev)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx =  dp_get_context();

	dp_info("DP interface create addr:" QDF_MAC_ADDR_FMT,
		QDF_MAC_ADDR_REF(intf_addr));

	dp_intf = __qdf_mem_malloc(sizeof(*dp_intf), __func__, __LINE__);
	if (!dp_intf) {
		dp_err("DP intf memory alloc failed addr:" QDF_MAC_ADDR_FMT,
		       QDF_MAC_ADDR_REF(intf_addr));
		return QDF_STATUS_E_FAILURE;
	}

	dp_intf->dp_ctx = dp_ctx;
	dp_intf->dev = ndev;
	dp_intf->intf_id = WLAN_UMAC_VDEV_ID_MAX;
	qdf_copy_macaddr(&dp_intf->mac_addr, intf_addr);

	qdf_spin_lock_bh(&dp_ctx->intf_list_lock);
	qdf_list_insert_front(&dp_ctx->intf_list, &dp_intf->node);
	qdf_spin_unlock_bh(&dp_ctx->intf_list_lock);

	dp_periodic_sta_stats_init(dp_intf);
	dp_periodic_sta_stats_mutex_create(dp_intf);
	dp_nud_init_tracking(dp_intf);
	dp_mic_init_work(dp_intf);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
ucfg_dp_destroy_intf(struct wlan_objmgr_psoc *psoc,
		     struct qdf_mac_addr *intf_addr)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx =  dp_get_context();

	dp_info("DP interface destroy addr:" QDF_MAC_ADDR_FMT,
		QDF_MAC_ADDR_REF(intf_addr));

	dp_intf = dp_get_intf_by_macaddr(dp_ctx, intf_addr);
	if (!dp_intf) {
		dp_err("DP interface not found addr:" QDF_MAC_ADDR_FMT,
		       QDF_MAC_ADDR_REF(intf_addr));
		return QDF_STATUS_E_FAILURE;
	}

	dp_periodic_sta_stats_mutex_destroy(dp_intf);
	dp_nud_deinit_tracking(dp_intf);
	dp_mic_deinit_work(dp_intf);

	qdf_spin_lock_bh(&dp_ctx->intf_list_lock);
	qdf_list_remove_node(&dp_ctx->intf_list, &dp_intf->node);
	qdf_spin_unlock_bh(&dp_ctx->intf_list_lock);

	__qdf_mem_free(dp_intf);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS ucfg_dp_init(void)
{
	QDF_STATUS status;

	dp_info("DP module dispatcher init");

	if (dp_allocate_ctx() != QDF_STATUS_SUCCESS) {
		dp_err("DP ctx allocation failed");
		return QDF_STATUS_E_FAULT;
	}

	status = wlan_objmgr_register_psoc_create_handler(
			WLAN_COMP_DP,
			dp_psoc_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register psoc create handler for DP");
		return status;
	}

	status = wlan_objmgr_register_psoc_destroy_handler(
			WLAN_COMP_DP,
			dp_psoc_obj_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register psoc destroy handler for DP");
		goto fail_destroy_psoc;
	}

	status = wlan_objmgr_register_pdev_create_handler(
			WLAN_COMP_DP,
			dp_pdev_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register pdev create handler for DP");
		goto fail_create_pdev;
	}

	status = wlan_objmgr_register_pdev_destroy_handler(
			WLAN_COMP_DP,
			dp_pdev_obj_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register pdev destroy handler for DP");
		goto fail_destroy_pdev;
	}

	status = wlan_objmgr_register_vdev_create_handler(
			WLAN_COMP_DP,
			dp_vdev_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register vdev create handler");
		goto fail_create_vdev;
	}

	status = wlan_objmgr_register_vdev_destroy_handler(
			WLAN_COMP_DP,
			dp_vdev_obj_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to register vdev destroy handler");
		goto fail_destroy_vdev;
	}
	return status;

fail_destroy_vdev:
	wlan_objmgr_unregister_vdev_create_handler(
				WLAN_COMP_DP,
				dp_vdev_obj_create_notification, NULL);

fail_create_vdev:
	wlan_objmgr_unregister_pdev_destroy_handler(
				WLAN_COMP_DP,
				dp_pdev_obj_destroy_notification, NULL);

fail_destroy_pdev:
	wlan_objmgr_unregister_pdev_create_handler(
				WLAN_COMP_DP,
				dp_pdev_obj_create_notification, NULL);

fail_create_pdev:
	wlan_objmgr_unregister_psoc_destroy_handler(
				WLAN_COMP_DP,
				dp_psoc_obj_destroy_notification, NULL);
fail_destroy_psoc:
	wlan_objmgr_unregister_psoc_create_handler(
				WLAN_COMP_DP,
				dp_psoc_obj_create_notification, NULL);

	dp_free_ctx();
	return status;
}

QDF_STATUS ucfg_dp_deinit(void)
{
	QDF_STATUS status;

	dp_info("DP module dispatcher deinit");

	status = wlan_objmgr_unregister_vdev_destroy_handler(
				WLAN_COMP_DP,
				dp_vdev_obj_destroy_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister vdev delete handler:%d", status);

	status = wlan_objmgr_unregister_vdev_create_handler(
				WLAN_COMP_DP,
				dp_vdev_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister vdev create handler:%d", status);

	status = wlan_objmgr_unregister_pdev_destroy_handler(
				WLAN_COMP_DP,
				dp_pdev_obj_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister pdev destroy handler:%d", status);

	status = wlan_objmgr_unregister_pdev_create_handler(
				WLAN_COMP_DP,
				dp_pdev_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister pdev create handler:%d", status);

	status = wlan_objmgr_unregister_psoc_destroy_handler(
				WLAN_COMP_DP,
				dp_psoc_obj_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister DP psoc delete handle:%d", status);

	status = wlan_objmgr_unregister_psoc_create_handler(
				WLAN_COMP_DP,
				dp_psoc_obj_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		dp_err("Failed to unregister DP psoc create handle:%d", status);

	dp_free_ctx();

	return status;
}

/**
 * ucfg_dp_suspend_handler() - suspend handler regsitered with PMO component
 * @psoc: psoc handle
 * @arg: Arguments passed by the suspend handler.
 *
 * This handler is used to update the wiphy suspend state in DP context
 *
 * Return: QDF_STATUS status -in case of success else return error
 */
static QDF_STATUS
ucfg_dp_suspend_handler(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("DP context not found");
		return QDF_STATUS_E_FAILURE;
	}

	dp_ctx->is_wiphy_suspended = true;
	return QDF_STATUS_SUCCESS;
}

/**
 * ucfg_dp_resume_handler() - resume handler regsitered with PMO component
 * @psoc: psoc handle
 * @arg: Arguments passed by the resume handler.
 *
 * This handler is used to update the wiphy resume state in DP context
 *
 * Return: QDF_STATUS status -in case of success else return error
 */
static QDF_STATUS
ucfg_dp_resume_handler(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("DP context not found");
		return QDF_STATUS_E_FAILURE;
	}

	dp_ctx->is_wiphy_suspended = false;
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_register_pmo_handler() - register suspend and resume handler
 * with PMO component
 *
 * Return: None
 */
static inline void dp_register_pmo_handler(void)
{
	pmo_register_suspend_handler(WLAN_COMP_DP,
				     ucfg_dp_suspend_handler, NULL);

	pmo_register_resume_handler(WLAN_COMP_DP,
				    ucfg_dp_resume_handler, NULL);
}

/**
 * dp_unregister_pmo_handler() - unregister suspend and resume handler
 * with PMO component
 *
 * Return: None
 */
static inline void dp_unregister_pmo_handler(void)
{
	pmo_unregister_suspend_handler(WLAN_COMP_DP, ucfg_dp_suspend_handler);

	pmo_unregister_resume_handler(WLAN_COMP_DP, ucfg_dp_resume_handler);
}

/**
 * ucfg_dp_store_qdf_dev() - Store qdf device instance in DP component
 * @psoc: psoc handle
 *
 * Return: QDF_STATUS status -in case of success else return error
 */
static inline QDF_STATUS
ucfg_dp_store_qdf_dev(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("DP context not found");
		return QDF_STATUS_E_FAILURE;
	}

	dp_ctx->qdf_dev = wlan_psoc_get_qdf_dev(psoc);
	if (!dp_ctx->qdf_dev) {
		dp_err("QDF_DEV is NULL");
		QDF_BUG(0);
		return QDF_STATUS_E_FAILURE;
	}
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS ucfg_dp_psoc_open(struct wlan_objmgr_psoc *psoc)
{
	ucfg_dp_store_qdf_dev(psoc);
	dp_rtpm_tput_policy_init(psoc);
	dp_register_pmo_handler();
	dp_bus_bandwidth_init(psoc);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS ucfg_dp_psoc_close(struct wlan_objmgr_psoc *psoc)
{
	dp_rtpm_tput_policy_deinit(psoc);
	dp_unregister_pmo_handler();
	dp_bus_bandwidth_deinit(psoc);

	return QDF_STATUS_SUCCESS;
}

#ifdef WLAN_FEATURE_RX_SOFTIRQ_TIME_LIMIT
/**
 * dp_get_config_rx_softirq_limits() - Update DP rx softirq limit config
 *                          datapath
 * @psoc: psoc handle
 * @params: DP Configuration parameters
 *
 * Return: None
 */
static
void dp_get_config_rx_softirq_limits(struct wlan_objmgr_psoc *psoc,
				     struct cdp_config_params *params)
{
	params->tx_comp_loop_pkt_limit = cfg_get(psoc,
						 CFG_DP_TX_COMP_LOOP_PKT_LIMIT);
	params->rx_reap_loop_pkt_limit = cfg_get(psoc,
						 CFG_DP_RX_REAP_LOOP_PKT_LIMIT);
	params->rx_hp_oos_update_limit = cfg_get(psoc,
						 CFG_DP_RX_HP_OOS_UPDATE_LIMIT);
}
#else
static
void dp_get_config_rx_softirq_limits(struct wlan_objmgr_psoc *psoc,
				     struct cdp_config_params *params)
{
}
#endif /* WLAN_FEATURE_RX_SOFTIRQ_TIME_LIMIT */

#if defined(QCA_LL_TX_FLOW_CONTROL_V2) || defined(QCA_LL_PDEV_TX_FLOW_CONTROL)
/**
 * dp_get_config_queue_threshold() - Update DP tx flow limit config
 *                          datapath
 * @psoc: psoc handle
 * @params: DP Configuration parameters
 *
 * Return: None
 */
static void
dp_get_config_queue_threshold(struct wlan_objmgr_psoc *psoc,
			      struct cdp_config_params *params)
{
	params->tx_flow_stop_queue_threshold =
			cfg_get(psoc, CFG_DP_TX_FLOW_STOP_QUEUE_TH);
	params->tx_flow_start_queue_offset =
			cfg_get(psoc, CFG_DP_TX_FLOW_START_QUEUE_OFFSET);
}
#else
static inline void
dp_get_config_queue_threshold(struct wlan_objmgr_psoc *psoc,
			      struct cdp_config_params *params)
{
}
#endif

QDF_STATUS
ucfg_dp_update_config(struct wlan_objmgr_psoc *psoc,
		      struct wlan_dp_user_config *req)
{
	struct cdp_config_params params = {0};
	struct wlan_dp_psoc_context *dp_ctx;
	QDF_STATUS status;
	void *soc;

	dp_ctx =  dp_psoc_get_priv(psoc);

	soc = cds_get_context(QDF_MODULE_ID_SOC);
	params.tso_enable = cfg_get(psoc, CFG_DP_TSO);
	params.lro_enable = cfg_get(psoc, CFG_DP_LRO);

	dp_get_config_queue_threshold(psoc, &params);
	params.flow_steering_enable =
		cfg_get(psoc, CFG_DP_FLOW_STEERING_ENABLED);
	params.napi_enable = dp_ctx->napi_enable;
	params.p2p_tcp_udp_checksumoffload =
		cfg_get(psoc, CFG_DP_P2P_TCP_UDP_CKSUM_OFFLOAD);
	params.nan_tcp_udp_checksumoffload =
		cfg_get(psoc, CFG_DP_NAN_TCP_UDP_CKSUM_OFFLOAD);
	params.tcp_udp_checksumoffload =
		cfg_get(psoc, CFG_DP_TCP_UDP_CKSUM_OFFLOAD);
	params.ipa_enable = req->ipa_enable;
	params.gro_enable = cfg_get(psoc, CFG_DP_GRO);
	params.tx_comp_loop_pkt_limit = cfg_get(psoc,
						CFG_DP_TX_COMP_LOOP_PKT_LIMIT);
	params.rx_reap_loop_pkt_limit = cfg_get(psoc,
						CFG_DP_RX_REAP_LOOP_PKT_LIMIT);
	params.rx_hp_oos_update_limit = cfg_get(psoc,
						CFG_DP_RX_HP_OOS_UPDATE_LIMIT);
	dp_get_config_rx_softirq_limits(psoc, &params);

	status = cdp_update_config_parameters(soc, &params);
	if (status) {
		dp_err("Failed to attach config parameters");
		return status;
	}

	return QDF_STATUS_SUCCESS;
}

uint64_t
ucfg_dp_get_rx_softirq_yield_duration(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);

	return dp_ctx->dp_cfg.rx_softirq_max_yield_duration_ns;
}

void ucfg_dp_register_rx_mic_error_ind_handler(void *soc)
{
	cdp_register_rx_mic_error_ind_handler(soc, dp_rx_mic_error_ind);
}

#ifdef WLAN_NUD_TRACKING
bool
ucfg_dp_is_roam_after_nud_enabled(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);
	struct wlan_dp_psoc_cfg *dp_cfg = &dp_ctx->dp_cfg;

	if (dp_cfg->enable_nud_tracking == DP_ROAM_AFTER_NUD_FAIL ||
	    dp_cfg->enable_nud_tracking == DP_DISCONNECT_AFTER_ROAM_FAIL)
		return true;

	return false;
}
#endif

int ucfg_dp_bbm_context_init(struct wlan_objmgr_psoc *psoc)
{
	return dp_bbm_context_init(psoc);
}

void ucfg_dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc)
{
	dp_bbm_context_deinit(psoc);
}

void ucfg_dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
					  struct bbm_params *params)
{
	dp_bbm_apply_independent_policy(psoc, params);
}

void ucfg_dp_set_rx_mode_rps(bool enable)
{
	dp_set_rx_mode_rps(enable);
}

void ucfg_dp_periodic_sta_stats_start(struct wlan_objmgr_vdev *vdev)
{
	dp_periodic_sta_stats_start(vdev);
}

void ucfg_dp_periodic_sta_stats_stop(struct wlan_objmgr_vdev *vdev)
{
	dp_periodic_sta_stats_stop(vdev);
}

void ucfg_dp_try_send_rps_ind(struct wlan_objmgr_vdev *vdev)
{
	dp_try_send_rps_ind(vdev);
}

void ucfg_dp_reg_ipa_rsp_ind(struct wlan_objmgr_pdev *pdev)
{
	ucfg_ipa_reg_rps_enable_cb(pdev, dp_set_rps);
}

void ucfg_dp_try_set_rps_cpu_mask(struct wlan_objmgr_psoc *psoc)
{
	dp_try_set_rps_cpu_mask(psoc);
}

void ucfg_dp_add_latency_critical_client(struct wlan_objmgr_vdev *vdev,
					 enum qca_wlan_802_11_mode phymode)
{
	dp_add_latency_critical_client(vdev, phymode);
}

void ucfg_dp_del_latency_critical_client(struct wlan_objmgr_vdev *vdev,
					 enum qca_wlan_802_11_mode phymode)
{
	dp_del_latency_critical_client(vdev, phymode);
}

void ucfg_dp_reset_tcp_delack(struct wlan_objmgr_psoc *psoc)
{
	dp_reset_tcp_delack(psoc);
}

void
ucfg_dp_set_current_throughput_level(struct wlan_objmgr_psoc *psoc,
				     enum pld_bus_width_type next_vote_level)
{
	dp_set_current_throughput_level(psoc, next_vote_level);
}

void
ucfg_dp_set_high_bus_bw_request(struct wlan_objmgr_psoc *psoc,
				uint8_t vdev_id,
				bool high_bus_bw)
{
	dp_set_high_bus_bw_request(psoc, vdev_id, high_bus_bw);
}

void ucfg_wlan_dp_display_tx_rx_histogram(struct wlan_objmgr_psoc *psoc)
{
	wlan_dp_display_tx_rx_histogram(psoc);
}

void ucfg_wlan_dp_clear_tx_rx_histogram(struct wlan_objmgr_psoc *psoc)
{
	wlan_dp_clear_tx_rx_histogram(psoc);
}

void ucfg_dp_bus_bw_compute_timer_start(struct wlan_objmgr_psoc *psoc)
{
	dp_bus_bw_compute_timer_start(psoc);
}

void ucfg_dp_bus_bw_compute_timer_try_start(struct wlan_objmgr_psoc *psoc)
{
	dp_bus_bw_compute_timer_try_start(psoc);
}

void ucfg_dp_bus_bw_compute_timer_stop(struct wlan_objmgr_psoc *psoc)
{
	dp_bus_bw_compute_timer_stop(psoc);
}

void ucfg_dp_bus_bw_compute_timer_try_stop(struct wlan_objmgr_psoc *psoc)
{
	dp_bus_bw_compute_timer_try_stop(psoc);
}

void ucfg_dp_bus_bw_compute_prev_txrx_stats(struct wlan_objmgr_vdev *vdev)
{
	dp_bus_bw_compute_prev_txrx_stats(vdev);
}

void
ucfg_dp_bus_bw_compute_reset_prev_txrx_stats(struct wlan_objmgr_vdev *vdev)
{
	dp_bus_bw_compute_reset_prev_txrx_stats(vdev);
}

void ucfg_dp_nud_set_gateway_addr(struct wlan_objmgr_vdev *vdev,
				  struct qdf_mac_addr gw_mac_addr)
{
	dp_nud_set_gateway_addr(vdev, gw_mac_addr);
}

void ucfg_dp_nud_event(struct qdf_mac_addr *netdev_mac_addr,
		       struct qdf_mac_addr *gw_mac_addr,
		       uint8_t nud_state)
{
	dp_nud_netevent_cb(netdev_mac_addr, gw_mac_addr, nud_state);
}

void ucfg_dp_nud_reset_tracking(struct wlan_objmgr_vdev *vdev)
{
	struct wlan_dp_intf *dp_intf = dp_get_vdev_priv_obj(vdev);

	if (!dp_intf) {
		dp_err("Unable to get DP Interface");
		return;
	}
	dp_nud_reset_tracking(dp_intf);
}

uint8_t ucfg_dp_nud_tracking_enabled(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);

	if (!dp_ctx) {
		dp_err("DP Context is NULL");
		return 0;
	}
	return dp_ctx->dp_cfg.enable_nud_tracking;
}

void ucfg_dp_nud_indicate_roam(struct wlan_objmgr_vdev *vdev)
{
	dp_nud_indicate_roam(vdev);
}

void ucfg_dp_register_hdd_callbacks(struct wlan_objmgr_psoc *psoc,
				    struct wlan_dp_psoc_callbacks *cb_obj)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);

	if (!dp_ctx) {
		dp_err("DP ctx is NULL");
		return;
	}
	dp_ctx->dp_ops.callback_ctx = cb_obj->callback_ctx;
	dp_ctx->dp_ops.wlan_dp_sta_get_dot11mode =
		cb_obj->wlan_dp_sta_get_dot11mode;
	dp_ctx->dp_ops.wlan_dp_get_ap_client_count =
		cb_obj->wlan_dp_get_ap_client_count;
	dp_ctx->dp_ops.wlan_dp_sta_ndi_connected =
		cb_obj->wlan_dp_sta_ndi_connected;
	dp_ctx->dp_ops.dp_any_adapter_connected =
		cb_obj->dp_any_adapter_connected;
	dp_ctx->dp_ops.dp_send_svc_nlink_msg = cb_obj->dp_send_svc_nlink_msg;
	dp_ctx->dp_ops.dp_pm_qos_update_request =
		cb_obj->dp_pm_qos_update_request;
	dp_ctx->dp_ops.dp_pld_remove_pm_qos = cb_obj->dp_pld_remove_pm_qos;
	dp_ctx->dp_ops.dp_pld_request_pm_qos = cb_obj->dp_pld_request_pm_qos;
	dp_ctx->dp_ops.dp_pm_qos_add_request = cb_obj->dp_pm_qos_add_request;
	dp_ctx->dp_ops.dp_pm_qos_remove_request =
		cb_obj->dp_pm_qos_remove_request;
	dp_ctx->dp_ops.wlan_dp_display_tx_multiq_stats =
		cb_obj->wlan_dp_display_tx_multiq_stats;
	dp_ctx->dp_ops.wlan_dp_display_netif_queue_history =
		cb_obj->wlan_dp_display_netif_queue_history;
	dp_ctx->dp_ops.dp_send_mscs_action_frame =
		cb_obj->dp_send_mscs_action_frame;
	dp_ctx->dp_ops.dp_pktlog_enable_disable =
		cb_obj->dp_pktlog_enable_disable;
	dp_ctx->dp_ops.dp_is_roaming_in_progress =
		cb_obj->dp_is_roaming_in_progress;
	dp_ctx->dp_ops.dp_is_ap_active = cb_obj->dp_is_ap_active;
	dp_ctx->dp_ops.dp_disable_rx_ol_for_low_tput =
		cb_obj->dp_disable_rx_ol_for_low_tput;
	dp_ctx->dp_ops.dp_napi_apply_throughput_policy =
		cb_obj->dp_napi_apply_throughput_policy;
	dp_ctx->dp_ops.dp_is_link_adapter = cb_obj->dp_is_link_adapter;
	dp_ctx->dp_ops.dp_get_pause_map = cb_obj->dp_get_pause_map;
	dp_ctx->dp_ops.dp_nud_failure_work = cb_obj->dp_nud_failure_work;
}
