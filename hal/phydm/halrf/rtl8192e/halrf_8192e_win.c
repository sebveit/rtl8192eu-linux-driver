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

#include "../phydm_precomp.h"

#if (RTL8192E_SUPPORT == 1)

/*---------------------------Define Local Constant---------------------------*/
/* 2010/04/25 MH Define the max tx power tracking tx agc power. */
#define		ODM_TXPWRTRACK_MAX_IDX_92E		6

/*---------------------------Define Local Constant---------------------------*/

/* 3============================================================
 * 3 Tx Power Tracking
 * 3============================================================ */


void halrf_rf_lna_setting_8192e(
	struct dm_struct	*dm,
	enum halrf_lna_set type
)
{
/*phydm_disable_lna*/
	if (type == HALRF_LNA_DISABLE) {
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x1);
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x30, 0xfffff, 0x18000);	/*select Rx mode*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x31, 0xfffff, 0x0000f);
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x32, 0xfffff, 0x37f82);	/*disable LNA*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x0);
		if (dm->rf_type > RF_1T1R) {
			odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, 0x80000, 0x1);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x30, 0xfffff, 0x18000);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x31, 0xfffff, 0x0000f);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x32, 0xfffff, 0x37f82);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, 0x80000, 0x0);
		}
	} else if (type == HALRF_LNA_ENABLE) {
		/*phydm_enable_lna*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x1);
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x30, 0xfffff, 0x18000);	/*select Rx mode*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x31, 0xfffff, 0x0000f);
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x32, 0xfffff, 0x77f82);	/*back to normal*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x0);
		if (dm->rf_type > RF_1T1R) {
			odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, 0x80000, 0x1);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x30, 0xfffff, 0x18000);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x31, 0xfffff, 0x0000f);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0x32, 0xfffff, 0x77f82);
			odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, 0x80000, 0x0);
		}
	}
}


void set_iqk_matrix_8192e(
	struct dm_struct	*dm,
	u8		OFDM_index,
	u8		rf_path,
	s32		iqk_result_x,
	s32		iqk_result_y
)
{
	s32			ele_A = 0, ele_D, ele_C = 0, value32;

	ele_D = (ofdm_swing_table_new[OFDM_index] & 0xFFC00000) >> 22;

	/*new element A = element D x X*/
	if ((iqk_result_x != 0) && (*(dm->band_type) == ODM_BAND_2_4G)) {
		if ((iqk_result_x & 0x00000200) != 0)	/* consider minus */
			iqk_result_x = iqk_result_x | 0xFFFFFC00;
		ele_A = ((iqk_result_x * ele_D) >> 8) & 0x000003FF;

		/* new element C = element D x Y */
		if ((iqk_result_y & 0x00000200) != 0)
			iqk_result_y = iqk_result_y | 0xFFFFFC00;
		ele_C = ((iqk_result_y * ele_D) >> 8) & 0x000003FF;

		/*if (rf_path == RF_PATH_A)// Remove this to Fix path B PowerTracking */
		switch (rf_path) {
		case RF_PATH_A:
			/* wirte new elements A, C, D to regC80 and regC94, element B is always 0 */
			value32 = (ele_D << 22) | ((ele_C & 0x3F) << 16) | ele_A;
			odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD, value32);

			value32 = (ele_C & 0x000003C0) >> 6;
			odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, MASKH4BITS, value32);

			value32 = ((iqk_result_x * ele_D) >> 7) & 0x01;
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(24), value32);
			break;
		case RF_PATH_B:
			/* wirte new elements A, C, D to regC88 and regC9C, element B is always 0 */
			value32 = (ele_D << 22) | ((ele_C & 0x3F) << 16) | ele_A;
			odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD, value32);

			value32 = (ele_C & 0x000003C0) >> 6;
			odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKH4BITS, value32);

			value32 = ((iqk_result_x * ele_D) >> 7) & 0x01;
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(28), value32);

			break;
		default:
			break;
		}
	} else {
		switch (rf_path) {
		case RF_PATH_A:
			odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD, ofdm_swing_table_new[OFDM_index]);
			odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, MASKH4BITS, 0x00);
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(24), 0x00);
			break;

		case RF_PATH_B:
			odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD, ofdm_swing_table_new[OFDM_index]);
			odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKH4BITS, 0x00);
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(28), 0x00);
			break;

		default:
			break;
		}
	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "TxPwrTracking path %c: X = 0x%x, Y = 0x%x ele_A = 0x%x ele_C = 0x%x ele_D = 0x%x 0xeb4 = 0x%x 0xebc = 0x%x\n",
		(rf_path == RF_PATH_A ? 'A' : 'B'), (u32)iqk_result_x, (u32)iqk_result_y, (u32)ele_A, (u32)ele_C, (u32)ele_D, (u32)iqk_result_x, (u32)iqk_result_y);
}

void do_iqk_8192e(
	void		*dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;

	odm_reset_iqk_result(dm);
	dm->rf_calibrate_info.thermal_value_iqk = thermal_value;
	halrf_iqk_trigger(dm, false);
}

/*-----------------------------------------------------------------------------
 * Function:	odm_TxPwrTrackSetPwr88E()
 *
 * Overview:	88E change all channel tx power accordign to flag.
 *				OFDM & CCK are all different.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	04/23/2012	MHC		Create version 0.
 *
 *---------------------------------------------------------------------------*/
void
odm_tx_pwr_track_set_pwr92_e(
	void				*dm_void,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
)
{
	struct dm_struct				*dm = (struct dm_struct *)dm_void;
	struct _ADAPTER *adapter = dm->adapter;
	//PHAL_DATA_TYPE			hal_data = GET_HAL_DATA(((PADAPTER)adapter));
	u8					pwr_tracking_limit_ofdm = 30; /* +0dB */
	u8					pwr_tracking_limit_cck = 28;  /* -2dB */
	u8					tx_rate = 0xFF;
	u8					final_ofdm_swing_index = 0;
	u8					final_cck_swing_index = 0;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);

	if (*(dm->mp_mode) == true) {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (MP_DRIVER == 1)
		PMPT_CONTEXT p_mpt_ctx = &(adapter->MptCtx);

		tx_rate = MptToMgntRate(p_mpt_ctx->MptRateIndex);
#endif
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
#ifdef CONFIG_MP_INCLUDED
		PMPT_CONTEXT p_mpt_ctx = &(adapter->mppriv.mpt_ctx);

		tx_rate = mpt_to_mgnt_rate(p_mpt_ctx->mpt_rate_index);
#endif
#endif
#endif
	} else {
		u16	rate	 = *(dm->forced_data_rate);

		if (!rate) { /*auto rate*/
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
			tx_rate = ((PADAPTER)adapter)->HalFunc.GetHwRateFromMRateHandler(dm->tx_rate);
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
			tx_rate = hw_rate_to_m_rate(dm->tx_rate);
#endif
		} else   /*force rate*/
			tx_rate = (u8)rate;
	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "Power Tracking tx_rate=0x%X\n", tx_rate);
	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "===>ODM_TxPwrTrackSetPwr8192EA\n");

	if (tx_rate != 0xFF) { /* 20130429 Mimic Modify High rate BBSwing Limit. */
		/* 2 CCK */
		if (((tx_rate >= MGN_1M) && (tx_rate <= MGN_5_5M)) || (tx_rate == MGN_11M))
			pwr_tracking_limit_cck = 28;  /* -2dB */
		/* 2 OFDM */
		else if ((tx_rate >= MGN_6M) && (tx_rate <= MGN_48M))
			pwr_tracking_limit_ofdm = 36; /* +3dB */
		else if (tx_rate == MGN_54M)
			pwr_tracking_limit_ofdm = 34; /* +2dB */

		/* 2 HT */
		else if ((tx_rate >= MGN_MCS0) && (tx_rate <= MGN_MCS2)) /* QPSK/BPSK */
			pwr_tracking_limit_ofdm = 38; /* +4dB */
		else if ((tx_rate >= MGN_MCS3) && (tx_rate <= MGN_MCS4)) /* 16QAM */
			pwr_tracking_limit_ofdm = 36; /* +3dB */
		else if ((tx_rate >= MGN_MCS5) && (tx_rate <= MGN_MCS7)) /* 64QAM */
			pwr_tracking_limit_ofdm = 34; /* +2dB */

		else if ((tx_rate >= MGN_MCS8) && (tx_rate <= MGN_MCS10)) /* QPSK/BPSK */
			pwr_tracking_limit_ofdm = 38; /* +4dB */
		else if ((tx_rate >= MGN_MCS11) && (tx_rate <= MGN_MCS12)) /* 16QAM */
			pwr_tracking_limit_ofdm = 36; /* +3dB */
		else if ((tx_rate >= MGN_MCS13) && (tx_rate <= MGN_MCS15)) /* 64QAM */
			pwr_tracking_limit_ofdm = 34; /* +2dB */

		else
			pwr_tracking_limit_ofdm =  cali_info->default_ofdm_index;   /* Default OFDM index = 30 */
	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "pwr_tracking_limit_cck=%d      pwr_tracking_limit_ofdm=%d\n", pwr_tracking_limit_cck, pwr_tracking_limit_ofdm);

	if (method == TXAGC) {

		u32	pwr = 0, tx_agc = 0;
		void *adapter = dm->adapter;

		RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "odm_TxPwrTrackSetPwr92E CH=%d\n", *(dm->channel));

		cali_info->remnant_ofdm_swing_idx[rf_path] = cali_info->absolute_ofdm_swing_idx[rf_path];   /* Remnant index equal to aboslute compensate value. */

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))

