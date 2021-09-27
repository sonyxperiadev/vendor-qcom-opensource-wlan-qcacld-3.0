/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * DOC: wlan_hdd_connectivity_logging.c
 *
 * Implementation for the Common connectivity and roam logging api.
 */

#include "wlan_hdd_connectivity_logging.h"

#define GET_ATTR_OFFSET(member) \
	qdf_offsetof(struct wlan_log_record, member)

#define ATTR_GET_VALUE(type, record, field_offset) \
	(*(type *)((uint8_t *)record + field_offset))

/**
 * struct connectivity_log_attr  - Connectivity logging attribute info
 * @attribute_id: Vendor attribute ID
 * @attribute_type: NL type of the attribute
 * @attribute_length: Length of the attribute
 * @field_offset: Field offset
 */
struct connectivity_log_attr {
	uint8_t attribute_id;
	uint8_t attribute_type;
	uint16_t attribute_length;
	uint16_t field_offset;
};

static const enum qca_vendor_attr_diag_event_type
hdd_connectivity_log_attr_table[WLAN_TAG_MAX] = {
	[WLAN_CONNECTING] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING,
	[WLAN_CONNECTING_FAIL] =
		QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING_FAIL,
	[WLAN_AUTH_REQ] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_AUTH_REQ,
	[WLAN_AUTH_RESP] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_AUTH_RESP,
	[WLAN_ASSOC_REQ] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ASSOC_REQ,
	[WLAN_ASSOC_RSP] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ASSOC_RESP,
	[WLAN_DEAUTH_RX] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DEAUTH_RX,
	[WLAN_DEAUTH_TX] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DEAUTH_TX,
	[WLAN_DISASSOC_RX] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DISASSOC_RX,
	[WLAN_DISASSOC_TX] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DISASSOC_TX,
	[WLAN_DISCONN_BMISS] =
			QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BEACON_LOSS_DISCONN,
	[WLAN_ROAM_SCAN_START] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_START,
	[WLAN_ROAM_SCAN_DONE] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_DONE,
	[WLAN_ROAM_SCORE_CURR_AP] =
		QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCORE_CURR_AP,
	[WLAN_ROAM_SCORE_CAND_AP] =
		QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCORE_CAND_AP,
	[WLAN_ROAM_RESULT] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_RESULT,
	[WLAN_ROAM_CANCEL] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_CANCEL,
	[WLAN_BTM_REQ] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_REQ,
	[WLAN_BTM_QUERY] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_QUERY,
	[WLAN_BTM_RESP] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_RESP,
	[WLAN_BTM_REQ_CANDI] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_REQ_CANDI,
	[WLAN_ROAM_WTC] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_WTC,
	[WLAN_DHCP_DISCOVER] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_DISCOVER,
	[WLAN_DHCP_OFFER] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_OFFER,
	[WLAN_DHCP_REQUEST] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_REQUEST,
	[WLAN_DHCP_ACK] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_ACK,
	[WLAN_DHCP_NACK] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_NACK,
	[WLAN_EAPOL_M1] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M1,
	[WLAN_EAPOL_M2] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M2,
	[WLAN_EAPOL_M3] =  QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M3,
	[WLAN_EAPOL_M4] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M4,
	[WLAN_GTK_M1] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_GTK_M1,
	[WLAN_GTK_M2] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_GTK_M2,
	[WLAN_EAP_REQUEST] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_REQUEST,
	[WLAN_EAP_RESPONSE] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_RESPONSE,
	[WLAN_EAP_SUCCESS] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_SUCCESS,
	[WLAN_EAP_FAILURE] = QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_FAILURE,
};

