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
  * DOC: wlan_dp_main.c
  *
  *
  */
#include "wlan_dp_main.h"
#include "wlan_dp_public_struct.h"
#include "cfg_ucfg_api.h"
#include "wlan_dp_bus_bandwidth.h"
#include <wlan_objmgr_psoc_obj_i.h>
#include <wlan_nlink_common.h>
#include <qdf_net_types.h>
#include "wlan_objmgr_vdev_obj.h"

/* Global DP context */
static struct wlan_dp_psoc_context *gp_dp_ctx;

QDF_STATUS dp_allocate_ctx(void)
{
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx = qdf_mem_malloc(sizeof(*dp_ctx));
	if (!dp_ctx) {
		dp_err("Failed to create DP context");
		return QDF_STATUS_E_NOMEM;
	}

	qdf_spinlock_create(&dp_ctx->intf_list_lock);
	qdf_list_create(&dp_ctx->intf_list, 0);

	dp_attach_ctx(dp_ctx);

	return QDF_STATUS_SUCCESS;
}

void dp_free_ctx(void)
{
	struct wlan_dp_psoc_context *dp_ctx;

	dp_ctx =  dp_get_context();

	qdf_spinlock_destroy(&dp_ctx->intf_list_lock);
	qdf_list_destroy(&dp_ctx->intf_list);
	dp_dettach_ctx();
	qdf_mem_free(dp_ctx);
}

QDF_STATUS dp_get_front_intf_no_lock(struct wlan_dp_psoc_context *dp_ctx,
				     struct wlan_dp_intf **out_intf)
{
	QDF_STATUS status;
	qdf_list_node_t *node;

	*out_intf = NULL;

	status = qdf_list_peek_front(&dp_ctx->intf_list, &node);

	if (QDF_IS_STATUS_ERROR(status))
		return status;

	*out_intf = qdf_container_of(node, struct wlan_dp_intf, node);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS dp_get_next_intf_no_lock(struct wlan_dp_psoc_context *dp_ctx,
				    struct wlan_dp_intf *cur_intf,
				    struct wlan_dp_intf **out_intf)
{
	QDF_STATUS status;
	qdf_list_node_t *node;

	if (!cur_intf)
		return QDF_STATUS_E_INVAL;

	*out_intf = NULL;

	status = qdf_list_peek_next(&dp_ctx->intf_list,
				    &cur_intf->node,
				    &node);

	if (QDF_IS_STATUS_ERROR(status))
		return status;

	*out_intf = qdf_container_of(node, struct wlan_dp_intf, node);

	return status;
}

struct wlan_dp_intf*
dp_get_intf_by_macaddr(struct wlan_dp_psoc_context *dp_ctx,
		       struct qdf_mac_addr *addr)
{
	struct wlan_dp_intf *dp_intf;

	qdf_spin_lock_bh(&dp_ctx->intf_list_lock);
	for (dp_get_front_intf_no_lock(dp_ctx, &dp_intf); dp_intf;
	     dp_get_next_intf_no_lock(dp_ctx, dp_intf, &dp_intf)) {
		if (qdf_is_macaddr_equal(&dp_intf->mac_addr, addr)) {
			qdf_spin_unlock_bh(&dp_ctx->intf_list_lock);
			return dp_intf;
		}
	}
	qdf_spin_unlock_bh(&dp_ctx->intf_list_lock);

	return NULL;
}

static void dp_cfg_init(struct wlan_dp_psoc_context *ctx)
{
}

QDF_STATUS
dp_vdev_obj_create_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	struct wlan_objmgr_psoc *psoc;
	struct wlan_dp_psoc_context *dp_ctx;
	struct wlan_dp_intf *dp_intf;
	QDF_STATUS status = QDF_STATUS_SUCCESS;
	struct qdf_mac_addr *mac_addr;

	dp_info("DP VDEV OBJ create notification");

	psoc = wlan_vdev_get_psoc(vdev);
	if (!psoc) {
		dp_err("Failed to get psoc");
		return QDF_STATUS_E_INVAL;
	}

	dp_ctx =  dp_psoc_get_priv(psoc);
	mac_addr = (struct qdf_mac_addr *)wlan_vdev_mlme_get_macaddr(vdev);

	dp_intf = dp_get_intf_by_macaddr(dp_ctx, mac_addr);
	if (!dp_intf) {
		dp_err("Failed to get dp intf mac:" QDF_MAC_ADDR_FMT,
		       QDF_MAC_ADDR_REF(mac_addr));
		return QDF_STATUS_E_INVAL;
	}

