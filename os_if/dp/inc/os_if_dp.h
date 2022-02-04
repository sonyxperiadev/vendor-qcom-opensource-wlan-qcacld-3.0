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
  * DOC: os_if_dp.h
  *
  *
  */
#ifndef __OSIF_DP_H__
#define __OSIF_DP_H__

#include "wlan_dp_public_struct.h"

/**
 * osif_dp_classify_pkt() - classify packet
 * @skb - sk buff
 *
 * Return: None
 */
void osif_dp_classify_pkt(struct sk_buff *skb);

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
					 union wlan_tp_data *data, uint8_t dir);
#else
static inline
void osif_dp_send_tcp_param_update_event(struct wlan_objmgr_psoc *psoc,
					 union wlan_tp_data *data, uint8_t dir)
{
}
#endif /* WLAN_FEATURE_DP_BUS_BANDWIDTH */
#endif /* __OSIF_DP_H__ */
