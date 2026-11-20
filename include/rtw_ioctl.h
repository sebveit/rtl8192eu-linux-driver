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
#ifndef _RTW_IOCTL_H_
#define _RTW_IOCTL_H_

/*	00 - Success
*	11 - Error */
#define STATUS_SUCCESS				(0x00000000L)
#define STATUS_PENDING				(0x00000103L)

#define STATUS_UNSUCCESSFUL			(0xC0000001L)
#define STATUS_INSUFFICIENT_RESOURCES		(0xC000009AL)
#define STATUS_NOT_SUPPORTED			(0xC00000BBL)

#define RTW_STATUS_SUCCESS			((RTW_STATUS)STATUS_SUCCESS)
#define RTW_STATUS_PENDING			((RTW_STATUS)STATUS_PENDING)
#define RTW_STATUS_NOT_RECOGNIZED		((RTW_STATUS)0x00010001L)
#define RTW_STATUS_NOT_COPIED			((RTW_STATUS)0x00010002L)
#define RTW_STATUS_NOT_ACCEPTED		((RTW_STATUS)0x00010003L)
#define RTW_STATUS_CALL_ACTIVE			((RTW_STATUS)0x00010007L)

#define RTW_STATUS_FAILURE			((RTW_STATUS)STATUS_UNSUCCESSFUL)
#define RTW_STATUS_RESOURCES			((RTW_STATUS)STATUS_INSUFFICIENT_RESOURCES)
#define RTW_STATUS_CLOSING			((RTW_STATUS)0xC0010002L)
#define RTW_STATUS_BAD_VERSION			((RTW_STATUS)0xC0010004L)
#define RTW_STATUS_BAD_CHARACTERISTICS		((RTW_STATUS)0xC0010005L)
#define RTW_STATUS_ADAPTER_NOT_FOUND		((RTW_STATUS)0xC0010006L)
#define RTW_STATUS_OPEN_FAILED			((RTW_STATUS)0xC0010007L)
#define RTW_STATUS_DEVICE_FAILED		((RTW_STATUS)0xC0010008L)
#define RTW_STATUS_MULTICAST_FULL		((RTW_STATUS)0xC0010009L)
#define RTW_STATUS_MULTICAST_EXISTS		((RTW_STATUS)0xC001000AL)
#define RTW_STATUS_MULTICAST_NOT_FOUND		((RTW_STATUS)0xC001000BL)
#define RTW_STATUS_REQUEST_ABORTED		((RTW_STATUS)0xC001000CL)
#define RTW_STATUS_RESET_IN_PROGRESS		((RTW_STATUS)0xC001000DL)
#define RTW_STATUS_CLOSING_INDICATING		((RTW_STATUS)0xC001000EL)
#define RTW_STATUS_NOT_SUPPORTED		((RTW_STATUS)STATUS_NOT_SUPPORTED)
#define RTW_STATUS_INVALID_PACKET		((RTW_STATUS)0xC001000FL)
#define RTW_STATUS_OPEN_LIST_FULL		((RTW_STATUS)0xC0010010L)
#define RTW_STATUS_ADAPTER_NOT_READY		((RTW_STATUS)0xC0010011L)
#define RTW_STATUS_ADAPTER_NOT_OPEN		((RTW_STATUS)0xC0010012L)
#define RTW_STATUS_NOT_INDICATING		((RTW_STATUS)0xC0010013L)
#define RTW_STATUS_INVALID_LENGTH		((RTW_STATUS)0xC0010014L)
#define RTW_STATUS_INVALID_DATA		((RTW_STATUS)0xC0010015L)
#define RTW_STATUS_BUFFER_TOO_SHORT		((RTW_STATUS)0xC0010016L)
#define RTW_STATUS_INVALID_OID			((RTW_STATUS)0xC0010017L)
#define RTW_STATUS_ADAPTER_REMOVED		((RTW_STATUS)0xC0010018L)
#define RTW_STATUS_UNSUPPORTED_MEDIA		((RTW_STATUS)0xC0010019L)
#define RTW_STATUS_GROUP_ADDRESS_IN_USE	((RTW_STATUS)0xC001001AL)
#define RTW_STATUS_FILE_NOT_FOUND		((RTW_STATUS)0xC001001BL)
#define RTW_STATUS_ERROR_READING_FILE		((RTW_STATUS)0xC001001CL)
#define RTW_STATUS_ALREADY_MAPPED		((RTW_STATUS)0xC001001DL)
#define RTW_STATUS_RESOURCE_CONFLICT		((RTW_STATUS)0xC001001EL)
#define RTW_STATUS_NO_CABLE			((RTW_STATUS)0xC001001FL)

