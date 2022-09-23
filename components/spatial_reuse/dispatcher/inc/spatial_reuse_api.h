/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * DOC : contains interface prototypes for spatial_reuse api
 */

#ifndef _SPATIAL_REUSE_API_H_
#define _SPATIAL_REUSE_API_H_

#include <qdf_types.h>
#include <qdf_trace.h>
#include <wlan_objmgr_vdev_obj.h>

/**
 * wlan_spatial_reuse_config_set() - Set spatial reuse config
 * @vdev: objmgr manager vdev
 * @sr_ctrl: spatial reuse control
 * @non_srg_max_pd_offset: non-srg max pd offset
 *
 * Return: QDF_STATUS
 */
QDF_STATUS wlan_spatial_reuse_config_set(struct wlan_objmgr_vdev *vdev,
					 uint8_t sr_ctrl,
					 uint8_t non_srg_max_pd_offset);

/**
 * wlan_spatial_reuse_he_siga_val15_allowed_set() - Set spatial reuse config
 *						    he_siga_val15_allowed
 * @vdev: objmgr manager vdev
 * @he_siga_va15_allowed: enable/disable he_siga_val15_allowed
 *
 * Return: QDF_STATUS
 */
QDF_STATUS wlan_spatial_reuse_he_siga_val15_allowed_set(
					struct wlan_objmgr_vdev *vdev,
					bool he_siga_va15_allowed);

/**
 * wlan_sr_setup_req() - Enable SR with provided pd threshold
 *
 * @vdev: objmgr vdev
 * @pdev: objmgr pdev
 * @is_sr_enable: sr enable/disable
 * @pd_threshold: pd threshold
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
wlan_sr_setup_req(struct wlan_objmgr_vdev *vdev,
		  struct wlan_objmgr_pdev *pdev, bool is_sr_enable,
		  int32_t pd_threshold);
#endif