	dp_intf->device_mode = wlan_vdev_mlme_get_opmode(vdev);
	dp_intf->intf_id = vdev->vdev_objmgr.vdev_id;
	dp_intf->vdev = vdev;

	status = wlan_objmgr_vdev_component_obj_attach(vdev,
						       WLAN_COMP_DP,
						       (void *)dp_intf,
						       QDF_STATUS_SUCCESS);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to attach dp_intf with vdev");
		return status;
	}

	return status;
}

QDF_STATUS
dp_vdev_obj_destroy_notification(struct wlan_objmgr_vdev *vdev, void *arg)

{
	struct wlan_dp_intf *dp_intf;
	QDF_STATUS status = QDF_STATUS_SUCCESS;

	dp_info("DP VDEV OBJ destroy notification");

	dp_intf = dp_get_vdev_priv_obj(vdev);
	if (!dp_intf) {
		dp_err("Failed to get DP interface obj");
		return QDF_STATUS_E_INVAL;
	}

	status = wlan_objmgr_vdev_component_obj_detach(vdev,
						       WLAN_COMP_DP,
						       (void *)dp_intf);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to detach dp_intf with vdev");
		return status;
	}

	return status;
}

QDF_STATUS
dp_pdev_obj_create_notification(struct wlan_objmgr_pdev *pdev, void *arg)
{
	struct wlan_objmgr_psoc *psoc;
	struct wlan_dp_psoc_context *dp_ctx;
	QDF_STATUS status;

	dp_info("DP PDEV OBJ create notification");
	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		obj_mgr_err("psoc is NULL in pdev");
		return QDF_STATUS_E_FAILURE;
	}
	dp_ctx =  dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("Failed to get dp_ctx from psoc");
		return QDF_STATUS_E_FAILURE;
	}
	status = wlan_objmgr_pdev_component_obj_attach(pdev,
						       WLAN_COMP_DP,
						       (void *)dp_ctx,
						       QDF_STATUS_SUCCESS);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to attach dp_ctx to pdev");
		return status;
	}

	dp_ctx->pdev = pdev;
	return status;
}

QDF_STATUS
dp_pdev_obj_destroy_notification(struct wlan_objmgr_pdev *pdev, void *arg)
{
	struct wlan_objmgr_psoc *psoc;
	struct wlan_dp_psoc_context *dp_ctx;
	QDF_STATUS status;

	dp_info("DP PDEV OBJ destroy notification");
	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		obj_mgr_err("psoc is NULL in pdev");
		return QDF_STATUS_E_FAILURE;
	}

	dp_ctx = wlan_objmgr_pdev_get_comp_private_obj(pdev, WLAN_COMP_DP);
	if (!dp_ctx) {
		dp_err("Failed to get dp_ctx from pdev");
		return QDF_STATUS_E_FAILURE;
	}
	status = wlan_objmgr_pdev_component_obj_detach(pdev,
						       WLAN_COMP_DP,
						       dp_ctx);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to detatch dp_ctx from pdev");
		return status;
	}
	if (!dp_ctx->pdev)
		dp_err("DP Pdev is NULL");

	dp_ctx->pdev = NULL;
	return status;
}

QDF_STATUS
dp_psoc_obj_create_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct wlan_dp_psoc_context *dp_ctx = gp_dp_ctx;
	QDF_STATUS status;

	status = wlan_objmgr_psoc_component_obj_attach(
				psoc, WLAN_COMP_DP,
				dp_ctx, QDF_STATUS_SUCCESS);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to attach psoc component obj");
		return status;
	}

	dp_ctx->psoc = psoc;
	dp_cfg_init(dp_ctx);
	return status;
}

QDF_STATUS
dp_psoc_obj_destroy_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct wlan_dp_psoc_context *dp_ctx;
	QDF_STATUS status;

	dp_ctx = dp_psoc_get_priv(psoc);
	if (!dp_ctx) {
		dp_err("psoc priv is NULL");
		return QDF_STATUS_E_FAILURE;
	}

	status = wlan_objmgr_psoc_component_obj_detach(
					psoc, WLAN_COMP_DP,
					dp_ctx);
	if (QDF_IS_STATUS_ERROR(status)) {
		dp_err("Failed to detach psoc component obj");
		return status;
	}

	return status;
}

void dp_attach_ctx(struct wlan_dp_psoc_context *dp_ctx)
{
	if (gp_dp_ctx)
		dp_debug("already attached global dp ctx");
	gp_dp_ctx = dp_ctx;
}

