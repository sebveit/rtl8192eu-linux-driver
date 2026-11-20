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
#ifndef __WLAN_BSSDEF_H__
#define __WLAN_BSSDEF_H__


#define MAX_IE_SZ	768



#define RTW_802_11_LENGTH_SSID         32
#define RTW_802_11_LENGTH_RATES        8
#define RTW_802_11_LENGTH_RATES_EX     16

typedef unsigned char   RTW_802_11_MAC_ADDRESS[ETH_ALEN];
typedef long    		RTW_802_11_RSSI;           /* in dBm */
typedef unsigned char   RTW_802_11_RATES[RTW_802_11_LENGTH_RATES];        /* Set of 8 data rates */
typedef unsigned char   RTW_802_11_RATES_EX[RTW_802_11_LENGTH_RATES_EX];  /* Set of 16 data rates */


typedef  u32  RTW_802_11_KEY_INDEX;
typedef unsigned long long RTW_802_11_KEY_RSC;


typedef struct _NDIS_802_11_SSID {
	u32  SsidLength;
	u8  Ssid[32];
} RTW_802_11_SSID, *PRTW_802_11_SSID;

typedef enum _NDIS_802_11_NETWORK_TYPE {
	Rtw802_11FH,
	Rtw802_11DS,
	Rtw802_11OFDM5,
	Rtw802_11OFDM24,
	Rtw802_11NetworkTypeMax    /* not a real type, defined as an upper bound */
} RTW_802_11_NETWORK_TYPE, *PRTW_802_11_NETWORK_TYPE;

typedef struct _NDIS_802_11_CONFIGURATION_FH {
	u32           Length;             /* Length of structure */
	u32           HopPattern;         /* As defined by 802.11, MSB set */
	u32           HopSet;             /* to one if non-802.11 */
	u32           DwellTime;          /* units are Kusec */
} RTW_802_11_CONFIGURATION_FH, *PRTW_802_11_CONFIGURATION_FH;


/*
	FW will only save the channel number in DSConfig.
	ODI Handler will convert the channel number to freq. number.
*/
typedef struct _NDIS_802_11_CONFIGURATION {
	u32           Length;             /* Length of structure */
	u32           BeaconPeriod;       /* units are Kusec */
	u32           ATIMWindow;         /* units are Kusec */
	u32           DSConfig;           /* channel number */
	RTW_802_11_CONFIGURATION_FH    FHConfig;
} RTW_802_11_CONFIGURATION, *PRTW_802_11_CONFIGURATION;



typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE {
	Rtw802_11IBSS,
	Rtw802_11Infrastructure,
	Rtw802_11AutoUnknown,
	Rtw802_11InfrastructureMax,     /* Not a real value, defined as upper bound */
	Rtw802_11APMode,
	Rtw802_11Monitor,
	Rtw802_11_mesh,
} RTW_802_11_NETWORK_INFRASTRUCTURE, *PRTW_802_11_NETWORK_INFRASTRUCTURE;





typedef struct _NDIS_802_11_FIXED_IEs {
	u8  Timestamp[8];
	u16  BeaconInterval;
	u16  Capabilities;
} RTW_802_11_FIXED_IEs, *PRTW_802_11_FIXED_IEs;



typedef struct _NDIS_802_11_VARIABLE_IEs {
	u8  ElementID;
	u8  Length;
	u8  data[1];
} RTW_802_11_VARIABLE_IEs, *PRTW_802_11_VARIABLE_IEs;



/*



Length is the 4 bytes multiples of the sume of
	sizeof (NDIS_802_11_MAC_ADDRESS) + 2 + sizeof (NDIS_802_11_SSID) + sizeof (ULONG)
+   sizeof (NDIS_802_11_RSSI) + sizeof (NDIS_802_11_NETWORK_TYPE) + sizeof (NDIS_802_11_CONFIGURATION)
+   sizeof (NDIS_802_11_RATES_EX) + IELength

Except the IELength, all other fields are fixed length. Therefore, we can define a marco to present the
partial sum.

*/

typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
	Rtw802_11AuthModeOpen,
	Rtw802_11AuthModeShared,
	Rtw802_11AuthModeAutoSwitch,
	Rtw802_11AuthModeWPA,
	Rtw802_11AuthModeWPAPSK,
	Rtw802_11AuthModeWPANone,
	Rtw802_11AuthModeWAPI,
	Rtw802_11AuthModeMax               /* Not a real mode, defined as upper bound */
} RTW_802_11_AUTHENTICATION_MODE, *PRTW_802_11_AUTHENTICATION_MODE;

typedef enum _NDIS_802_11_WEP_STATUS {
	Rtw802_11WEPEnabled,
	Rtw802_11Encryption1Enabled = Rtw802_11WEPEnabled,
	Rtw802_11WEPDisabled,
	Rtw802_11EncryptionDisabled = Rtw802_11WEPDisabled,
	Rtw802_11WEPKeyAbsent,
	Rtw802_11Encryption1KeyAbsent = Rtw802_11WEPKeyAbsent,
	Rtw802_11WEPNotSupported,
	Rtw802_11EncryptionNotSupported = Rtw802_11WEPNotSupported,
	Rtw802_11Encryption2Enabled,
	Rtw802_11Encryption2KeyAbsent,
	Rtw802_11Encryption3Enabled,
	Rtw802_11Encryption3KeyAbsent,
	Rtw802_11_EncrypteionWAPI
} RTW_802_11_WEP_STATUS, *PRTW_802_11_WEP_STATUS,
RTW_802_11_ENCRYPTION_STATUS, *PRTW_802_11_ENCRYPTION_STATUS;


#define RTW_802_11_AI_REQFI_CAPABILITIES      1
#define RTW_802_11_AI_REQFI_LISTENINTERVAL    2
#define RTW_802_11_AI_REQFI_CURRENTAPADDRESS  4

#define RTW_802_11_AI_RESFI_CAPABILITIES      1
#define RTW_802_11_AI_RESFI_STATUSCODE        2
#define RTW_802_11_AI_RESFI_ASSOCIATIONID     4

typedef struct _NDIS_802_11_AI_REQFI {
	u16 Capabilities;
	u16 ListenInterval;
	RTW_802_11_MAC_ADDRESS  CurrentAPAddress;
} RTW_802_11_AI_REQFI, *PRTW_802_11_AI_REQFI;

typedef struct _NDIS_802_11_AI_RESFI {
	u16 Capabilities;
	u16 StatusCode;
	u16 AssociationId;
} RTW_802_11_AI_RESFI, *PRTW_802_11_AI_RESFI;

typedef struct _NDIS_802_11_ASSOCIATION_INFORMATION {
	u32                   Length;
	u16                  AvailableRequestFixedIEs;
	RTW_802_11_AI_REQFI    RequestFixedIEs;
	u32                   RequestIELength;
	u32                   OffsetRequestIEs;
	u16                  AvailableResponseFixedIEs;
	RTW_802_11_AI_RESFI    ResponseFixedIEs;
	u32                   ResponseIELength;
	u32                   OffsetResponseIEs;
} RTW_802_11_ASSOCIATION_INFORMATION, *PRTW_802_11_ASSOCIATION_INFORMATION;

typedef enum _NDIS_802_11_RELOAD_DEFAULTS {
	Rtw802_11ReloadWEPKeys
} RTW_802_11_RELOAD_DEFAULTS, *PRTW_802_11_RELOAD_DEFAULTS;


/* Key mapping keys require a BSSID */
typedef struct _NDIS_802_11_KEY {
	u32           Length;             /* Length of this structure */
	u32           KeyIndex;
	u32           KeyLength;          /* length of key in bytes */
	RTW_802_11_MAC_ADDRESS BSSID;
	RTW_802_11_KEY_RSC KeyRSC;
	u8           KeyMaterial[32];     /* variable length depending on above field */
} RTW_802_11_KEY, *PRTW_802_11_KEY;

typedef struct _NDIS_802_11_REMOVE_KEY {
	u32                   Length;        /* Length of this structure */
	u32                   KeyIndex;
	RTW_802_11_MAC_ADDRESS BSSID;
} RTW_802_11_REMOVE_KEY, *PRTW_802_11_REMOVE_KEY;

typedef struct _NDIS_802_11_WEP {
	u32     Length;        /* Length of this structure */
	u32     KeyIndex;      /* 0 is the per-client key, 1-N are the global keys */
	u32     KeyLength;     /* length of key in bytes */
	u8     KeyMaterial[16];/* variable length depending on above field */
} RTW_802_11_WEP, *PRTW_802_11_WEP;

