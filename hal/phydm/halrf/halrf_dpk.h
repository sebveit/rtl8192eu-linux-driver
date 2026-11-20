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

#ifndef __HALRF_DPK_H__
#define __HALRF_DPK_H__

/*@--------------------------Define Parameters-------------------------------*/
#define GAIN_LOSS 1
#define DO_DPK 2
#define DPK_ON 3
#define DPK_LOK 4
#define DPK_TXK 5

#define DAGC 4
#define LOSS_CHK 0
#define GAIN_CHK 1
#define PAS_READ 2
#define AVG_THERMAL_NUM 8
#define AVG_THERMAL_NUM_DPK 8
#define THERMAL_DPK_AVG_NUM 4

/*@---------------------------End Define Parameters---------------------------*/

struct dm_dpk_info {

	boolean	is_dpk_enable;
	boolean	is_dpk_pwr_on;
	boolean	is_dpk_by_channel;
	u16 dpk_path_ok;
	/*@BIT(15)~BIT(12) : 5G reserved, BIT(11)~BIT(8) 5G_S3~5G_S0*/
	/*@BIT(7)~BIT(4) : 2G reserved, BIT(3)~BIT(0) 2G_S3~2G_S0*/
	u8	thermal_dpk;
	u8	thermal_dpk_avg[AVG_THERMAL_NUM_DPK];
	u8	thermal_dpk_avg_index;



};

#endif /*__HALRF_DPK_H__*/
