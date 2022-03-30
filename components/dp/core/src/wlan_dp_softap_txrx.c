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
  * DOC: wlan_dp_softap_txrx.c
  * DP Soft AP TX/RX path implementation
  *
  *
  */

#include <wlan_dp_txrx.h>
#include <qdf_types.h>
#include <qdf_nbuf.h>

QDF_STATUS dp_softap_start_xmit(qdf_nbuf_t nbuf, struct wlan_dp_intf *dp_intf)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS dp_softap_rx_packet_cbk(void *intf_ctx, qdf_nbuf_t rx_buf)
{
	return QDF_STATUS_SUCCESS;
}
