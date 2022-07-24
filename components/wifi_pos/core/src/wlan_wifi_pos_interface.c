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
/**
 * DOC: define internal APIs related to the mlme component, legacy APIs are
 *	called for the time being, but will be cleaned up after convergence
 */
#include "wma_pasn_peer_api.h"
#include "wifi_pos_api.h"
#include "wlan_wifi_pos_interface.h"

#if defined(WIFI_POS_CONVERGED) && defined(WLAN_FEATURE_RTT_11AZ_SUPPORT)
/**
 * wlan_wifi_pos_pasn_peer_create() - Callback to create ranging peer
 * @psoc: Pointer to PSOC
 * @peer_addr: Address of the peer for which PASN peer is to be created
 * @vdev_id: Vdev id
 *
 * Return: QDF_STATUS
 */
static QDF_STATUS wlan_wifi_pos_pasn_peer_create(struct wlan_objmgr_psoc *psoc,
						 struct qdf_mac_addr *peer_addr,
						 uint8_t vdev_id)
{
	return wma_pasn_peer_create(psoc, peer_addr, vdev_id);
}

/**
 * wlan_wifi_pos_pasn_peer_create() - Callback to create ranging peer
 * @psoc: Pointer to PSOC
 * @peer_addr: Address of the peer for which PASN peer is to be created
 * @vdev_id: Vdev id
 *
 * Return: QDF_STATUS
 */
static QDF_STATUS wlan_wifi_pos_pasn_peer_delete(struct wlan_objmgr_psoc *psoc,
						 struct qdf_mac_addr *peer_addr,
						 uint8_t vdev_id,
						 bool no_fw_peer_delete)
{
	return wma_pasn_peer_remove(psoc, peer_addr, vdev_id,
				    no_fw_peer_delete);
}

/**
 * wlan_wifi_pos_vdev_delete_resume() - Resume vdev delete operation
 * after deleting all pasn peers
 * @vdev: Pointer to objmgr vdev
 *
 * Return: QDF_STATUS
 */
static QDF_STATUS
wlan_wifi_pos_vdev_delete_resume(struct wlan_objmgr_vdev *vdev)
{
	return QDF_STATUS_SUCCESS;
}

static struct wifi_pos_legacy_ops wifi_pos_ops = {
	.pasn_peer_create_cb = wlan_wifi_pos_pasn_peer_create,
	.pasn_peer_delete_cb = wlan_wifi_pos_pasn_peer_delete,
	.pasn_vdev_delete_resume_cb = wlan_wifi_pos_vdev_delete_resume,
};

QDF_STATUS
wifi_pos_register_legacy_ops(struct wlan_objmgr_psoc *psoc)
{
	return wifi_pos_set_legacy_ops(psoc, &wifi_pos_ops);
}

QDF_STATUS
wifi_pos_deregister_legacy_ops(struct wlan_objmgr_psoc *psoc)
{
	return wifi_pos_set_legacy_ops(psoc, NULL);
}
#endif