#if (MP_DRIVER != 1)
		/* PHY_SetTxPowerLevelByPath8192E(adapter, hal_data->current_channel, rf_path);   */ /* Using new set power function */
		/* phy_set_tx_power_level8192e(dm->adapter, *dm->channel); */
		cali_info->modify_tx_agc_flag_path_a = true;
		cali_info->modify_tx_agc_flag_path_b = true;
		cali_info->modify_tx_agc_flag_path_a_cck = true;
		if (rf_path == RF_PATH_A) {
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, CCK);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, OFDM);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS0_MCS7);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS8_MCS15);
		} else {
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, CCK);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, OFDM);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS0_MCS7);
			PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS8_MCS15);
		}
#else

		if (rf_path == RF_PATH_A) {
			/* CCK path A */
			pwr = PHY_QueryBBReg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
			pwr += cali_info->power_index_offset[RF_PATH_A];
			PHY_SetBBReg(adapter, REG_TX_AGC_A_CCK_1_MCS32, MASKBYTE1, pwr);
			tx_agc = (pwr << 16) | (pwr << 8) | (pwr);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_CCK_11_A_CCK_2_11, 0x00ffffff, tx_agc);
			RT_DISP(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr92_e: CCK Tx-rf(A) Power = 0x%x\n", tx_agc));

			/* OFDM path A */
			pwr = PHY_QueryBBReg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
			pwr += (cali_info->bb_swing_idx_ofdm[RF_PATH_A] - cali_info->bb_swing_idx_ofdm_base[RF_PATH_A]);
			tx_agc = ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_RATE18_06, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_RATE54_24, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_MCS03_MCS00, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_MCS07_MCS04, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_MCS11_MCS08, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_A_MCS15_MCS12, MASKDWORD, tx_agc);
			RT_DISP(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr92_e: OFDM Tx-rf(A) Power = 0x%x\n", tx_agc));
		} else if (rf_path == RF_PATH_B) {
			/* CCK path B */
			pwr = PHY_QueryBBReg(adapter, REG_TX_AGC_B_RATE18_06, 0xFF);
			pwr += cali_info->power_index_offset[RF_PATH_B];
			PHY_SetBBReg(adapter, REG_TX_AGC_B_CCK_1_55_MCS32, MASKBYTE3, pwr);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_CCK_11_A_CCK_2_11, 0xff000000, pwr);
			RT_DISP(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr92_e: CCK Tx-rf(B) Power = 0x%x\n", pwr));

			/* OFDM path B */
			pwr = PHY_QueryBBReg(adapter, REG_TX_AGC_B_RATE18_06, 0xFF);
			pwr += (cali_info->bb_swing_idx_ofdm[RF_PATH_B] - cali_info->bb_swing_idx_ofdm_base[RF_PATH_B]);
			tx_agc = ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_RATE18_06, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_RATE54_24, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_MCS03_MCS00, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_MCS07_MCS04, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_MCS11_MCS08, MASKDWORD, tx_agc);
			PHY_SetBBReg(adapter, REG_TX_AGC_B_MCS15_MCS12, MASKDWORD, tx_agc);
			RT_DISP(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr92_e: OFDM Tx-rf(B) Power = 0x%x\n", tx_agc));
		}
#endif

#endif
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
		/* phy_rf6052_set_cck_tx_power(dm->priv, *(dm->channel)); */
		/* phy_rf6052_set_ofdm_tx_power(dm->priv, *(dm->channel)); */