static const struct connectivity_log_attr
connectivity_attr_table[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_MAX + 1]
			[QCA_WLAN_VENDOR_ATTR_DIAG_MAX] = {
	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,    sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP, NLA_U64, sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SSID, NLA_BINARY,
	 GET_ATTR_OFFSET(conn_info.ssid_len), GET_ATTR_OFFSET(conn_info.ssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID_HINT, NLA_BINARY, sizeof(uint8_t),
	 GET_ATTR_OFFSET(conn_info.bssid_hint)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FREQ,       NLA_U16,    sizeof(uint16_t),
	 GET_ATTR_OFFSET(conn_info.freq)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FREQ_HINT,  NLA_U16,    sizeof(uint16_t),
	 GET_ATTR_OFFSET(conn_info.freq_hint)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_PAIRWISE_SUITE,   NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(conn_info.pairwise)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_AKM,              NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(conn_info.akm)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_GROUP_SUITE,      NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(conn_info.group)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_GROUP_MGMT_SUITE, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(conn_info.group_mgmt)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_AUTH_ALGO,        NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(conn_info.auth_type)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BT_COEX_ACTIVE,   NLA_FLAG, sizeof(uint8_t),
	 GET_ATTR_OFFSET(conn_info.is_bt_coex_active)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING_FAIL] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64, sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,      NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FREQ,       NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(conn_info.freq)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_AUTH_REQ] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,        NLA_S32,  sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_AUTH_ALGO,   NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.auth_algo)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SAE_AUTH_FRAME_TYPE, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.auth_type)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER, NLA_U16,  sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS,   NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IS_RETRY_FRAME, NLA_FLAG, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.is_retry_frame)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_AUTH_RESP] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64, sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,      NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_AUTH_ALGO,  NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.auth_algo)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SAE_AUTH_FRAME_TYPE, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.auth_type)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ASSOC_REQ] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,        NLA_S32,  sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS,   NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IS_RETRY_FRAME,    NLA_FLAG, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.is_retry_frame)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ASSOC_RESP] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DEAUTH_RX] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,            NLA_S32,  sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DEAUTH_TX] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,              NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DISASSOC_RX] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX, NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID, NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI, NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DISASSOC_TX] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP, NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX, NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID, NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI, NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_SEQUENCE_NUMBER,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(pkt_info.seq_num)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.frame_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BEACON_LOSS_DISCONN] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,       NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,        NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(pkt_info.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_REASON_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(conn_info.conn_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_START] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_TRIGGER_REASON,  NLA_U8,
	 sizeof(uint8_t), GET_ATTR_OFFSET(roam_trig.trigger_reason)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_TRIGGER_SUB_REASON,  NLA_U8,
	 sizeof(uint8_t), GET_ATTR_OFFSET(roam_trig.trigger_sub_reason)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,     NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(roam_trig.current_rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_CU,  NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(roam_trig.cu_load)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_SCAN_TYPE, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(roam_trig.is_full_scan)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_RSSI_THRESHOLD, NLA_U32,
	 sizeof(uint8_t), GET_ATTR_OFFSET(roam_trig.rssi_threshold)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_DONE] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_AP_COUNT,  NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(roam_scan.cand_ap_count)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_FREQ_LIST, NLA_NESTED, sizeof(uint16_t),
	 GET_ATTR_OFFSET(roam_scan.scan_freq)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCORE_CURR_AP] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(ap.cand_bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FREQ,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(ap.freq)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,   NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(ap.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_CU, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(ap.cu_load)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_SCORE,  NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(ap.total_score)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCORE_CAND_AP] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_AP_RANK, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(ap.idx)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_CU, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(ap.cu_load)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(ap.cand_bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FREQ,   NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(ap.freq)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_RSSI,   NLA_S32, sizeof(int32_t),
	 GET_ATTR_OFFSET(ap.rssi)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_SCORE,  NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(ap.total_score)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_AP_ETP, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(ap.etp)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_RESULT] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_CANCEL] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_QUERY] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_TOKEN,  NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(btm_info.token)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_QUERY_REASON, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_info.reason)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_REQ] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64, sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_TOKEN,  NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(btm_info.token)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_MODE,   NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_info.mode)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_DISASSOC_TIMER, NLA_U32,
	sizeof(uint32_t), GET_ATTR_OFFSET(btm_info.disassoc_timer)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_VALIDITY_INTERVAL, NLA_U32,
	 sizeof(uint32_t), GET_ATTR_OFFSET(btm_info.disassoc_timer)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_CANDIDATE_LIST_COUNT, NLA_U8,
	 sizeof(uint8_t), GET_ATTR_OFFSET(btm_info.candidate_list_count)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_WTC] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_REASON_CODE, NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_info.reason)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_WTC_SUB_REASON_CODE, NLA_U8,
	 sizeof(uint8_t), GET_ATTR_OFFSET(btm_info.sub_reason)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_WTC_DURATION, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(btm_info.wtc_duration)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_REQ_CANDI] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,  NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,   NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,     NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(btm_cand.bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_PREFERENCE, NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_cand.preference)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_BTM_RESP] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32, sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64, sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_TOKEN,  NLA_U16, sizeof(uint16_t),
	 GET_ATTR_OFFSET(btm_info.token)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_STATUS_CODE, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_info.btm_status_code)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BTM_DELAY, NLA_U8,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(btm_info.btm_delay)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_BSSID,  NLA_BINARY, QDF_MAC_ADDR_SIZE,
	 GET_ATTR_OFFSET(btm_info.target_bssid)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_DISCOVER] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_OFFER] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_REQUEST] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_ACK] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_DHCP_NACK] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M1] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M2] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS,   NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M3] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAPOL_M4] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_GTK_M1] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_GTK_M2] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP, NLA_U64,
	 sizeof(uint64_t), GET_ATTR_OFFSET(fw_timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_REQUEST] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_EAP_TYPE,   NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.eap_type)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_EAP_LEN,    NLA_U16,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.eap_len)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_RESPONSE] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_EAP_TYPE,   NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.eap_type)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_EAP_LEN,    NLA_U16,  sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.eap_len)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS, NLA_U8, sizeof(uint8_t),
	 GET_ATTR_OFFSET(pkt_info.tx_status)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_SUCCESS] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},

	[QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_EAP_FAILURE] = {
	{QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE, NLA_U32,  sizeof(uint32_t),
	 GET_ATTR_OFFSET(log_subtype)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_HOST_TIMESTAMP,  NLA_U64,  sizeof(uint64_t),
	 GET_ATTR_OFFSET(timestamp_us)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_IFINDEX,    NLA_U8,   sizeof(uint8_t),
	 GET_ATTR_OFFSET(vdev_id)},
	{QCA_WLAN_VENDOR_ATTR_DIAG_MAX,   0, 0, 0},
	},
};