typedef struct _NDIS_802_11_AUTHENTICATION_REQUEST {
	u32 Length;            /* Length of structure */
	RTW_802_11_MAC_ADDRESS Bssid;
	u32 Flags;
} RTW_802_11_AUTHENTICATION_REQUEST, *PRTW_802_11_AUTHENTICATION_REQUEST;

typedef enum _NDIS_802_11_STATUS_TYPE {
	Rtw802_11StatusType_Authentication,
	Rtw802_11StatusType_MediaStreamMode,
	Rtw802_11StatusType_PMKID_CandidateList,
	Rtw802_11StatusTypeMax    /* not a real type, defined as an upper bound */
} RTW_802_11_STATUS_TYPE, *PRTW_802_11_STATUS_TYPE;

typedef struct _NDIS_802_11_STATUS_INDICATION {
	RTW_802_11_STATUS_TYPE StatusType;
} RTW_802_11_STATUS_INDICATION, *PRTW_802_11_STATUS_INDICATION;

/* mask for authentication/integrity fields */
#define RTW_802_11_AUTH_REQUEST_AUTH_FIELDS        0x0f
#define RTW_802_11_AUTH_REQUEST_REAUTH			0x01
#define RTW_802_11_AUTH_REQUEST_KEYUPDATE		0x02
#define RTW_802_11_AUTH_REQUEST_PAIRWISE_ERROR		0x06
#define RTW_802_11_AUTH_REQUEST_GROUP_ERROR		0x0E

/* MIC check time, 60 seconds. */
#define MIC_CHECK_TIME	60000000

typedef struct _NDIS_802_11_AUTHENTICATION_EVENT {
	RTW_802_11_STATUS_INDICATION       Status;
	RTW_802_11_AUTHENTICATION_REQUEST  Request[1];
} RTW_802_11_AUTHENTICATION_EVENT, *PRTW_802_11_AUTHENTICATION_EVENT;

typedef struct _NDIS_802_11_TEST {
	u32 Length;
	u32 Type;
	union {
		RTW_802_11_AUTHENTICATION_EVENT AuthenticationEvent;
		RTW_802_11_RSSI RssiTrigger;
	} tt;
} RTW_802_11_TEST, *PRTW_802_11_TEST;



#ifndef Rtw802_11APMode
#define Rtw802_11APMode (Rtw802_11InfrastructureMax+1)
#endif

typedef struct _WLAN_PHY_INFO {
	u8	SignalStrength;/* (in percentage) */
	u8	SignalQuality;/* (in percentage) */
	u8	Optimum_antenna;  /* for Antenna diversity */
	u8	is_cck_rate;	/* 1:cck_rate */
	s8	rx_snr[4];
#ifdef CONFIG_RTW_80211K
	u32	free_cnt; 	/* freerun counter */
	u8	rm_en_cap[5];
#endif
} WLAN_PHY_INFO, *PWLAN_PHY_INFO;

typedef struct _WLAN_BCN_INFO {
	/* these infor get from rtw_get_encrypt_info when
	 *	 * translate scan to UI */
	u8 encryp_protocol;/* ENCRYP_PROTOCOL_E: OPEN/WEP/WPA/WPA2/WAPI */
	int group_cipher; /* WPA/WPA2 group cipher */
	int pairwise_cipher;/* //WPA/WPA2/WEP pairwise cipher */
	int is_8021x;

	/* bwmode 20/40 and ch_offset UP/LOW */
	unsigned short	ht_cap_info;
	unsigned char	ht_info_infos_0;
} WLAN_BCN_INFO, *PWLAN_BCN_INFO;

enum bss_type {
	BSS_TYPE_UNDEF,
	BSS_TYPE_BCN = 1,
	BSS_TYPE_PROB_REQ = 2,
	BSS_TYPE_PROB_RSP = 3,
};

