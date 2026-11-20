/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#ifndef _RTW_CFGVENDOR_H_
#define _RTW_CFGVENDOR_H_

#define OUI_GOOGLE  0x001A11
#define ATTRIBUTE_U32_LEN                  (NLA_HDRLEN  + 4)
#define VENDOR_ID_OVERHEAD                 ATTRIBUTE_U32_LEN
#define VENDOR_SUBCMD_OVERHEAD             ATTRIBUTE_U32_LEN
#define VENDOR_DATA_OVERHEAD               (NLA_HDRLEN)

#define SCAN_RESULTS_COMPLETE_FLAG_LEN       ATTRIBUTE_U32_LEN
#define SCAN_INDEX_HDR_LEN                   (NLA_HDRLEN)
#define SCAN_ID_HDR_LEN                      ATTRIBUTE_U32_LEN
#define SCAN_FLAGS_HDR_LEN                   ATTRIBUTE_U32_LEN
#define GSCAN_NUM_RESULTS_HDR_LEN            ATTRIBUTE_U32_LEN
#define GSCAN_RESULTS_HDR_LEN                (NLA_HDRLEN)
#define GSCAN_BATCH_RESULT_HDR_LEN  (SCAN_INDEX_HDR_LEN + SCAN_ID_HDR_LEN + \
				     SCAN_FLAGS_HDR_LEN + \
				     GSCAN_NUM_RESULTS_HDR_LEN + \
				     GSCAN_RESULTS_HDR_LEN)

#define VENDOR_REPLY_OVERHEAD       (VENDOR_ID_OVERHEAD + \
				     VENDOR_SUBCMD_OVERHEAD + \
				     VENDOR_DATA_OVERHEAD)
typedef enum {
    /* don't use 0 as a valid subcommand */
    VENDOR_NL80211_SUBCMD_UNSPECIFIED,

    /* define all vendor startup commands between 0x0 and 0x0FFF */
    VENDOR_NL80211_SUBCMD_RANGE_START = 0x0001,
    VENDOR_NL80211_SUBCMD_RANGE_END   = 0x0FFF,

    /* define all GScan related commands between 0x1000 and 0x10FF */
    ANDROID_NL80211_SUBCMD_GSCAN_RANGE_START = 0x1000,
    ANDROID_NL80211_SUBCMD_GSCAN_RANGE_END   = 0x10FF,

    /* define all NearbyDiscovery related commands between 0x1100 and 0x11FF */
    ANDROID_NL80211_SUBCMD_NBD_RANGE_START = 0x1100,
    ANDROID_NL80211_SUBCMD_NBD_RANGE_END   = 0x11FF,

    /* define all RTT related commands between 0x1100 and 0x11FF */
    ANDROID_NL80211_SUBCMD_RTT_RANGE_START = 0x1100,
    ANDROID_NL80211_SUBCMD_RTT_RANGE_END   = 0x11FF,

    ANDROID_NL80211_SUBCMD_LSTATS_RANGE_START = 0x1200,
    ANDROID_NL80211_SUBCMD_LSTATS_RANGE_END   = 0x12FF,

    /* define all Logger related commands between 0x1400 and 0x14FF */
    ANDROID_NL80211_SUBCMD_DEBUG_RANGE_START = 0x1400,
    ANDROID_NL80211_SUBCMD_DEBUG_RANGE_END   = 0x14FF,

    /* define all wifi offload related commands between 0x1600 and 0x16FF */
    ANDROID_NL80211_SUBCMD_WIFI_OFFLOAD_RANGE_START = 0x1600,
    ANDROID_NL80211_SUBCMD_WIFI_OFFLOAD_RANGE_END   = 0x16FF,

    /* define all NAN related commands between 0x1700 and 0x17FF */
    ANDROID_NL80211_SUBCMD_NAN_RANGE_START = 0x1700,
    ANDROID_NL80211_SUBCMD_NAN_RANGE_END   = 0x17FF,

    /* define all Android Packet Filter related commands between 0x1800 and 0x18FF */
    ANDROID_NL80211_SUBCMD_PKT_FILTER_RANGE_START = 0x1800,
    ANDROID_NL80211_SUBCMD_PKT_FILTER_RANGE_END   = 0x18FF,

    /* This is reserved for future usage */

} ANDROID_VENDOR_SUB_COMMAND;

