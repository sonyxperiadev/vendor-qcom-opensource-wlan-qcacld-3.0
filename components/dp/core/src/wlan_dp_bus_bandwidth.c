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
 * DOC: wlan_dp_bus_bandwidth.c
 *
 * Bus Bandwidth Manager implementation
 */

#include "wlan_dp_bus_bandwidth.h"
#include "wlan_dp_main.h"
#include <wlan_objmgr_psoc_obj_i.h>
#include "cds_api.h"

void dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
				     struct bbm_params *params)
{
}

int dp_bbm_context_init(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

void dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc)
{
}