#endif

	} else if (method == BBSWING) {
		final_ofdm_swing_index = cali_info->default_ofdm_index + cali_info->absolute_ofdm_swing_idx[rf_path];
		final_cck_swing_index = cali_info->default_cck_index + cali_info->absolute_ofdm_swing_idx[rf_path];

		if (final_ofdm_swing_index >= pwr_tracking_limit_ofdm)
			final_ofdm_swing_index = pwr_tracking_limit_ofdm;
		else if (final_ofdm_swing_index < 0)
			final_ofdm_swing_index = 0;

		if (final_cck_swing_index >= CCK_TABLE_SIZE)
			final_cck_swing_index = CCK_TABLE_SIZE - 1;
		else if (cali_info->bb_swing_idx_cck < 0)
			final_cck_swing_index = 0;

		/* Adjust BB swing by OFDM IQ matrix */
		if (rf_path == RF_PATH_A) {
			set_iqk_matrix_8192e(dm, final_ofdm_swing_index, RF_PATH_A,
				cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
				cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

			/* Adjust BB swing by CCK filter coefficient */
			if (*dm->channel != 14) {
				odm_write_1byte(dm, 0xa22, cck_swing_table_ch1_ch13_new[final_cck_swing_index][0]);
				odm_write_1byte(dm, 0xa23, cck_swing_table_ch1_ch13_new[final_cck_swing_index][1]);
				odm_write_1byte(dm, 0xa24, cck_swing_table_ch1_ch13_new[final_cck_swing_index][2]);
				odm_write_1byte(dm, 0xa25, cck_swing_table_ch1_ch13_new[final_cck_swing_index][3]);
				odm_write_1byte(dm, 0xa26, cck_swing_table_ch1_ch13_new[final_cck_swing_index][4]);
				odm_write_1byte(dm, 0xa27, cck_swing_table_ch1_ch13_new[final_cck_swing_index][5]);
				odm_write_1byte(dm, 0xa28, cck_swing_table_ch1_ch13_new[final_cck_swing_index][6]);
				odm_write_1byte(dm, 0xa29, cck_swing_table_ch1_ch13_new[final_cck_swing_index][7]);
			} else {
				odm_write_1byte(dm, 0xa22, cck_swing_table_ch14_new[final_cck_swing_index][0]);
				odm_write_1byte(dm, 0xa23, cck_swing_table_ch14_new[final_cck_swing_index][1]);
				odm_write_1byte(dm, 0xa24, cck_swing_table_ch14_new[final_cck_swing_index][2]);
				odm_write_1byte(dm, 0xa25, cck_swing_table_ch14_new[final_cck_swing_index][3]);
				odm_write_1byte(dm, 0xa26, cck_swing_table_ch14_new[final_cck_swing_index][4]);
				odm_write_1byte(dm, 0xa27, cck_swing_table_ch14_new[final_cck_swing_index][5]);
				odm_write_1byte(dm, 0xa28, cck_swing_table_ch14_new[final_cck_swing_index][6]);
				odm_write_1byte(dm, 0xa29, cck_swing_table_ch14_new[final_cck_swing_index][7]);
			}
		} else if (rf_path == RF_PATH_B) {
			set_iqk_matrix_8192e(dm, final_ofdm_swing_index, RF_PATH_B,
				cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][4],
				cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][5]);
		}
	} else if (method == MIX_MODE) {
		RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "cali_info->default_ofdm_index=%d,  dm->DefaultCCKIndex=%d, cali_info->absolute_ofdm_swing_idx[rf_path]=%d, rf_path = %d\n",
			cali_info->default_ofdm_index, cali_info->default_cck_index, cali_info->absolute_ofdm_swing_idx[rf_path], rf_path);

		final_ofdm_swing_index = cali_info->default_ofdm_index + cali_info->absolute_ofdm_swing_idx[rf_path];

		if (rf_path == RF_PATH_A) {
			final_cck_swing_index = cali_info->default_cck_index + cali_info->absolute_ofdm_swing_idx[rf_path];    /* CCK Follow path-A and lower CCK index means higher power. */

			if (final_ofdm_swing_index > pwr_tracking_limit_ofdm) {   /* BBSwing higher then Limit */
				cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index - pwr_tracking_limit_ofdm;

				set_iqk_matrix_8192e(dm, pwr_tracking_limit_ofdm, RF_PATH_A,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

				cali_info->modify_tx_agc_flag_path_a = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, OFDM);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS0_MCS7);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS8_MCS15);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A Over BBSwing Limit, pwr_tracking_limit = %d, Remnant tx_agc value = %d\n", pwr_tracking_limit_ofdm, cali_info->remnant_ofdm_swing_idx[rf_path]);
			} else if (final_ofdm_swing_index < 0) {
				cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index ;

				set_iqk_matrix_8192e(dm, 0, RF_PATH_A,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);


				cali_info->modify_tx_agc_flag_path_a = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, OFDM);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS0_MCS7);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS8_MCS15);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A Lower then BBSwing lower bound  0, Remnant tx_agc value = %d\n", cali_info->remnant_ofdm_swing_idx[rf_path]);
			} else {
				set_iqk_matrix_8192e(dm, final_ofdm_swing_index, RF_PATH_A,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A Compensate with BBSwing, final_ofdm_swing_index = %d\n", final_ofdm_swing_index);

				if (cali_info->modify_tx_agc_flag_path_a) { /* If tx_agc has changed, reset tx_agc again */
					cali_info->remnant_ofdm_swing_idx[rf_path] = 0;

					/* Set tx_agc Page C{}; */
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, OFDM);
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS0_MCS7);
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, HT_MCS8_MCS15);

					cali_info->modify_tx_agc_flag_path_a = false;

					RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A dm->Modify_TxAGC_Flag = false\n");
				}
			}

			if (final_cck_swing_index > pwr_tracking_limit_cck) {
				cali_info->remnant_cck_swing_idx = final_cck_swing_index - pwr_tracking_limit_cck;

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A CCK Over Limit, pwr_tracking_limit_cck = %d, cali_info->remnant_cck_swing_idx  = %d\n", pwr_tracking_limit_cck, cali_info->remnant_cck_swing_idx);

				/* Adjust BB swing by CCK filter coefficient */

				if (*dm->channel != 14) {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch1_ch13_new[pwr_tracking_limit_cck][7]);
				} else {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch14_new[pwr_tracking_limit_cck][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch14_new[pwr_tracking_limit_cck][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch14_new[pwr_tracking_limit_cck][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch14_new[pwr_tracking_limit_cck][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch14_new[pwr_tracking_limit_cck][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch14_new[pwr_tracking_limit_cck][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch14_new[pwr_tracking_limit_cck][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch14_new[pwr_tracking_limit_cck][7]);
				}

				cali_info->modify_tx_agc_flag_path_a_cck = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, CCK);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, CCK);

			} else if (final_cck_swing_index < 0) { /* Lowest CCK index = 0 */
				cali_info->remnant_cck_swing_idx = final_cck_swing_index;

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A CCK Under Limit, pwr_tracking_limit_cck = %d, cali_info->remnant_cck_swing_idx  = %d\n", 0, cali_info->remnant_cck_swing_idx);

				if (*dm->channel != 14) {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch1_ch13_new[0][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch1_ch13_new[0][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch1_ch13_new[0][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch1_ch13_new[0][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch1_ch13_new[0][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch1_ch13_new[0][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch1_ch13_new[0][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch1_ch13_new[0][7]);
				} else {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch14_new[0][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch14_new[0][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch14_new[0][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch14_new[0][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch14_new[0][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch14_new[0][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch14_new[0][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch14_new[0][7]);
				}

				cali_info->modify_tx_agc_flag_path_a_cck = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, CCK);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, CCK);

			} else {
				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A CCK Compensate with BBSwing, final_cck_swing_index = %d\n", final_cck_swing_index);

				if (*dm->channel != 14) {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch1_ch13_new[final_cck_swing_index][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch1_ch13_new[final_cck_swing_index][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch1_ch13_new[final_cck_swing_index][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch1_ch13_new[final_cck_swing_index][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch1_ch13_new[final_cck_swing_index][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch1_ch13_new[final_cck_swing_index][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch1_ch13_new[final_cck_swing_index][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch1_ch13_new[final_cck_swing_index][7]);
				} else {
					odm_write_1byte(dm, 0xa22, cck_swing_table_ch14_new[final_cck_swing_index][0]);
					odm_write_1byte(dm, 0xa23, cck_swing_table_ch14_new[final_cck_swing_index][1]);
					odm_write_1byte(dm, 0xa24, cck_swing_table_ch14_new[final_cck_swing_index][2]);
					odm_write_1byte(dm, 0xa25, cck_swing_table_ch14_new[final_cck_swing_index][3]);
					odm_write_1byte(dm, 0xa26, cck_swing_table_ch14_new[final_cck_swing_index][4]);
					odm_write_1byte(dm, 0xa27, cck_swing_table_ch14_new[final_cck_swing_index][5]);
					odm_write_1byte(dm, 0xa28, cck_swing_table_ch14_new[final_cck_swing_index][6]);
					odm_write_1byte(dm, 0xa29, cck_swing_table_ch14_new[final_cck_swing_index][7]);
				}

				if (cali_info->modify_tx_agc_flag_path_a_cck) { /* If tx_agc has changed, reset tx_agc again */
					cali_info->remnant_cck_swing_idx = 0;

					/* Set tx_agc Page C{}; */
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_A, *dm->channel, CCK);
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, CCK);

					cali_info->modify_tx_agc_flag_path_a_cck = false;

					RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_A dm->Modify_TxAGC_Flag_CCK = false\n");
				}

			}
		}
		if (rf_path == RF_PATH_B) {
			if (final_ofdm_swing_index > pwr_tracking_limit_ofdm) {  /* BBSwing higher then Limit */
				cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index - pwr_tracking_limit_ofdm;

				set_iqk_matrix_8192e(dm, pwr_tracking_limit_ofdm, RF_PATH_B,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][4],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][5]);

				cali_info->modify_tx_agc_flag_path_a = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, OFDM);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS0_MCS7);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS8_MCS15);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_B Over BBSwing Limit, pwr_tracking_limit = %d, Remnant tx_agc value = %d\n", pwr_tracking_limit_ofdm, cali_info->remnant_ofdm_swing_idx[rf_path]);
			} else if (final_ofdm_swing_index < 0) {
				cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index ;

				set_iqk_matrix_8192e(dm, 0, RF_PATH_B,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][4],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][5]);

				cali_info->modify_tx_agc_flag_path_a = true;

				/* Set tx_agc Page C{}; */
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, OFDM);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS0_MCS7);
				PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS8_MCS15);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_B Lower then BBSwing lower bound  0, Remnant tx_agc value = %d\n", cali_info->remnant_ofdm_swing_idx[rf_path]);
			} else {
				set_iqk_matrix_8192e(dm, final_ofdm_swing_index, RF_PATH_B,
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][4],
					cali_info->iqk_matrix_reg_setting[channel_mapped_index].value[0][5]);

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_B Compensate with BBSwing, final_ofdm_swing_index = %d\n", final_ofdm_swing_index);

				if (cali_info->modify_tx_agc_flag_path_b) { /* If tx_agc has changed, reset tx_agc again */
					cali_info->remnant_ofdm_swing_idx[rf_path] = 0;

					/* Set tx_agc Page C{}; */
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, OFDM);
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS0_MCS7);
					PHY_SetTxPowerIndexByRateSection(adapter, RF_PATH_B, *dm->channel, HT_MCS8_MCS15);

					cali_info->modify_tx_agc_flag_path_a = false;

					RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "******Path_B dm->Modify_TxAGC_Flag = false\n");
				}
			}
		}
	} else
		return;
}	/* odm_TxPwrTrackSetPwr88E */

void
get_delta_swing_table_8192e(
	void			*dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	struct _ADAPTER *adapter = dm->adapter;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);
	u8			tx_rate			= 0xFF;
	u8			channel		 = *dm->channel;

	if (*(dm->mp_mode) == true) {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (MP_DRIVER == 1)
		PMPT_CONTEXT p_mpt_ctx = &(adapter->MptCtx);

		tx_rate = MptToMgntRate(p_mpt_ctx->MptRateIndex);
#endif
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
#ifdef CONFIG_MP_INCLUDED
		PMPT_CONTEXT p_mpt_ctx = &(adapter->mppriv.mpt_ctx);

		tx_rate = mpt_to_mgnt_rate(p_mpt_ctx->mpt_rate_index);
#endif
#endif
#endif
	} else {
		u16	rate	 = *(dm->forced_data_rate);

		if (!rate) { /*auto rate*/
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
			tx_rate = ((PADAPTER)adapter)->HalFunc.GetHwRateFromMRateHandler(dm->tx_rate);
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
			tx_rate = hw_rate_to_m_rate(dm->tx_rate);
#endif
		} else   /*force rate*/
			tx_rate = (u8)rate;
	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "Power Tracking tx_rate=0x%X\n", tx_rate);
	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "Call get_delta_swing_table_8192e Power Tracking tx_rate=0x%X\n", tx_rate);


	if (1 <= channel && channel <= 14) {
		if (IS_CCK_RATE(tx_rate)) {
			*temperature_up_a   = cali_info->delta_swing_table_idx_2g_cck_a_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2g_cck_a_n;
			*temperature_up_b   = cali_info->delta_swing_table_idx_2g_cck_b_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2g_cck_b_n;
		} else {
			*temperature_up_a   = cali_info->delta_swing_table_idx_2ga_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2ga_n;
			*temperature_up_b   = cali_info->delta_swing_table_idx_2gb_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2gb_n;
		}
	} else {
		*temperature_up_a   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_a = (u8 *)delta_swing_table_idx_2ga_n_8188e;
		*temperature_up_b   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_b = (u8 *)delta_swing_table_idx_2ga_n_8188e;
	}

	return;
}



