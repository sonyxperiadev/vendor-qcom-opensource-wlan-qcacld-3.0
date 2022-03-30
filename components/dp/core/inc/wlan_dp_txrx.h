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

#ifndef __WLAN_DP_TXRX_H__
#define __WLAN_DP_TXRX_H__

#include <wlan_dp_priv.h>
#include <wlan_dp_main.h>
#include <qdf_types.h>

/**
 * dp_softap_start_xmit() - Transmit a frame for SAP interface
 * @nbuf: pointer to Network buffer
 * @dp_intf: DP interface
 *
 * Return: QDF_STATUS_SUCCESS on successful transmission
 */
QDF_STATUS
dp_softap_start_xmit(qdf_nbuf_t nbuf, struct wlan_dp_intf *dp_intf);

/**
 * dp_softap_rx_packet_cbk() - Receive packet handler for SAP
 * @dp_intf_context: pointer to DP interface context
 * @rxBuf: pointer to rx qdf_nbuf
 *
 * Receive callback registered with data path.  DP will call this to notify
 * when one or more packets were received for a registered
 * STA.
 *
 * Return: QDF_STATUS_E_FAILURE if any errors encountered,
 *	   QDF_STATUS_SUCCESS otherwise
 */
QDF_STATUS
dp_softap_rx_packet_cbk(void *intf_ctx, qdf_nbuf_t rx_buf);

/**
 * dp_start_xmit() - Transmit a frame for STA interface
 * @nbuf: pointer to Network buffer
 * @dp_intf: DP interface
 *
 * Return: QDF_STATUS_SUCCESS on successful transmission
 */
QDF_STATUS
dp_start_xmit(struct wlan_dp_intf *dp_intf, qdf_nbuf_t nbuf);

/**
 * dp_rx_packet_cbk() - Receive packet handler
 * @dp_intf_context: pointer to DP interface context
 * @rx_buf: pointer to rx qdf_nbuf
 *
 * Receive callback registered with data path.  DP will call this to notify
 * when one or more packets were received for a registered
 * STA.
 *
 * Return: QDF_STATUS_E_FAILURE if any errors encountered,
 *	   QDF_STATUS_SUCCESS otherwise
 */
QDF_STATUS dp_rx_packet_cbk(void *dp_intf_context, qdf_nbuf_t rx_buf);

#endif