enum rtw_vendor_subcmd {
    GSCAN_SUBCMD_GET_CAPABILITIES = ANDROID_NL80211_SUBCMD_GSCAN_RANGE_START,

    GSCAN_SUBCMD_SET_CONFIG,                            /* 0x1001 */

    GSCAN_SUBCMD_SET_SCAN_CONFIG,                       /* 0x1002 */
    GSCAN_SUBCMD_ENABLE_GSCAN,                          /* 0x1003 */
    GSCAN_SUBCMD_GET_SCAN_RESULTS,                      /* 0x1004 */
    GSCAN_SUBCMD_SCAN_RESULTS,                          /* 0x1005 */

    GSCAN_SUBCMD_SET_HOTLIST,                           /* 0x1006 */

    GSCAN_SUBCMD_SET_SIGNIFICANT_CHANGE_CONFIG,         /* 0x1007 */
    GSCAN_SUBCMD_ENABLE_FULL_SCAN_RESULTS,              /* 0x1008 */
    GSCAN_SUBCMD_GET_CHANNEL_LIST,                       /* 0x1009 */

    WIFI_SUBCMD_GET_FEATURE_SET,                         /* 0x100A */
    WIFI_SUBCMD_GET_FEATURE_SET_MATRIX,                  /* 0x100B */
    WIFI_SUBCMD_SET_PNO_RANDOM_MAC_OUI,                  /* 0x100C */
    WIFI_SUBCMD_NODFS_SET,                               /* 0x100D */
    WIFI_SUBCMD_SET_COUNTRY_CODE,                             /* 0x100E */
    /* Add more sub commands here */
    GSCAN_SUBCMD_SET_EPNO_SSID,                          /* 0x100F */

    WIFI_SUBCMD_SET_SSID_WHITE_LIST,                    /* 0x1010 */
    WIFI_SUBCMD_SET_ROAM_PARAMS,                        /* 0x1011 */
    WIFI_SUBCMD_ENABLE_LAZY_ROAM,                       /* 0x1012 */
    WIFI_SUBCMD_SET_BSSID_PREF,                         /* 0x1013 */
    WIFI_SUBCMD_SET_BSSID_BLACKLIST,                     /* 0x1014 */

    GSCAN_SUBCMD_ANQPO_CONFIG,                          /* 0x1015 */
    WIFI_SUBCMD_SET_RSSI_MONITOR,                       /* 0x1016 */
    WIFI_SUBCMD_CONFIG_ND_OFFLOAD,                      /* 0x1017 */
    /* Add more sub commands here */

    GSCAN_SUBCMD_MAX,

	RTT_SUBCMD_SET_CONFIG = ANDROID_NL80211_SUBCMD_RTT_RANGE_START,
	RTT_SUBCMD_CANCEL_CONFIG,
	RTT_SUBCMD_GETCAPABILITY,

    APF_SUBCMD_GET_CAPABILITIES = ANDROID_NL80211_SUBCMD_PKT_FILTER_RANGE_START,
    APF_SUBCMD_SET_FILTER,
    
    LOGGER_START_LOGGING = ANDROID_NL80211_SUBCMD_DEBUG_RANGE_START,
    LOGGER_TRIGGER_MEM_DUMP,
    LOGGER_GET_MEM_DUMP,
    LOGGER_GET_VER,
    LOGGER_GET_RING_STATUS,
    LOGGER_GET_RING_DATA,
    LOGGER_GET_FEATURE,
    LOGGER_RESET_LOGGING,
    LOGGER_TRIGGER_DRIVER_MEM_DUMP,
    LOGGER_GET_DRIVER_MEM_DUMP,
    LOGGER_START_PKT_FATE_MONITORING,
    LOGGER_GET_TX_PKT_FATES,
    LOGGER_GET_RX_PKT_FATES,

	VENDOR_SUBCMD_MAX
};