void configure_txpower_track_8192e(
	struct txpwrtrack_cfg	*config
)
{
	config->swing_table_size_cck = CCK_TABLE_SIZE;
	config->swing_table_size_ofdm = OFDM_TABLE_SIZE;
	config->threshold_iqk = IQK_THRESHOLD;
	config->average_thermal_num = AVG_THERMAL_NUM_92E;
	config->rf_path_count = MAX_PATH_NUM_8192E;
	config->thermal_reg_addr = RF_T_METER_88E;

	config->odm_tx_pwr_track_set_pwr = odm_tx_pwr_track_set_pwr92_e;
	config->do_iqk = do_iqk_8192e;
	config->phy_lc_calibrate = halrf_lck_trigger;
	config->get_delta_swing_table = get_delta_swing_table_8192e;
}

/* 1 7.	IQK */
#define MAX_TOLERANCE		5
#define IQK_DELAY_TIME		1		/* ms */

u8			/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_a_iqk_8192e(
	struct dm_struct		*dm,
	boolean		config_path_b
)
{
	u32 reg_eac, reg_e94, reg_e9c;
	u8 result = 0x00;
	RF_DBG(dm, DBG_RF_IQK, "path A IQK!\n");

	/*8192E IQK V2.1 20150713*/
	/*1 Tx IQK*/
	/* path-A IQK setting */

	/*	PA/PAD controlled by 0x0 */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x180);

	/* modify TXIQK mode table */
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x20000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0x07f77);  /* PA off, default: 0x7f7f */

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);


	/*	RF_DBG(dm,DBG_RF_IQK, "path-A IQK setting!\n"); */
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82140303);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x68160000);




	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x00462911);

	/* One shot, path A LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path A LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);

	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else
		RF_DBG(dm, DBG_RF_IQK, "pathA TX IQK is not success\n");

	return result;


}

u8			/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_a_rx_iqk_92e(
	struct dm_struct		*dm,
	boolean		config_path_b
)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4, u4tmp;
	u8 result = 0x00;
	RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK!\n");

	/* 1 Get TXIMR setting */
	RF_DBG(dm, DBG_RF_IQK, "Get RXIQK TXIMR!\n");

	/* modify RXIQK mode table
	* 	RF_DBG(dm,DBG_RF_IQK, "path-A Rx IQK modify RXIQK mode table!\n"); */
	/* leave IQK mode */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);

	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf1173); /* PA off, deafault:0xf117b */

	odm_set_rf_reg(dm, RF_PATH_B, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_B, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G2, RFREGOFFSETMASK, 0xf1173); /* PA off, deafault:0xf117b */

	/*	PA/PAD control by 0x56, and set = 0x0 */
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x980);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x56, RFREGOFFSETMASK, 0x511e0);

	/* enter IQK mode */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/* path-A IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x8216031f);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x6816031f);

	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/* One shot, path A LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path A LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);


	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else {						/*if Tx not OK, ignore Rx*/
		/*	PA/PAD controlled by 0x0*/
		odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x180);
		RF_DBG(dm, DBG_RF_IQK, "pathA get TXIMR is not success\n");
		return result;

	}

	u4tmp = 0x80007C00 | (reg_e94 & 0x3FF0000)  | ((reg_e9c & 0x3FF0000) >> 16);
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, u4tmp);
	RF_DBG(dm, DBG_RF_IQK, "0xe40 = 0x%x u4tmp = 0x%x\n", odm_get_bb_reg(dm, REG_TX_IQK, MASKDWORD), u4tmp);


	/* 1 RX IQK */
	/* modify RXIQK mode table */
	RF_DBG(dm, DBG_RF_IQK, "Do RXIQK!\n");
	/*	RF_DBG(dm,DBG_RF_IQK, "path-A Rx IQK modify RXIQK mode table 2!\n"); */
	/* leave IQK mode */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);

	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7ff2);  /*PA off : default:0xf7ffa*/

	odm_set_rf_reg(dm, RF_PATH_B, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_B, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7ff2);  /*PA off : default:0xf7ffa*/


	/*	PA/PAD control by 0x56, and set = 0x0 */
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x980);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x56, RFREGOFFSETMASK, 0x510e0);
	/*odm_set_rf_reg(dm, RF_PATH_A, RF_0x56, RFREGOFFSETMASK, 0x51000 );*/

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/* path-A IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x821608ff);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x281608ff);

	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160cff);
	 *	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x28160cff); */

	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a891);

	/* One shot, path A LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path A LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);

	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_ea4 = odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xea4 = 0x%x, 0xeac = 0x%x\n", reg_ea4, reg_eac);
	/* monitor image power before & after IQK */
	RF_DBG(dm, DBG_RF_IQK, "0xea0(before IQK)= 0x%x, 0xea8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xea0, MASKDWORD), odm_get_bb_reg(dm, R_0xea8, MASKDWORD));

	/*	PA/PAD controlled by 0x0 */
	/* leave IQK mode */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x180);

	if (!(reg_eac & BIT(27)) &&	/*if Tx is OK, check whether Rx is OK*/
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	else
		RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK is not success!!\n");

	return result;


}

u8				/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_b_iqk_8192e(
	struct dm_struct		*dm
)
{
	u32 reg_eac, reg_eb4, reg_ebc;
	u8	result = 0x00;
	RF_DBG(dm, DBG_RF_IQK, "path B IQK!\n");
	/*1 Tx IQK*/
	/* path-B IQK setting
	* 	RF_DBG(dm,DBG_RF_IQK, "path-B IQK setting!\n"); */

	/*disable path-A PI, prevent path-A re-LOK*/
	odm_set_bb_reg(dm, R_0x820, BIT(8), 0x0);

	/*	PA/PAD controlled by 0x0 */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK, 0x180);


	/* modify TXIQK mode table */
	odm_set_rf_reg(dm, RF_PATH_B, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_B, RF_RCK_OS, RFREGOFFSETMASK, 0x20000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G2, RFREGOFFSETMASK, 0x07f77);  /* PA off, default: 0x7f7f */

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

#if 0
	odm_set_bb_reg(dm, R_0xe28, 0xffffff00, 0x000000);
	RF_DBG(dm, DBG_RF_IQK, "path A 0xdf = 0x%x\n", odm_get_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK));
	RF_DBG(dm, DBG_RF_IQK, "path B 0xdf = 0x%x\n", odm_get_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK));
	odm_set_bb_reg(dm, R_0xe28, 0xffffff00, 0x808000);
