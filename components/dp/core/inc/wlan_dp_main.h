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
  * DOC: wlan_dp_main.h
  *
  *
  */
#ifndef __WLAN_DP_MAIN_H__
#define __WLAN_DP_MAIN_H__

#include "wlan_dp_public_struct.h"
#include "wlan_dp_priv.h"
#include "wlan_dp_objmgr.h"

/**
 * dp_allocate_ctx() - Allocate DP context
 *
 */
QDF_STATUS dp_allocate_ctx(void);

/**
 * dp_free_ctx() - Free DP context
 *
 */
void dp_free_ctx(void);

/**
 * dp_get_front_intf_no_lock() - Get the first interface from the intf list
 * This API doesnot use any lock in it's implementation. It is the caller's
 * directive to ensure concurrency safety.
 * @dp_ctx: pointer to the DP context
 * @out_intf: double pointer to pass the next interface
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
dp_get_front_intf_no_lock(struct wlan_dp_psoc_context *dp_ctx,
			  struct wlan_dp_intf **out_intf);

/**
 * dp_get_next_intf_no_lock() - Get the next intf from the intf list
 * This API doesnot use any lock in it's implementation. It is the caller's
 * directive to ensure concurrency safety.
 * @dp_ctx: pointer to the DP context
 * @cur_intf: pointer to the current intf
 * @out_intf: double pointer to pass the next intf
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
dp_get_next_intf_no_lock(struct wlan_dp_psoc_context *dp_ctx,
			 struct wlan_dp_intf *cur_intf,
			 struct wlan_dp_intf **out_intf);

/**
 * __dp_take_ref_and_fetch_front_intf_safe - Helper macro to lock, fetch
 * front and next intf, take ref and unlock.
 * @dp_ctx: the global DP context
 * @dp_intf: an dp_intf pointer to use as a cursor
 * @dp_intf_next: dp_intf pointer to next intf
 *
 */
#define __dp_take_ref_and_fetch_front_intf_safe(dp_ctx, dp_intf, \
						dp_intf_next) \
	qdf_spin_lock_bh(&dp_ctx->intf_list_lock), \
	dp_get_front_intf_no_lock(dp_ctx, &dp_intf), \
	dp_get_next_intf_no_lock(dp_ctx, dp_intf, &dp_intf_next), \
	qdf_spin_unlock_bh(&dp_ctx->intf_list_lock)

/**
 * __dp_take_ref_and_fetch_next_intf_safe - Helper macro to lock, fetch next
 * interface, take ref and unlock.
 * @dp_ctx: the global DP context
 * @dp_intf: dp_intf pointer to use as a cursor
 * @dp_intf_next: dp_intf pointer to next interface
 *
 */
#define __dp_take_ref_and_fetch_next_intf_safe(dp_ctx, dp_intf, \
					       dp_intf_next) \
	qdf_spin_lock_bh(&dp_ctx->intf_list_lock), \
	dp_intf = dp_intf_next, \
	dp_get_next_intf_no_lock(dp_ctx, dp_intf, &dp_intf_next), \
	qdf_spin_unlock_bh(&dp_ctx->intf_list_lock)

/**
 * __dp_is_intf_valid - Helper macro to return true/false for valid interface.
 * @_dp_intf: an dp_intf pointer to use as a cursor
 */
#define __dp_is_intf_valid(_dp_intf) !!(_dp_intf)

/**
 * dp_for_each_intf_held_safe - Interface iterator called
 *                                      in a delete safe manner
 * @dp_ctx: the global DP context
 * @dp_intf: an dp_intf pointer to use as a cursor
 * @dp_intf_next: dp_intf pointer to the next interface
 *
 */
#define dp_for_each_intf_held_safe(dp_ctx, dp_intf, dp_intf_next) \
	for (__dp_take_ref_and_fetch_front_intf_safe(dp_ctx, dp_intf, \
						     dp_intf_next); \
	     __dp_is_intf_valid(dp_intf); \
	     __dp_take_ref_and_fetch_next_intf_safe(dp_ctx, dp_intf, \
						    dp_intf_next))

/**
 * dp_get_intf_by_macaddr() - Api to Get interface from MAC address
 * @dp_ctx: DP context
 * @addr: MAC address
 *
 * Return: Pointer to DP interface.
 */
struct wlan_dp_intf*
dp_get_intf_by_macaddr(struct wlan_dp_psoc_context *dp_ctx,
		       struct qdf_mac_addr *addr);

/**
 * dp_vdev_obj_destroy_notification() - Free per DP vdev object
 * @vdev: vdev context
 * @arg: Pointer to arguments
 *
 * This function gets called from object manager when vdev is being
 * deleted and delete DP vdev context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_vdev_obj_destroy_notification(struct wlan_objmgr_vdev *vdev, void *arg);

/**
 * dp_vdev_obj_create_notification() - Allocate per DP vdev object
 * @vdev: vdev context
 * @arg: Pointer to arguments
 *
 * This function gets called from object manager when vdev is being
 * created and creates DP vdev context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_vdev_obj_create_notification(struct wlan_objmgr_vdev *vdev, void *arg);

/**
 * dp_pdev_obj_create_notification() - Allocate per DP pdev object
 * @pdev: pdev context
 * @arg: Pointer to arguments
 *
 * This function gets called from object manager when pdev is being
 * created and creates DP pdev context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_pdev_obj_create_notification(struct wlan_objmgr_pdev *pdev, void *arg);

/**
 * dp_pdev_obj_destroy_notification() - Free per DP pdev object
 * @pdev: pdev context
 * @arg: Pointer to arguments
 *
 * This function gets called from object manager when pdev is being
 * deleted and delete DP pdev context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_pdev_obj_destroy_notification(struct wlan_objmgr_pdev *pdev, void *arg);

/**
 * dp_psoc_obj_create_notification() - Function to allocate per DP
 * psoc private object
 * @psoc: psoc context
 * @arg: Pointer to arguments
 *
 * This function gets called from object manager when psoc is being
 * created and creates DP soc context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_psoc_obj_create_notification(struct wlan_objmgr_psoc *psoc, void *arg);

/**
 * dp_psoc_obj_destroy_notification() - Free psoc private object
 * @psoc: psoc context
 * @data: Pointer to data
 *
 * This function gets called from object manager when psoc is being
 * deleted and delete DP soc context.
 *
 * Return: QDF_STATUS_SUCCESS - in case of success
 */
QDF_STATUS
dp_psoc_obj_destroy_notification(struct wlan_objmgr_psoc *psoc, void *arg);

/**
 * dp_ttach_ctx() - Api to attach dp ctx
 * @dp_ctx : DP Context
 *
 * Helper function to attach dp ctx
 *
 * Return: None.
 */
void dp_attach_ctx(struct wlan_dp_psoc_context *dp_ctx);

/**
 * dp_dettach_ctx() - to dettach dp context
 *
 * Helper function to dettach dp context
 *
 * Return: None.
 */
void dp_dettach_ctx(void);

/**
 * dp_get_context() - to get dp context
 *
 * Helper function to get dp context
 *
 * Return: dp context.
 */
struct wlan_dp_psoc_context *dp_get_context(void);

#endif
