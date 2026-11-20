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

#ifndef __HALRF_IQK_H__
#define __HALRF_IQK_H__

/*@--------------------------Define Parameters-------------------------------*/
#define LOK_delay 1
#define WBIQK_delay 10
#define TX_IQK 0
#define RX_IQK 1
#define TXIQK 0
#define RXIQK1 1
#define RXIQK2 2
#define kcount_limit_80m 2
#define kcount_limit_others 4
#define rxiqk_gs_limit 10
#define TXWBIQK_EN 1
#define RXWBIQK_EN 1
#define NUM 4
/*@-----------------------End Define Parameters-----------------------*/

struct dm_dack_info {
	u32 ic_a;
	u32 qc_a;
	u32 ic_b;
	u32 qc_b;
};

struct dm_iqk_info {
	boolean lok_fail[NUM];
	boolean iqk_fail[2][NUM];
	u32 iqc_matrix[2][NUM];
	u8 iqk_times;
	u32 rf_reg18;
	u32 rf_reg08;
	u32 lna_idx;
	u8 iqk_step;
	u8 rxiqk_step;
	u8 tmp1bcc;
	u8 txgain;
	u8 kcount;
	u8 rfk_ing; /*bit0:IQKing, bit1:LCKing, bit2:DPKing*/
	boolean rfk_forbidden;
	u8 rxbb;
};

#endif /*__HALRF_IQK_H__*/