#endif

	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82140303);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x68160000);

	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x00462911);

	/* One shot, path B LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path B LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xfa000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/*	odm_set_bb_reg(dm, REG_IQK_AGC_CONT, MASKDWORD, 0x00000002);
	 *	odm_set_bb_reg(dm, REG_IQK_AGC_CONT, MASKDWORD, 0x00000000); */

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path B LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);

	/*enable path-A PI*/
	odm_set_bb_reg(dm, R_0x820, BIT(8), 0x1);

	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_eb4 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_B, MASKDWORD);
	reg_ebc = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_B, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xeb4 = 0x%x, 0xebc = 0x%x\n", reg_eb4, reg_ebc);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xeb0(before IQK)= 0x%x, 0xeb8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xeb0, MASKDWORD), odm_get_bb_reg(dm, R_0xeb8, MASKDWORD));


	if (!(reg_eac & BIT(31)) &&
	    (((reg_eb4 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_ebc & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else
		RF_DBG(dm, DBG_RF_IQK, "path B TX IQK is not success\n");

	return result;
}



u8			/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_b_rx_iqk_92e(
	struct dm_struct		*dm,
	boolean		config_path_b
)
{
	u32 reg_eac, reg_eb4, reg_ebc, reg_ecc, reg_ec4, u4tmp;
	u8 result = 0x00;
	RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK!\n");

	/* 1 Get TXIMR setting */
	RF_DBG(dm, DBG_RF_IQK, "Get RXIQK TXIMR!\n");
	/* modify RXIQK mode table
	* 	RF_DBG(dm,DBG_RF_IQK, "path-B Rx IQK modify RXIQK mode table!\n"); */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);

	odm_set_rf_reg(dm, RF_PATH_B, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_B, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G2, RFREGOFFSETMASK, 0xf1173);  /*PA off, default: 0xf117b*/

	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf1173);  /*PA off, default: 0xf117b*/

	/*	PA/PAD all off */
	odm_set_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK, 0x980);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x56, RFREGOFFSETMASK, 0x511e0);

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/* path-B IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x8216031f);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x6816031f);

	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/* One shot, path B LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path B LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xfa000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path B LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);

	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_eb4 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_B, MASKDWORD);
	reg_ebc = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_B, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xeb4 = 0x%x, 0xebc = 0x%x\n", reg_eb4, reg_ebc);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xeb0(before IQK)= 0x%x, 0xeb8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xeb0, MASKDWORD), odm_get_bb_reg(dm, R_0xeb8, MASKDWORD));


	if (!(reg_eac & BIT(31)) &&
	    (((reg_eb4 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_ebc & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else {						/*if Tx not OK, ignore Rx*/
		/*	PA/PAD controlled by 0x0 */
		odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
		odm_set_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK, 0x180);
		RF_DBG(dm, DBG_RF_IQK, "pathB get TXIMR is not success\n");
		return result;
	}

	u4tmp = 0x80007C00 | (reg_eb4 & 0x3FF0000)  | ((reg_ebc & 0x3FF0000) >> 16);
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, u4tmp);
	RF_DBG(dm, DBG_RF_IQK, "0xe40 = 0x%x u4tmp = 0x%x\n", odm_get_bb_reg(dm, REG_TX_IQK, MASKDWORD), u4tmp);


	/* 1 RX IQK */
	RF_DBG(dm, DBG_RF_IQK, "Do RXIQK!\n");

	/* modify RXIQK mode table
	* 	RF_DBG(dm,DBG_RF_IQK, "path-B Rx IQK modify RXIQK mode table 2!\n"); */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);

	odm_set_rf_reg(dm, RF_PATH_B, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_B, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_B, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7ff2);  /* PA off, default:0xf7ffa*/

	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7ff2);   /* PA off, default:0xf7ffa*/

	/*	PA/PAD all off */
	odm_set_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK, 0x980);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x56, RFREGOFFSETMASK, 0x510e0);
	/*	odm_set_rf_reg(dm, RF_PATH_B, RF_0x56, RFREGOFFSETMASK, 0x51000 ); */

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/* path-B IQK setting */
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x18008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x821608ff);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x281608ff);

	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82160cff);
	 *	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28160cff); */

	/* LO calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "LO calibration setting!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a891);

	/* One shot, path B LOK & IQK
	* 	RF_DBG(dm,DBG_RF_IQK, "One shot, path B LOK & IQK!\n"); */
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xfa000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms
	* 	RF_DBG(dm,DBG_RF_IQK, "delay %d ms for One shot, path B LOK & IQK.\n", IQK_DELAY_TIME_92E); */
	/* platform_stall_execution(IQK_DELAY_TIME_92E*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_92E);


	/* Check failed */
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_ec4 = odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_B_2, MASKDWORD);
	reg_ecc = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_B_2, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xec4 = 0x%x, 0xecc = 0x%x\n", reg_ec4, reg_ecc);
	/* monitor image power before & after IQK */
	RF_DBG(dm, DBG_RF_IQK, "0xec0(before IQK)= 0x%x, 0xec8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xec0, MASKDWORD), odm_get_bb_reg(dm, R_0xec8, MASKDWORD));

	/*	PA/PAD controlled by 0x0 */
	/* leave IQK mode */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0xdf, RFREGOFFSETMASK, 0x180);

	if (!(reg_eac & BIT(30)) &&/*if Tx is OK, check whether Rx is OK*/
	    (((reg_ec4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_ecc & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	else
		RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK is not success!!\n");

	return result;


}


void
_phy_path_a_fill_iqk_matrix_92e(
	struct dm_struct		*dm,
	boolean	is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only
)
{
	u32	oldval_0, X, TX0_A, reg;
	s32	Y, TX0_C;
	RF_DBG(dm, DBG_RF_IQK, "path A IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed");

	if (final_candidate == 0xFF)
		return;

	else if (is_iqk_ok) {
		oldval_0 = (odm_get_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][0];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;
		TX0_A = (X * oldval_0) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "X = 0x%x, TX0_A = 0x%x, oldval_0 0x%x\n", X, TX0_A, oldval_0);
		odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x3FF, TX0_A);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(31), ((X * oldval_0 >> 7) & 0x1));

		Y = result[final_candidate][1];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;


		TX0_C = (Y * oldval_0) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "Y = 0x%x, TX = 0x%x\n", Y, TX0_C);
		odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, 0xF0000000, ((TX0_C & 0x3C0) >> 6));
		odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x003F0000, (TX0_C & 0x3F));

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(29), ((Y * oldval_0 >> 7) & 0x1));

		if (is_tx_only) {
			RF_DBG(dm, DBG_RF_IQK, "_phy_path_a_fill_iqk_matrix_92e only Tx OK\n");
			return;
		}

		reg = result[final_candidate][2];
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		if (RTL_ABS(reg, 0x100) >= 16)
			reg = 0x100;
#endif
		odm_set_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0x3FF, reg);

		reg = result[final_candidate][3] & 0x3F;
		odm_set_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0xFC00, reg);

		reg = (result[final_candidate][3] >> 6) & 0xF;
		odm_set_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, 0xF0000000, reg);
	}
}

void
_phy_path_b_fill_iqk_matrix_92e(
	struct dm_struct		*dm,
	boolean	is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only			/* do Tx only */
)
{
	u32	oldval_1, X, TX1_A, reg;
	s32	Y, TX1_C;
	RF_DBG(dm, DBG_RF_IQK, "path B IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed");

	if (final_candidate == 0xFF)
		return;

	else if (is_iqk_ok) {
		oldval_1 = (odm_get_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][4];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;
		TX1_A = (X * oldval_1) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "X = 0x%x, TX1_A = 0x%x\n", X, TX1_A);
		odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x3FF, TX1_A);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(27), ((X * oldval_1 >> 7) & 0x1));

		Y = result[final_candidate][5];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;

		TX1_C = (Y * oldval_1) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "Y = 0x%x, TX1_C = 0x%x\n", Y, TX1_C);
		odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, 0xF0000000, ((TX1_C & 0x3C0) >> 6));
		odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x003F0000, (TX1_C & 0x3F));

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(25), ((Y * oldval_1 >> 7) & 0x1));

		if (is_tx_only)
			return;

		reg = result[final_candidate][6];
		odm_set_bb_reg(dm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0x3FF, reg);

		reg = result[final_candidate][7] & 0x3F;
		odm_set_bb_reg(dm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0xFC00, reg);

		reg = (result[final_candidate][7] >> 6) & 0xF;
		odm_set_bb_reg(dm, REG_OFDM_0_AGC_RSSI_TABLE, 0x0000F000, reg);
	}
}

void
_phy_save_adda_registers_92e(
	struct dm_struct		*dm,
	u32		*adda_reg,
	u32		*adda_backup,
	u32		register_num
)
{
	u32	i;

	if (odm_check_power_status(dm) == false)
		return;

	/*	RF_DBG(dm,DBG_RF_IQK, "Save ADDA parameters.\n"); */
	for (i = 0 ; i < register_num ; i++)
		adda_backup[i] = odm_get_bb_reg(dm, adda_reg[i], MASKDWORD);
}


void
_phy_save_mac_registers_92e(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;
	/*	RF_DBG(dm,DBG_RF_IQK, "Save MAC parameters.\n"); */
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		mac_backup[i] = odm_read_1byte(dm, mac_reg[i]);
	mac_backup[i] = odm_read_4byte(dm, mac_reg[i]);

}


void
_phy_reload_adda_registers_92e(
	struct dm_struct		*dm,
	u32		*adda_reg,
	u32		*adda_backup,
	u32		regiester_num
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Reload ADDA power saving parameters !\n");
	for (i = 0 ; i < regiester_num; i++)
		odm_set_bb_reg(dm, adda_reg[i], MASKDWORD, adda_backup[i]);
}

void
_phy_reload_mac_registers_92e(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Reload MAC parameters !\n");
#if 0
	odm_set_bb_reg(dm, R_0x520, MASKBYTE2, 0x0);
#else
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		odm_write_1byte(dm, mac_reg[i], (u8)mac_backup[i]);
	odm_write_4byte(dm, mac_reg[i], mac_backup[i]);
#endif
}


void
_phy_path_adda_on_92e(
	struct dm_struct		*dm,
	u32		*adda_reg,
	boolean		is_path_a_on,
	boolean		is2T
)
{
	u32	path_on;
	u32	i;
	
	/*	RF_DBG(dm,DBG_RF_IQK, "ADDA ON.\n"); */

	path_on = is_path_a_on ? 0x0fc01616 : 0x0fc01616;
	if (false == is2T) {
		path_on = 0x0fc01616;
		odm_set_bb_reg(dm, adda_reg[0], MASKDWORD, 0x0fc01616);
	} else
		odm_set_bb_reg(dm, adda_reg[0], MASKDWORD, path_on);

	for (i = 1 ; i < IQK_ADDA_REG_NUM ; i++)
		odm_set_bb_reg(dm, adda_reg[i], MASKDWORD, path_on);

}

void
_phy_mac_setting_calibration_92e(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	/* u32	i = 0; */
	/*	RF_DBG(dm,DBG_RF_IQK, "MAC settings for Calibration.\n"); */
	/*
		odm_write_1byte(dm, mac_reg[i], 0x3F);

		for(i = 1 ; i < (IQK_MAC_REG_NUM - 1); i++){
			odm_write_1byte(dm, mac_reg[i], (u8)(mac_backup[i]&(~BIT(3))));
		}
		odm_write_1byte(dm, mac_reg[i], (u8)(mac_backup[i]&(~BIT(5))));
	*/

	/* odm_set_bb_reg(dm, R_0x522, MASKBYTE0, 0x7f); */
	/* odm_set_bb_reg(dm, R_0x550, MASKBYTE0, 0x15); */
	/* odm_set_bb_reg(dm, R_0x551, MASKBYTE0, 0x00); */

	odm_set_bb_reg(dm, R_0x520, 0x00ff0000, 0xff);
	odm_set_bb_reg(dm, R_0x040, 0x20, 0x0);
	/*	odm_set_bb_reg(dm, R_0x550, 0x0000ffff, 0x0015); */


}