void dp_dettach_ctx(void)
{
	if (!gp_dp_ctx) {
		dp_err("global dp ctx is already dettached");
		return;
	}
	gp_dp_ctx = NULL;
}

struct wlan_dp_psoc_context *dp_get_context(void)
{
	return gp_dp_ctx;
}

/**
 * dp_hex_string_to_u16_array() - convert a hex string to a uint16 array
 * @str: input string
 * @int_array: pointer to input array of type uint16
 * @len: pointer to number of elements which the function adds to the array
 * @int_array_max_len: maximum number of elements in input uint16 array
 *
 * This function is used to convert a space separated hex string to an array of
 * uint16_t. For example, an input string str = "a b c d" would be converted to
 * a unint16 array, int_array = {0xa, 0xb, 0xc, 0xd}, *len = 4.
 * This assumes that input value int_array_max_len >= 4.
 *
 * Return: QDF_STATUS_SUCCESS - if the conversion is successful
 *         non zero value     - if the conversion is a failure
 */
static QDF_STATUS
dp_hex_string_to_u16_array(char *str, uint16_t *int_array, uint8_t *len,
			   uint8_t int_array_max_len)
{
	char *s = str;
	uint32_t val = 0;

	if (!str || !int_array || !len)
		return QDF_STATUS_E_INVAL;

	dp_debug("str %pK intArray %pK intArrayMaxLen %d",
		 s, int_array, int_array_max_len);

	*len = 0;

	while ((s) && (*len < int_array_max_len)) {
		/*
		 * Increment length only if sscanf successfully extracted one
		 * element. Any other return value means error. Ignore it.
		 */
		if (sscanf(s, "%x", &val) == 1) {
			int_array[*len] = (uint16_t)val;
			dp_debug("s %pK val %x intArray[%d]=0x%x",
				 s, val, *len, int_array[*len]);
			*len += 1;
		}
		s = strpbrk(s, " ");
		if (s)
			s++;
	}
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_get_interface() - to get dp interface matching the mode
 * @dp_ctx: dp context
 * @mode: interface mode
 *
 * This routine will return the pointer to dp interface matching
 * with the passed mode.
 *
 * Return: pointer to interface or null
 */
static struct
wlan_dp_intf *dp_get_interface(struct wlan_dp_psoc_context *dp_ctx,
			       enum QDF_OPMODE mode)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_intf *dp_intf_next;

	dp_for_each_intf_held_safe(dp_ctx, dp_intf, dp_intf_next) {
		if (!dp_intf)
			continue;

		if (dp_intf->device_mode == mode)
			return dp_intf;
	}

	return NULL;
}

void dp_send_rps_ind(struct wlan_dp_intf *dp_intf)
{
	int i;
	uint8_t cpu_map_list_len = 0;
	struct wlan_dp_psoc_context *dp_ctx = dp_intf->dp_ctx;
	struct wlan_rps_data rps_data;
	struct cds_config_info *cds_cfg;

	cds_cfg = cds_get_ini_config();
	if (!cds_cfg) {
		dp_err("cds_cfg is NULL");
		return;
	}

	rps_data.num_queues = NUM_RX_QUEUES;

	dp_info("cpu_map_list '%s'", dp_ctx->dp_cfg.cpu_map_list);

	/* in case no cpu map list is provided, simply return */
	if (!strlen(dp_ctx->dp_cfg.cpu_map_list)) {
		dp_info("no cpu map list found");
		goto err;
	}

	if (QDF_STATUS_SUCCESS !=
		dp_hex_string_to_u16_array(dp_ctx->dp_cfg.cpu_map_list,
					   rps_data.cpu_map_list,
					   &cpu_map_list_len,
					   WLAN_SVC_IFACE_NUM_QUEUES)) {
		dp_err("invalid cpu map list");
		goto err;
	}

	rps_data.num_queues =
		(cpu_map_list_len < rps_data.num_queues) ?
				cpu_map_list_len : rps_data.num_queues;

	for (i = 0; i < rps_data.num_queues; i++) {
		dp_info("cpu_map_list[%d] = 0x%x",
			i, rps_data.cpu_map_list[i]);
	}

	strlcpy(rps_data.ifname, qdf_netdev_get_devname(dp_intf->dev),
		sizeof(rps_data.ifname));
	dp_ctx->dp_ops.dp_send_svc_nlink_msg(cds_get_radio_index(),
					     WLAN_SVC_RPS_ENABLE_IND,
					     &rps_data, sizeof(rps_data));

	cds_cfg->rps_enabled = true;

	return;

err:
	dp_info("Wrong RPS configuration. enabling rx_thread");
	cds_cfg->rps_enabled = false;
}

