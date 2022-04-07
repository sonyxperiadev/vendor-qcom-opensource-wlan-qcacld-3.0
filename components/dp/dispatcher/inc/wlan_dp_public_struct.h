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
 * DOC: Contains DP public data structure definations.
 *
 */

#ifndef _WLAN_DP_PUBLIC_STRUCT_H_
#define _WLAN_DP_PUBLIC_STRUCT_H_

#include "wlan_cmn.h"
#include "wlan_objmgr_cmn.h"
#include "wlan_objmgr_global_obj.h"
#include "qdf_status.h"
#include <wlan_nlink_common.h>
#include <qca_vendor.h>
#include <ani_system_defs.h>
#include <qdf_defer.h>

/**
 * struct dp_mic_info - mic error info in dp
 * @ta_mac_addr: transmitter mac address
 * @multicast: Flag for multicast
 * @key_id: Key ID
 * @tsc: Sequence number
 * @vdev_id: vdev id
 *
 */
struct dp_mic_error_info {
	struct qdf_mac_addr ta_mac_addr;
	bool multicast;
	uint8_t key_id;
	uint8_t tsc[SIR_CIPHER_SEQ_CTR_SIZE];
	uint16_t vdev_id;
};

enum dp_mic_work_status {
	DP_MIC_UNINITIALIZED,
	DP_MIC_INITIALIZED,
	DP_MIC_SCHEDULED,
	DP_MIC_DISABLED
};

/**
 * struct dp_mic_work - mic work info in dp
 * @mic_error_work: mic error work
 * @status: sattus of mic error work
 * @info: Pointer to mic error information
 * @lock: lock to synchronixe mic error work
 *
 */
struct dp_mic_work {
	qdf_work_t work;
	enum dp_mic_work_status status;
	struct dp_mic_error_info *info;
	qdf_spinlock_t lock;
};

/**
 * typedef hdd_cb_handle - HDD Handle
 *
 * Handle to the HDD.  The HDD handle is given to the DP component from the
 * HDD during start modules.  The HDD handle is an input to all HDD function
 * calls and represents an opaque handle to the HDD instance that is
 * tied to the DP context
 *
 * The HDD must be able to derive it's internal instance structure
 * pointer through this handle.
 */
/*
 * NOTE WELL: struct opaque_hdd_callback_handle is not defined anywhere. This
 * reference is used to help ensure that a hdd_cb_handle is never used
 * where a different handle type is expected
 */
struct opaque_hdd_callback_handle;
typedef struct opaque_hdd_callback_handle *hdd_cb_handle;

/**
 * enum bus_bw_level - bus bandwidth vote levels
 *
 * @BUS_BW_LEVEL_NONE: No vote for bus bandwidth
 * @BUS_BW_LEVEL_1: vote for level-1 bus bandwidth
 * @BUS_BW_LEVEL_2: vote for level-2 bus bandwidth
 * @BUS_BW_LEVEL_3: vote for level-3 bus bandwidth
 * @BUS_BW_LEVEL_4: vote for level-4 bus bandwidth
 * @BUS_BW_LEVEL_5: vote for level-5 bus bandwidth
 * @BUS_BW_LEVEL_6: vote for level-6 bus bandwidth
 * @BUS_BW_LEVEL_7: vote for level-7 bus bandwidth
 */
enum bus_bw_level {
	BUS_BW_LEVEL_NONE,
	BUS_BW_LEVEL_1,
	BUS_BW_LEVEL_2,
	BUS_BW_LEVEL_3,
	BUS_BW_LEVEL_4,
	BUS_BW_LEVEL_5,
	BUS_BW_LEVEL_6,
	BUS_BW_LEVEL_7,
	BUS_BW_LEVEL_8,
	BUS_BW_LEVEL_MAX,
};

#define BUS_BW_LEVEL_RESUME BUS_BW_LEVEL_3

/**
 * enum tput_level - throughput levels
 *
 * @TPUT_LEVEL_NONE: No throughput
 * @TPUT_LEVEL_IDLE: idle throughtput level
 * @TPUT_LEVEL_LOW: low throughput level
 * @TPUT_LEVEL_MEDIUM: medium throughtput level
 * @TPUT_LEVEL_HIGH: high throughput level
 * @TPUT_LEVEL_VERY_HIGH: very high throughput level
 * @TPUT_LEVEL_ULTRA_HIGH: ultra high throughput level
 * @TPUT_LEVEL_SUPER_HIGH: super high throughput level
 */
