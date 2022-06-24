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

#ifndef _WLAN_PRE_CAC_PUBLIC_STRUCT_H_
#define _WLAN_PRE_CAC_PUBLIC_STRUCT_H_

#include "wlan_objmgr_psoc_obj.h"

/**
 * pre_cac_ops: pre cac osif callbacks
 * @pre_cac_send_conditional_freq_switch_status: send conditional frequency
 *                                               switch status
 * @pre_cac_complete_cb: Pre cac complete callback
 */
struct pre_cac_ops {
	void (*pre_cac_conditional_csa_ind_cb)(
			struct wlan_objmgr_psoc *psoc, uint8_t vdev_id,
			bool status);
	void (*pre_cac_complete_cb)(struct wlan_objmgr_vdev *vdev,
				    QDF_STATUS status);
};
#endif /* _WLAN_PRE_CAC_PUBLIC_STRUCT_H_ */
