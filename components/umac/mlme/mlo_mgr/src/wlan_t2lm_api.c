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