enum tput_level {
	TPUT_LEVEL_NONE,
	TPUT_LEVEL_IDLE,
	TPUT_LEVEL_LOW,
	TPUT_LEVEL_MEDIUM,
	TPUT_LEVEL_HIGH,
	TPUT_LEVEL_VERY_HIGH,
	TPUT_LEVEL_ULTRA_HIGH,
	TPUT_LEVEL_SUPER_HIGH,
	TPUT_LEVEL_MAX,
};

/**
 * enum bbm_non_per_flag - Non persistent policy related flag
 *
 * @BBM_APPS_RESUME: system resume flag
 * @BBM_APPS_SUSPEND: system suspend flag
 */
enum bbm_non_per_flag {
	BBM_APPS_RESUME,
	BBM_APPS_SUSPEND,
	BBM_FLAG_MAX,
};

/**
 * enum bbm_policy - BBM policy
 *
 * @BBM_DRIVER_MODE_POLICY: driver mode policy
 * @BBM_TPUT_POLICY: throughput policy
 * @BBM_USER_POLICY: user policy
 * @BBM_NON_PERSISTENT_POLICY: non persistent policy. For example, bus resume
 *  sets the bus bw level to LEVEL_3 if any adapter is connected but
 *  this is only a one time setting and is not persistent. This bus bw level
 *  is set without taking other policy vote levels into consideration.
 * @BBM_SELECT_TABLE_POLICY: policy where bus bw table is selected based on
 *  the latency level.
 */
enum bbm_policy {
	BBM_DRIVER_MODE_POLICY,
	BBM_TPUT_POLICY,
	BBM_USER_POLICY,
	BBM_NON_PERSISTENT_POLICY,
	BBM_SELECT_TABLE_POLICY,
	BBM_MAX_POLICY,
};

/**
 * enum wlm_ll_level - WLM latency levels
 *
 * @WLM_LL_NORMAL: normal latency level
 * @WLM_LL_LOW: low latency level
 */
enum wlm_ll_level {
	WLM_LL_NORMAL,
	WLM_LL_LOW,
	WLM_LL_MAX,
};

/**
 * union bbm_policy_info - BBM policy specific info. Only one of the value
 *  would be valid based on the BBM policy.
 *
 * @driver_mode: global driver mode. valid for BBM_DRIVER_MODE_POLICY.
 * @flag: BBM non persistent flag. valid for BBM_NON_PERSISTENT_POLICY.
 * @tput_level: throughput level. valid for BBM_TPUT_POLICY.
 * @wlm_level: latency level. valid for BBM_WLM_POLICY.
 * @user_level: user bus bandwidth vote. valid for BBM_USER_POLICY.
 * @set: set or reset user level. valid for BBM_USER_POLICY.
 */
union bbm_policy_info {
	enum QDF_GLOBAL_MODE driver_mode;
	enum bbm_non_per_flag flag;
	enum tput_level tput_level;
	enum wlm_ll_level wlm_level;
	struct {
		enum bus_bw_level user_level;
		bool set;
	} usr;
};

/**
 * struct bbm_params - BBM params
 *
 * @policy: BBM policy
 * @policy_info: policy related info
 */
struct bbm_params {
	enum bbm_policy policy;
	union bbm_policy_info policy_info;
};

/**
 * wlan_tp_data : union of TCP msg for Tx and Rx Dir
 */
union wlan_tp_data {
	struct wlan_tx_tp_data tx_tp_data;
	struct wlan_rx_tp_data rx_tp_data;
};

