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
 * DOC: os_if_qmi.h
 *
 * This file contains declarations for wrapper APIs for QMI HLOS APIs
 */

#ifndef _OS_IF_QMI_H_
#define _OS_IF_QMI_H_

#include "wlan_qmi_public_struct.h"
#include <qdf_types.h>
#include <linux/soc/qcom/qmi.h>
#include <linux/net.h>
#include <linux/types.h>
#include "wlan_cfg80211.h"
#include "wlan_objmgr_psoc_obj.h"
#include <qdf_time.h>

#define QMI_WFDS_MAX_RECV_BUF_SIZE  4096
#define QMI_WFDS_SERVICE_INS_ID_V01 0x0
#define QMI_WFDS_TIMEOUT_MS         10000
#define QMI_WFDS_TIMEOUT_JF         \
	qdf_system_msecs_to_ticks(QMI_WFDS_TIMEOUT_MS)


#ifdef QMI_COMPONENT_ENABLE
/**
 * os_if_qmi_handle_init() - Initialize QMI handle
 * @qmi_hdl: QMI handle to initialize
 * @recv_buf_size: maximum size of incoming message
 * @ops: reference to callbacks for QRTR notifications
 * @qmi_msg_handlers: NULL-terminated list of QMI message handlers
 *
 * Returns: QDF status
 */
QDF_STATUS
os_if_qmi_handle_init(struct qmi_handle *qmi_hdl, qdf_size_t recv_buf_size,
		      const struct qmi_ops *ops,
		      const struct qmi_msg_handler *qmi_msg_handlers);

/**
 * os_if_qmi_handle_release() - Release QMI handle
 * @qmi_hdl: QMI handle to release
 *
 * Returns: None
 */
void os_if_qmi_handle_release(struct qmi_handle *qmi_hdl);

/**
 * os_if_qmi_add_lookup() - Register a new lookup with the name service
 * @qmi_hdl: QMI handle
 * @service: service id of the request
 * @instance: instance id of the request
 * @version: version number of the request
 *
 * Return: QDF status
 */
QDF_STATUS os_if_qmi_add_lookup(struct qmi_handle *qmi_hdl,
				unsigned int service, unsigned int version,
				unsigned int instance);

/**
 * os_if_qmi_connect_to_svc() - Connect to QMI service
 * @qmi_hdl: QMI handle
 * @qmi_svc: QMI service handle
 *
 * Return: QDF status
 */
QDF_STATUS os_if_qmi_connect_to_svc(struct qmi_handle *qmi_hdl,
				    struct qmi_service *qmi_svc);

/**
 * os_if_qmi_txn_init() - Initialize QMI transaction
 * @qmi_hdl: QMI handle
 * @qmi_txn: QMI transaction handle
 * @qmi_ei: description of how to decode a matching response (optional)
 * @resp: pointer to the object to decode the response into (optional)
 *
 * Return: QDF status
 */
QDF_STATUS os_if_qmi_txn_init(struct qmi_handle *qmi_hdl,
			      struct qmi_txn *qmi_txn,
			      struct qmi_elem_info *qmi_ei, void *resp);

/**
 * os_if_qmi_send_request() - Connect to QMI service
 * @qmi_hdl: QMI handle
 * @sq: destination sockaddr
 * @qmi_txn: QMI transaction handle
 * @msg_id: message id
 * @len: max length of the QMI message
 * @ei: QMI message description
 * @req: message to be encoded
 *
 * Return: QDF status
 */
QDF_STATUS os_if_qmi_send_request(struct qmi_handle *qmi_hdl,
				  struct sockaddr_qrtr *sq,
				  struct qmi_txn *qmi_txn, int msg_id,
				  uint32_t len, struct qmi_elem_info *ei,
				  const void *req);

/**
 * os_if_qmi_txn_wait() - Wait for transaction response
 * @qmi_txn: QMI transaction handle
 * @timeout: Timeout value in jiffies
 *
 * Return: QDF status
 */
QDF_STATUS os_if_qmi_txn_wait(struct qmi_txn *qmi_txn, unsigned long timeout);

/**
 * os_if_qmi_txn_cancel() - Cancel the QMI transaction
 * @qmi_txn: QMI transaction handle
 *
 * Return: None
 */
void os_if_qmi_txn_cancel(struct qmi_txn *qmi_txn);

/**
 * os_if_qmi_register_callbacks() - Register callback handlers
 * @psoc: Pointer to psoc context
 * @cb_obj: Callback object pointer
 *
 * Return: None
 */
void os_if_qmi_register_callbacks(struct wlan_objmgr_psoc *psoc,
				  struct wlan_qmi_psoc_callbacks *cb_obj);

#ifdef QMI_WFDS
/**
 * os_if_qmi_wfds_register_callbacks() - Register WFDS callback handlers
 * @cb_obj: Callback object pointer
 *
 * Return: None
 */
void os_if_qmi_wfds_register_callbacks(struct wlan_qmi_psoc_callbacks *cb_obj);
#else
static inline
void os_if_qmi_wfds_register_callbacks(struct wlan_qmi_psoc_callbacks *cb_obj)
{
}
#endif
#else
static inline
QDF_STATUS os_if_qmi_handle_init(struct qmi_handle *qmi_hdl,
				 qdf_size_t recv_buf_size,
				 const struct qmi_ops *ops,
				 const struct qmi_msg_handler *qmi_msg_handlers)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
void os_if_qmi_handle_release(struct qmi_handle *qmi_hdl)
{
}

static inline
QDF_STATUS os_if_qmi_add_lookup(struct qmi_handle *qmi_hdl,
				unsigned int service, unsigned int version,
				unsigned int instance)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS os_if_qmi_connect_to_svc(struct qmi_handle *qmi_hdl,
				    struct qmi_service *qmi_svc)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS os_if_qmi_txn_init(struct qmi_handle *qmi_hdl,
			      struct qmi_txn *qmi_txn,
			      struct qmi_elem_info *qmi_ei, void *resp)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS os_if_qmi_send_request(struct qmi_handle *qmi_hdl,
				  struct sockaddr_qrtr *sq,
				  struct qmi_txn *qmi_txn, int msg_id,
				  uint32_t len, struct qmi_elem_info *ei,
				  const void *req)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
QDF_STATUS os_if_qmi_txn_wait(struct qmi_txn *qmi_txn, unsigned long timeout)
{
	return QDF_STATUS_E_NOSUPPORT;
}

static inline
void os_if_qmi_txn_cancel(struct qmi_txn *qmi_txn)
{
}

static inline
void os_if_qmi_register_callbacks(struct wlan_objmgr_psoc *psoc,
				  struct wlan_qmi_psoc_callbacks *cb_obj)
{
}
#endif
#endif
