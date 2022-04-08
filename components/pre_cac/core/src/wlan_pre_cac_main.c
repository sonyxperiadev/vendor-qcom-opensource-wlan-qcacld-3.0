/*
 * Copyright (c) 2012-2021 The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2022, Qualcomm Innovation Center, Inc. All rights reserved.
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
 * DOC: Implement various notification handlers which are accessed
 * internally in pre_cac component only.
 */

#include "wlan_pre_cac_main.h"
#include "wlan_objmgr_global_obj.h"

struct pre_cac_vdev_priv *
pre_cac_vdev_get_priv_fl(struct wlan_objmgr_vdev *vdev,
			 const char *func, uint32_t line)
{
	struct pre_cac_vdev_priv *vdev_priv;

	vdev_priv = wlan_objmgr_vdev_get_comp_private_obj(vdev,
							WLAN_UMAC_COMP_PRE_CAC);
	if (!vdev_priv) {
		pre_cac_nofl_err("%s:%u: vdev id: %d, vdev_priv is NULL",
				 func, line, wlan_vdev_get_id(vdev));
	}

	return vdev_priv;
}

struct pre_cac_psoc_priv *
pre_cac_psoc_get_priv_fl(struct wlan_objmgr_psoc *psoc,
			 const char *func, uint32_t line)
{
	struct pre_cac_psoc_priv *psoc_priv;

	psoc_priv = wlan_objmgr_psoc_get_comp_private_obj(psoc,
					WLAN_UMAC_COMP_PRE_CAC);
	if (!psoc_priv)
		pre_cac_nofl_err("%s:%u: psoc_priv is NULL", func, line);

	return psoc_priv;
}

QDF_STATUS
pre_cac_vdev_create_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	struct pre_cac_vdev_priv *vdev_priv;
	QDF_STATUS status;

	vdev_priv = qdf_mem_malloc(sizeof(*vdev_priv));
	if (!vdev_priv) {
		status = QDF_STATUS_E_NOMEM;
		goto exit;
	}

	status = wlan_objmgr_vdev_component_obj_attach(
				vdev, WLAN_UMAC_COMP_PRE_CAC,
				(void *)vdev_priv, QDF_STATUS_SUCCESS);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to attach priv with vdev");
		goto free_vdev_priv;
	}

	goto exit;

free_vdev_priv:
	qdf_mem_free(vdev_priv);
	status = QDF_STATUS_E_INVAL;
exit:
	return status;
}

QDF_STATUS
pre_cac_vdev_destroy_notification(struct wlan_objmgr_vdev *vdev, void *arg)
{
	struct pre_cac_vdev_priv *vdev_priv = NULL;
	QDF_STATUS status = QDF_STATUS_E_FAILURE;

	vdev_priv = pre_cac_vdev_get_priv(vdev);
	if (!vdev_priv) {
		pre_cac_err("vdev priv is NULL");
		goto exit;
	}

	status = wlan_objmgr_vdev_component_obj_detach(
					vdev, WLAN_UMAC_COMP_PRE_CAC,
					(void *)vdev_priv);
	if (QDF_IS_STATUS_ERROR(status))
		pre_cac_err("Failed to detach priv with vdev");

	qdf_mem_free(vdev_priv);
	vdev_priv = NULL;

exit:
	return status;
}

QDF_STATUS
pre_cac_psoc_create_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct pre_cac_psoc_priv *psoc_priv;
	QDF_STATUS status;

	psoc_priv = qdf_mem_malloc(sizeof(*psoc_priv));
	if (!psoc_priv)
		return QDF_STATUS_E_NOMEM;

	status = wlan_objmgr_psoc_component_obj_attach(psoc,
				WLAN_UMAC_COMP_PRE_CAC,
				psoc_priv, QDF_STATUS_SUCCESS);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to attach psoc component obj");
		goto free_psoc_priv;
	}

	return status;

free_psoc_priv:
	qdf_mem_free(psoc_priv);
	return status;
}

QDF_STATUS
pre_cac_psoc_destroy_notification(struct wlan_objmgr_psoc *psoc, void *arg)
{
	struct pre_cac_psoc_priv *psoc_priv;
	QDF_STATUS status;

	psoc_priv = pre_cac_psoc_get_priv(psoc);
	if (!psoc_priv) {
		pre_cac_err("psoc priv is NULL");
		return QDF_STATUS_E_FAILURE;
	}

	status = wlan_objmgr_psoc_component_obj_detach(psoc,
					WLAN_UMAC_COMP_PRE_CAC,
					psoc_priv);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to detach psoc component obj");
		return status;
	}

	qdf_mem_free(psoc_priv);
	return status;
}

QDF_STATUS pre_cac_init(void)
{
	QDF_STATUS status;

	status = wlan_objmgr_register_psoc_create_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_psoc_create_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to register psoc create handler");
		return status;
	}

	status = wlan_objmgr_register_psoc_destroy_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_psoc_destroy_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to register psoc delete handler");
		goto fail_destroy_psoc;
	}

	status = wlan_objmgr_register_vdev_create_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_vdev_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to register vdev create handler");
		goto fail_create_vdev;
	}

	status = wlan_objmgr_register_vdev_destroy_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_vdev_destroy_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status)) {
		pre_cac_err("Failed to register vdev destroy handler");
		goto fail_destroy_vdev;
	}
	return status;

fail_destroy_vdev:
	wlan_objmgr_unregister_vdev_create_handler(WLAN_UMAC_COMP_PRE_CAC,
		pre_cac_vdev_create_notification, NULL);

fail_create_vdev:
	wlan_objmgr_unregister_psoc_destroy_handler(WLAN_UMAC_COMP_PRE_CAC,
		pre_cac_psoc_destroy_notification, NULL);

fail_destroy_psoc:
	wlan_objmgr_unregister_psoc_create_handler(WLAN_UMAC_COMP_PRE_CAC,
		pre_cac_psoc_create_notification, NULL);

	return status;
}

void pre_cac_deinit(void)
{
	QDF_STATUS status;

	status = wlan_objmgr_unregister_vdev_destroy_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_vdev_destroy_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status))
		pre_cac_err("Failed to unregister vdev destroy handler");

	status = wlan_objmgr_unregister_vdev_create_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_vdev_create_notification, NULL);
	if (QDF_IS_STATUS_ERROR(status))
		pre_cac_err("Failed to unregister vdev create handler");

	status = wlan_objmgr_unregister_psoc_destroy_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_psoc_destroy_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status))
		pre_cac_err("Failed to unregister psoc destroy handler");

	status = wlan_objmgr_unregister_psoc_create_handler(
				WLAN_UMAC_COMP_PRE_CAC,
				pre_cac_psoc_create_notification,
				NULL);
	if (QDF_IS_STATUS_ERROR(status))
		pre_cac_err("Failed to unregister psoc create handler");
}