void
_phy_path_a_stand_by_92e(
	struct dm_struct		*dm
)
{
	RF_DBG(dm, DBG_RF_IQK, "path-A standby mode!\n");

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	/*	odm_set_bb_reg(dm, R_0x840, MASKDWORD, 0x00010000); */
	odm_set_rf_reg(dm, (enum rf_path)0x0, RF_0x0, RFREGOFFSETMASK, 0x10000);

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);
}

void
_phy_path_b_stand_by_92e(
	struct dm_struct		*dm
)
{
	RF_DBG(dm, DBG_RF_IQK, "path-A standby mode!\n");

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);
	odm_set_rf_reg(dm, (enum rf_path)0x1, RF_0x0, RFREGOFFSETMASK, 0x10000);

	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);
}

void
_phy_pi_mode_switch_92e(
	struct dm_struct		*dm,
	boolean		pi_mode
)
{
	u32	mode;

	/*	RF_DBG(dm,DBG_RF_IQK, "BB Switch to %s mode!\n", (pi_mode ? "PI" : "SI")); */

	mode = pi_mode ? 0x01000100 : 0x01000000;
	odm_set_bb_reg(dm, REG_FPGA0_XA_HSSI_PARAMETER1, MASKDWORD, mode);
	odm_set_bb_reg(dm, REG_FPGA0_XB_HSSI_PARAMETER1, MASKDWORD, mode);
}

boolean
phy_simularity_compare_8192e(
	struct dm_struct		*dm,
	s32		result[][8],
	u8		 c1,
	u8		 c2
)
{
	u32		i, j, diff, simularity_bit_map, bound = 0;
	u8		final_candidate[2] = {0xFF, 0xFF};	/* for path A and path B */
	boolean		is_result = true;
	/*#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)*/
	/*	bool		is2T = IS_92C_SERIAL( hal_data->version_id);*/
	/*#else*/
	boolean		is2T = true;
	/*#endif*/

	s32 tmp1 = 0, tmp2 = 0;

	if (is2T)
		bound = 8;
	else
		bound = 4;

	RF_DBG(dm, DBG_RF_IQK, "===> IQK:phy_simularity_compare_8192e c1 %d c2 %d!!!\n", c1, c2);


	simularity_bit_map = 0;

	for (i = 0; i < bound; i++) {

		if ((i == 1) || (i == 3) || (i == 5) || (i == 7)) {
			if ((result[c1][i] & 0x00000200) != 0)
				tmp1 = result[c1][i] | 0xFFFFFC00;
			else
				tmp1 = result[c1][i];

			if ((result[c2][i] & 0x00000200) != 0)
				tmp2 = result[c2][i] | 0xFFFFFC00;
			else
				tmp2 = result[c2][i];
		} else {
			tmp1 = result[c1][i];
			tmp2 = result[c2][i];
		}

		diff = (tmp1 > tmp2) ? (tmp1 - tmp2) : (tmp2 - tmp1);

		if (diff > MAX_TOLERANCE) {
			RF_DBG(dm, DBG_RF_IQK, "IQK:differnece overflow %d index %d compare1 0x%x compare2 0x%x!!!\n",  diff, i, result[c1][i], result[c2][i]);

			if ((i == 2 || i == 6) && !simularity_bit_map) {
				if (result[c1][i] + result[c1][i + 1] == 0)
					final_candidate[(i / 4)] = c2;
				else if (result[c2][i] + result[c2][i + 1] == 0)
					final_candidate[(i / 4)] = c1;
				else
					simularity_bit_map = simularity_bit_map | (1 << i);
			} else
				simularity_bit_map = simularity_bit_map | (1 << i);
		}
	}

	RF_DBG(dm, DBG_RF_IQK, "IQK:phy_simularity_compare_8192e simularity_bit_map   %x !!!\n", simularity_bit_map);

	if (simularity_bit_map == 0) {
		for (i = 0; i < (bound / 4); i++) {
			if (final_candidate[i] != 0xFF) {
				for (j = i * 4; j < (i + 1) * 4 - 2; j++)
					result[3][j] = result[final_candidate[i]][j];
				is_result = false;
			}
		}
		return is_result;
	} else {

		if (!(simularity_bit_map & 0x03)) {		/*path A TX OK*/
			for (i = 0; i < 2; i++)
				result[3][i] = result[c1][i];
		}

		if (!(simularity_bit_map & 0x0c)) {		/*path A RX OK*/
			for (i = 2; i < 4; i++)
				result[3][i] = result[c1][i];
		}

		if (!(simularity_bit_map & 0x30)) {	/*path B TX OK*/
			for (i = 4; i < 6; i++)
				result[3][i] = result[c1][i];

		}

		if (!(simularity_bit_map & 0xc0)) {	/*path B RX OK*/
			for (i = 6; i < 8; i++)
				result[3][i] = result[c1][i];
		}

		return false;
	}



}



void
_phy_iq_calibrate_8192e(
	struct dm_struct		*dm,
	s32		result[][8],
	u8		t,
	boolean		is2T
)
{
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);
	u32			i;
	u8			path_aok = 0, path_bok = 0;
	u8			tmp0xc50 = (u8)odm_get_bb_reg(dm, R_0xc50, MASKBYTE0);
	u8			tmp0xc58 = (u8)odm_get_bb_reg(dm, R_0xc58, MASKBYTE0);
	u32			ADDA_REG[IQK_ADDA_REG_NUM] = {
		REG_FPGA0_XCD_SWITCH_CONTROL,	REG_BLUE_TOOTH,
		REG_RX_WAIT_CCA,		REG_TX_CCK_RFON,
		REG_TX_CCK_BBON,	REG_TX_OFDM_RFON,
		REG_TX_OFDM_BBON,	REG_TX_TO_RX,
		REG_TX_TO_TX,		REG_RX_CCK,
		REG_RX_OFDM,		REG_RX_WAIT_RIFS,
		REG_RX_TO_RX,		REG_STANDBY,
		REG_SLEEP,			REG_PMPD_ANAEN
	};
	u32			IQK_MAC_REG[IQK_MAC_REG_NUM] = {
		REG_TXPAUSE,		REG_BCN_CTRL,
		REG_BCN_CTRL_1,	REG_GPIO_MUXCFG
	};

	/*since 92C & 92D have the different define in IQK_BB_REG*/
	u32	IQK_BB_REG_92C[IQK_BB_REG_NUM] = {
		REG_OFDM_0_TRX_PATH_ENABLE,		REG_OFDM_0_TR_MUX_PAR,
		REG_FPGA0_XCD_RF_INTERFACE_SW,	REG_CONFIG_ANT_A,	REG_CONFIG_ANT_B,
		0x92c,	0x930,
		0x938,	REG_CCK_0_AFE_SETTING
	};

#if MP_DRIVER
	const u32	retry_count = 9;
#else
	const u32	retry_count = 2;
#endif

	/*Note: IQ calibration must be performed after loading*/
	/*PHY_REG.txt,and radio_a,radio_b.txt*/

	/* u32 bbvalue; */



	if (t == 0) {
		/*bbvalue = odm_get_bb_reg(dm, REG_FPGA0_RFMOD, MASKDWORD);*/
		/*RT_DISP(FINIT, INIT_IQK, ("_phy_iq_calibrate_8188e()==>0x%08x\n",bbvalue));*/
		/*RF_DBG(dm,DBG_RF_IQK, "IQ Calibration for %s for %d times\n", (is2T ? "2T2R" : "1T1R"), t);*/

		/*Save ADDA parameters, turn path A ADDA on*/
		_phy_save_adda_registers_92e(dm, ADDA_REG, cali_info->ADDA_backup, IQK_ADDA_REG_NUM);
		_phy_save_mac_registers_92e(dm, IQK_MAC_REG, cali_info->IQK_MAC_backup);
		_phy_save_adda_registers_92e(dm, IQK_BB_REG_92C, cali_info->IQK_BB_backup, IQK_BB_REG_NUM);
	}
	RF_DBG(dm, DBG_RF_IQK, "IQ Calibration for %s for %d times\n", (is2T ? "2T2R" : "1T1R"), t);

	_phy_path_adda_on_92e(dm, ADDA_REG, true, is2T);

#if 0
	if (t == 0)
		cali_info->is_rf_pi_enable = (u8)odm_get_bb_reg(dm, REG_FPGA0_XA_HSSI_PARAMETER1, BIT(8));

	if (!cali_info->is_rf_pi_enable) {
		/*  Switch BB to PI mode to do IQ Calibration. */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		_phy_pi_mode_switch_92e(adapter, true);
#else
		_phy_pi_mode_switch_92e(dm, true);
#endif
	}
