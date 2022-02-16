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
 * DOC: wlan_dp_bus_bandwidth.c
 *
 * Bus Bandwidth Manager implementation
 */

#include "wlan_dp_bus_bandwidth.h"
#include "wlan_dp_main.h"
#include <wlan_objmgr_psoc_obj_i.h>
#include "pld_common.h"
#include "cds_api.h"
#include <wlan_nlink_common.h>
#include "wlan_ipa_ucfg_api.h"
#include "dp_txrx.h"

#ifdef FEATURE_BUS_BANDWIDTH_MGR
/**
 * bus_bw_table_default - default table which provides bus bandwidth level
 *  corresonding to a given connection mode and throughput level.
 */
static bus_bw_table_type bus_bw_table_default = {
	[QCA_WLAN_802_11_MODE_11B] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_1,
				      BUS_BW_LEVEL_2, BUS_BW_LEVEL_3,
				      BUS_BW_LEVEL_4, BUS_BW_LEVEL_6,
				      BUS_BW_LEVEL_7, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11G] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5},
	[QCA_WLAN_802_11_MODE_11A] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5,
				      BUS_BW_LEVEL_5, BUS_BW_LEVEL_5},
	[QCA_WLAN_802_11_MODE_11N] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_1,
				      BUS_BW_LEVEL_2, BUS_BW_LEVEL_3,
				      BUS_BW_LEVEL_4, BUS_BW_LEVEL_6,
				      BUS_BW_LEVEL_7, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11AC] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_1,
				       BUS_BW_LEVEL_2, BUS_BW_LEVEL_3,
				       BUS_BW_LEVEL_4, BUS_BW_LEVEL_6,
				       BUS_BW_LEVEL_7, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11AX] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_1,
				       BUS_BW_LEVEL_2, BUS_BW_LEVEL_3,
				       BUS_BW_LEVEL_4, BUS_BW_LEVEL_6,
				       BUS_BW_LEVEL_7, BUS_BW_LEVEL_8},
};

/**
 * bus_bw_table_low_latency - table which provides bus bandwidth level
 *  corresonding to a given connection mode and throughput level in low
 *  latency setting.
 */
static bus_bw_table_type bus_bw_table_low_latency = {
	[QCA_WLAN_802_11_MODE_11B] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11G] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11A] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11N] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				      BUS_BW_LEVEL_8, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11AC] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8},
	[QCA_WLAN_802_11_MODE_11AX] = {BUS_BW_LEVEL_NONE, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8,
				       BUS_BW_LEVEL_8, BUS_BW_LEVEL_8}
};

/**
 * bbm_convert_to_pld_bus_lvl() - Convert from internal bus vote level to
 *  PLD bus vote level
 * @vote_lvl: internal bus bw vote level
 *
 * Returns: PLD bus vote level
 */
static enum pld_bus_width_type
bbm_convert_to_pld_bus_lvl(enum bus_bw_level vote_lvl)
{
	switch (vote_lvl) {
	case BUS_BW_LEVEL_1:
		return PLD_BUS_WIDTH_IDLE;
	case BUS_BW_LEVEL_2:
		return PLD_BUS_WIDTH_LOW;
	case BUS_BW_LEVEL_3:
		return PLD_BUS_WIDTH_MEDIUM;
	case BUS_BW_LEVEL_4:
		return PLD_BUS_WIDTH_HIGH;
	case BUS_BW_LEVEL_5:
		return PLD_BUS_WIDTH_LOW_LATENCY;
	case BUS_BW_LEVEL_6:
		return PLD_BUS_WIDTH_VERY_HIGH;
	case BUS_BW_LEVEL_7:
		return PLD_BUS_WIDTH_ULTRA_HIGH;
	case BUS_BW_LEVEL_8:
		return PLD_BUS_WIDTH_MAX;
	case BUS_BW_LEVEL_NONE:
	default:
		return PLD_BUS_WIDTH_NONE;
	}
}

/**
 * bbm_get_bus_bw_level_vote() - Select bus bw vote level per interface based
 *  on connection mode and throughput level
 * @dp_intf: DP Interface, caller assure that interface is valid.
 * @tput_level: throughput level
 *
 * Returns: Bus bw level
 */
