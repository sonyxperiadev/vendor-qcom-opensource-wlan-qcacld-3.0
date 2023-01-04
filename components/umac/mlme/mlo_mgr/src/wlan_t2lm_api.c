/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
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
 * DOC: contains TID to Link mapping related functionality
 */
#include <wlan_cmn.h>
#include <wlan_cm_public_struct.h>
#include "wlan_t2lm_api.h"
#include <wlan_mlo_t2lm.h>

static
const char *t2lm_get_event_str(enum wlan_t2lm_evt event)
{
	if (event > WLAN_T2LM_EV_ACTION_FRAME_MAX)
		return "";

	switch (event) {
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_RX_REQ);
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_TX_RESP);
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_TX_REQ);
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_RX_RESP);
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_RX_TEARDOWN);
	CASE_RETURN_STRING(WLAN_T2LM_EV_ACTION_FRAME_TX_TEARDOWN);
	default:
		return "Unknown";
	}
}

QDF_STATUS t2lm_handle_rx_req(struct wlan_objmgr_vdev *vdev,
			      struct t2lm_event_data *event_data)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_handle_tx_resp(struct wlan_objmgr_vdev *vdev,
			       struct t2lm_event_data *event_data)
{
	if (!vdev) {
		mlme_err("vdev is NULL");
		return QDF_STATUS_E_NULL_VALUE;
	}

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_handle_tx_req(struct wlan_objmgr_vdev *vdev,
			      struct t2lm_event_data *event_data)
{
	if (!vdev) {
		mlme_err("vdev is NULL");
		return QDF_STATUS_E_NULL_VALUE;
	}

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_handle_rx_resp(struct wlan_objmgr_vdev *vdev,
			       struct t2lm_event_data *event_data)
{
	if (!vdev) {
		mlme_err("vdev is NULL");
		return QDF_STATUS_E_NULL_VALUE;
	}

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_handle_rx_teardown(struct wlan_objmgr_vdev *vdev,
				   struct t2lm_event_data *event_data)
{
	if (!vdev) {
		mlme_err("vdev is NULL");
		return QDF_STATUS_E_NULL_VALUE;
	}

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_handle_tx_teardown(struct wlan_objmgr_vdev *vdev,
				   struct t2lm_event_data *event_data)
{
	if (!vdev) {
		mlme_err("vdev is NULL");
		return QDF_STATUS_E_NULL_VALUE;
	}

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS t2lm_deliver_event(struct wlan_objmgr_vdev *vdev,
			      enum wlan_t2lm_evt event,
			      struct t2lm_event_data *event_data)
{
	struct wlan_objmgr_psoc *psoc;
	QDF_STATUS status;

	psoc = wlan_vdev_get_psoc(vdev);
	if (!psoc)
		return QDF_STATUS_E_FAILURE;

	mlme_debug("T2LM event received: %s(%d)",
		   t2lm_get_event_str(event), event);

	switch (event) {
	case WLAN_T2LM_EV_ACTION_FRAME_RX_REQ:
		status = t2lm_handle_rx_req(vdev, event_data);
		break;
	case WLAN_T2LM_EV_ACTION_FRAME_TX_RESP:
		status = t2lm_handle_tx_resp(vdev, event_data);
		break;
	case WLAN_T2LM_EV_ACTION_FRAME_TX_REQ:
		status = t2lm_handle_tx_req(vdev, event_data);
		break;
	case WLAN_T2LM_EV_ACTION_FRAME_RX_RESP:
		status = t2lm_handle_rx_resp(vdev, event_data);
		break;
	case WLAN_T2LM_EV_ACTION_FRAME_RX_TEARDOWN:
		status = t2lm_handle_rx_teardown(vdev, event_data);
		break;
	case WLAN_T2LM_EV_ACTION_FRAME_TX_TEARDOWN:
		status = t2lm_handle_tx_teardown(vdev, event_data);
		break;
	default:
		status = QDF_STATUS_E_FAILURE;
		mlme_err("Unhandled T2LM event");
	}

