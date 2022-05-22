/*
 * Copyright (c) 2012-2021 The Linux Foundation. All rights reserved.
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

#ifndef _WLAN_HDD_PRE_CAC_H_
#define _WLAN_HDD_PRE_CAC_H_

#ifdef PRE_CAC_SUPPORT

/* default pre cac channel bandwidth */
#define DEFAULT_PRE_CAC_BANDWIDTH CH_WIDTH_80MHZ

/*
 * Code under PRE_CAC_COMP will be cleaned up
 * once pre cac component is done
 */
#ifndef PRE_CAC_COMP
/**
 * wlan_hdd_sap_pre_cac_success() - Process the pre cac success
 * @data: AP adapter
 *
 * Return: None
 */
void wlan_hdd_sap_pre_cac_success(void *data);
#else
static inline void wlan_hdd_sap_pre_cac_success(void *data)
{
}
#endif /* PRE_CAC_COMP */

/**
 * wlan_hdd_sap_pre_cac_failure() - Process the pre cac failure
 * @data: AP adapter
 *
 * Deletes the pre cac adapter
 *
 * Return: None
 */
void wlan_hdd_sap_pre_cac_failure(void *data);

/*
 * Code under PRE_CAC_COMP will be cleaned up
 * once pre cac component is done
 */
#ifndef PRE_CAC_COMP
/**
 * hdd_clean_up_pre_cac_interface() - Clean up the pre cac interface
 * @hdd_ctx: HDD context
 *
 * Cleans up the pre cac interface, if it exists
 *
 * Return: None
 */
void hdd_clean_up_pre_cac_interface(struct hdd_context *hdd_ctx);
#endif

/**
 * wlan_hdd_request_pre_cac() - Start pre CAC in the driver
 * @hdd_ctx: the HDD context to operate against
 * @chan_freq: channel freq option provided by userspace
 *
 * Sets the driver to the required hardware mode and start an adapter for
 * pre CAC which will mimic an AP.
 *
 * Return: Zero on success, non-zero value on error
 */
int wlan_hdd_request_pre_cac(struct hdd_context *hdd_ctx, uint32_t chan_freq);

/**
 * hdd_send_conditional_chan_switch_status() - Send conditional channel switch
 * status
 * @hdd_ctx: HDD context
 * @wdev: Wireless device structure
 * @status: Status of conditional channel switch
 * (0: Success, Non-zero: Failure)
 *
 * Sends the status of conditional channel switch to user space. This is named
 * conditional channel switch because the SAP will move to the provided channel
 * after some condition (pre-cac) is met.
 *
 * Return: None
 */
void hdd_send_conditional_chan_switch_status(struct hdd_context *hdd_ctx,
					     struct wireless_dev *wdev,
					     bool status);

/**
 * hdd_close_pre_cac_adapter() - Close pre CAC adapter
 * @hdd_ctx: the HDD context to operate against
 *
 * Return: None
 */
void hdd_close_pre_cac_adapter(struct hdd_context *hdd_ctx);

/*
 * Code under PRE_CAC_COMP will be cleaned up
 * once pre cac component is done
 */
#ifdef PRE_CAC_COMP
/**
 * hdd_pre_cac_register_cb() - Sets legacy callbacks to osif
 *
 * API to set legacy callbacks to osif
 *
 * Return: QDF_STATUS
 */
QDF_STATUS hdd_pre_cac_register_cb(void);

/**
 * void hdd_pre_cac_unregister_cb(void)() - Resets legacy callbacks to osif
 *
 * API to reset legacy callbacks to osif
 *
 * Return: QDF_STATUS
 */
void hdd_pre_cac_unregister_cb(void);
#else
static inline QDF_STATUS hdd_pre_cac_register_cb(void)
{
	return QDF_STATUS_SUCCESS;
}

static inline void hdd_pre_cac_unregister_cb(void)
{
}
#endif /* PRE_CAC_COMP */
#else
static inline void wlan_hdd_sap_pre_cac_success(void *data)
{
}

static inline void wlan_hdd_sap_pre_cac_failure(void *data)
{
}

/*
 * Code under PRE_CAC_COMP will be cleaned up
 * once pre cac component is done
 */
#ifndef PRE_CAC_COMP
static inline void
hdd_clean_up_pre_cac_interface(struct hdd_context *hdd_ctx)
{
}
#endif

static inline int
wlan_hdd_request_pre_cac(struct hdd_context *hdd_ctx, uint32_t chan_freq)
{
	return 0;
}

static inline void
hdd_send_conditional_chan_switch_status(struct hdd_context *hdd_ctx,
					struct wireless_dev *wdev,
					bool status)
{
}

static inline void
hdd_close_pre_cac_adapter(struct hdd_context *hdd_ctx)
{
}

static inline QDF_STATUS hdd_pre_cac_register_cb(void)
{
	return QDF_STATUS_SUCCESS;
}

static inline void hdd_pre_cac_unregister_cb(void)
{
}
#endif /* PRE_CAC_SUPPORT */
#endif /* _WLAN_HDD_PRE_CAC_H_ */
