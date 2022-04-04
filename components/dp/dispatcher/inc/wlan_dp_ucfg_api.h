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
 * DOC: wlan_dp_ucfg_api.h
 *
 * TDLS north bound interface declaration
 */

#if !defined(_WLAN_DP_UCFG_API_H_)
#define _WLAN_DP_UCFG_API_H_

#include <scheduler_api.h>
#include <wlan_objmgr_cmn.h>
#include <wlan_objmgr_psoc_obj.h>
#include <wlan_objmgr_pdev_obj.h>
#include <wlan_objmgr_vdev_obj.h>
#include <wlan_dp_public_struct.h>

/**
 * ucfg_dp_create_intf() - update DP interface MAC address
 * @psoc: psoc handle
 * @cur_mac: Curent MAC address
 * @new_mac: new MAC address
 *
 */
void ucfg_dp_update_inf_mac(struct wlan_objmgr_psoc *psoc,
			    struct qdf_mac_addr *cur_mac,
			    struct qdf_mac_addr *new_mac);

/**
 * ucfg_dp_destroy_intf() - DP module interface deletion
 * @psoc: psoc handle
 * @intf_addr: Interface MAC address
 *
 */
QDF_STATUS ucfg_dp_destroy_intf(struct wlan_objmgr_psoc *psoc,
				struct qdf_mac_addr *intf_addr);

/**
 * ucfg_dp_create_intf() - DP module interface creation
 * @psoc: psoc handle
 * @intf_addr: Interface MAC address
 * @ndev : netdev object
 *
 */
QDF_STATUS ucfg_dp_create_intf(struct wlan_objmgr_psoc *psoc,
			       struct qdf_mac_addr *intf_addr,
			       qdf_netdev_t ndev);

/**
 * ucfg_dp_init() - DP module initialization API
 *
 * Return: QDF_STATUS
 */
QDF_STATUS ucfg_dp_init(void);

/**
 * ucfg_dp_deinit() - DP module deinitialization API
 *
 * Return: QDF_STATUS
 */
QDF_STATUS ucfg_dp_deinit(void);

/**
 * ucfg_dp_psoc_open() - DP component Open
 * @psoc: pointer to psoc object
 *
 * Return: QDF Status
 */
QDF_STATUS ucfg_dp_psoc_open(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_psoc_close() - DP component Close
 * @psoc: pointer to psoc object
 *
 * Return: QDF Status
 */
QDF_STATUS ucfg_dp_psoc_close(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_suspend_wlan() - update suspend state in DP component
 * @psoc: pointer to psoc object
 *
 * Return: None
 */
void ucfg_dp_suspend_wlan(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_resume_wlan() - update resume state in DP component
 * @psoc: pointer to psoc object
 *
 * Return: None
 */
void ucfg_dp_resume_wlan(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_update_config() - DP module config update
 * @psoc: pointer to psoc object
 * @req : user config
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
ucfg_dp_update_config(struct wlan_objmgr_psoc *psoc,
		      struct wlan_dp_user_config *req);

/**
 * ucfg_dp_get_rx_softirq_yield_duration() - Get rx soft IRQ yield duration
 * @psoc: pointer to psoc object
 *
 * Return: soft IRQ yield duration
 */
uint64_t
ucfg_dp_get_rx_softirq_yield_duration(struct wlan_objmgr_psoc *psoc);

#endif