static enum qca_roam_reason
wlan_hdd_convert_roam_reason(enum roam_trigger_reason roam_reason)
{
	switch (roam_reason) {
	case ROAM_TRIGGER_REASON_PER:
		return QCA_ROAM_REASON_PER;
	case ROAM_TRIGGER_REASON_BMISS:
		return QCA_ROAM_REASON_BEACON_MISS;
	case ROAM_TRIGGER_REASON_LOW_RSSI:
		return QCA_ROAM_REASON_POOR_RSSI;
	case ROAM_TRIGGER_REASON_HIGH_RSSI:
		return QCA_ROAM_REASON_BETTER_RSSI;
	case ROAM_TRIGGER_REASON_PERIODIC:
		return QCA_ROAM_REASON_PERIODIC_TIMER;
	case ROAM_TRIGGER_REASON_DENSE:
		return QCA_ROAM_REASON_CONGESTION;
	case ROAM_TRIGGER_REASON_BACKGROUND:
		return QCA_ROAM_REASON_BACKGROUND_SCAN;
	case ROAM_TRIGGER_REASON_FORCED:
		return QCA_ROAM_REASON_USER_TRIGGER;
	case ROAM_TRIGGER_REASON_BTM:
		return QCA_ROAM_REASON_BTM;
	case ROAM_TRIGGER_REASON_BSS_LOAD:
		return QCA_ROAM_REASON_BSS_LOAD;
	case ROAM_TRIGGER_REASON_DEAUTH:
		return QCA_ROAM_REASON_DISCONNECTION;
	case ROAM_TRIGGER_REASON_IDLE:
		return QCA_ROAM_REASON_IDLE;
	case ROAM_TRIGGER_REASON_WTC_BTM:
		return QCA_ROAM_REASON_WTC;
	case ROAM_TRIGGER_REASON_BTC:
		return QCA_ROAM_REASON_BT_ACTIVITY;
	default:
		break;
	}

	return QCA_ROAM_REASON_UNKNOWN;
}