enum gscan_attributes {
	GSCAN_ATTRIBUTE_NUM_BUCKETS = 10,
	GSCAN_ATTRIBUTE_BASE_PERIOD,
	GSCAN_ATTRIBUTE_BUCKETS_BAND,
	GSCAN_ATTRIBUTE_BUCKET_ID,
	GSCAN_ATTRIBUTE_BUCKET_PERIOD,
	GSCAN_ATTRIBUTE_BUCKET_NUM_CHANNELS,
	GSCAN_ATTRIBUTE_BUCKET_CHANNELS,
	GSCAN_ATTRIBUTE_NUM_AP_PER_SCAN,
	GSCAN_ATTRIBUTE_REPORT_THRESHOLD,
	GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE,
	GSCAN_ATTRIBUTE_BAND = GSCAN_ATTRIBUTE_BUCKETS_BAND,

	GSCAN_ATTRIBUTE_ENABLE_FEATURE = 20,
	GSCAN_ATTRIBUTE_SCAN_RESULTS_COMPLETE,
	GSCAN_ATTRIBUTE_FLUSH_FEATURE,
	GSCAN_ATTRIBUTE_ENABLE_FULL_SCAN_RESULTS,
	GSCAN_ATTRIBUTE_REPORT_EVENTS,
	/* remaining reserved for additional attributes */
	GSCAN_ATTRIBUTE_NUM_OF_RESULTS = 30,
	GSCAN_ATTRIBUTE_FLUSH_RESULTS,
	GSCAN_ATTRIBUTE_SCAN_RESULTS,                       /* flat array of wifi_scan_result */
	GSCAN_ATTRIBUTE_SCAN_ID,                            /* indicates scan number */
	GSCAN_ATTRIBUTE_SCAN_FLAGS,                         /* indicates if scan was aborted */
	GSCAN_ATTRIBUTE_AP_FLAGS,                           /* flags on significant change event */
	GSCAN_ATTRIBUTE_NUM_CHANNELS,
	GSCAN_ATTRIBUTE_CHANNEL_LIST,

	/* remaining reserved for additional attributes */

	GSCAN_ATTRIBUTE_SSID = 40,
	GSCAN_ATTRIBUTE_BSSID,
	GSCAN_ATTRIBUTE_CHANNEL,
	GSCAN_ATTRIBUTE_RSSI,
	GSCAN_ATTRIBUTE_TIMESTAMP,
	GSCAN_ATTRIBUTE_RTT,
	GSCAN_ATTRIBUTE_RTTSD,

	/* remaining reserved for additional attributes */

	GSCAN_ATTRIBUTE_HOTLIST_BSSIDS = 50,
	GSCAN_ATTRIBUTE_RSSI_LOW,
	GSCAN_ATTRIBUTE_RSSI_HIGH,
	GSCAN_ATTRIBUTE_HOSTLIST_BSSID_ELEM,
	GSCAN_ATTRIBUTE_HOTLIST_FLUSH,

	/* remaining reserved for additional attributes */
	GSCAN_ATTRIBUTE_RSSI_SAMPLE_SIZE = 60,
	GSCAN_ATTRIBUTE_LOST_AP_SAMPLE_SIZE,
	GSCAN_ATTRIBUTE_MIN_BREACHING,
	GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_BSSIDS,
	GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH,
	GSCAN_ATTRIBUTE_MAX
};

enum gscan_bucket_attributes {
	GSCAN_ATTRIBUTE_CH_BUCKET_1,
	GSCAN_ATTRIBUTE_CH_BUCKET_2,
	GSCAN_ATTRIBUTE_CH_BUCKET_3,
	GSCAN_ATTRIBUTE_CH_BUCKET_4,
	GSCAN_ATTRIBUTE_CH_BUCKET_5,
	GSCAN_ATTRIBUTE_CH_BUCKET_6,
	GSCAN_ATTRIBUTE_CH_BUCKET_7
};

enum gscan_ch_attributes {
	GSCAN_ATTRIBUTE_CH_ID_1,
	GSCAN_ATTRIBUTE_CH_ID_2,
	GSCAN_ATTRIBUTE_CH_ID_3,
	GSCAN_ATTRIBUTE_CH_ID_4,
	GSCAN_ATTRIBUTE_CH_ID_5,
	GSCAN_ATTRIBUTE_CH_ID_6,
	GSCAN_ATTRIBUTE_CH_ID_7
};

