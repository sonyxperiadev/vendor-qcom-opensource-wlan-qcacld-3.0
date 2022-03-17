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
#include "wlan_dp_ucfg_api.h"

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
static
void osif_dp_send_tcp_param_update_event(struct wlan_objmgr_psoc *psoc,
					 union wlan_tp_data *data,
					 uint8_t dir)
{
	struct sk_buff *vendor_event;
	struct wlan_objmgr_pdev *pdev;
	struct pdev_osif_priv *os_priv;
	uint32_t event_len;
	bool tcp_limit_output = false;
	bool tcp_del_ack_ind_enabled = false;
	bool tcp_adv_win_scl_enabled = false;
	enum wlan_tp_level next_tp_level = WLAN_SVC_TP_NONE;

	event_len = sizeof(uint8_t) + sizeof(uint8_t) + NLMSG_HDRLEN;
	pdev = wlan_objmgr_get_pdev_by_id(psoc, 0, WLAN_OSIF_ID);
	os_priv = wlan_pdev_get_ospriv(pdev);

	if (dir == 0) /*TX Flow */ {
		struct wlan_tx_tp_data *tx_tp_data =
				(struct wlan_tx_tp_data *)data;

		next_tp_level = tx_tp_data->level;

		if (tx_tp_data->tcp_limit_output) {
			/* TCP_LIMIT_OUTPUT_BYTES */
			event_len += sizeof(uint32_t);
			tcp_limit_output = true;
		}
	} else if (dir == 1) /* RX Flow */ {
		struct wlan_rx_tp_data *rx_tp_data =
				(struct wlan_rx_tp_data *)data;

		next_tp_level = rx_tp_data->level;

		if (rx_tp_data->rx_tp_flags & TCP_DEL_ACK_IND_MASK) {
			event_len += sizeof(uint32_t); /* TCP_DELACK_SEG */
			tcp_del_ack_ind_enabled = true;
		}
		if (rx_tp_data->rx_tp_flags & TCP_ADV_WIN_SCL_MASK) {
			event_len += sizeof(uint32_t); /* TCP_ADV_WIN_SCALE */
			tcp_adv_win_scl_enabled = true;
		}
	} else {
		dp_err("Invalid Direction [%d]", dir);
		wlan_objmgr_pdev_release_ref(pdev, WLAN_DP_ID);
		return;
	}

	vendor_event =
	cfg80211_vendor_event_alloc(
			os_priv->wiphy,
			NULL, event_len,
			QCA_NL80211_VENDOR_SUBCMD_THROUGHPUT_CHANGE_EVENT_INDEX,
			GFP_KERNEL);

	if (!vendor_event) {
		dp_err("cfg80211_vendor_event_alloc failed");
		wlan_objmgr_pdev_release_ref(pdev, WLAN_DP_ID);
		return;
	}

	if (nla_put_u8(
		vendor_event,
		QCA_WLAN_VENDOR_ATTR_THROUGHPUT_CHANGE_DIRECTION,
		dir))
		goto tcp_param_change_nla_failed;

	if (nla_put_u8(
		vendor_event,
		QCA_WLAN_VENDOR_ATTR_THROUGHPUT_CHANGE_THROUGHPUT_LEVEL,
		(next_tp_level == WLAN_SVC_TP_LOW ?
		QCA_WLAN_THROUGHPUT_LEVEL_LOW :
		QCA_WLAN_THROUGHPUT_LEVEL_HIGH)))
		goto tcp_param_change_nla_failed;

	if (tcp_limit_output &&
	    nla_put_u32(
		vendor_event,
		QCA_WLAN_VENDOR_ATTR_THROUGHPUT_CHANGE_TCP_LIMIT_OUTPUT_BYTES,
		(next_tp_level == WLAN_SVC_TP_LOW ?
		 TCP_LIMIT_OUTPUT_BYTES_LOW :
		 TCP_LIMIT_OUTPUT_BYTES_HI)))
		goto tcp_param_change_nla_failed;

	if (tcp_del_ack_ind_enabled &&
	    (nla_put_u32(
		vendor_event,
		QCA_WLAN_VENDOR_ATTR_THROUGHPUT_CHANGE_TCP_DELACK_SEG,
		(next_tp_level == WLAN_SVC_TP_LOW ?
		 TCP_DEL_ACK_LOW : TCP_DEL_ACK_HI))))
		goto tcp_param_change_nla_failed;

	if (tcp_adv_win_scl_enabled &&
	    (nla_put_u32(
		vendor_event,
		QCA_WLAN_VENDOR_ATTR_THROUGHPUT_CHANGE_TCP_ADV_WIN_SCALE,
		(next_tp_level == WLAN_SVC_TP_LOW ?
		 WIN_SCALE_LOW : WIN_SCALE_HI))))
		goto tcp_param_change_nla_failed;

	cfg80211_vendor_event(vendor_event, GFP_KERNEL);
	wlan_objmgr_pdev_release_ref(pdev, WLAN_DP_ID);
	return;

tcp_param_change_nla_failed:
	wlan_objmgr_pdev_release_ref(pdev, WLAN_DP_ID);
	dp_err("nla_put api failed");
	kfree_skb(vendor_event);
}
#else
static
void osif_dp_send_tcp_param_update_event(struct wlan_objmgr_psoc *psoc,
					 union wlan_tp_data *data,
					 uint8_t dir)
{
}
#endif /*WLAN_FEATURE_DP_BUS_BANDWIDTH*/

void os_if_dp_register_hdd_callbacks(struct wlan_objmgr_psoc *psoc,
				     struct wlan_dp_psoc_callbacks *cb_obj)
{
	cb_obj->osif_dp_send_tcp_param_update_event =
		osif_dp_send_tcp_param_update_event;

	ucfg_dp_register_hdd_callbacks(psoc, cb_obj);
}
