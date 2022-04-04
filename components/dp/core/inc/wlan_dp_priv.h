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
  * DOC: Declare various struct, macros which are used for private to DP.
  *
  * Note: This file shall not contain public API's prototype/declarations.
  *
  */

#ifndef _WLAN_DP_PRIV_STRUCT_H_
#define _WLAN_DP_PRIV_STRUCT_H_

#include "wlan_dp_public_struct.h"
#include "cdp_txrx_cmn.h"
#include "wlan_dp_cfg.h"
#include "wlan_dp_objmgr.h"
#include <cdp_txrx_misc.h>
#include "qdf_periodic_work.h"

#ifndef NUM_CPUS
#ifdef QCA_CONFIG_SMP
#define NUM_CPUS NR_CPUS
#else
#define NUM_CPUS 1
#endif
#endif

#ifndef NUM_TX_RX_HISTOGRAM
#define NUM_TX_RX_HISTOGRAM 128
#endif

#define NUM_TX_RX_HISTOGRAM_MASK (NUM_TX_RX_HISTOGRAM - 1)

#if defined(WLAN_FEATURE_DP_BUS_BANDWIDTH) && defined(FEATURE_RUNTIME_PM)
/**
 * enum dp_rtpm_tput_policy_state - states to track runtime_pm tput policy
 * @RTPM_TPUT_POLICY_STATE_INVALID: invalid state
 * @RTPM_TPUT_POLICY_STATE_REQUIRED: state indicating runtime_pm is required
 * @RTPM_TPUT_POLICY_STATE_NOT_REQUIRED: state indicating runtime_pm is NOT
 * required
 */
enum dp_rtpm_tput_policy_state {
	DP_RTPM_TPUT_POLICY_STATE_INVALID,
	DP_RTPM_TPUT_POLICY_STATE_REQUIRED,
	DP_RTPM_TPUT_POLICY_STATE_NOT_REQUIRED
};

/**
 * struct dp_rtpm_tput_policy_context - RTPM throughput policy context
 * @curr_state: current state of throughput policy (RTPM require or not)
 * @wake_lock: wakelock for QDF wake_lock acquire/release APIs
 * @rtpm_lock: lock use for QDF rutime PM prevent/allow APIs
 * @high_tput_vote: atomic variable to keep track of voting
 */
struct dp_rtpm_tput_policy_context {
	enum dp_rtpm_tput_policy_state curr_state;
	qdf_wake_lock_t wake_lock;
	qdf_runtime_lock_t rtpm_lock;
	qdf_atomic_t high_tput_vote;
};
#endif

/**
 * struct wlan_dp_psoc_cfg - DP configuration parameters.
 */
struct wlan_dp_psoc_cfg {
	bool tx_orphan_enable;

	uint32_t rx_mode;
	uint32_t tx_comp_loop_pkt_limit;
	uint32_t rx_reap_loop_pkt_limit;
	uint32_t rx_hp_oos_update_limit;
	uint64_t rx_softirq_max_yield_duration_ns;
#ifdef WLAN_FEATURE_PERIODIC_STA_STATS
	/* Periodicity of logging */
	uint32_t periodic_stats_timer_interval;
	/* Duration for which periodic logging should be done */
	uint32_t periodic_stats_timer_duration;
#endif /* WLAN_FEATURE_PERIODIC_STA_STATS */
#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
	/* bandwidth threshold for ultra high bandwidth */
	uint32_t bus_bw_ultra_high_threshold;
	/* bandwidth threshold for very high bandwidth */
	uint32_t bus_bw_very_high_threshold;
	/* bandwidth threshold for DBS mode bandwidth */
	uint32_t bus_bw_dbs_threshold;
	/* bandwidth threshold for high bandwidth */
	uint32_t bus_bw_high_threshold;
	/* bandwidth threshold for medium bandwidth */
	uint32_t bus_bw_medium_threshold;
	/* bandwidth threshold for low bandwidth */
	uint32_t bus_bw_low_threshold;
	uint32_t bus_bw_compute_interval;
	uint32_t enable_tcp_delack;
	bool     enable_tcp_limit_output;
	uint32_t enable_tcp_adv_win_scale;
	uint32_t tcp_delack_thres_high;
	uint32_t tcp_delack_thres_low;
	uint32_t tcp_tx_high_tput_thres;
	uint32_t tcp_delack_timer_count;
	bool     enable_tcp_param_update;
	uint32_t bus_low_cnt_threshold;
	bool enable_latency_crit_clients;
#endif /*WLAN_FEATURE_DP_BUS_BANDWIDTH*/

#ifdef WLAN_FEATURE_MSCS
	uint32_t mscs_pkt_threshold;
	uint32_t mscs_voice_interval;
#endif /* WLAN_FEATURE_MSCS */

#ifdef QCA_SUPPORT_TXRX_DRIVER_TCP_DEL_ACK
	bool del_ack_enable;
	uint32_t del_ack_threshold_high;
	uint32_t del_ack_threshold_low;
	uint16_t del_ack_timer_value;
	uint16_t del_ack_pkt_count;
#endif
	uint32_t napi_cpu_affinity_mask;
	/* CPU affinity mask for rx_thread */
	uint32_t rx_thread_ul_affinity_mask;
	uint32_t rx_thread_affinity_mask;
	uint8_t cpu_map_list[CFG_DP_RPS_RX_QUEUE_CPU_MAP_LIST_LEN];
	bool multicast_replay_filter;
	uint32_t rx_wakelock_timeout;
	uint8_t num_dp_rx_threads;
#ifdef CONFIG_DP_TRACE
	bool enable_dp_trace;
	uint8_t dp_trace_config[DP_TRACE_CONFIG_STRING_LENGTH];
#endif
	uint8_t enable_nud_tracking;

#ifdef WLAN_SUPPORT_TXRX_HL_BUNDLE
	uint32_t pkt_bundle_threshold_high;
	uint32_t pkt_bundle_threshold_low;
	uint16_t pkt_bundle_timer_value;
	uint16_t pkt_bundle_size;
#endif
	uint32_t dp_proto_event_bitmap;
	uint32_t fisa_enable;