static enum bus_bw_level
bbm_get_bus_bw_level_vote(struct wlan_dp_intf *dp_intf,
			  enum tput_level tput_level)
{
	enum qca_wlan_802_11_mode i;
	enum qca_wlan_802_11_mode dot11_mode;
	enum bus_bw_level vote_lvl = BUS_BW_LEVEL_NONE;
	struct wlan_dp_psoc_context *dp_ctx = dp_intf->dp_ctx;
	struct bbm_context *bbm_ctx = dp_ctx->bbm_ctx;
	bus_bw_table_type *lkp_table = bbm_ctx->curr_bus_bw_lookup_table;
	uint16_t client_count[QCA_WLAN_802_11_MODE_INVALID];
	struct wlan_dp_psoc_callbacks *cb_obj = &dp_ctx->dp_ops;
	hdd_cb_handle ctx = cb_obj->callback_ctx;

	switch (dp_intf->device_mode) {
	case QDF_STA_MODE:
	case QDF_P2P_CLIENT_MODE:
		if (!cb_obj->wlan_dp_sta_get_dot11mode(ctx,
						       dp_intf->intf_id,
						       &dot11_mode))
			break;

		if (dot11_mode >= QCA_WLAN_802_11_MODE_INVALID) {
			dp_err("invalid STA/P2P-CLI dot11 mode");
			break;
		}

		return (*lkp_table)[dot11_mode][tput_level];
	case QDF_SAP_MODE:
	case QDF_P2P_GO_MODE:
		if (!cb_obj->wlan_dp_get_ap_client_count(ctx,
							 dp_intf->intf_id,
							 client_count))
			break;

		for (i = QCA_WLAN_802_11_MODE_11B;
		     i < QCA_WLAN_802_11_MODE_INVALID; i++) {
			if (client_count[i] &&
			    (*lkp_table)[i][tput_level] > vote_lvl)
				vote_lvl = (*lkp_table)[i][tput_level];
		}

		return vote_lvl;
	case QDF_NDI_MODE:
		if (!cb_obj->wlan_dp_sta_ndi_connected(ctx,
						       dp_intf->intf_id))
			break;

		/*
		 * NDI dot11mode is currently hardcoded to 11AC in driver and
		 * since the bus bw levels in table do not differ between 11AC
		 * and 11AX, using max supported mode instead. Dot11mode of the
		 * peers are not saved in driver and legacy modes are not
		 * supported in NAN.
		 */
		return (*lkp_table)[QCA_WLAN_802_11_MODE_11AX][tput_level];
	default:
		break;
	}

	return vote_lvl;
}

/**
 * bbm_apply_tput_policy() - Apply tput BBM policy by considering
 *  throughput level and connection modes across adapters
 * @dp_ctx: DP context
 * @tput_level: throughput level
 *
 * Returns: None
 */
static void
bbm_apply_tput_policy(struct wlan_dp_psoc_context *dp_ctx,
		      enum tput_level tput_level)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_intf *dp_intf_next;
	struct wlan_objmgr_psoc *psoc;
	enum bus_bw_level next_vote = BUS_BW_LEVEL_NONE;
	enum bus_bw_level tmp_vote;
	struct bbm_context *bbm_ctx = dp_ctx->bbm_ctx;
	hdd_cb_handle ctx = dp_ctx->dp_ops.callback_ctx;

	if (tput_level == TPUT_LEVEL_NONE) {
		/*
		 * This is to handle the scenario where bus bw periodic work
		 * is force cancelled
		 */
		if (dp_ctx->dp_ops.dp_any_adapter_connected(ctx))
			bbm_ctx->per_policy_vote[BBM_TPUT_POLICY] = next_vote;
		return;
	}

	psoc = dp_ctx->psoc;
	if (!psoc) {
		dp_err("psoc is NULL");
		return;
	}

	dp_for_each_intf_held_safe(dp_ctx, dp_intf, dp_intf_next) {
		if (!dp_intf)
			continue;
		tmp_vote = bbm_get_bus_bw_level_vote(dp_intf, tput_level);
		if (tmp_vote > next_vote)
			next_vote = tmp_vote;
	}

	bbm_ctx->per_policy_vote[BBM_TPUT_POLICY] = next_vote;
}

/**
 * bbm_apply_driver_mode_policy() - Apply driver mode BBM policy
 * @bbm_ctx: bus bw mgr context
 * @driver_mode: global driver mode
 *
 * Returns: None
 */