enum wifi_rssi_monitor_attr {
	RSSI_MONITOR_ATTRIBUTE_MAX_RSSI,
	RSSI_MONITOR_ATTRIBUTE_MIN_RSSI,
	RSSI_MONITOR_ATTRIBUTE_START,
};


enum rtt_attributes {
	RTT_ATTRIBUTE_TARGET_CNT,
	RTT_ATTRIBUTE_TARGET_INFO,
	RTT_ATTRIBUTE_TARGET_MAC,
	RTT_ATTRIBUTE_TARGET_TYPE,
	RTT_ATTRIBUTE_TARGET_PEER,
	RTT_ATTRIBUTE_TARGET_CHAN,
	RTT_ATTRIBUTE_TARGET_MODE,
	RTT_ATTRIBUTE_TARGET_INTERVAL,
	RTT_ATTRIBUTE_TARGET_NUM_MEASUREMENT,
	RTT_ATTRIBUTE_TARGET_NUM_PKT,
	RTT_ATTRIBUTE_TARGET_NUM_RETRY
};

enum logger_attributes {
	LOGGER_ATTRIBUTE_GET_DRIVER,
	LOGGER_ATTRIBUTE_GET_FW,
	LOGGER_ATTRIBUTE_RING_ID,
	LOGGER_ATTRIBUTE_RING_NAME,
	LOGGER_ATTRIBUTE_RING_FLAGS,
	LOGGER_ATTRIBUTE_LOG_LEVEL,
	LOGGER_ATTRIBUTE_LOG_TIME_INTVAL,
	LOGGER_ATTRIBUTE_LOG_MIN_DATA_SIZE,
	LOGGER_ATTRIBUTE_FW_DUMP_LEN,
	LOGGER_ATTRIBUTE_FW_DUMP_DATA,
	LOGGERG_ATTRIBUTE_RING_DATA,
	LOGGER_ATTRIBUTE_RING_STATUS,
	LOGGER_ATTRIBUTE_RING_NUM
};
typedef enum rtw_vendor_event {
    RTK_RESERVED1,
    RTK_RESERVED2,
    GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS ,
    GSCAN_EVENT_HOTLIST_RESULTS_FOUND,
    GSCAN_EVENT_SCAN_RESULTS_AVAILABLE,
    GSCAN_EVENT_FULL_SCAN_RESULTS,
    RTT_EVENT_COMPLETE,
    GSCAN_EVENT_COMPLETE_SCAN,
    GSCAN_EVENT_HOTLIST_RESULTS_LOST,
    GSCAN_EVENT_EPNO_EVENT,
    GOOGLE_DEBUG_RING_EVENT,
    GOOGLE_DEBUG_MEM_DUMP_EVENT,
    GSCAN_EVENT_ANQPO_HOTSPOT_MATCH,
    GOOGLE_RSSI_MONITOR_EVENT
} rtw_vendor_event_t;

enum andr_wifi_feature_set_attr {
	ANDR_WIFI_ATTRIBUTE_NUM_FEATURE_SET,
	ANDR_WIFI_ATTRIBUTE_FEATURE_SET,
	ANDR_WIFI_ATTRIBUTE_RANDOM_MAC_OUI,
	ANDR_WIFI_ATTRIBUTE_NODFS_SET,
	ANDR_WIFI_ATTRIBUTE_COUNTRY,
	ANDR_WIFI_ATTRIBUTE_ND_OFFLOAD_VALUE
	// Add more attribute here
};

typedef enum rtw_vendor_gscan_attribute {
	ATTR_START_GSCAN,
	ATTR_STOP_GSCAN,
	ATTR_SET_SCAN_BATCH_CFG_ID, /* set batch scan params */
	ATTR_SET_SCAN_GEOFENCE_CFG_ID, /* set list of bssids to track */
	ATTR_SET_SCAN_SIGNIFICANT_CFG_ID, /* set list of bssids, rssi threshold etc.. */
	ATTR_SET_SCAN_CFG_ID, /* set common scan config params here */
	ATTR_GET_GSCAN_CAPABILITIES_ID,
	/* Add more sub commands here */
	ATTR_GSCAN_MAX
} rtw_vendor_gscan_attribute_t;

