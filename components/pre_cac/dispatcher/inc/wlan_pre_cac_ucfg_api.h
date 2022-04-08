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

/**
 * DOC: Declare public API related to the pre_cac called by north bound
 * HDD/OSIF/LIM
 */

#ifndef _WLAN_PRE_CAC_UCFG_API_H_
#define _WLAN_PRE_CAC_UCFG_API_H_

#include <qdf_status.h>
#include <qdf_types.h>
#include "wlan_pre_cac_public_struct.h"

#ifdef PRE_CAC_SUPPORT
/**
 * ucfg_pre_cac_init() - pre cac component initialization.
 *
 * This function initializes the pre cac component and registers
 * the handlers which are invoked on vdev creation.
 *
 * Return: For successful registration - QDF_STATUS_SUCCESS,
 *         else QDF_STATUS error codes.
 */
QDF_STATUS ucfg_pre_cac_init(void);

/**
 * ucfg_pre_cac_deinit() - pre cac component deinit.
 *
 * This function deinits pre cac component.
 *
 * Return: None
 */
void ucfg_pre_cac_deinit(void);

/*
 * ucfg_pre_cac_set_osif_cb() - set pre cac osif callbacks.
 * @pre_cac_ops: pre cac ops
 *
 * Return: None
 */
void ucfg_pre_cac_set_osif_cb(struct pre_cac_ops *pre_cac_ops);
#else
static inline
QDF_STATUS ucfg_pre_cac_init(void)
{
	return QDF_STATUS_SUCCESS;
}

static inline
void ucfg_pre_cac_deinit(void)
{
}

static inline void
ucfg_pre_cac_set_osif_cb(struct pre_cac_ops *pre_cac_ops)
{
}
#endif /* PRE_CAC_SUPPORT */
#endif /* _WLAN_PRE_CAC_UCFG_API_H_ */