static void
bbm_apply_driver_mode_policy(struct bbm_context *bbm_ctx,
			     enum QDF_GLOBAL_MODE driver_mode)
{
	switch (driver_mode) {
	case QDF_GLOBAL_MONITOR_MODE:
	case QDF_GLOBAL_FTM_MODE:
		bbm_ctx->per_policy_vote[BBM_DRIVER_MODE_POLICY] =
							    BUS_BW_LEVEL_6;
		return;
	default:
		bbm_ctx->per_policy_vote[BBM_DRIVER_MODE_POLICY] =
							 BUS_BW_LEVEL_NONE;
		return;
	}
}

/**
 * bbm_apply_non_persistent_policy() - Apply non persistent policy and set
 *  the bus bandwidth
 * @dp_ctx: DP context
 * @flag: flag
 *
 * Returns: None
 */
static void
bbm_apply_non_persistent_policy(struct wlan_dp_psoc_context *dp_ctx,
				enum bbm_non_per_flag flag)
{
	hdd_cb_handle ctx = dp_ctx->dp_ops.callback_ctx;

	switch (flag) {
	case BBM_APPS_RESUME:
		if (dp_ctx->dp_ops.dp_any_adapter_connected(ctx)) {
			dp_ctx->bbm_ctx->curr_vote_level = BUS_BW_LEVEL_RESUME;
			pld_request_bus_bandwidth(dp_ctx->qdf_dev->dev,
			       bbm_convert_to_pld_bus_lvl(BUS_BW_LEVEL_RESUME));
		} else {
			dp_ctx->bbm_ctx->curr_vote_level = BUS_BW_LEVEL_NONE;
			pld_request_bus_bandwidth(dp_ctx->qdf_dev->dev,
				 bbm_convert_to_pld_bus_lvl(BUS_BW_LEVEL_NONE));
		}
		return;
	case BBM_APPS_SUSPEND:
		dp_ctx->bbm_ctx->curr_vote_level = BUS_BW_LEVEL_NONE;
		pld_request_bus_bandwidth(dp_ctx->qdf_dev->dev,
			    bbm_convert_to_pld_bus_lvl(BUS_BW_LEVEL_NONE));
		return;
	default:
		dp_info("flag %d not handled in res/sus BBM policy", flag);
		return;
	}
}

/**
 * bbm_apply_wlm_policy() - Apply WLM based BBM policy by selecting
 *  lookup tables based on the latency level
 * @bbm_ctx: Bus BW mgr context
 * @wlm_level: WLM latency level
 *
 * Returns: None
 */
static void
bbm_apply_wlm_policy(struct bbm_context *bbm_ctx, enum wlm_ll_level wlm_level)
{
	switch (wlm_level) {
	case WLM_LL_NORMAL:
		bbm_ctx->curr_bus_bw_lookup_table = &bus_bw_table_default;
		break;
	case WLM_LL_LOW:
		bbm_ctx->curr_bus_bw_lookup_table = &bus_bw_table_low_latency;
		break;
	default:
		dp_info("wlm level %d not handled in BBM WLM policy",
			wlm_level);
		break;
	}
}

/**
 * bbm_apply_user_policy() - Apply user specified bus voting
 *  level
 * @bbm_ctx: Bus BW mgr context
 * @set: set or reset flag
 * @user_level: user bus vote level
 *
 * Returns: qdf status
 */
static QDF_STATUS
bbm_apply_user_policy(struct bbm_context *bbm_ctx, bool set,
		      enum bus_bw_level user_level)
{
	if (user_level >= BUS_BW_LEVEL_MAX) {
		dp_err("Invalid user vote level %d", user_level);
		return QDF_STATUS_E_FAILURE;
	}

	if (set)
		bbm_ctx->per_policy_vote[BBM_USER_POLICY] = user_level;
	else
		bbm_ctx->per_policy_vote[BBM_USER_POLICY] = BUS_BW_LEVEL_NONE;

	return QDF_STATUS_SUCCESS;
}

/**
 * bbm_request_bus_bandwidth() - Set bus bandwidth level
 * @dp_ctx: DP context
 *
 * Returns: None
 */
