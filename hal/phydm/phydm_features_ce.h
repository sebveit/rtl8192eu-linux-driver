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

#ifndef __PHYDM_FEATURES_CE_H__
#define __PHYDM_FEATURES_CE_H__

	#define PHYDM_LA_MODE_SUPPORT			0


	#define NHM_SUPPORT
	#define CLM_SUPPORT


/*@#define PHYDM_TDMA_DIG_SUPPORT*/











	#define	CONFIG_RECEIVER_BLOCKING



#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
#define CONFIG_DYNAMIC_TX_TWR
#endif
#define PHYDM_SUPPORT_CCKPD
#define PHYDM_SUPPORT_ADAPTIVITY

/*@Antenna Diversity*/
#ifdef CONFIG_ANTENNA_DIVERSITY
	#define CONFIG_PHYDM_ANTENNA_DIVERSITY

	#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY




	#endif
#endif

/*@[SmartAntenna]*/
/*@#define	CONFIG_SMART_ANTENNA*/
#ifdef CONFIG_SMART_ANTENNA
	/*@#define	CONFIG_CUMITEK_SMART_ANTENNA*/
#endif
/* @--------------------------------------------------*/

#ifdef CONFIG_DFS_MASTER
	#define CONFIG_PHYDM_DFS_MASTER
#endif

	/*@#define	CONFIG_RA_FW_DBG_CODE*/

#define	CONFIG_PSD_TOOL
/*@#define	CONFIG_ANT_DETECTION*/
/*@#define	CONFIG_PATH_DIVERSITY*/
/*@#define	CONFIG_RA_DYNAMIC_RTY_LIMIT*/
#define	CONFIG_BB_TXBF_API
#define	CONFIG_PHYDM_DEBUG_FUNCTION

#ifdef CONFIG_BT_COEXIST
	#define	ODM_CONFIG_BT_COEXIST
#endif
#define	PHYDM_SUPPORT_RSSI_MONITOR
/*@#define	PHYDM_AUTO_DEGBUG*/
#define CFG_DIG_DAMPING_CHK


#ifdef PHYDM_BEAMFORMING_SUPPORT
		#define	PHYDM_BEAMFORMING_VERSION1
#endif




#endif