#endif
	/* MAC settings */
	_phy_mac_setting_calibration_92e(dm, IQK_MAC_REG, cali_info->IQK_MAC_backup);

	/* BB setting */
	/* odm_set_bb_reg(dm, REG_FPGA0_RFMOD, BIT24, 0x00); */
	odm_set_bb_reg(dm, REG_CCK_0_AFE_SETTING, 0x0f000000, 0xf);
	odm_set_bb_reg(dm, REG_OFDM_0_TRX_PATH_ENABLE, MASKDWORD, 0x03a05600);
	odm_set_bb_reg(dm, REG_OFDM_0_TR_MUX_PAR, MASKDWORD, 0x000800e4);
	odm_set_bb_reg(dm, REG_FPGA0_XCD_RF_INTERFACE_SW, MASKDWORD, 0x55204200);

	if ((dm->ext_lna) && !(dm->ext_pa)) { /* external LNA / external PA = 1 /0 */
		/*PAPE force to high*/
		/*just for high power with external LNA, without external PA*/
		odm_set_bb_reg(dm, R_0x930, 0xf, 0x7);
		odm_set_bb_reg(dm, R_0x930, 0x0f000000, 0x7);
		odm_set_bb_reg(dm, R_0x938, 0xf, 0x7);
		odm_set_bb_reg(dm, R_0x938, 0x0f000000, 0x7);
		odm_set_bb_reg(dm, R_0x92c, MASKDWORD, 0x00410041);
	} else if (dm->ext_pa) { /* external PA = 1*/
		/*PAPE force to low*/
		/*just for high power with external PA, without external LNA*/
		odm_set_bb_reg(dm, R_0x930, 0xf, 0x7);
		odm_set_bb_reg(dm, R_0x930, 0x0f000000, 0x7);
		odm_set_bb_reg(dm, R_0x938, 0xf, 0x7);
		odm_set_bb_reg(dm, R_0x938, 0x0f000000, 0x7);
		odm_set_bb_reg(dm, R_0x92c, MASKDWORD, 0x00000000);
	}

	/* IQ calibration setting
	* 	RF_DBG(dm,DBG_RF_IQK, "IQK setting!\n"); */
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	if (is2T) {
		_phy_path_b_stand_by_92e(dm);
		/* Turn ADDA on */
		_phy_path_adda_on_92e(dm, ADDA_REG, false, is2T);
	}

	/* path A TXIQK */
#if 1
	for (i = 0 ; i < retry_count ; i++) {
		path_aok = phy_path_a_iqk_8192e(dm, is2T);
		/*		if(path_aok == 0x03){ */
		if (path_aok == 0x01) {
			RF_DBG(dm, DBG_RF_IQK, "path A Tx IQK Success!!\n");
			result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
			result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
			break;
		} else
			RF_DBG(dm, DBG_RF_IQK, "path A Tx IQK Fail!!\n");
#if 0
		else if (i == (retry_count - 1) && path_aok == 0x01) {	/*Tx IQK OK*/
			RT_DISP(FINIT, INIT_IQK, ("path A IQK Only  Tx Success!!\n"));

			result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
			result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
		}
#endif
	}
#endif

	/* path A RXIQK */
#if 1
	for (i = 0 ; i < retry_count ; i++) {
		path_aok = phy_path_a_rx_iqk_92e(dm, is2T);
		if (path_aok == 0x03) {
			RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK Success!!\n");
			/*				result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD)&0x3FF0000)>>16;
			 *				result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD)&0x3FF0000)>>16; */
			result[t][2] = (odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
			result[t][3] = (odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
			break;
		} else
			RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK Fail!!\n");
	}

	if (0x00 == path_aok)
		RF_DBG(dm, DBG_RF_IQK, "path A IQK failed!!\n");

#endif

	if (is2T) {
		_phy_path_a_stand_by_92e(dm);
		/* Turn ADDA on */
		_phy_path_adda_on_92e(dm, ADDA_REG, false, is2T);
		/* IQ calibration setting */
		/*RF_DBG(dm,DBG_RF_IQK, "IQK setting!\n");*/
		odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x808000);
		odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
		odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

		/* path B Tx IQK */
#if 1
		for (i = 0 ; i < retry_count ; i++) {
			path_bok = phy_path_b_iqk_8192e(dm);
			/*		if(path_bok == 0x03){ */
			if (path_bok == 0x01) {
				RF_DBG(dm, DBG_RF_IQK, "path B Tx IQK Success!!\n");
				result[t][4] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_B, MASKDWORD) & 0x3FF0000) >> 16;
				result[t][5] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_B, MASKDWORD) & 0x3FF0000) >> 16;
				break;
			}
#if 0
			else if (i == (retry_count - 1) && path_aok == 0x01) {	/*Tx IQK OK*/
				RT_DISP(FINIT, INIT_IQK, ("path B IQK Only  Tx Success!!\n"));

				result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_B, MASKDWORD) & 0x3FF0000) >> 16;
				result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_B, MASKDWORD) & 0x3FF0000) >> 16;
			}
#endif
		}
#endif

		/* path B RX IQK */
#if 1

		for (i = 0 ; i < retry_count ; i++) {
			path_bok = phy_path_b_rx_iqk_92e(dm, is2T);
			if (path_bok == 0x03) {
				RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK Success!!\n");
				/*				result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD)&0x3FF0000)>>16;
				 *				result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD)&0x3FF0000)>>16; */
				result[t][6] = (odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_B_2, MASKDWORD) & 0x3FF0000) >> 16;
				result[t][7] = (odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_B_2, MASKDWORD) & 0x3FF0000) >> 16;
				break;
			}
		}

		if (0x00 == path_bok) {
			RF_DBG(dm, DBG_RF_IQK, "path B IQK failed!!\n");
			/**/
		}
#endif
	}

	/* Back to BB mode, load original value */
	RF_DBG(dm, DBG_RF_IQK, "IQK:Back to BB mode, load original value!\n");
	odm_set_bb_reg(dm, REG_FPGA0_IQK, 0xffffff00, 0x000000);

	if (t != 0) {
		/* Reload ADDA power saving parameters*/
		_phy_reload_adda_registers_92e(dm, ADDA_REG, cali_info->ADDA_backup, IQK_ADDA_REG_NUM);
		/* Reload MAC parameters*/
		_phy_reload_mac_registers_92e(dm, IQK_MAC_REG, cali_info->IQK_MAC_backup);
		_phy_reload_adda_registers_92e(dm, IQK_BB_REG_92C, cali_info->IQK_BB_backup, IQK_BB_REG_NUM);
		/*Allen initial gain 0xc50*/
		/* Restore RX initial gain*/
		odm_set_bb_reg(dm, R_0xc50, MASKBYTE0, 0x50);
		odm_set_bb_reg(dm, R_0xc50, MASKBYTE0, tmp0xc50);
		if (is2T) {
			odm_set_bb_reg(dm, R_0xc58, MASKBYTE0, 0x50);
			odm_set_bb_reg(dm, R_0xc58, MASKBYTE0, tmp0xc58);
		}
		/* load 0xe30 IQC default value */
		odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x01008c00);
		odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x01008c00);
	}
	RF_DBG(dm, DBG_RF_IQK, "_phy_iq_calibrate_8192e() <==\n");
}


void
_phy_lc_calibrate_8192e(
	struct dm_struct		*dm,
	boolean		is2T
)
{
	u8	tmp_reg, bb_clk;
	u32	rf_amode = 0, rf_bmode = 0, lc_cal;
	/* Check continuous TX and Packet TX */
	tmp_reg = odm_read_1byte(dm, 0xd03);

	if ((tmp_reg & 0x70) != 0)				/*Deal with contisuous TX case*/
		odm_write_1byte(dm, 0xd03, tmp_reg & 0x8F);	/*disable all continuous TX*/
	else							/* Deal with Packet TX case*/
		odm_write_1byte(dm, REG_TXPAUSE, 0xFF);	/* block all queues*/

	if ((tmp_reg & 0x70) != 0) {
		/* 1. Read original RF mode */
		/* path-A */
		rf_amode = odm_get_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS);

		/* path-B */
		if (is2T)
			rf_bmode = odm_get_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS);

		/* 2. Set RF mode = standby mode */
		/* path-A */
		odm_set_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS, (rf_amode & 0x8FFFF) | 0x10000);

		/* path-B */
		if (is2T)
			odm_set_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS, (rf_bmode & 0x8FFFF) | 0x10000);
	}

	/* 3. Read RF reg18 */
	lc_cal = odm_get_rf_reg(dm, RF_PATH_A, RF_CHNLBW, MASK12BITS);

	/*backup bb_clk and set bb_clk to 80MHz to avoid LCK fail for 10M application*/
	bb_clk = odm_read_1byte(dm, 0xce7);
	odm_set_bb_reg(dm, R_0xce4, 0xc0000000, 0x0);

	/* 4. Set LC calibration begin	bit15 */
	odm_set_rf_reg(dm, RF_PATH_A, RF_CHNLBW, MASK12BITS, lc_cal | 0x08000);

	ODM_delay_ms(100);


	/* Restore original situation */

	odm_write_1byte(dm, 0xce7, bb_clk);
	
	if ((tmp_reg & 0x70) != 0) {	/*Deal with contisuous TX case*/
		/* path-A */
		odm_write_1byte(dm, 0xd03, tmp_reg);
		odm_set_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS, rf_amode);

		/* path-B */
		if (is2T)
			odm_set_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS, rf_bmode);
	} else     /*Deal with Packet TX case*/
		odm_write_1byte(dm, REG_TXPAUSE, 0x00);
}


