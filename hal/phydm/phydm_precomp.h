/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#ifndef __ODM_PRECOMP_H__
#define __ODM_PRECOMP_H__

#include "phydm_types.h"
#include "halrf/halrf_features.h"

	#define		TEST_FALG___		1

/* @2 Config Flags and Structs - defined by each ODM type */

	#define __PACK
	#define __WLAN_ATTRIB_PACK__

/* @2 OutSrc Header Files */

#include "phydm.h"
#include "phydm_hwconfig.h"
#include "phydm_phystatus.h"
#include "phydm_debug.h"
#include "phydm_regdefine11ac.h"
#include "phydm_regdefine11n.h"
#include "phydm_interface.h"
#include "phydm_reg.h"
#include "halrf/halrf_debug.h"


void phy_set_tx_power_limit(
	struct dm_struct *dm,
	u8 *regulation,
	u8 *band,
	u8 *bandwidth,
	u8 *rate_section,
	u8 *rf_path,
	u8 *channel,
	u8 *power_limit);

enum hal_status
rtw_phydm_fw_iqk(
	struct dm_struct *dm,
	u8 clear,
	u8 segment);

enum hal_status
rtw_phydm_cfg_phy_para(
	struct dm_struct *dm,
	enum phydm_halmac_param config_type,
	u32 offset,
	u32 data,
	u32 mask,
	enum rf_path e_rf_path,
	u32 delay_time);


/* @Judy ADD 20180125 */


#define RTL8197F_SUPPORT 0	/*@Just for PHYDM API development*/
#define	RTL8195B_SUPPORT 0	/*@Just for PHYDM API development*/
#define	RTL8198F_SUPPORT 0	/*@Just for PHYDM API development*/
#define	RTL8812F_SUPPORT 0	/*@Just for PHYDM API development*/
#define	RTL8197G_SUPPORT 0	/*@Just for PHYDM API development*/



		#include "halrf/rtl8192e/halrf_8192e_ce.h" /*@FOR_8192E_IQK*/

	#include "rtl8192e/phydm_rtl8192e.h" /* @FOR_8192E_IQK */
	#include "rtl8192e/version_rtl8192e.h"
		#include "rtl8192e/halhwimg8192e_bb.h"
		#include "rtl8192e/halhwimg8192e_mac.h"
		#include "rtl8192e/halhwimg8192e_rf.h"
		#include "rtl8192e/phydm_regconfig8192e.h"
		#include "rtl8192e/hal8192ereg.h"
		#include "rtl8192e_hal.h"


















#endif /* @__ODM_PRECOMP_H__ */
