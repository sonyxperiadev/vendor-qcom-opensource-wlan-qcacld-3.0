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
 * DOC: contains nud event tracking function declarations
 */

#ifndef _WLAN_DP_NUD_TRACKING_H_
#define _WLAN_DP_NUD_TRACKING_H_

#ifdef WLAN_NUD_TRACKING

/**
 * dp_nud_init_tracking() - initialize NUD tracking
 * @dp_intf: Pointer to dp interface
 *
 * Return: None
 */
void dp_nud_init_tracking(struct wlan_dp_intf *dp_intf);

/**
 * dp_nud_deinit_tracking() - deinitialize NUD tracking
 * @dp_intf: Pointer to dp interface
 *
 * Return: None
 */
void dp_nud_deinit_tracking(struct wlan_dp_intf *dp_intf);

#else
static inline void dp_nud_init_tracking(struct wlan_dp_intf *dp_intf)
{
}

static inline void dp_nud_deinit_tracking(struct wlan_dp_intf *dp_intf)
{
}
#endif
#endif
