/*
 * Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
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
 * DOC: Public API implementation of pre cac called by north bound iface.
 */

#include "../../core/src/wlan_pre_cac_main.h"
#include "wlan_pre_cac_ucfg_api.h"

void ucfg_pre_cac_get_vdev_id(struct wlan_objmgr_psoc *psoc,
			      uint8_t *vdev_id)
{
	pre_cac_get_vdev_id(psoc, vdev_id);
}

int ucfg_pre_cac_validate_and_get_freq(struct wlan_objmgr_pdev *pdev,
				       uint32_t chan_freq,
				       uint32_t *pre_cac_chan_freq)
{
	return pre_cac_validate_and_get_freq(pdev, chan_freq,
					     pre_cac_chan_freq);
}

QDF_STATUS ucfg_pre_cac_set_status(struct wlan_objmgr_vdev *vdev, bool status)
{
	return pre_cac_set_status(vdev, status);
}

bool ucfg_pre_cac_is_active(struct wlan_objmgr_psoc *psoc)
{
	return pre_cac_is_active(psoc);
}

void ucfg_pre_cac_set_osif_cb(struct pre_cac_ops *osif_pre_cac_ops)
{
	pre_cac_set_osif_cb(osif_pre_cac_ops);
}

QDF_STATUS ucfg_pre_cac_init(void)
{
	return pre_cac_init();
}

void ucfg_pre_cac_deinit(void)
{
	pre_cac_deinit();
}
