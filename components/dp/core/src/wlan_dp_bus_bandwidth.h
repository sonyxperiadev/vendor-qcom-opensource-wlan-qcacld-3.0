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

#if !defined(WLAN_DP_BUS_BANDWIDTH_H)
#define WLAN_DP_BUS_BANDWIDTH_H
/**
 * DOC: wlan_dp_bus_bandwidth.h
 *
 * Bus Bandwidth Manager implementation
 */

#include "wlan_dp_priv.h"
#include <qdf_types.h>
#include <qca_vendor.h>
#include <wlan_objmgr_psoc_obj.h>
#include "wlan_dp_public_struct.h"
#include "wlan_dp_priv.h"

typedef const enum bus_bw_level
	bus_bw_table_type[QCA_WLAN_802_11_MODE_INVALID][TPUT_LEVEL_MAX];

/**
 * struct bbm_context: Bus Bandwidth Manager context
 *
 * @curr_bus_bw_lookup_table: current bus bw lookup table
 * @curr_vote_level: current vote level
 * @per_policy_vote: per BBM policy related vote
 * @bbm_lock: BBM API lock
 */
struct bbm_context {
	bus_bw_table_type *curr_bus_bw_lookup_table;
	enum bus_bw_level curr_vote_level;
	enum bus_bw_level per_policy_vote[BBM_MAX_POLICY];
	qdf_mutex_t bbm_lock;
};

#ifdef FEATURE_BUS_BANDWIDTH_MGR
/**
 * dp_bbm_context_init() - Initialize BBM context
 * @psoc: psoc Handle
 *
 * Returns: error code
 */
int dp_bbm_context_init(struct wlan_objmgr_psoc *psoc);

/**
 * dp_bbm_context_deinit() - De-initialize BBM context
 * @psoc: psoc Handle
 *
 * Returns: None
 */
void dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc);

/**
 * dp_bbm_apply_independent_policy() - Function to apply independent policies
 *  to set the bus bw level
 * @psoc: psoc Handle
 * @params: BBM policy related params
 *
 * The function applies BBM related policies and appropriately sets the bus
 * bandwidth level.
 *
 * Returns: None
 */
void dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
				     struct bbm_params *params);
#else
static inline int dp_bbm_context_init(struct wlan_objmgr_psoc *psoc)
{
	return 0;
}

static inline void dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc)
{
}

static inline
void dp_bbm_apply_independent_policy(struct wlan_objmgr_psoc *psoc,
				     struct bbm_params *params)
{
}
#endif /* FEATURE_BUS_BANDWIDTH_MGR */

#if defined(WLAN_FEATURE_DP_BUS_BANDWIDTH) && defined(FEATURE_RUNTIME_PM)
/**
 * dp_rtpm_tput_policy_init() - Initialize RTPM tput policy
 * @psoc: psoc handle
 *
 * Returns: None
 */
void dp_rtpm_tput_policy_init(struct wlan_objmgr_psoc *psoc);

/**
 * dp_rtpm_tput_policy_deinit() - Deinitialize RTPM tput policy
 * @psoc: psoc handle
 *
 * Returns: None
 */
void dp_rtpm_tput_policy_deinit(struct wlan_objmgr_psoc *psoc);

/**
 * dp_rtpm_tput_policy_apply() - Apply RTPM tput policy
 * @dp_ctx: dp_ctx handle
 * @tput_level : Tput level
 *
 * Returns: None
 */
void dp_rtpm_tput_policy_apply(struct wlan_dp_psoc_context *dp_ctx,
			       enum tput_level tput_level);

/**
 * dp_rtpm_tput_policy_get_vote() - Get RTPM tput policy vote
 * @dp_ctx: dp_ctx handle
 *
 * Returns: Current vote
 */
int dp_rtpm_tput_policy_get_vote(struct wlan_dp_psoc_context *dp_ctx);
#else
static inline
void dp_rtpm_tput_policy_init(struct wlan_objmgr_psoc *psoc)
{
}

static inline
void dp_rtpm_tput_policy_deinit(struct wlan_objmgr_psoc *psoc)
{
}

static inline
void dp_rtpm_tput_policy_apply(struct wlan_dp_psoc_context *dp_ctx,
			       enum tput_level tput_level)
{
}

static inline int
dp_rtpm_tput_policy_get_vote(struct wlan_dp_psoc_context *dp_ctx)
{
	return -EINVAL;
}
#endif /* WLAN_FEATURE_DP_BUS_BANDWIDTH && FEATURE_RUNTIME_PM */
#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
/**
 * dp_reset_tcp_delack() - Reset tcp delack value to default
 * @psoc: psoc handle
 *
 * Function used to reset TCP delack value to its default value
 *
 * Return: None
 */
void dp_reset_tcp_delack(struct wlan_objmgr_psoc *psoc);

/**
 * wlan_dp_update_tcp_rx_param() - update TCP param in RX dir
 * @dp_ctx: Pointer to DP context
 * @data: Parameters to update
 *
 * Return: None
 */
void wlan_dp_update_tcp_rx_param(struct wlan_dp_psoc_context *dp_ctx,
				 struct wlan_rx_tp_data *data);
#else
static inline
void dp_reset_tcp_delack(struct wlan_objmgr_psoc *psoc);
{
}

static inline
void wlan_dp_update_tcp_rx_param(struct wlan_dp_psoc_context *dp_ctx,
				 struct wlan_rx_tp_data *data)
{
}
#endif /* WLAN_FEATURE_DP_BUS_BANDWIDTH */
#endif /* WLAN_DP_BUS_BANDWIDTH_H */
