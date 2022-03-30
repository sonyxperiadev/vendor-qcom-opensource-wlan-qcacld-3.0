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

#include <qdf_types.h>
#include <qca_vendor.h>
#include <wlan_objmgr_psoc_obj.h>

#ifdef FEATURE_BUS_BANDWIDTH_MGR

/**
 * struct bbm_params - BBM params
 *
 */
struct bbm_params {
};

/**
 * dp_bbm_context_init() - Initialize BBM context
 * @dp_ctx: DP context
 *
 * Returns: error code
 */
int dp_bbm_context_init(struct wlan_objmgr_psoc *psoc);

/**
 * dp_bbm_context_deinit() - De-initialize BBM context
 * @dp_ctx: DP context
 *
 * Returns: None
 */
void dp_bbm_context_deinit(struct wlan_objmgr_psoc *psoc);

/**
 * dp_bbm_apply_independent_policy() - Function to apply independent policies
 *  to set the bus bw level
 * @dp_ctx: DP context
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
#endif /* WLAN_DP_BUS_BANDWIDTH_H */
