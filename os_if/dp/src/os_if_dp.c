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
  * DOC: os_if_dp.c
  *
  *
  */
#include "os_if_dp.h"
#include "wlan_nlink_srv.h"
#include <wlan_cfg80211.h>
#include <wlan_osif_priv.h>
#include <cdp_txrx_cmn.h>
#include "qca_vendor.h"

#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
/**
 * osif_dp_send_tcp_param_update_event() - Send vendor event to update
 * TCP parameter through Wi-Fi HAL
 * @psoc: Pointer to psoc context
 * @data: Parameters to update
 * @dir: Direction(tx/rx) to update
 *
 * Return: None
 */
void osif_dp_send_tcp_param_update_event(struct wlan_objmgr_psoc *psoc,
					 void *data,
					 uint8_t dir)
{
}
#endif /*WLAN_FEATURE_DP_BUS_BANDWIDTH*/
