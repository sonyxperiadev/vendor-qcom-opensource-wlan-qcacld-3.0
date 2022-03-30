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

/**
 * dp_get_intf_by_macaddr() - Get DP interface by mac address.
 * @dp_ctx: dp_ctx handle
 * @addr: MAC address
 *
 * Return: DP interface on success else NULL.
 */
struct wlan_dp_intf*
dp_get_intf_by_macaddr(struct wlan_dp_psoc_context *dp_ctx,
		       struct qdf_mac_addr *addr)
{
	return NULL;
}

/**
 * dp_vdev_obj_create_notification() - DP vdev object creation notification
 * @vdev: vdev handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_vdev_obj_create_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_vdev_obj_destroy_notification() - DP vdev object destroy notification
 * @vdev: vdev handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_vdev_obj_destroy_notification(struct wlan_objmgr_vdev *vdev, void *arg)

{
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_pdev_obj_create_notification() - DP pdev object creation notification
 * @pdev: pdev handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_pdev_obj_create_notification(struct wlan_objmgr_pdev *pdev, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_pdev_obj_destroy_notification() - DP pdev object destroy notification
 * @pdev: pdev handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_pdev_obj_destroy_notification(struct wlan_objmgr_pdev *pdev, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_psoc_obj_create_notification() - DP pdev object creation notification
 * @psoc: psoc handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_psoc_obj_create_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	return QDF_STATUS_SUCCESS;
}

/**
 * dp_psoc_obj_destroy_notification() - DP psoc object destroy notification
 * @psoc: psoc handle
 * @arg_list: arguments list
 *
 * Return: QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
dp_psoc_obj_destroy_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	return QDF_STATUS_SUCCESS;
}