static enum qca_roam_sub_reason
wlan_hdd_get_converted_roam_sub_reason(enum roam_trigger_sub_reason sub_reason)
{
	switch (sub_reason) {
	case ROAM_TRIGGER_SUB_REASON_PERIODIC_TIMER:
		return QCA_ROAM_SUB_REASON_PERIODIC_TIMER;

	case ROAM_TRIGGER_SUB_REASON_INACTIVITY_TIMER_LOW_RSSI:
		return QCA_ROAM_SUB_REASON_INACTIVITY_TIMER_LOW_RSSI;

	case ROAM_TRIGGER_SUB_REASON_BTM_DI_TIMER:
		return QCA_ROAM_SUB_REASON_BTM_DI_TIMER;

	case ROAM_TRIGGER_SUB_REASON_FULL_SCAN:
		return QCA_ROAM_SUB_REASON_FULL_SCAN;

	case ROAM_TRIGGER_SUB_REASON_LOW_RSSI_PERIODIC:
		return QCA_ROAM_SUB_REASON_LOW_RSSI_PERIODIC;

	case ROAM_TRIGGER_SUB_REASON_CU_PERIODIC:
		return QCA_ROAM_SUB_REASON_CU_PERIODIC;

	case ROAM_TRIGGER_SUB_REASON_PERIODIC_TIMER_AFTER_INACTIVITY:
		return QCA_ROAM_SUB_REASON_PERIODIC_TIMER_AFTER_INACTIVITY_LOW_RSSI;

	case ROAM_TRIGGER_SUB_REASON_PERIODIC_TIMER_AFTER_INACTIVITY_CU:
		return QCA_ROAM_SUB_REASON_PERIODIC_TIMER_AFTER_INACTIVITY_CU;

	case ROAM_TRIGGER_SUB_REASON_INACTIVITY_TIMER_CU:
		return QCA_ROAM_SUB_REASON_INACTIVITY_TIMER_CU;

	default:
		break;
	}

	return 0;
}

static enum qca_wlan_vendor_tx_status
wlan_hdd_get_converted_tx_status(uint8_t tx_status)
{
	switch (tx_status) {
	case QDF_TX_RX_STATUS_FW_DISCARD:
	case QDF_TX_RX_STATUS_INVALID:
	case QDF_TX_RX_STATUS_DROP:
	case QDF_TX_RX_STATUS_DOWNLOAD_SUCC:
	case QDF_TX_RX_STATUS_DEFAULT:
	default:
		return QCA_WLAN_VENDOR_TX_STATUS_FAIL;
	case QDF_TX_RX_STATUS_NO_ACK:
		return QCA_WLAN_VENDOR_TX_STATUS_NO_ACK;
	case QDF_TX_RX_STATUS_OK:
		return QCA_WLAN_VENDOR_TX_STATUS_ACK;
	}

	return QCA_WLAN_VENDOR_TX_STATUS_FAIL;
}

static uint16_t
wlan_hdd_get_connectivity_log_tlv_len(struct wlan_log_record *rec)
{
	uint16_t j, attr_id, log_evt_type, len = 0;

	log_evt_type = hdd_connectivity_log_attr_table[rec->log_subtype];
	if (log_evt_type >= (QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_MAX + 1))
			return 0;

	for (j = 0; j < QCA_WLAN_VENDOR_ATTR_DIAG_MAX; j++) {
		attr_id =
			connectivity_attr_table[log_evt_type][j].attribute_id;
		if (attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_MAX)
			break;

		if (log_evt_type ==
		    QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FREQ &&
		    !rec->conn_info.freq)
			continue;

		if (log_evt_type ==
		    QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FREQ_HINT &&
		    !rec->conn_info.freq_hint)
			continue;

		if (attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP &&
		    !rec->fw_timestamp_us)
			continue;

		if (log_evt_type ==
		    QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_DONE &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_FREQ_LIST) {
			len += NLMSG_HDRLEN +
			       (rec->roam_scan.num_scanned_freq *
				sizeof(rec->roam_scan.scan_freq[0]));
		} else {
			len += connectivity_attr_table[log_evt_type][j].attribute_length;
		}
	}

	return len;
}