/**
 * struct wlan_dp_psoc_callbacks - struct containing callback
 * to non-converged driver
 * @os_if_dp_gro_rx: OS IF Callback to handle GRO packet to n/w stack
 * @callback_ctx : Opaque callback context
 * @wlan_dp_sta_get_dot11mode: Callback to get dot11 mode
 * @wlan_dp_get_ap_client_count: Callback to get client count connected to AP
 * @wlan_dp_sta_ndi_connected: Callback to get NDI connected status
 * @dp_any_adapter_connected: Callback to check if any adapter is connected
 * @dp_send_svc_nlink_msg: Callback API to send svc nlink message
 * @osif_dp_send_tcp_param_update_event: OS IF callback to send TCP param
 * @dp_send_mscs_action_frame: Callback to send MSCS action frame
 * @dp_pm_qos_add_request: Callback to send add pm qos request
 * @dp_pm_qos_remove_request: Callback to send remove pm qos request
 * @dp_pm_qos_update_request: Callback to send update pm qos request
 * @dp_pld_remove_pm_qos: Callback to send remove pld pm qos request
 * @dp_pld_request_pm_qos: Callback to send pld pm qos request
 * @dp_pktlog_enable_disable:Callback to set packet log
 * @dp_is_roaming_in_progress:Callback to check if roaming is in progress
 * @dp_is_ap_active:Callback to check if AP is active
 * @dp_napi_apply_throughput_policy:Callback to apply NAPI throughput policy
 * @wlan_dp_display_tx_multiq_stats: Callback to display Tx Mulit queue stats
 * @wlan_dp_display_netif_queue_history: Callback to display Netif queue history
 * @osif_dp_process_sta_mic_error: osif callback to process STA MIC error
 * @osif_dp_process_sap_mic_error: osif callback to process SAP MIC error
 */
struct wlan_dp_psoc_callbacks {
	void (*os_if_dp_gro_rx)(struct sk_buff *skb, uint8_t napi_to_use,
				bool flush_gro);
	hdd_cb_handle callback_ctx;

	bool
	(*wlan_dp_sta_get_dot11mode)(hdd_cb_handle context, uint8_t vdev_id,
				     enum qca_wlan_802_11_mode *dot11_mode);
	bool (*wlan_dp_get_ap_client_count)(hdd_cb_handle context,
					    uint8_t vdev_id,
					    uint16_t *client_count);
	bool (*wlan_dp_sta_ndi_connected)(hdd_cb_handle context,
					  uint8_t vdev_id);
	bool (*dp_any_adapter_connected)(hdd_cb_handle context);
	void (*dp_send_svc_nlink_msg)(int radio, int type, void *data, int len);

	void
	(*osif_dp_send_tcp_param_update_event)(struct wlan_objmgr_psoc *psoc,
					       union wlan_tp_data *data,
					       uint8_t dir);
	void (*dp_send_mscs_action_frame)(hdd_cb_handle context,
					  uint8_t vdev_id);
	void (*dp_pm_qos_add_request)(hdd_cb_handle context);
	void (*dp_pm_qos_remove_request)(hdd_cb_handle context);
	void (*dp_pm_qos_update_request)(hdd_cb_handle context,
					 cpumask_t *mask);
	void (*dp_pld_remove_pm_qos)(hdd_cb_handle context);
	void (*dp_pld_request_pm_qos)(hdd_cb_handle context);
	int (*dp_pktlog_enable_disable)(hdd_cb_handle context,
					bool enable_disable_flag,
					uint8_t user_triggered, int size);
	bool (*dp_is_roaming_in_progress)(hdd_cb_handle context);
	bool (*dp_is_ap_active)(hdd_cb_handle context, uint8_t vdev_id);
	void (*dp_disable_rx_ol_for_low_tput)(hdd_cb_handle context,
					      bool disable);
	int (*dp_napi_apply_throughput_policy)(hdd_cb_handle context,
					       uint64_t tx_packets,
					       uint64_t rx_packets);
	void (*wlan_dp_display_tx_multiq_stats)(hdd_cb_handle context,
						uint8_t vdev_id);
	void (*wlan_dp_display_netif_queue_history)(hdd_cb_handle context,
				enum qdf_stats_verbosity_level verb_lvl);
	void (*osif_dp_process_sta_mic_error)(struct dp_mic_error_info *info,
					      struct wlan_objmgr_vdev *vdev);
	void (*osif_dp_process_sap_mic_error)(struct dp_mic_error_info *info,
					      struct wlan_objmgr_vdev *vdev);
};

/**
 * struct wlan_dp_psoc_sb_ops - struct containing callback
 * to south bound APIs. callbacks to call traget_if APIs
 */
struct wlan_dp_psoc_sb_ops {
	/*TODO to add target if TX ops*/
};

/**
 * struct wlan_dp_psoc_nb_ops - struct containing callback
 * to north bound APIs. callbacks APIs to be called by target_if APIs
 */
struct wlan_dp_psoc_nb_ops {
	/*TODO to add target if RX ops*/
};

/**
 * struct wlan_dp_user_config - DP component user config
 * @ipa_enable: IPA enabled/disabled config
 */
struct wlan_dp_user_config {
	bool ipa_enable;
};

#endif /* end  of _WLAN_DP_PUBLIC_STRUCT_H_ */
