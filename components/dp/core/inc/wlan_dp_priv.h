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
  * DOC: Declare various struct, macros which are used for private to DP.
  *
  * Note: This file shall not contain public API's prototype/declarations.
  *
  */

#ifndef _WLAN_DP_PRIV_STRUCT_H_
#define _WLAN_DP_PRIV_STRUCT_H_

#include "wlan_dp_public_struct.h"
#include "cdp_txrx_cmn.h"
#include "hdd_dp_cfg.h"
#include "wlan_dp_cfg.h"
#include "wlan_dp_objmgr.h"
#include <cdp_txrx_misc.h>
#include "qdf_periodic_work.h"

#if defined(WLAN_FEATURE_DP_BUS_BANDWIDTH) && defined(FEATURE_RUNTIME_PM)

/**
 * struct dp_rtpm_tput_policy_context - RTPM throughput policy context
 */
struct dp_rtpm_tput_policy_context {
};
#endif

struct wlan_dp_psoc_cfg {
};

/**
 * struct tx_rx_histogram - structure to keep track of tx and rx packets
 *				received over 100ms intervals
 */
struct tx_rx_histogram {
};

struct dp_tx_rx_stats {
};

struct dp_stats {
	struct dp_tx_rx_stats tx_rx_stats;
};

/**
 * struct wlan_dp_intf - DP interface object related info
 * @dp_ctx: DP context reference
 */
struct wlan_dp_intf {
	struct wlan_dp_psoc_context *dp_ctx;
};

/**
 * struct wlan_dp_psoc_context - psoc related data required for DP
 */
struct wlan_dp_psoc_context {
};

#endif /* end  of _WLAN_DP_PRIV_STRUCT_H_ */