void dp_try_send_rps_ind(struct wlan_objmgr_vdev *vdev)
{
	struct wlan_dp_intf *dp_intf = dp_get_vdev_priv_obj(vdev);

	if (!dp_intf) {
		dp_err("dp interface is NULL");
		return;
	}
	if (dp_intf->dp_ctx->rps)
		dp_send_rps_ind(dp_intf);
}

void dp_send_rps_disable_ind(struct wlan_dp_intf *dp_intf)
{
	struct wlan_rps_data rps_data;
	struct cds_config_info *cds_cfg;

	cds_cfg = cds_get_ini_config();

	if (!cds_cfg) {
		dp_err("cds_cfg is NULL");
		return;
	}

	rps_data.num_queues = NUM_RX_QUEUES;

	dp_info("Set cpu_map_list 0");

	qdf_mem_zero(&rps_data.cpu_map_list, sizeof(rps_data.cpu_map_list));

	strlcpy(rps_data.ifname, qdf_netdev_get_devname(dp_intf->dev),
		sizeof(rps_data.ifname));
	dp_intf->dp_ctx->dp_ops.dp_send_svc_nlink_msg(cds_get_radio_index(),
				    WLAN_SVC_RPS_ENABLE_IND,
				    &rps_data, sizeof(rps_data));

	cds_cfg->rps_enabled = false;
}

#ifdef QCA_CONFIG_RPS
void dp_set_rps(uint8_t vdev_id, bool enable)
{
	struct wlan_objmgr_vdev *vdev;
	struct wlan_dp_psoc_context *dp_ctx;
	struct wlan_dp_intf *dp_intf;

	dp_ctx = dp_get_context();
	if (!dp_ctx)
		return;

	vdev = wlan_objmgr_get_vdev_by_id_from_psoc(dp_ctx->psoc,
						    vdev_id, WLAN_DP_ID);
	if (!vdev)
		return;

	dp_intf = dp_get_vdev_priv_obj(vdev);
	if (!dp_intf) {
		dp_comp_vdev_put_ref(vdev);
		dp_err_rl("DP interface not found for vdev_id: %d", vdev_id);
		return;
	}

	dp_info("Set RPS to %d for vdev_id %d", enable, vdev_id);
	if (!dp_ctx->rps) {
		if (enable)
			dp_send_rps_ind(dp_intf);
		else
			dp_send_rps_disable_ind(dp_intf);
	}
	dp_comp_vdev_put_ref(vdev);
}
#endif

void dp_set_rx_mode_rps(bool enable)
{
	struct wlan_dp_psoc_context *dp_ctx;
	struct wlan_dp_intf *dp_intf;
	struct cds_config_info *cds_cfg;

	dp_ctx = dp_get_context();
	cds_cfg = cds_get_ini_config();
	if (!dp_ctx || !cds_cfg)
		return;

	dp_intf = dp_get_interface(dp_ctx, QDF_SAP_MODE);
	if (!dp_intf)
		return;

	if (!dp_intf->dp_ctx->rps && cds_cfg->uc_offload_enabled) {
		if (enable && !cds_cfg->rps_enabled)
			dp_send_rps_ind(dp_intf);
		else if (!enable && cds_cfg->rps_enabled)
			dp_send_rps_disable_ind(dp_intf);
	}
}

void dp_set_rps_cpu_mask(struct wlan_dp_psoc_context *dp_ctx)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_intf *dp_intf_next;

	dp_for_each_intf_held_safe(dp_ctx, dp_intf, dp_intf_next) {
		if (!dp_intf)
			continue;

		dp_send_rps_ind(dp_intf);
	}
}

void dp_try_set_rps_cpu_mask(struct wlan_objmgr_psoc *psoc)
{
	struct wlan_dp_psoc_context *dp_ctx = dp_psoc_get_priv(psoc);

	if (!dp_ctx) {
		dp_err("dp context is NULL");
		return;
	}
	dp_set_rps_cpu_mask(dp_ctx);
}

void dp_clear_rps_cpu_mask(struct wlan_dp_psoc_context *dp_ctx)
{
	struct wlan_dp_intf *dp_intf;
	struct wlan_dp_intf *dp_intf_next;

	dp_for_each_intf_held_safe(dp_ctx, dp_intf, dp_intf_next) {
		if (!dp_intf)
			continue;

		dp_send_rps_disable_ind(dp_intf);
	}
}