static void
bbm_request_bus_bandwidth(struct wlan_dp_psoc_context *dp_ctx)
{
	enum bbm_policy i;
	enum bus_bw_level next_vote = BUS_BW_LEVEL_NONE;
	enum pld_bus_width_type pld_vote;
	struct bbm_context *bbm_ctx = dp_ctx->bbm_ctx;

	for (i = BBM_DRIVER_MODE_POLICY; i < BBM_MAX_POLICY; i++) {
		if (bbm_ctx->per_policy_vote[i] > next_vote)
			next_vote = bbm_ctx->per_policy_vote[i];
	}

	if (next_vote != bbm_ctx->curr_vote_level) {
		pld_vote = bbm_convert_to_pld_bus_lvl(next_vote);
		dp_info("Bus bandwidth vote level change from %d to %d pld_vote: %d",
			bbm_ctx->curr_vote_level, next_vote, pld_vote);
		bbm_ctx->curr_vote_level = next_vote;
		pld_request_bus_bandwidth(dp_ctx->qdf_dev->dev, pld_vote);
	}
}

void dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
				     struct bbm_params *params)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct bbm_context *bbm_ctx;
	QDF_STATUS status;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx || !params)
		return;

	bbm_ctx = dp_ctx->bbm_ctx;

	qdf_mutex_acquire(&bbm_ctx->bbm_lock);

	switch (params->policy) {
	case BBM_TPUT_POLICY:
		bbm_apply_tput_policy(dp_ctx, params->policy_info.tput_level);
		break;
	case BBM_NON_PERSISTENT_POLICY:
		bbm_apply_non_persistent_policy(dp_ctx,
						params->policy_info.flag);
		goto done;
	case BBM_DRIVER_MODE_POLICY:
		bbm_apply_driver_mode_policy(bbm_ctx,
					     params->policy_info.driver_mode);
		break;
	case BBM_SELECT_TABLE_POLICY:
		bbm_apply_wlm_policy(bbm_ctx, params->policy_info.wlm_level);
		goto done;
	case BBM_USER_POLICY:
		/*
		 * This policy is not used currently.
		 */
		status = bbm_apply_user_policy(bbm_ctx,
					       params->policy_info.usr.set,
					       params->policy_info.usr.user_level);
		if (QDF_IS_STATUS_ERROR(status))
			goto done;
		break;
	default:
		dp_info("BBM policy %d not handled", params->policy);
		goto done;
	}

	bbm_request_bus_bandwidth(dp_ctx);

done:
	qdf_mutex_release(&bbm_ctx->bbm_lock);
}

int dp_bbm_context_init(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct bbm_context *bbm_ctx;
	QDF_STATUS status;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx)
		return -EINVAL;
	bbm_ctx = qdf_mem_malloc(sizeof(*bbm_ctx));
	if (!bbm_ctx)
		return -ENOMEM;

	bbm_ctx->curr_bus_bw_lookup_table = &bus_bw_table_default;

	status = qdf_mutex_create(&bbm_ctx->bbm_lock);
	if (QDF_IS_STATUS_ERROR(status))
		goto free_ctx;

	dp_ctx->bbm_ctx = bbm_ctx;

	return 0;

free_ctx:
	qdf_mem_free(bbm_ctx);

	return qdf_status_to_os_return(status);
}

void dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct bbm_context *bbm_ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx)
		return;
	bbm_ctx = dp_ctx->bbm_ctx;
	if (!bbm_ctx)
		return;

	dp_ctx->bbm_ctx = NULL;
	qdf_mutex_destroy(&bbm_ctx->bbm_lock);

	qdf_mem_free(bbm_ctx);
}
#endif /* FEATURE_BUS_BANDWIDTH_MGR */
#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
#ifdef FEATURE_RUNTIME_PM
void dp_rtpm_tput_policy_init(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct dp_rtpm_tput_policy_context *ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("Unable to get DP context");
		return;
	}

	ctx = &dp_ctx->rtpm_tput_policy_ctx;
	qdf_runtime_lock_init(&ctx->rtpm_lock);
	ctx->curr_state = DP_RTPM_TPUT_POLICY_STATE_REQUIRED;
	qdf_atomic_init(&ctx->high_tput_vote);
}

void dp_rtpm_tput_policy_deinit(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct dp_rtpm_tput_policy_context *ctx;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("Unable to get DP context");
		return;
	}

	ctx = &dp_ctx->rtpm_tput_policy_ctx;
	ctx->curr_state = DP_RTPM_TPUT_POLICY_STATE_INVALID;
	qdf_runtime_lock_deinit(&ctx->rtpm_lock);
}