#define RTW_STATUS_INVALID_SAP			((RTW_STATUS)0xC0010020L)
#define RTW_STATUS_SAP_IN_USE			((RTW_STATUS)0xC0010021L)
#define RTW_STATUS_INVALID_ADDRESS		((RTW_STATUS)0xC0010022L)
#define RTW_STATUS_VC_NOT_ACTIVATED		((RTW_STATUS)0xC0010023L)
#define RTW_STATUS_DEST_OUT_OF_ORDER		((RTW_STATUS)0xC0010024L)  /* cause 27 */
#define RTW_STATUS_VC_NOT_AVAILABLE		((RTW_STATUS)0xC0010025L)  /* cause 35, 45 */
#define RTW_STATUS_CELLRATE_NOT_AVAILABLE	((RTW_STATUS)0xC0010026L)  /* cause 37 */
#define RTW_STATUS_INCOMPATABLE_QOS		((RTW_STATUS)0xC0010027L)  /* cause 49 */
#define RTW_STATUS_AAL_PARAMS_UNSUPPORTED	((RTW_STATUS)0xC0010028L)  /* cause 93 */
#define RTW_STATUS_NO_ROUTE_TO_DESTINATION	((RTW_STATUS)0xC0010029L)  /* cause 3 */


#ifndef OID_802_11_CAPABILITY
	#define OID_802_11_CAPABILITY                   0x0d010122
#endif

#ifndef OID_802_11_PMKID
	#define OID_802_11_PMKID                        0x0d010123
#endif


/* For DDK-defined OIDs */
#define OID_NDIS_SEG1	0x00010100
#define OID_NDIS_SEG2	0x00010200
#define OID_NDIS_SEG3	0x00020100
#define OID_NDIS_SEG4	0x01010100
#define OID_NDIS_SEG5	0x01020100
#define OID_NDIS_SEG6	0x01020200
#define OID_NDIS_SEG7	0xFD010100
#define OID_NDIS_SEG8	0x0D010100
#define OID_NDIS_SEG9	0x0D010200
#define OID_NDIS_SEG10	0x0D020200

#define SZ_OID_NDIS_SEG1		  23
#define SZ_OID_NDIS_SEG2		    3
#define SZ_OID_NDIS_SEG3		    6
#define SZ_OID_NDIS_SEG4		    6
#define SZ_OID_NDIS_SEG5		    4
#define SZ_OID_NDIS_SEG6		    8
#define SZ_OID_NDIS_SEG7		    7
#define SZ_OID_NDIS_SEG8		  36
#define SZ_OID_NDIS_SEG9		  24
#define SZ_OID_NDIS_SEG10		  19

/* For Realtek-defined OIDs */
#define OID_MP_SEG1		0xFF871100
#define OID_MP_SEG2		0xFF818000

#define OID_MP_SEG3		0xFF818700
#define OID_MP_SEG4		0xFF011100

enum oid_type {
	QUERY_OID,
	SET_OID
};

struct oid_funs_node {
	unsigned int oid_start; /* the starting number for OID */
	unsigned int oid_end; /* the ending number for OID */
	struct oid_obj_priv *node_array;
	unsigned int array_sz; /* the size of node_array */
	int query_counter; /* count the number of query hits for this segment  */
	int set_counter; /* count the number of set hits for this segment  */
};

struct oid_par_priv {
	void		*adapter_context;
	RTW_OID	oid;
	void		*information_buf;
	u32		information_buf_len;
	u32		*bytes_rw;
	u32		*bytes_needed;
	enum oid_type	type_of_oid;
	u32		dbg;
};

struct oid_obj_priv {
	unsigned char	dbg; /* 0: without OID debug message  1: with OID debug message */
	RTW_STATUS(*oidfuns)(struct oid_par_priv *poid_par_priv);
};

#if defined(CONFIG_MP_INCLUDED) && defined(_RTW_MP_IOCTL_C_)
static RTW_STATUS oid_null_function(struct oid_par_priv *poid_par_priv)
{
	return RTW_STATUS_SUCCESS;
}
#endif



extern void rtw_request_wps_pbc_event(_adapter *padapter);

extern	RTW_STATUS drv_query_info(
	_nic_hdl		MiniportAdapterContext,
	RTW_OID		Oid,
	void			*InformationBuffer,
	u32			InformationBufferLength,
	u32			*BytesWritten,
	u32			*BytesNeeded
);

extern	RTW_STATUS	drv_set_info(
	_nic_hdl		MiniportAdapterContext,
	RTW_OID		Oid,
	void			*InformationBuffer,
	u32			InformationBufferLength,
	u32			*BytesRead,
	u32			*BytesNeeded
);

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
extern int rtw_vendor_ie_get_raw_data(struct net_device *, u32, char *, u32);
extern int rtw_vendor_ie_get_data(struct net_device*, int , char*);
extern int rtw_vendor_ie_get(struct net_device *, struct iw_request_info *, union iwreq_data *, char *);
extern int rtw_vendor_ie_set(struct net_device*, struct iw_request_info*, union iwreq_data*, char*);
#endif

#endif /*  #ifndef __INC_CEINFO_ */
