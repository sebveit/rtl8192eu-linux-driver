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
#ifndef __PHYDM_HAL_TXBF_API_H__
#define __PHYDM_HAL_TXBF_API_H__

#if (defined(CONFIG_BB_TXBF_API))

#define tx_bf_nr(a, b) ((a > b) ? (b) : (a))

u8 beamforming_get_htndp_tx_rate(void *dm_void, u8 bfer_str_num);

u8 beamforming_get_vht_ndp_tx_rate(void *dm_void, u8 bfer_str_num);


#define phydm_get_beamforming_sounding_info(dm, tp, bfee_num, rate) 0
#define phydm_get_ndpa_rate(dm)
#define phydm_get_mu_bfee_snding_decision(dm, tp)

#ifdef PHYSTS_3RD_TYPE_IC
/*this function is only used for BFer*/
void phydm_txbf_rfmode(void *dm_void, u8 su_bfee_cnt, u8 mu_bfee_cnt);

void phydm_txbf_avoid_hang(void *dm_void);


#endif /*PHYSTS_3RD_TYPE_IC*/

#endif
#endif