	return status;
}

static uint16_t
t2lm_get_tids_mapped_link_id(uint16_t link_map_tid)
{
	uint16_t all_tids_mapped_link_id = 0;
	uint8_t i;
	uint8_t bit_mask = 1;

	for (i = 0; i < WLAN_T2LM_MAX_NUM_LINKS; i++) {
		if (link_map_tid & bit_mask)
			all_tids_mapped_link_id = i;
		bit_mask = bit_mask << 1;
	}

	return all_tids_mapped_link_id;
}

static QDF_STATUS
t2lm_find_tid_mapped_link_id(struct wlan_t2lm_info *t2lm_info,
			     uint16_t *tid_mapped_link_id)
{
	uint16_t link_map_tid;
	uint8_t tid;

	if (!t2lm_info)
		return QDF_STATUS_E_NULL_VALUE;

	if (t2lm_info->default_link_mapping) {
		t2lm_debug("T2LM ie has default link mapping");
		*tid_mapped_link_id = 0xFFFF;
		return QDF_STATUS_SUCCESS;
	}

	link_map_tid = t2lm_info->ieee_link_map_tid[0];
	for (tid = 1; tid < T2LM_MAX_NUM_TIDS; tid++) {
		if (link_map_tid != t2lm_info->ieee_link_map_tid[tid]) {
			mlme_debug("all tids are not mapped to same link set");
			return QDF_STATUS_E_FAILURE;
		}
	}

	*tid_mapped_link_id = t2lm_get_tids_mapped_link_id(link_map_tid);
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
wlan_t2lm_validate_candidate(struct cnx_mgr *cm_ctx,
			     struct scan_cache_entry *scan_entry)
{
	QDF_STATUS status;
	struct wlan_objmgr_vdev *vdev;
	struct wlan_t2lm_context t2lm_ctx;
	uint8_t tid_map_link_id = 0;
	uint16_t tid_mapped_link_id[WLAN_MAX_T2LM_IE];
	uint8_t i;

	if (!scan_entry)
		return QDF_STATUS_E_NULL_VALUE;

	if (!cm_ctx || !cm_ctx->vdev)
		return QDF_STATUS_E_NULL_VALUE;

	vdev = cm_ctx->vdev;

	if (scan_entry->ie_list.t2lm[0]) {
		if (!vdev || !vdev->mlo_dev_ctx) {
			status = QDF_STATUS_E_NULL_VALUE;
			goto end;
		}

		t2lm_ctx = vdev->mlo_dev_ctx->t2lm_ctx;
		status = wlan_mlo_parse_bcn_prbresp_t2lm_ie(&t2lm_ctx,
							    scan_entry->ie_list.t2lm[0]);
		if (QDF_IS_STATUS_ERROR(status))
			goto end;

		if (!t2lm_ctx.num_of_t2lm_ie) {
			t2lm_debug("T2LM IE parsing failed");
			status = QDF_STATUS_E_FAILURE;
			goto end;
		}

		for (i = 0; i < t2lm_ctx.num_of_t2lm_ie; i++) {
			status = t2lm_find_tid_mapped_link_id(&t2lm_ctx.t2lm_ie[i].t2lm,
							      &tid_mapped_link_id[i]);
			if (QDF_IS_STATUS_ERROR(status))
				goto end;
		}

		if (t2lm_ctx.num_of_t2lm_ie == WLAN_MAX_T2LM_IE) {
			tid_map_link_id = tid_mapped_link_id[0] & tid_mapped_link_id[1];
			if (!tid_map_link_id)
				tid_map_link_id = tid_mapped_link_id[0];
		} else {
			tid_map_link_id = tid_mapped_link_id[0];
		}

		if (tid_map_link_id == scan_entry->ml_info.self_link_id) {
			t2lm_debug("self link id %d, tid map link id %d",
				   scan_entry->ml_info.self_link_id,
				   tid_map_link_id);
			status = QDF_STATUS_SUCCESS;
		}
	} else {
		t2lm_debug("T2LM IE is not present in scan entry");
		status = QDF_STATUS_SUCCESS;
	}

end:
	return status;
}
