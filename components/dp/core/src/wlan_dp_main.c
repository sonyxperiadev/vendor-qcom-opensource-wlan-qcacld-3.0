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
