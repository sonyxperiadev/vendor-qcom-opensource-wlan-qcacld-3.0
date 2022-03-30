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
 * DOC: contains nud event tracking main function definitions
 */

#include "osif_sync.h"
#include "wlan_dp_main.h"
#include "wlan_dlm_ucfg_api.h"
#include "wlan_dp_cfg.h"
#include <cdp_txrx_misc.h>
#include "wlan_cm_roam_ucfg_api.h"
#include <wlan_cm_ucfg_api.h>
#include "wlan_dp_nud_tracking.h"

#ifdef WLAN_NUD_TRACKING
void dp_nud_deinit_tracking(struct wlan_dp_intf *dp_intf)
{
}

void dp_nud_init_tracking(struct wlan_dp_intf *dp_intf)
{
}
#endif