/**
 * dp_rtpm_tput_policy_prevent() - prevent a runtime bus suspend
 * @dp_ctx: DP handle
 *
 * return: None
 */
static void dp_rtpm_tput_policy_prevent(struct wlan_dp_psoc_context *dp_ctx)
{
	struct dp_rtpm_tput_policy_context *ctx;

	ctx = &dp_ctx->rtpm_tput_policy_ctx;
	qdf_runtime_pm_prevent_suspend(&ctx->rtpm_lock);
}

/**
 * dp_rtpm_tput_policy_allow() - allow a runtime bus suspend
 * @dp_ctx: DP handle
 *
 * return: None
 */
static void dp_rtpm_tput_policy_allow(struct wlan_dp_psoc_context *dp_ctx)
{
	struct dp_rtpm_tput_policy_context *ctx;

	ctx = &dp_ctx->rtpm_tput_policy_ctx;
	qdf_runtime_pm_allow_suspend(&ctx->rtpm_lock);
}

#define DP_RTPM_POLICY_HIGH_TPUT_THRESH TPUT_LEVEL_MEDIUM

void dp_rtpm_tput_policy_apply(struct wlan_dp_psoc_context *dp_ctx,
			       enum tput_level tput_level)
{
	int vote;
	enum dp_rtpm_tput_policy_state temp_state;
	struct dp_rtpm_tput_policy_context *ctx;
	ol_txrx_soc_handle soc = cds_get_context(QDF_MODULE_ID_SOC);

	if (qdf_unlikely(!soc))
		return;

	ctx = &dp_ctx->rtpm_tput_policy_ctx;

	if (tput_level >= DP_RTPM_POLICY_HIGH_TPUT_THRESH)
		temp_state = DP_RTPM_TPUT_POLICY_STATE_NOT_REQUIRED;
	else
		temp_state = DP_RTPM_TPUT_POLICY_STATE_REQUIRED;

	if (ctx->curr_state == temp_state)
		return;

	if (temp_state == DP_RTPM_TPUT_POLICY_STATE_REQUIRED) {
		cdp_set_rtpm_tput_policy_requirement(soc, false);
		qdf_atomic_dec(&ctx->high_tput_vote);
		dp_rtpm_tput_policy_allow(dp_ctx);
	} else {
		cdp_set_rtpm_tput_policy_requirement(soc, true);
		qdf_atomic_inc(&ctx->high_tput_vote);
		dp_rtpm_tput_policy_prevent(dp_ctx);
	}

	ctx->curr_state = temp_state;
	vote = qdf_atomic_read(&ctx->high_tput_vote);

	if (vote < 0 || vote > 1) {
		dp_alert_rl("Incorrect vote!");
		QDF_BUG(0);
	}
}

int dp_rtpm_tput_policy_get_vote(struct wlan_dp_psoc_context *dp_ctx)
{
	struct dp_rtpm_tput_policy_context *ctx;

	ctx = &dp_ctx->rtpm_tput_policy_ctx;
	return qdf_atomic_read(&ctx->high_tput_vote);
}
#endif /* FEATURE_RUNTIME_PM */

void dp_reset_tcp_delack(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);

	enum wlan_tp_level next_level = WLAN_SVC_TP_LOW;
	struct wlan_rx_tp_data rx_tp_data = {0};

	if (!dp_ctx->en_tcp_delack_no_lro)
		return;

	rx_tp_data.rx_tp_flags |= TCP_DEL_ACK_IND;
	rx_tp_data.level = next_level;
	dp_ctx->rx_high_ind_cnt = 0;
	wlan_dp_update_tcp_rx_param(dp_ctx, &rx_tp_data);
}

/**
 * dp_reset_tcp_adv_win_scale() - Reset TCP advance window scaling
 * value to default
 * @dp_ctx: pointer to DP context (Should not be NULL)
 *
 * Function used to reset TCP advance window scaling
 * value to its default value
 *
 * Return: None
 */
static void dp_reset_tcp_adv_win_scale(struct wlan_dp_psoc_context *dp_ctx)
{
	enum wlan_tp_level next_level = WLAN_SVC_TP_NONE;
	struct wlan_rx_tp_data rx_tp_data = {0};

	if (!dp_ctx->dp_cfg.enable_tcp_adv_win_scale)
		return;

	rx_tp_data.rx_tp_flags |= TCP_ADV_WIN_SCL;
	rx_tp_data.level = next_level;
	dp_ctx->cur_rx_level = WLAN_SVC_TP_NONE;
	wlan_dp_update_tcp_rx_param(dp_ctx, &rx_tp_data);
}