	int icmp_req_to_fw_mark_interval;

	uint32_t cfg_wmi_credit_cnt;
};

/**
 * struct tx_rx_histogram - structure to keep track of tx and rx packets
 *				received over 100ms intervals
 * @interval_rx:	# of rx packets received in the last 100ms interval
 * @interval_tx:	# of tx packets received in the last 100ms interval
 * @next_vote_level:	pld_bus_width_type voting level (high or low)
 *			determined on the basis of total tx and rx packets
 *			received in the last 100ms interval
 * @next_rx_level:	pld_bus_width_type voting level (high or low)
 *			determined on the basis of rx packets received in the
 *			last 100ms interval
 * @next_tx_level:	pld_bus_width_type voting level (high or low)
 *			determined on the basis of tx packets received in the
 *			last 100ms interval
 * @is_rx_pm_qos_high	Capture rx_pm_qos voting
 * @is_tx_pm_qos_high	Capture tx_pm_qos voting
 * @qtime		timestamp when the record is added
 *
 * The structure keeps track of throughput requirements of wlan driver.
 * An entry is added if either of next_vote_level, next_rx_level or
 * next_tx_level changes. An entry is not added for every 100ms interval.
 */
struct tx_rx_histogram {
	uint64_t interval_rx;
	uint64_t interval_tx;
	uint32_t next_vote_level;
	uint32_t next_rx_level;
	uint32_t next_tx_level;
	bool is_rx_pm_qos_high;
	bool is_tx_pm_qos_high;
	uint64_t qtime;
};

struct dp_tx_rx_stats {
	struct {
		/* start_xmit stats */
		__u32    tx_called;
		__u32    tx_dropped;
		__u32    tx_orphaned;
		__u32    tx_classified_ac[WLAN_MAX_AC];
		__u32    tx_dropped_ac[WLAN_MAX_AC];
#ifdef TX_MULTIQ_PER_AC
		/* Neither valid socket nor skb->hash */
		uint32_t inv_sk_and_skb_hash;
		/* skb->hash already calculated */
		uint32_t qselect_existing_skb_hash;
		/* valid tx queue id in socket */
		uint32_t qselect_sk_tx_map;
		/* skb->hash calculated in select queue */
		uint32_t qselect_skb_hash_calc;
#endif
		/* rx stats */
		__u32 rx_packets;
		__u32 rx_dropped;
		__u32 rx_delivered;
		__u32 rx_refused;
	} per_cpu[NUM_CPUS];

	qdf_atomic_t rx_usolict_arp_n_mcast_drp;

	/* rx gro */
	__u32 rx_aggregated;
	__u32 rx_gro_dropped;
	__u32 rx_non_aggregated;
	__u32 rx_gro_flush_skip;
	__u32 rx_gro_low_tput_flush;

	/* txflow stats */
	bool     is_txflow_paused;
	__u32    txflow_pause_cnt;
	__u32    txflow_unpause_cnt;
	__u32    txflow_timer_cnt;

	/*tx timeout stats*/
	__u32 tx_timeout_cnt;
	__u32 cont_txtimeout_cnt;
	u64 jiffies_last_txtimeout;
};

/**
 * struct dp_stats - DP stats
 * @tx_rx_stats : Tx/Rx debug stats
 */
struct dp_stats {
	struct dp_tx_rx_stats tx_rx_stats;
};

/**
 * struct wlan_dp_intf - DP interface object related info
 * @dp_ctx: DP context reference
 * @mac_addr: Device MAC address
 * @device_mode: Device Mode
 * @intf_id: Interface ID
 * @node: list node for membership in the interface list
 * @tx_rx_disallow_mask: TX/RX disallow mask
 * @vdev: object manager vdev context
 * @dev: netdev reference
 */
struct wlan_dp_intf {
	struct wlan_dp_psoc_context *dp_ctx;