/* temporally add #pragma pack for structure alignment issue of
*   WLAN_BSSID_EX and get_WLAN_BSSID_EX_sz()
*/
typedef struct _WLAN_BSSID_EX {
	u32  Length;
	RTW_802_11_MAC_ADDRESS  MacAddress;
	u8  Reserved[2];/* [0]: IS beacon frame , bss_type*/
	RTW_802_11_SSID  Ssid;
	RTW_802_11_SSID  mesh_id;
	u32  Privacy;
	RTW_802_11_RSSI  Rssi;/* (in dBM,raw data ,get from PHY) */
	RTW_802_11_NETWORK_TYPE  NetworkTypeInUse;
	RTW_802_11_CONFIGURATION  Configuration;
	RTW_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
	RTW_802_11_RATES_EX  SupportedRates;
	WLAN_PHY_INFO	PhyInfo;
	u32  IELength;
	u8  IEs[MAX_IE_SZ];	/* (timestamp, beacon interval, and capability information) */
} __attribute__((packed)) WLAN_BSSID_EX, *PWLAN_BSSID_EX;

#define BSS_EX_IES(bss_ex) ((bss_ex)->IEs)
#define BSS_EX_IES_LEN(bss_ex) ((bss_ex)->IELength)
#define BSS_EX_FIXED_IE_OFFSET(bss_ex) ((bss_ex)->Reserved[0] == BSS_TYPE_PROB_REQ ? 0 : 12)
#define BSS_EX_TLV_IES(bss_ex) (BSS_EX_IES((bss_ex)) + BSS_EX_FIXED_IE_OFFSET((bss_ex)))
#define BSS_EX_TLV_IES_LEN(bss_ex) (BSS_EX_IES_LEN((bss_ex)) - BSS_EX_FIXED_IE_OFFSET((bss_ex)))

__inline  static uint get_WLAN_BSSID_EX_sz(WLAN_BSSID_EX *bss)
{
	return sizeof(WLAN_BSSID_EX) - MAX_IE_SZ + bss->IELength;
}

struct	wlan_network {
	_list	list;
	int	network_type;	/* refer to ieee80211.h for WIRELESS_11A/B/G */
	int	fixed;			/* set to fixed when not to be removed as site-surveying */
	systime last_scanned; /* timestamp for the network */
	int	aid;			/* will only be valid when a BSS is joinned. */
	int	join_res;
	WLAN_BSSID_EX	network; /* must be the last item */

};

enum VRTL_CARRIER_SENSE {
	DISABLE_VCS,
	ENABLE_VCS,
	AUTO_VCS
};

enum VCS_TYPE {
	NONE_VCS,
	RTS_CTS,
	CTS_TO_SELF
};




#define PWR_CAM 0
#define PWR_MINPS 1
#define PWR_MAXPS 2
#define PWR_UAPSD 3
#define PWR_VOIP 4


enum UAPSD_MAX_SP {
	NO_LIMIT,
	TWO_MSDU,
	FOUR_MSDU,
	SIX_MSDU
};


/* john */
#define NUM_PRE_AUTH_KEY 16
#define NUM_PMKID_CACHE NUM_PRE_AUTH_KEY

/*
*	WPA2
*/

typedef struct _PMKID_CANDIDATE {
	RTW_802_11_MAC_ADDRESS BSSID;
	u32 Flags;
} PMKID_CANDIDATE, *PPMKID_CANDIDATE;

typedef struct _NDIS_802_11_PMKID_CANDIDATE_LIST {
	u32 Version;       /* Version of the structure */
	u32 NumCandidates; /* No. of pmkid candidates */
	PMKID_CANDIDATE CandidateList[1];
} RTW_802_11_PMKID_CANDIDATE_LIST, *PRTW_802_11_PMKID_CANDIDATE_LIST;


typedef struct _NDIS_802_11_AUTHENTICATION_ENCRYPTION {
	RTW_802_11_AUTHENTICATION_MODE AuthModeSupported;
	RTW_802_11_ENCRYPTION_STATUS EncryptStatusSupported;

} RTW_802_11_AUTHENTICATION_ENCRYPTION, *PRTW_802_11_AUTHENTICATION_ENCRYPTION;

typedef struct _NDIS_802_11_CAPABILITY {
	u32  Length;
	u32  Version;
	u32  NoOfPMKIDs;
	u32  NoOfAuthEncryptPairsSupported;
	RTW_802_11_AUTHENTICATION_ENCRYPTION AuthenticationEncryptionSupported[1];

} RTW_802_11_CAPABILITY, *PRTW_802_11_CAPABILITY;


#endif /* #ifndef WLAN_BSSDEF_H_ */