void wlan_dp_update_tcp_rx_param(struct wlan_dp_psoc_context *dp_ctx,
				 struct wlan_rx_tp_data *data)
{
	struct wlan_dp_psoc_callbacks *dp_ops = &dp_ctx->dp_ops;

	if (!dp_ctx) {
		dp_err("psoc is null");
		return;
	}

	if (!data) {
		dp_err("Data is null");
		return;
	}

	if (dp_ctx->dp_cfg.enable_tcp_param_update)
		dp_ops->osif_dp_send_tcp_param_update_event(dp_ctx->psoc,
							    (union wlan_tp_data *)data,
							    1);
	else
		dp_ops->dp_send_svc_nlink_msg(cds_get_radio_index(),
					      WLAN_SVC_WLAN_TP_IND,
					      (void *)data,
					      sizeof(struct wlan_rx_tp_data));
}

/**
 * wlan_dp_update_tcp_tx_param() - update TCP param in Tx dir
 * @dp_ctx: Pointer to DP context
 * @data: Parameters to update
 *
 * Return: None
 */
static void wlan_dp_update_tcp_tx_param(struct wlan_dp_psoc_context *dp_ctx,
					struct wlan_tx_tp_data *data)
{
	enum wlan_tp_level next_tx_level;
	struct wlan_tx_tp_data *tx_tp_data;
	struct wlan_dp_psoc_callbacks *dp_ops = &dp_ctx->dp_ops;

	if (!dp_ctx) {
		dp_err("psoc is null");
		return;
	}

	if (!data) {
		dp_err("Data is null");
		return;
	}

	tx_tp_data = (struct wlan_tx_tp_data *)data;
	next_tx_level = tx_tp_data->level;

	if (dp_ctx->dp_cfg.enable_tcp_param_update)
		dp_ops->osif_dp_send_tcp_param_update_event(dp_ctx->psoc,
							    (union wlan_tp_data *)data,
							    0);
	else
		dp_ops->dp_send_svc_nlink_msg(cds_get_radio_index(),
					      WLAN_SVC_WLAN_TP_TX_IND,
					      &next_tx_level,
					      sizeof(next_tx_level));
}

/**
 * dp_low_tput_gro_flush_skip_handler() - adjust GRO flush for low tput
 * @dp_ctx: dp_ctx object
 * @next_vote_level: next bus bandwidth level
 * @legacy_client: legacy connection mode active
 *
 * If bus bandwidth level is PLD_BUS_WIDTH_LOW consistently and hit
 * the bus_low_cnt_threshold, set flag to skip GRO flush.
 * If bus bandwidth keeps going to PLD_BUS_WIDTH_IDLE, perform a GRO
 * flush to avoid TCP traffic stall
 *
 * Return: none
 */
static inline void dp_low_tput_gro_flush_skip_handler(
			struct wlan_dp_psoc_context *dp_ctx,
			enum pld_bus_width_type next_vote_level,
			bool legacy_client)
{
	uint32_t threshold = dp_ctx->dp_cfg.bus_low_cnt_threshold;
	ol_txrx_soc_handle soc = cds_get_context(QDF_MODULE_ID_SOC);
	int i;

	if (next_vote_level == PLD_BUS_WIDTH_LOW && legacy_client) {
		if (++dp_ctx->bus_low_vote_cnt >= threshold)
			qdf_atomic_set(&dp_ctx->low_tput_gro_enable, 1);
	} else {
		if (qdf_atomic_read(&dp_ctx->low_tput_gro_enable) &&
		    dp_ctx->enable_dp_rx_threads) {
			/* flush pending rx pkts when LOW->IDLE */
			dp_info("flush queued GRO pkts");
			for (i = 0; i < cdp_get_num_rx_contexts(soc); i++) {
				dp_rx_gro_flush_ind(soc, i,
						    DP_RX_GRO_NORMAL_FLUSH);
			}
		}

		dp_ctx->bus_low_vote_cnt = 0;
		qdf_atomic_set(&dp_ctx->low_tput_gro_enable, 0);
	}
}