	struct qdf_mac_addr mac_addr;

	enum QDF_OPMODE device_mode;

	uint8_t intf_id;

	qdf_list_node_t node;

	uint32_t tx_rx_disallow_mask;
	struct wlan_objmgr_vdev *vdev;
	qdf_netdev_t dev;
	/**Device TX/RX statistics*/
	struct dp_stats dp_stats;
#ifdef WLAN_FEATURE_PERIODIC_STA_STATS
	/* Indicate whether to display sta periodic stats */
	bool is_sta_periodic_stats_enabled;
	uint16_t periodic_stats_timer_count;
	uint32_t periodic_stats_timer_counter;
	qdf_mutex_t sta_periodic_stats_lock;
#endif /* WLAN_FEATURE_PERIODIC_STA_STATS */
#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
	unsigned long prev_rx_packets;
	unsigned long prev_tx_packets;
	unsigned long prev_tx_bytes;
	uint64_t prev_fwd_tx_packets;
	uint64_t prev_fwd_rx_packets;
#endif /*WLAN_FEATURE_DP_BUS_BANDWIDTH*/
#ifdef WLAN_FEATURE_MSCS
	unsigned long mscs_prev_tx_vo_pkts;
	uint32_t mscs_counter;
#endif /* WLAN_FEATURE_MSCS */

};

/**
 * struct wlan_dp_psoc_context - psoc related data required for DP
 * @pdev: object manager pdev context
 * @dp_cfg: place holder for DP configuration
 * @intf_list: DP interfaces list
 * @cb_obj: DP callbacks registered from other modules
 * @sb_ops: South bound direction call backs registered in DP
 * @nb_ops: North bound direction call backs registered in DP
 * @bus_bw_work: work for periodically computing DDR bus bandwidth requirements
 * @cur_vote_level: Current vote level
 * @bus_bw_lock: Bus bandwidth work lock
 * @cur_rx_level: Current Rx level
 * @rtpm_tput_policy_ctx: Runtime Tput policy context
 * @txrx_hist: TxRx histogram
 * @bbm_ctx: bus bandwidth manager context
 */
struct wlan_dp_psoc_context {
	struct wlan_objmgr_psoc *psoc;
	struct wlan_objmgr_pdev *pdev;
	/** Pointer to the qdf device */
	qdf_device_t qdf_dev;
	/** Config values read from ini file */
	struct wlan_dp_psoc_cfg dp_cfg;

	qdf_spinlock_t intf_list_lock;
	qdf_list_t intf_list;

	bool rps;
	bool dynamic_rps;
	bool enable_rxthread;
	/* support for DP RX threads */
	bool enable_dp_rx_threads;
	bool napi_enable;

	struct wlan_dp_psoc_callbacks dp_ops;
	struct wlan_dp_psoc_sb_ops sb_ops;
	struct wlan_dp_psoc_nb_ops nb_ops;

	bool en_tcp_delack_no_lro;
	/* For Rx thread non GRO/LRO packet accounting */
	uint64_t no_rx_offload_pkt_cnt;
	uint64_t no_tx_offload_pkt_cnt;
#ifdef QCA_CONFIG_SMP
	bool is_ol_rx_thread_suspended;
#endif
	bool wlan_suspended;
	/* Flag keeps track of wiphy suspend/resume */
	bool is_wiphy_suspended;

	qdf_atomic_t num_latency_critical_clients;
	uint8_t high_bus_bw_request;
	uint64_t bw_vote_time;
#ifdef WLAN_FEATURE_DP_BUS_BANDWIDTH
	struct qdf_periodic_work bus_bw_work;
	int cur_vote_level;
	qdf_spinlock_t bus_bw_lock;
	int cur_rx_level;
	uint64_t prev_no_rx_offload_pkts;
	uint64_t prev_rx_offload_pkts;
	/* Count of non TSO packets in previous bus bw delta time */
	uint64_t prev_no_tx_offload_pkts;
	/* Count of TSO packets in previous bus bw delta time */
	uint64_t prev_tx_offload_pkts;
	int cur_tx_level;
	uint64_t prev_tx;
	qdf_atomic_t low_tput_gro_enable;
	uint32_t bus_low_vote_cnt;
#ifdef FEATURE_RUNTIME_PM
	struct dp_rtpm_tput_policy_context rtpm_tput_policy_ctx;
#endif
#endif /*WLAN_FEATURE_DP_BUS_BANDWIDTH*/
#ifdef WLAN_NS_OFFLOAD
	/* IPv6 notifier callback for handling NS offload on change in IP */
	struct notifier_block ipv6_notifier;
#endif

	uint16_t txrx_hist_idx;
	struct tx_rx_histogram *txrx_hist;

	uint32_t rx_high_ind_cnt;
#ifdef FEATURE_BUS_BANDWIDTH_MGR
	struct bbm_context *bbm_ctx;
#endif
};

#endif /* end  of _WLAN_DP_PRIV_STRUCT_H_ */