static uint16_t
wlan_hdd_get_connectivity_log_event_len(struct wlan_log_record *rec,
					uint8_t num_records)
{
	uint16_t len = 0;
	uint8_t i;

	for (i = 0; i < num_records; i++)
		len += wlan_hdd_get_connectivity_log_tlv_len(&rec[i]);

	return len;
}

static QDF_STATUS
wlan_hdd_put_connectivity_logging_data(struct sk_buff *skb,
				       struct wlan_log_record *rec,
				       struct connectivity_log_attr *attribute)
{
	int errno = 0;
	struct qdf_mac_addr bssid = {0};
	enum  qca_vendor_attr_diag_event_type attribute_id =
						attribute->attribute_id;
	uint16_t field_offset = attribute->field_offset;

	switch (attribute_id) {
	case QCA_WLAN_VENDOR_ATTR_DIAG_SSID:
		errno = nla_put(skb, attribute_id, rec->conn_info.ssid_len,
				rec->conn_info.ssid);

		break;
	case QCA_WLAN_VENDOR_ATTR_DIAG_BSSID:
		bssid = ATTR_GET_VALUE(struct qdf_mac_addr, rec, field_offset);
		if (qdf_is_macaddr_zero(&bssid) ||
		    qdf_is_macaddr_broadcast(&bssid))
			break;

		nla_put(skb, attribute_id, QDF_MAC_ADDR_SIZE,
			bssid.bytes);
		break;
	case QCA_WLAN_VENDOR_ATTR_DIAG_BSSID_HINT:
		if (qdf_is_macaddr_zero(&rec->bssid) ||
		    qdf_is_macaddr_broadcast(&rec->bssid))
			break;

		nla_put(skb, attribute_id, QDF_MAC_ADDR_SIZE,
			rec->conn_info.bssid_hint.bytes);
		break;
	default:
		break;
	}

	if (errno)
		return QDF_STATUS_E_FAILURE;

	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS
wlan_hdd_fill_roam_scan_frequencies(struct sk_buff *skb,
				    struct wlan_log_record *rec)
{
	int errno;
	struct nlattr *attr, *attr1;
	uint8_t i;
	QDF_STATUS status = QDF_STATUS_SUCCESS;

	attr = nla_nest_start(
			skb, QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_FREQ_LIST);
	if (!attr)
		return QDF_STATUS_E_FAILURE;

	for (i = 0; i < rec->roam_scan.num_scanned_freq; i++) {
		attr1 = nla_nest_start(skb, i);
		if (!attr1) {
			status = QDF_STATUS_E_FAILURE;
			break;
		}

		errno = nla_put_u16(skb, QCA_WLAN_VENDOR_ATTR_DIAG_FREQ,
				    rec->roam_scan.scan_freq[i]);
		if (errno) {
			status = qdf_status_from_os_return(errno);
			break;
		}

		nla_nest_end(skb, attr1);
	}

	nla_nest_end(skb, attr);

	return status;
}

static QDF_STATUS
wlan_hdd_fill_connectivity_logging_data(struct sk_buff *skb,
					struct wlan_log_record *rec)
{
	uint8_t i, val8, attr_val8;
	uint16_t attr_id;
	enum qca_vendor_attr_diag_event_type evt_type;
	struct connectivity_log_attr attr;
	QDF_STATUS status;
	int errno;

	/*
	 * Continue to the next event if the Event type is invalid
	 */
	evt_type = hdd_connectivity_log_attr_table[rec->log_subtype];
	if (evt_type >= (QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_MAX + 1))
		return QDF_STATUS_SUCCESS;

	errno = nla_put_u32(skb, QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE,
			    evt_type);
	if (errno)
		return QDF_STATUS_E_FAILURE;

