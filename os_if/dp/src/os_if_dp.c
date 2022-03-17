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
#include "osif_vdev_sync.h"
#include "osif_sync.h"
#include <net/netevent.h>

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

/**
 * osif_dp_get_net_dev_from_vdev() - Get netdev object from vdev
 * @vdev: Pointer to vdev manager
 * @out_net_dev: Pointer to output netdev
 *
 * Return: 0 on success, error code on failure
 */
static int osif_dp_get_net_dev_from_vdev(struct wlan_objmgr_vdev *vdev,
					 struct net_device **out_net_dev)
{
	struct vdev_osif_priv *priv;

	if (!vdev)
		return -EINVAL;

	priv = wlan_vdev_get_ospriv(vdev);
	if (!priv || !priv->wdev || !priv->wdev->netdev)
		return -EINVAL;

	*out_net_dev = priv->wdev->netdev;

	return 0;
}

/**
 * osif_dp_process_sta_mic_error() - Indicate STA mic error to supplicant
 * @info: MIC error information
 * @vdev: vdev handle
 *
 * Return: None
 */
static void
osif_dp_process_sta_mic_error(struct dp_mic_error_info *info,
			      struct wlan_objmgr_vdev *vdev)
{
	struct net_device *dev;
	struct osif_vdev_sync *vdev_sync;
	int errno;

	errno = osif_dp_get_net_dev_from_vdev(vdev, &dev);
	if (errno) {
		dp_err("failed to get netdev");
		return;
	}
	if (osif_vdev_sync_op_start(dev, &vdev_sync))
		return;

	/* inform mic failure to nl80211 */
	cfg80211_michael_mic_failure(dev,
				     (uint8_t *)&info->ta_mac_addr,
				     info->multicast ?
				     NL80211_KEYTYPE_GROUP :
				     NL80211_KEYTYPE_PAIRWISE,
				     info->key_id,
				     info->tsc,
				     GFP_KERNEL);
}

/**
 * osif_dp_process_sap_mic_error() - Indicate SAP mic error to supplicant
 * @info: MIC error information
 * @vdev: vdev handle
 *
 * Return: None
 */
static void
osif_dp_process_sap_mic_error(struct dp_mic_error_info *info,
			      struct wlan_objmgr_vdev *vdev)
{
	struct net_device *dev;
	int errno;
	struct osif_vdev_sync *vdev_sync;

	errno = osif_dp_get_net_dev_from_vdev(vdev, &dev);
	if (errno) {
		dp_err("failed to get netdev");
		return;
	}
	if (osif_vdev_sync_op_start(dev, &vdev_sync))
		return;

	/* inform mic failure to nl80211 */
	cfg80211_michael_mic_failure(dev,
				     (uint8_t *)&info->ta_mac_addr,
				     info->multicast ?
				     NL80211_KEYTYPE_GROUP :
				     NL80211_KEYTYPE_PAIRWISE,
				     info->key_id,
				     info->tsc,
				     GFP_KERNEL);
}

#ifdef WLAN_NUD_TRACKING
/**
 * nud_state_osif_to_dp() - convert os_if to enum
 * @curr_state: Current NUD state
 *
 * Return: DP enum equivalent to NUD state
 */
static inline enum dp_nud_state nud_state_osif_to_dp(uint8_t curr_state)
{
	switch (curr_state) {
	case NUD_NONE:
		return DP_NUD_NONE;
	case NUD_INCOMPLETE:
		return DP_NUD_INCOMPLETE;
	case NUD_REACHABLE:
		return DP_NUD_REACHABLE;
	case NUD_STALE:
		return DP_NUD_STALE;
	case NUD_DELAY:
		return DP_NUD_DELAY;
	case NUD_PROBE:
		return DP_NUD_PROBE;
	case NUD_FAILED:
		return DP_NUD_FAILED;
	case NUD_NOARP:
		return DP_NUD_NOARP;
	case NUD_PERMANENT:
		return DP_NUD_PERMANENT;
	default:
		return DP_NUD_STATE_INVALID;
	}
}

/**
 * os_if_dp_nud_stats_info() - print NUD stats info
 * @vdev: vdev handle
 *
 * Return: None
 */
static void os_if_dp_nud_stats_info(struct wlan_objmgr_vdev *vdev)
{
	struct netdev_queue *txq;
	struct net_device *net_dev;
	int i = 0, errno;

	errno = osif_dp_get_net_dev_from_vdev(vdev, &net_dev);
	if (errno) {
		dp_err("failed to get netdev");
		return;
	}
	dp_info("carrier state: %d", netif_carrier_ok(net_dev));

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		txq = netdev_get_tx_queue(net_dev, i);
		dp_info("Queue: %d status: %d txq->trans_start: %lu",
			i, netif_tx_queue_stopped(txq), txq->trans_start);
	}
}

/**
 * os_if_dp_nud_netevent_cb() - netevent callback
 * @nb: Pointer to notifier block
 * @event: Net Event triggered
 * @data: Pointer to neighbour struct
 *
 * Callback for netevent
 *
 * Return: 0 on success
 */
static int os_if_dp_nud_netevent_cb(struct notifier_block *nb,
				    unsigned long event,
				    void *data)
{
	struct neighbour *neighbor = data;
	struct osif_vdev_sync *vdev_sync;
	const struct net_device *netdev = neighbor->dev;
	int errno;

	errno = osif_vdev_sync_op_start(neighbor->dev, &vdev_sync);
	if (errno)
		return errno;

	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		ucfg_dp_nud_event((struct qdf_mac_addr *)netdev->dev_addr,
				  (struct qdf_mac_addr *)&neighbor->ha[0],
				  nud_state_osif_to_dp(neighbor->nud_state));
		break;
	default:
		break;
	}

	osif_vdev_sync_op_stop(vdev_sync);

	return 0;
}

static struct notifier_block wlan_netevent_nb = {
	.notifier_call = os_if_dp_nud_netevent_cb
};

int osif_dp_nud_register_netevent_notifier(struct wlan_objmgr_psoc *psoc)
{
	int ret = 0;

	if (ucfg_dp_nud_tracking_enabled(psoc)) {
		ret = register_netevent_notifier(&wlan_netevent_nb);
		if (!ret)
			dp_info("Registered netevent notifier");
	}
	return ret;
}

void osif_dp_nud_unregister_netevent_notifier(struct wlan_objmgr_psoc *psoc)
{
	int ret = 0;

	if (ucfg_dp_nud_tracking_enabled(psoc)) {
		ret = unregister_netevent_notifier(&wlan_netevent_nb);
		if (!ret)
			dp_info("Unregistered netevent notifier");
	}
}
#else
static void os_if_dp_nud_stats_info(struct wlan_objmgr_vdev *vdev)
{
}

int osif_dp_nud_register_netevent_notifier(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

void osif_dp_nud_unregister_netevent_notifier(struct wlan_objmgr_psoc *psoc)
{
}
#endif
void os_if_dp_register_hdd_callbacks(struct wlan_objmgr_psoc *psoc,
				     struct wlan_dp_psoc_callbacks *cb_obj)
{
	cb_obj->osif_dp_send_tcp_param_update_event =
		osif_dp_send_tcp_param_update_event;
	cb_obj->os_if_dp_nud_stats_info = os_if_dp_nud_stats_info;
	cb_obj->osif_dp_process_sta_mic_error = osif_dp_process_sta_mic_error;
	cb_obj->osif_dp_process_sap_mic_error = osif_dp_process_sap_mic_error;

	ucfg_dp_register_hdd_callbacks(psoc, cb_obj);
}
