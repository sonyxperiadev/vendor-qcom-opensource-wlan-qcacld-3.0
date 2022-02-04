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

/**
 * ucfg_dp_bbm_context_init() - Initialize BBM context
 * @psoc: psoc handle
 *
 * Returns: error code
 */
int ucfg_dp_bbm_context_init(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_bbm_context_deinit() - De-initialize BBM context
 * @psoc: psoc handle
 *
 * Returns: None
 */
void ucfg_dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_bbm_apply_independent_policy() - Apply independent policies
 *  to set the bus bw level
 * @psoc: psoc handle
 * @params: BBM policy related params
 *
 * The function applies BBM related policies and appropriately sets the bus
 * bandwidth level.
 *
 * Returns: None
 */
void ucfg_dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
					  struct bbm_params *params);

/**
 * ucfg_dp_set_rx_mode_rps() - Enable/disable RPS in SAP mode
 * @enable: Set true to enable RPS in SAP mode
 *
 * Callback function registered with datapath
 *
 * Return: none
 */
void ucfg_dp_set_rx_mode_rps(bool enable);

/**
 * ucfg_dp_try_send_rps_ind() - send rps indication to daemon
 * @vdev: vdev handle
 *
 * If RPS feature enabled by INI, send RPS enable indication to daemon
 * Indication contents is the name of interface to find correct sysfs node
 * Should send all available interfaces
 *
 * Return: none
 */
void ucfg_dp_try_send_rps_ind(struct wlan_objmgr_vdev *vdev);

/**
 * ucfg_dp_reg_ipa_rsp_ind() - Resiter RSP IND cb with IPA component
 * @psoc: psoc handle
 * @cb_obj: Callback object
 *
 * Returns: None
 */
void ucfg_dp_reg_ipa_rsp_ind(struct wlan_objmgr_pdev *pdev);

/**
 * ucfg_dp_try_set_rps_cpu_mask - set RPS CPU mask for interfaces
 * @psoc: psoc handle
 *
 * Return: none
 */
void ucfg_dp_try_set_rps_cpu_mask(struct wlan_objmgr_psoc *psoc);

/**
 * ucfg_dp_register_hdd_callbacks() - Resiter HDD callbacks with DP component
 * @psoc: psoc handle
 * @cb_obj: Callback object
 *
 * Returns: None
 */
void ucfg_dp_register_hdd_callbacks(struct wlan_objmgr_psoc *psoc,
				    struct wlan_dp_psoc_callbacks *cb_obj);

#endif /* _WLAN_DP_UCFG_API_H_ */
