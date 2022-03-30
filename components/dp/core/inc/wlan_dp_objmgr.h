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
 * DOC: This file contains various object manager related wrappers and helpers
 */

#ifndef __WLAN_DP_OBJMGR_H
#define __WLAN_DP_OBJMGR_H

#include "wlan_cmn.h"
#include "wlan_objmgr_cmn.h"
#include "wlan_objmgr_peer_obj.h"
#include "wlan_objmgr_vdev_obj.h"
#include "wlan_objmgr_pdev_obj.h"
#include "wlan_objmgr_psoc_obj.h"
#include "wlan_utility.h"

/**
 * dp_get_vdev_priv_obj() - Wrapper to retrieve vdev priv obj
 * @vdev: vdev pointer
 *
 * Return: DP vdev private object
 */
static inline struct wlan_dp_intf *
dp_get_vdev_priv_obj(struct wlan_objmgr_vdev *vdev)
{
	return NULL;
}

/**
 * dp_psoc_get_priv() - Wrapper to retrieve psoc priv obj
 * @psoc: psoc pointer
 *
 * Return: DP psoc private object
 */
static inline struct wlan_dp_psoc_context *
dp_psoc_get_priv(struct wlan_objmgr_psoc *psoc)
{
	return NULL;
}
#endif /* __WLAN_DP_OBJMGR_H */