typedef enum gscan_batch_attribute {
	ATTR_GSCAN_BATCH_BESTN,
	ATTR_GSCAN_BATCH_MSCAN,
	ATTR_GSCAN_BATCH_BUFFER_THRESHOLD
} gscan_batch_attribute_t;

typedef enum gscan_geofence_attribute {
	ATTR_GSCAN_NUM_HOTLIST_BSSID,
	ATTR_GSCAN_HOTLIST_BSSID
} gscan_geofence_attribute_t;

typedef enum gscan_complete_event {
	WIFI_SCAN_BUFFER_FULL,
	WIFI_SCAN_COMPLETE
} gscan_complete_event_t;
/* wifi_hal.h */
/* WiFi Common definitions */
typedef unsigned char byte;
typedef int wifi_request_id;
typedef int wifi_channel;                       // indicates channel frequency in MHz
typedef int wifi_rssi;
typedef byte mac_addr[6];
typedef byte oui[3];
typedef int64_t wifi_timestamp;                 // In microseconds (us)
typedef int64_t wifi_timespan;                  // In picoseconds  (ps)

struct wifi_info;
struct wifi_interface_info;
typedef struct wifi_info *wifi_handle;
typedef struct wifi_interface_info *wifi_interface_handle;

/* channel operating width */
typedef enum {
    WIFI_CHAN_WIDTH_20    = 0,
    WIFI_CHAN_WIDTH_40    = 1,
    WIFI_CHAN_WIDTH_80    = 2,
    WIFI_CHAN_WIDTH_160   = 3,
    WIFI_CHAN_WIDTH_80P80 = 4,
    WIFI_CHAN_WIDTH_5     = 5,
    WIFI_CHAN_WIDTH_10    = 6,
    WIFI_CHAN_WIDTH_INVALID = -1
} wifi_channel_width;

typedef int wifi_radio;

typedef struct {
    wifi_channel_width width;
    int center_frequency0;
    int center_frequency1;
    int primary_frequency;
} wifi_channel_spec;

typedef enum {
    WIFI_SUCCESS = 0,
    WIFI_ERROR_NONE = 0,
    WIFI_ERROR_UNKNOWN = -1,
    WIFI_ERROR_UNINITIALIZED = -2,
    WIFI_ERROR_NOT_SUPPORTED = -3,
    WIFI_ERROR_NOT_AVAILABLE = -4,              // Not available right now, but try later
    WIFI_ERROR_INVALID_ARGS = -5,
    WIFI_ERROR_INVALID_REQUEST_ID = -6,
    WIFI_ERROR_TIMED_OUT = -7,
    WIFI_ERROR_TOO_MANY_REQUESTS = -8,          // Too many instances of this request
    WIFI_ERROR_OUT_OF_MEMORY = -9,
    WIFI_ERROR_BUSY = -10,
} wifi_error;

typedef int wifi_ring_buffer_id;
/* ring buffer params */
/**
 * written_bytes and read_bytes implement a producer consumer API
 *     hence written_bytes >= read_bytes
 * a modulo arithmetic of the buffer size has to be applied to those counters:
 * actual offset into ring buffer = written_bytes % ring_buffer_byte_size
 *
 */
typedef struct {
    u8 name[32];
    u32 flags;
    wifi_ring_buffer_id ring_id; // unique integer representing the ring
    u32 ring_buffer_byte_size;   // total memory size allocated for the buffer
    u32 verbose_level;           // verbose level for ring buffer
    u32 written_bytes;           // number of bytes that was written to the buffer by driver,
                                 // monotonously increasing integer
    u32 read_bytes;              // number of bytes that was read from the buffer by user land,
                                 // monotonously increasing integer
    u32 written_records;         // number of records that was written to the buffer by driver,
                                 // monotonously increasing integer
} wifi_ring_buffer_status;



extern int rtw_cfgvendor_attach(struct wiphy *wiphy);
extern int rtw_cfgvendor_detach(struct wiphy *wiphy);
extern int rtw_cfgvendor_send_async_event(struct wiphy *wiphy,
	struct net_device *dev, int event_id, const void  *data, int len);




#endif /* _RTW_CFGVENDOR_H_ */