#ifdef WDI3_STATS_UPDATE
/**
 * dp_ipa_set_perf_level() - set IPA perf level
 * @dp_ctx: handle to dp context
 * @tx_pkts: transmit packet count
 * @rx_pkts: receive packet count
 * @ipa_tx_pkts: IPA transmit packet count
 * @ipa_rx_pkts: IPA receive packet count
 *
 * Return: none
 */
static inline
void dp_ipa_set_perf_level(struct wlan_dp_psoc_context *dp_ctx,
			   uint64_t *tx_pkts, uint64_t *rx_pkts,
			   uint32_t *ipa_tx_pkts, uint32_t *ipa_rx_pkts)
{
}
#else
static void dp_ipa_set_perf_level(struct wlan_dp_psoc_context *dp_ctx,
				  uint64_t *tx_pkts, uint64_t *rx_pkts,
				  uint32_t *ipa_tx_pkts, uint32_t *ipa_rx_pkts)
{
	if (ucfg_ipa_is_fw_wdi_activated(dp_ctx->pdev)) {
		ucfg_ipa_uc_stat_query(dp_ctx->pdev, ipa_tx_pkts,
				       ipa_rx_pkts);
		*tx_pkts += *ipa_tx_pkts;
		*rx_pkts += *ipa_rx_pkts;

		ucfg_ipa_set_perf_level(dp_ctx->pdev, *tx_pkts, *rx_pkts);
		ucfg_ipa_uc_stat_request(dp_ctx->pdev, 2);
	}
}
#endif /* WDI3_STATS_UPDATE */

#ifdef WLAN_SUPPORT_TXRX_HL_BUNDLE
/**
 * dp_set_vdev_bundle_require_flag() - set vdev bundle require flag
 * @vdev_id: vdev id
 * @dp_ctx: handle to dp context
 * @tx_bytes: Tx bytes
 *
 * Return: none
 */
static inline
void dp_set_vdev_bundle_require_flag(uint16_t vdev_id,
				     struct wlan_dp_psoc_context *dp_ctx,
				     uint64_t tx_bytes)
{
	struct wlan_dp_psoc_cfg *cfg = dp_ctx->dp_cfg;

	cdp_vdev_set_bundle_require_flag(cds_get_context(QDF_MODULE_ID_SOC),
					 vdev_id, tx_bytes,
					 cfg->bus_bw_compute_interval,
					 cfg->pkt_bundle_threshold_high,
					 cfg->pkt_bundle_threshold_low);
}
#else
static inline
void dp_set_vdev_bundle_require_flag(uint16_t vdev_id,
				     struct wlan_dp_psoc_context *dp_ctx,
				     uint64_t tx_bytes)
{
}
#endif /* WLAN_SUPPORT_TXRX_HL_BUNDLE */

#ifdef QCA_SUPPORT_TXRX_DRIVER_TCP_DEL_ACK
/**
 * dp_set_driver_del_ack_enable() - set driver delayed ack enabled flag
 * @vdev_id: vdev id
 * @dp_ctx: handle to dp context
 * @rx_packets: receive packet count
 *
 * Return: none
 */
static inline
void dp_set_driver_del_ack_enable(uint16_t vdev_id,
				  struct wlan_dp_psoc_context *dp_ctx,
				  uint64_t rx_packets)
{
	struct wlan_dp_psoc_cfg *cfg = dp_ctx->dp_cfg;

	cdp_vdev_set_driver_del_ack_enable(cds_get_context(QDF_MODULE_ID_SOC),
					   vdev_id, rx_packets,
					   cfg->bus_bw_compute_interval,
					   cfg->del_ack_threshold_high,
					   cfg->del_ack_threshold_low);
}
#else
static inline
void dp_set_driver_del_ack_enable(uint16_t vdev_id,
				  struct wlan_dp_psoc_context *dp_ctx,
				  uint64_t rx_packets)
{
}
#endif /* QCA_SUPPORT_TXRX_DRIVER_TCP_DEL_ACK */

#define DP_BW_GET_DIFF(_x, _y) ((unsigned long)((ULONG_MAX - (_y)) + (_x) + 1))

#ifdef RX_PERFORMANCE
bool dp_is_current_high_throughput(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_get_psoc_priv_obj(psoc);

	if (dp_ctx->cur_vote_level < PLD_BUS_WIDTH_MEDIUM)
		return false;
	else
		return true;
}
#endif /* RX_PERFORMANCE */
#endif /* WLAN_FEATURE_DP_BUS_BANDWIDTH */