	/*
	 * zeroth index in the attribute table is always
	 * QCA_WLAN_VENDOR_ATTR_DIAG_EVENT_TYPE. So
	 * start the loop from first index.
	 */
	for (i = 1; i < QCA_WLAN_VENDOR_ATTR_DIAG_MAX; i++) {
		attr = connectivity_attr_table[evt_type][i];
		attr_id = attr.attribute_id;
		if (attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_MAX)
			break;

		if (evt_type == QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FREQ &&
		    !rec->conn_info.freq)
			continue;

		if (evt_type == QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_CONNECTING &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FREQ_HINT &&
		    !rec->conn_info.freq_hint)
			continue;

		if (attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_FIRMWARE_TIMESTAMP &&
		    !rec->fw_timestamp_us)
			continue;

		if (evt_type ==
		    QCA_WLAN_VENDOR_DIAG_EVENT_TYPE_ROAM_SCAN_DONE &&
		    attr_id == QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_FREQ_LIST) {
			status = wlan_hdd_fill_roam_scan_frequencies(skb, rec);
			if (QDF_IS_STATUS_ERROR(status))
				continue;
		}

		switch (attr.attribute_type) {
		case NLA_U8:
			val8 = ATTR_GET_VALUE(uint8_t, rec, attr.field_offset);
			attr_val8 = val8;
			if (attr.attribute_id ==
			    QCA_WLAN_VENDOR_ATTR_DIAG_BTM_WTC_SUB_REASON_CODE)
				attr_val8 = wlan_hdd_get_converted_roam_sub_reason(val8);
			else if (attr.attribute_id ==
				 QCA_WLAN_VENDOR_ATTR_DIAG_FRAME_TX_STATUS)
				attr_val8 = wlan_hdd_get_converted_tx_status(val8);
			else if (attr.attribute_id ==
				 QCA_WLAN_VENDOR_ATTR_DIAG_ROAM_TRIGGER_REASON)
				attr_val8 = wlan_hdd_convert_roam_reason(val8);

			errno = nla_put_u8(skb, attr.attribute_id, attr_val8);
			if (errno)
				return QDF_STATUS_E_FAILURE;
			break;
		case NLA_U16:
			errno = nla_put_u16(skb, attr.attribute_id,
					    ATTR_GET_VALUE(uint16_t, rec,
							   attr.field_offset));
			if (errno)
				return QDF_STATUS_E_FAILURE;
			break;
		case NLA_U32:
			errno = nla_put_u32(skb, attr.attribute_id,
					    ATTR_GET_VALUE(uint32_t, rec,
							   attr.field_offset));
			if (errno)
				return QDF_STATUS_E_FAILURE;
			break;
		case NLA_S32:
			errno = nla_put_s32(skb, attr.attribute_id,
					    ATTR_GET_VALUE(int32_t, rec,
							   attr.field_offset));
			if (errno)
				return QDF_STATUS_E_FAILURE;
			break;
		case NLA_U64:
			errno = wlan_cfg80211_nla_put_u64(
			    skb, attr.attribute_id,
			    ATTR_GET_VALUE(uint64_t, rec, attr.field_offset));
			if (errno)
				return QDF_STATUS_E_FAILURE;
			break;
		case NLA_FLAG:
			if (ATTR_GET_VALUE(bool, rec, attr.field_offset))
				nla_put_flag(skb, attr.attribute_id);
			break;
		case NLA_BINARY:
			status = wlan_hdd_put_connectivity_logging_data(
						skb, rec, &attr);
			if (QDF_IS_STATUS_ERROR(status))
				return status;
			break;
		default:
			break;
		}
	}

