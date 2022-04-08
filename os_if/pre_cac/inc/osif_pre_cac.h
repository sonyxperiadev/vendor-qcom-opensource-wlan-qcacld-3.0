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

#ifndef _OSIF_PRE_CAC_H_
#define _OSIF_PRE_CAC_H_

#ifdef PRE_CAC_SUPPORT
/**
 * osif_pre_cac_ops: pre cac legacy callbacks
 */
struct osif_pre_cac_legacy_ops {
};

/**
 * osif_pre_cac_set_legacy_cb() - Sets legacy callbacks to osif
 * @osif_legacy_ops:  Function pointer to legacy ops structure
 *
 * API to set legacy callbacks to osif
 * Context: Any context.
 *
 * Return: void
 */
void
osif_pre_cac_set_legacy_cb(struct osif_pre_cac_legacy_ops *osif_legacy_ops);

/**
 * osif_pre_cac_reset_legacy_cb() - Resets legacy callbacks to osif
 *
 * API to reset legacy callbacks to osif
 * Context: Any context.
 *
 * Return: void
 */
void osif_pre_cac_reset_legacy_cb(void);

/**
 * osif_pre_cac_register_cb() - API to register pre cac callbacks.
 *
 * Return: QDF_STATUS
 */
QDF_STATUS osif_pre_cac_register_cb(void);
#endif /* PRE_CAC_SUPPORT */
#endif /* _OSIF_PRE_CAC_H_ */
