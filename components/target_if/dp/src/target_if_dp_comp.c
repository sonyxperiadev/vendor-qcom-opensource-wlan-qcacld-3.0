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
 * DOC: target_if_dp_comp.c
 */

#include "target_if_dp_comp.h"
#include "target_if.h"
#include "qdf_status.h"
#include "wmi.h"
#include "wmi_unified_api.h"
#include "wmi_unified_priv.h"
#include "wmi_unified_param.h"
#include "wlan_objmgr_psoc_obj.h"
#include "wlan_dp_public_struct.h"
#include "cdp_txrx_cmn.h"
#include "wlan_dp_main.h"
#include <wlan_cm_api.h>

void target_if_dp_register_tx_ops(struct wlan_dp_psoc_sb_ops *sb_ops)
{
}

void target_if_dp_register_rx_ops(struct wlan_dp_psoc_nb_ops *nb_ops)
{
}