	return QDF_STATUS_SUCCESS;
}

/**
 * wlan_hdd_send_connectivity_log_to_user  - Send the connectivity log buffer
 * to userspace
 * @rec: Pointer to the log record
 * @num_records: Number of records
 *
 * Return: QDF_STATUS
 */
static QDF_STATUS
wlan_hdd_send_connectivity_log_to_user(struct wlan_log_record *rec,
				       void *hdd_context,
				       uint8_t num_records)
{
	struct hdd_context *hdd_ctx;
	struct nlattr *attr, *attr1;
	struct sk_buff *vendor_event;
	uint16_t len, i = 0;
	QDF_STATUS status;

	hdd_enter();

	hdd_ctx = hdd_context;
	if (wlan_hdd_validate_context(hdd_ctx))
		return QDF_STATUS_E_FAILURE;

	len = wlan_hdd_get_connectivity_log_event_len(rec, num_records);

	vendor_event = cfg80211_vendor_event_alloc(
			hdd_ctx->wiphy, NULL, len + NLMSG_HDRLEN,
			QCA_NL80211_VENDOR_SUBCMD_DIAG_EVENT_INDEX,
			GFP_ATOMIC);
	if (!vendor_event) {
		hdd_err("cfg80211_vendor_event_alloc failed");
		return QDF_STATUS_E_NOMEM;
	}

	attr = nla_nest_start(vendor_event,
			      QCA_WLAN_VENDOR_ATTR_DIAG_EVENT);
	if (!attr)
		goto failure;

	for (i = 0; i < num_records; i++) {
		attr1 = nla_nest_start(vendor_event, i);
		if (!attr1)
			goto failure;

		status = wlan_hdd_fill_connectivity_logging_data(vendor_event,
								 &rec[i]);
		if (QDF_IS_STATUS_ERROR(status))
			goto failure;

		nla_nest_end(vendor_event, attr1);
	}

	nla_nest_end(vendor_event, attr);
	cfg80211_vendor_event(vendor_event, GFP_ATOMIC);

	hdd_exit();

	return QDF_STATUS_SUCCESS;
failure:
	hdd_err("NLA fill failed");
	kfree_skb(vendor_event);

	return QDF_STATUS_E_FAILURE;
}

void wlan_hdd_start_connectivity_logging(struct hdd_context *hdd_ctx)
{
	struct wlan_cl_osif_cbks hdd_cb;

	hdd_cb.wlan_connectivity_log_send_to_usr =
			wlan_hdd_send_connectivity_log_to_user;
	wlan_connectivity_logging_start(&hdd_cb, hdd_ctx);
}

void wlan_hdd_connectivity_event_connecting(struct hdd_context *hdd_ctx,
					    struct cfg80211_connect_params *req,
					    uint8_t vdev_id)
{
	struct wlan_log_record *record;

	record = qdf_mem_malloc(sizeof(*record));
	if (!record)
		return;

	record->timestamp_us = qdf_get_time_of_the_day_us();
	record->vdev_id = vdev_id;

	record->conn_info.ssid_len = req->ssid_len;
	if (req->ssid_len > WLAN_SSID_MAX_LEN)
		record->conn_info.ssid_len = WLAN_SSID_MAX_LEN;
	qdf_mem_copy(record->conn_info.ssid, req->ssid, req->ssid_len);

	if (req->bssid)
		qdf_mem_copy(record->bssid.bytes, req->bssid,
			     QDF_MAC_ADDR_SIZE);
	else if (req->bssid_hint)
		qdf_mem_copy(record->conn_info.bssid_hint.bytes,
			     req->bssid_hint,
			     QDF_MAC_ADDR_SIZE);

	if (req->channel)
		record->conn_info.freq = req->channel->center_freq;

	if (req->channel_hint)
		record->conn_info.freq_hint = req->channel_hint->center_freq;

	record->conn_info.pairwise = req->crypto.ciphers_pairwise[0];
	record->conn_info.group = req->crypto.cipher_group;
	record->conn_info.akm = req->crypto.akm_suites[0];
	record->conn_info.auth_type = req->auth_type;
	if (hdd_ctx->bt_a2dp_active || hdd_ctx->bt_vo_active)
		record->conn_info.is_bt_coex_active = true;

	wlan_connectivity_log_enqueue(record);

	qdf_mem_free(record);
}

void
wlan_hdd_connectivity_fail_event(struct wlan_objmgr_vdev *vdev,
				 struct wlan_cm_connect_resp *rsp)
{
	uint8_t vdev_id;
	struct wlan_log_record *log;
	enum QDF_OPMODE op_mode;

	/* Send the event only for failure reason, else return */
	if (!rsp->reason)
		return;

	vdev_id = wlan_vdev_get_id(vdev);
	op_mode = wlan_vdev_mlme_get_opmode(vdev);
	if (op_mode != QDF_STA_MODE)
		return;

	log = qdf_mem_malloc(sizeof(*log));
	if (!log)
		return;

	log->timestamp_us = qdf_get_time_of_the_day_us();
	log->vdev_id = vdev_id;
	log->bssid = rsp->bssid;
	log->conn_info.freq = rsp->freq;
	log->conn_info.conn_status = rsp->reason;

	wlan_connectivity_log_enqueue(log);
	qdf_mem_free(log);
}