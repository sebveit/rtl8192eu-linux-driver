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

#include "mp_precomp.h"
#include "phydm_precomp.h"

#if (defined(CONFIG_BB_TXBF_API))
u8 beamforming_get_htndp_tx_rate(void *dm_void, u8 bfer_str_num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 nr_index = 0;
	u8 ndp_tx_rate;
/*@Find nr*/
		nr_index = tx_bf_nr(1, bfer_str_num);

	switch (nr_index) {
	case 1:
		ndp_tx_rate = ODM_MGN_MCS8;
		break;

	case 2:
		ndp_tx_rate = ODM_MGN_MCS16;
		break;

	case 3:
		ndp_tx_rate = ODM_MGN_MCS24;
		break;

	default:
		ndp_tx_rate = ODM_MGN_MCS8;
		break;
	}

	return ndp_tx_rate;
}

u8 beamforming_get_vht_ndp_tx_rate(void *dm_void, u8 bfer_str_num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 nr_index = 0;
	u8 ndp_tx_rate;
/*@Find nr*/
		nr_index = tx_bf_nr(1, bfer_str_num);

	switch (nr_index) {
	case 1:
		ndp_tx_rate = ODM_MGN_VHT2SS_MCS0;
		break;

	case 2:
		ndp_tx_rate = ODM_MGN_VHT3SS_MCS0;
		break;

	case 3:
		ndp_tx_rate = ODM_MGN_VHT4SS_MCS0;
		break;

	default:
		ndp_tx_rate = ODM_MGN_VHT2SS_MCS0;
		break;
	}

	return ndp_tx_rate;
}
#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
/*this function is only used for BFer*/
void phydm_txbf_rfmode(void *dm_void, u8 su_bfee_cnt, u8 mu_bfee_cnt)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 i;

	if (dm->rf_type == RF_1T1R)
		return;
}

void phydm_txbf_avoid_hang(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/* avoid CCK CCA hang when the BF mode */
	odm_set_bb_reg(dm, R_0x1e6c, 0x100000, 0x1);
}

#endif /*PHYSTS_3RD_TYPE_IC*/
#endif /*CONFIG_BB_TXBF_API*/