void
phy_iq_calibrate_8192e(
	void		*dm_void,
	boolean	is_recovery
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);
	s32			result[4][8];	/* last is final result */
	u8			i, final_candidate, indexforchannel;
	boolean			is_patha_ok, is_pathb_ok;
	s32			rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc;
	boolean			is12simular, is13simular, is23simular;
	u32			IQK_BB_REG_92C[IQK_BB_REG_NUM] = {
		REG_OFDM_0_XA_RX_IQ_IMBALANCE,	REG_OFDM_0_XB_RX_IQ_IMBALANCE,
		REG_OFDM_0_ECCA_THRESHOLD,	REG_OFDM_0_AGC_RSSI_TABLE,
		REG_OFDM_0_XA_TX_IQ_IMBALANCE,	REG_OFDM_0_XB_TX_IQ_IMBALANCE,
		REG_OFDM_0_XC_TX_AFE,			REG_OFDM_0_XD_TX_AFE,
		REG_OFDM_0_RX_IQ_EXT_ANTA
	};

 /*for ODM_WIN*/
	if (is_recovery && (!dm->is_in_hct_test)) {/*YJ,add for PowerTest,120405*/
		RF_DBG(dm, DBG_RF_INIT, "PHY_IQCalibrate_92E: Return due to is_recovery!\n");
		_phy_reload_adda_registers_92e(dm, IQK_BB_REG_92C, cali_info->IQK_BB_backup_recover, 9);
		return;
	}
	RF_DBG(dm, DBG_RF_IQK, "IQK:Start!!!\n");

	for (i = 0; i < 8; i++) {
		result[0][i] = 0;
		result[1][i] = 0;
		result[2][i] = 0;

		if ((i == 0) || (i == 2) || (i == 4)  || (i == 6))
			result[3][i] = 0x100;
		else
			result[3][i] = 0;
	}

	final_candidate = 0xff;
	is_patha_ok = false;
	is_pathb_ok = false;
	is12simular = false;
	is23simular = false;
	is13simular = false;

	for (i = 0; i < 3; i++) {
		/*	 		_phy_iq_calibrate_8192e(dm, result, i, false); */
		_phy_iq_calibrate_8192e(dm, result, i, true);
		if (i == 1) {
			is12simular = phy_simularity_compare_8192e(dm, result, 0, 1);
			if (is12simular) {
				final_candidate = 0;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is12simular final_candidate is %x\n", final_candidate);
				break;
			}
		}

		if (i == 2) {
			is13simular = phy_simularity_compare_8192e(dm, result, 0, 2);
			if (is13simular) {
				final_candidate = 0;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is13simular final_candidate is %x\n", final_candidate);

				break;
			}
			is23simular = phy_simularity_compare_8192e(dm, result, 1, 2);
			if (is23simular) {
				final_candidate = 1;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is23simular final_candidate is %x\n", final_candidate);
			} else {
				/*
								for(i = 0; i < 4; i++)
									reg_tmp &= result[3][i*2];

								if(reg_tmp != 0)
									final_candidate = 3;
								else
									final_candidate = 0xFF;
				*/
				final_candidate = 3;

			}
		}
	}

	for (i = 0; i < 4; i++) {
		rege94 = result[i][0];
		rege9c = result[i][1];
		regea4 = result[i][2];
		regeac = result[i][3];
		regeb4 = result[i][4];
		regebc = result[i][5];
		regec4 = result[i][6];
		regecc = result[i][7];
		RF_DBG(dm, DBG_RF_IQK, "IQK: rege94=%x rege9c=%x regea4=%x regeac=%x regeb4=%x regebc=%x regec4=%x regecc=%x\n ", rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc);
	}

	if (final_candidate != 0xff) {
		cali_info->rege94 = rege94 = result[final_candidate][0];
		cali_info->rege9c = rege9c = result[final_candidate][1];
		regea4 = result[final_candidate][2];
		regeac = result[final_candidate][3];
		cali_info->regeb4 = regeb4 = result[final_candidate][4];
		cali_info->regebc = regebc = result[final_candidate][5];
		regec4 = result[final_candidate][6];
		regecc = result[final_candidate][7];
		RF_DBG(dm, DBG_RF_IQK, "IQK: final_candidate is %x\n", final_candidate);
		RF_DBG(dm, DBG_RF_IQK, "IQK: TX0_X=%x TX0_Y=%x RX0_X=%x RX0_Y=%x TX1_X=%x TX1_Y=%x RX1_X=%x RX1_Y=%x\n ", rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc);
		is_patha_ok = is_pathb_ok = true;
	} else {
		RF_DBG(dm, DBG_RF_IQK, "IQK: FAIL use default value\n");

		cali_info->rege94 = cali_info->regeb4 = 0x100;	/* X default value */
		cali_info->rege9c = cali_info->regebc = 0x0;		/* Y default value */
	}

	if ((rege94 != 0)/*&&(regea4 != 0)*/)
		_phy_path_a_fill_iqk_matrix_92e(dm, is_patha_ok, result, final_candidate, (regea4 == 0));
		_phy_path_b_fill_iqk_matrix_92e(dm, is_pathb_ok, result, final_candidate, (regec4 == 0));


	indexforchannel = odm_get_right_chnl_place_for_iqk(*dm->channel);

	/* To Fix BSOD when final_candidate is 0xff
	 * by sherry 20120321 */
	if (final_candidate < 4) {
		for (i = 0; i < iqk_matrix_reg_num; i++)
			cali_info->iqk_matrix_reg_setting[indexforchannel].value[0][i] = result[final_candidate][i];
		cali_info->iqk_matrix_reg_setting[indexforchannel].is_iqk_done = true;
	}
	/* RT_DISP(FINIT, INIT_IQK, ("\nIQK OK indexforchannel %d.\n", indexforchannel)); */
	RF_DBG(dm, DBG_RF_IQK, "\nIQK OK indexforchannel %d.\n", indexforchannel);
	_phy_save_adda_registers_92e(dm, IQK_BB_REG_92C, cali_info->IQK_BB_backup_recover, IQK_BB_REG_NUM);
	RF_DBG(dm, DBG_RF_IQK, "IQK finished\n");
}


void
phy_lc_calibrate_8192e(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;

	_phy_lc_calibrate_8192e(dm, true);
}


void _phy_set_rf_path_switch_8192e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	boolean		is_main,
	boolean		is2T
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(((PADAPTER)adapter));
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct dm_struct		*dm = &hal_data->odmpriv;
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct dm_struct		*dm = &hal_data->DM_OutSrc;
#endif
#endif

	/* MAIN: WiFi */
	u32 reg = odm_get_mac_reg(dm, R_0x64, MASKDWORD);
	reg &= 0xFFFFF000;
	reg = (is_main) ? (reg | 0x00000100) : (reg | 0x00000004);
	odm_set_mac_reg(dm, R_0x64, MASKDWORD, reg);
}

void phy_set_rf_path_switch_8192e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	boolean		is_main
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	/* HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(((PADAPTER)adapter)); */
#endif

#if DISABLE_BB_RF
	return;
#endif

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)

	_phy_set_rf_path_switch_8192e(adapter, is_main, true);
#endif

}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
/* return value true => Main; false => Aux */
boolean _phy_query_rf_path_switch_8192e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm,
#else
	void	*adapter,
#endif
	boolean		is2T
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(((PADAPTER)adapter));
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct dm_struct		*dm = &hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct dm_struct		*dm = &hal_data->DM_OutSrc;
#endif
#endif
	/* MAIN: WiFi */

	if (odm_get_mac_reg(dm, R_0x64, 0xFFF) == 0x100)
		return true;
	else
		return false;

}



/* return value true => Main; false => Aux */
boolean phy_query_rf_path_switch_8192e(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct dm_struct		*dm
#else
	void	*adapter
#endif
)
{
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(((PADAPTER)adapter));

#if DISABLE_BB_RF
	return true;
#endif
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	if (IS_2T2R(hal_data->VersionID))
		return _phy_query_rf_path_switch_8192e(adapter, true);
	else
#endif
	{
		/* For 88C 1T1R */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		return _phy_query_rf_path_switch_8192e(adapter, false);
#else
		return _phy_query_rf_path_switch_8192e(dm, false);
#endif
	}
}
#endif

#else	/* #if (RTL8192E_SUPPORT == 1)*/

void
phy_iq_calibrate_8192e(
	void		*dm_void,
	boolean	is_recovery
) {}
void
phy_lc_calibrate_8192e(
	void			*dm_void
) {}

void
odm_tx_pwr_track_set_pwr92_e(
	void			*dm_void,
	enum pwrtrack_method		method,
	u8				rf_path,
	u8				channel_mapped_index
) {}

#endif	/* #if (RTL8192E_SUPPORT == 1)*/y
