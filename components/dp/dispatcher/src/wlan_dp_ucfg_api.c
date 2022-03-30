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
#include "wlan_dp_main.h"
#include "wlan_dp_objmgr.h"
#include "cdp_txrx_cmn.h"
#include "cfg_ucfg_api.h"

void ucfg_dp_update_inf_mac(struct wlan_objmgr_psoc *psoc,
			    struct qdf_mac_addr *cur_mac,
			    struct qdf_mac_addr *new_mac)
{
}

QDF_STATUS
ucfg_dp_create_intf(struct wlan_objmgr_psoc *psoc,
		    struct qdf_mac_addr *intf_addr)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
ucfg_dp_destroy_intf(struct wlan_objmgr_psoc *psoc,
		     struct qdf_mac_addr *intf_addr)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS ucfg_dp_init(void)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS ucfg_dp_deinit(void)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
ucfg_dp_update_config(struct wlan_objmgr_psoc *psoc,
		      struct wlan_dp_user_config *req)
{
	return QDF_STATUS_SUCCESS;
}

uint64_t
ucfg_dp_get_rx_softirq_yield_duration(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}
