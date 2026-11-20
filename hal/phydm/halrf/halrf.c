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

/*@************************************************************
 * include files
 * ************************************************************
 */

#include "phydm_precomp.h"


u8 halrf_match_iqk_version(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	u32 iqk_version = 0;
	char temp[10] = {0};

	odm_move_memory(dm, temp, HALRF_IQK_VER, sizeof(temp));
	PHYDM_SSCANF(temp + 2, DCMD_HEX, &iqk_version);

	if (dm->support_ic_type == ODM_RTL8822B) {
		if (iqk_version >= 0x24 && (odm_get_hw_img_version(dm) >= 72))
			return 1;
		else if ((iqk_version <= 0x23) &&
			 (odm_get_hw_img_version(dm) <= 71))
			return 1;
		else
			return 0;
	}

	if (dm->support_ic_type == ODM_RTL8821C) {
		if (iqk_version >= 0x18 && (odm_get_hw_img_version(dm) >= 37))
			return 1;
		else
			return 0;
	}

	return 1;
}

void halrf_rf_lna_setting(void *dm_void, enum halrf_lna_set type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	switch (dm->support_ic_type) {
	case ODM_RTL8192E:
		halrf_rf_lna_setting_8192e(dm, type);
		break;

	default:
		break;
	}
}

void halrf_support_ability_debug(void *dm_void, char input[][16], u32 *_used,
				 char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;
	u32 dm_value[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i;

	for (i = 0; i < 5; i++)
		if (input[i + 1][0])
			PHYDM_SSCANF(input[i + 2], DCMD_DECIMAL, &dm_value[i]);

	if (dm_value[0] == 100) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\n[RF Supportability]\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "00. (( %s ))Power Tracking\n",
			 ((rf->rf_supportability & HAL_RF_TX_PWR_TRACK) ?
			 ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "01. (( %s ))IQK\n",
			 ((rf->rf_supportability & HAL_RF_IQK) ? ("V") :
			 (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "02. (( %s ))LCK\n",
			 ((rf->rf_supportability & HAL_RF_LCK) ? ("V") :
			 (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DPK\n",
			 ((rf->rf_supportability & HAL_RF_DPK) ? ("V") :
			 (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "04. (( %s ))HAL_RF_TXGAPK\n",
			 ((rf->rf_supportability & HAL_RF_TXGAPK) ? ("V") :
			 (".")));
	} else {
		if (dm_value[1] == 1) /* enable */
			rf->rf_supportability |= BIT(dm_value[0]);
		else if (dm_value[1] == 2) /* disable */
			rf->rf_supportability &= ~(BIT(dm_value[0]));
		else
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "[Warning!!!]  1:enable,  2:disable\n");
	}
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "\nCurr-RF_supportability =  0x%x\n\n", rf->rf_supportability);

	*_used = used;
	*_out_len = out_len;
}

void halrf_cmn_info_init(void *dm_void, enum halrf_cmninfo_init cmn_info,
			 u32 value)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	switch (cmn_info) {
	case HALRF_CMNINFO_EEPROM_THERMAL_VALUE:
		rf->eeprom_thermal = (u8)value;
		break;
	case HALRF_CMNINFO_PWT_TYPE:
		rf->pwt_type = (u8)value;
		break;
	default:
		break;
	}
}

void halrf_cmn_info_hook(void *dm_void, enum halrf_cmninfo_hook cmn_info,
			 void *value)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	switch (cmn_info) {
	case HALRF_CMNINFO_CON_TX:
		rf->is_con_tx = (boolean *)value;
		break;
	case HALRF_CMNINFO_SINGLE_TONE:
		rf->is_single_tone = (boolean *)value;
		break;
	case HALRF_CMNINFO_CARRIER_SUPPRESSION:
		rf->is_carrier_suppresion = (boolean *)value;
		break;
	case HALRF_CMNINFO_MP_RATE_INDEX:
		rf->mp_rate_index = (u8 *)value;
		break;
	default:
		/*do nothing*/
		break;
	}
}

void halrf_cmn_info_set(void *dm_void, u32 cmn_info, u64 value)
{
	/* This init variable may be changed in run time. */
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	switch (cmn_info) {
	case HALRF_CMNINFO_ABILITY:
		rf->rf_supportability = (u32)value;
		break;

	case HALRF_CMNINFO_DPK_EN:
		rf->dpk_en = (u8)value;
		break;
	case HALRF_CMNINFO_RFK_FORBIDDEN:
		dm->IQK_info.rfk_forbidden = (boolean)value;
		break;
	case HALRF_CMNINFO_RATE_INDEX:
		rf->p_rate_index = (u32)value;
		break;
	default:
		/* do nothing */
		break;
	}
}

u64 halrf_cmn_info_get(void *dm_void, u32 cmn_info)
{
	/* This init variable may be changed in run time. */
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;
	u64 return_value = 0;

	switch (cmn_info) {
	case HALRF_CMNINFO_ABILITY:
		return_value = (u32)rf->rf_supportability;
		break;
	case HALRF_CMNINFO_RFK_FORBIDDEN:
		return_value = dm->IQK_info.rfk_forbidden;
		break;
	default:
		/* do nothing */
		break;
	}

	return return_value;
}

void halrf_supportability_init_mp(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	switch (dm->support_ic_type) {
	case ODM_RTL8814B:
		break;

	default:
		rf->rf_supportability =
			HAL_RF_TX_PWR_TRACK |
			HAL_RF_IQK |
			HAL_RF_LCK |
			/*@HAL_RF_DPK |*/
			/*@HAL_RF_TXGAPK |*/
			0;
		break;
	}

	RF_DBG(dm, DBG_RF_INIT,
	       "IC = ((0x%x)), RF_Supportability Init MP = ((0x%x))\n",
	       dm->support_ic_type, rf->rf_supportability);
}

void halrf_supportability_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	switch (dm->support_ic_type) {
	case ODM_RTL8814B:
		break;

	default:
		rf->rf_supportability =
			HAL_RF_TX_PWR_TRACK |
			HAL_RF_IQK |
			HAL_RF_LCK |
			/*@HAL_RF_DPK |*/
			0;
		break;
	}

	RF_DBG(dm, DBG_RF_INIT,
	       "IC = ((0x%x)), RF_Supportability Init = ((0x%x))\n",
	       dm->support_ic_type, rf->rf_supportability);
}

void halrf_watchdog(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	phydm_rf_watchdog(dm);
}


void halrf_dack_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	u64 start_time;

	start_time = odm_get_current_time(dm);

	switch (dm->support_ic_type) {
	default:
		break;
	}
	rf->dpk_progressing_time = odm_get_progressing_time(dm, start_time);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK progressing_time = %lld ms\n",
	       rf->dpk_progressing_time);
}

void halrf_iqk_trigger(void *dm_void, boolean is_recovery)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_iqk_info *iqk_info = &dm->IQK_info;
	struct _hal_rf_ *rf = &dm->rf_table;
	u64 start_time;


	if (dm->mp_mode &&
	    rf->is_con_tx &&
	    rf->is_single_tone &&
	    rf->is_carrier_suppresion)
		if (*dm->mp_mode &&
		    ((*rf->is_con_tx ||
		     *rf->is_single_tone ||
		     *rf->is_carrier_suppresion)))
			return;

	if (!(rf->rf_supportability & HAL_RF_IQK))
		return;

#if DISABLE_BB_RF
	return;
#endif

	if (iqk_info->rfk_forbidden)
		return;

	if (!dm->rf_calibrate_info.is_iqk_in_progress) {
		odm_acquire_spin_lock(dm, RT_IQK_SPINLOCK);
		dm->rf_calibrate_info.is_iqk_in_progress = true;
		odm_release_spin_lock(dm, RT_IQK_SPINLOCK);
		start_time = odm_get_current_time(dm);
		switch (dm->support_ic_type) {
		case ODM_RTL8192E:
			phy_iq_calibrate_8192e(dm, is_recovery);
			break;

		default:
			break;
		}
		dm->rf_calibrate_info.iqk_progressing_time =
				odm_get_progressing_time(dm, start_time);
		RF_DBG(dm, DBG_RF_IQK, "[IQK]IQK progressing_time = %lld ms\n",
		       dm->rf_calibrate_info.iqk_progressing_time);

		odm_acquire_spin_lock(dm, RT_IQK_SPINLOCK);
		dm->rf_calibrate_info.is_iqk_in_progress = false;
		odm_release_spin_lock(dm, RT_IQK_SPINLOCK);
	} else {
		RF_DBG(dm, DBG_RF_IQK,
		       "== Return the IQK CMD, because RFKs in Progress ==\n");
	}
}

void halrf_lck_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_iqk_info *iqk_info = &dm->IQK_info;
	struct _hal_rf_ *rf = &dm->rf_table;
	u64 start_time;


	if (dm->mp_mode &&
	    rf->is_con_tx &&
	    rf->is_single_tone &&
	    rf->is_carrier_suppresion)
		if (*dm->mp_mode &&
		    ((*rf->is_con_tx ||
		     *rf->is_single_tone ||
		     *rf->is_carrier_suppresion)))
			return;

	if (!(rf->rf_supportability & HAL_RF_LCK))
		return;

#if DISABLE_BB_RF
	return;
#endif
	if (iqk_info->rfk_forbidden)
		return;
	while (*dm->is_scan_in_process) {
		RF_DBG(dm, DBG_RF_IQK, "[LCK]scan is in process, bypass LCK\n");
		return;
	}

	if (!dm->rf_calibrate_info.is_lck_in_progress) {
		odm_acquire_spin_lock(dm, RT_IQK_SPINLOCK);
		dm->rf_calibrate_info.is_lck_in_progress = true;
		odm_release_spin_lock(dm, RT_IQK_SPINLOCK);
		start_time = odm_get_current_time(dm);
		switch (dm->support_ic_type) {
		case ODM_RTL8192E:
			phy_lc_calibrate_8192e(dm);
			break;
		default:
			break;
		}
		dm->rf_calibrate_info.lck_progressing_time =
				odm_get_progressing_time(dm, start_time);
		RF_DBG(dm, DBG_RF_IQK, "[IQK]LCK progressing_time = %lld ms\n",
		       dm->rf_calibrate_info.lck_progressing_time);
		odm_acquire_spin_lock(dm, RT_IQK_SPINLOCK);
		dm->rf_calibrate_info.is_lck_in_progress = false;
		odm_release_spin_lock(dm, RT_IQK_SPINLOCK);
	} else {
		RF_DBG(dm, DBG_RF_IQK,
		       "= Return the LCK CMD, because RFK is in Progress =\n");
	}
}

void halrf_aac_check(struct dm_struct *dm)
{
	switch (dm->support_ic_type) {
	default:
		break;
	}
}

void halrf_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	RF_DBG(dm, DBG_RF_INIT, "HALRF_Init\n");

	halrf_init_debug_setting(dm);

	if (*dm->mp_mode)
		halrf_supportability_init_mp(dm);
	else
		halrf_supportability_init(dm);
	/*Init all RF funciton*/
	halrf_aac_check(dm);
	halrf_dack_trigger(dm);
	halrf_tssi_init(dm);
}

void halrf_dpk_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	u64 start_time;

	start_time = odm_get_current_time(dm);

	switch (dm->support_ic_type) {


	default:
		break;
	}
	rf->dpk_progressing_time = odm_get_progressing_time(dm, start_time);
	RF_DBG(dm, DBG_RF_DPK, "[DPK]DPK progressing_time = %lld ms\n",
	       rf->dpk_progressing_time);
}

u8 halrf_dpk_result_check(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	u8 result = 0;

	switch (dm->support_ic_type) {


	default:
		break;
	}
	return result;
}

void halrf_dpk_sram_read(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;


	switch (dm->support_ic_type) {


	default:
		break;
	}
}

void halrf_dpk_enable_disable(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	switch (dm->support_ic_type) {

	default:
		break;
	}
}

void halrf_dpk_track(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	switch (dm->support_ic_type) {


	default:
		break;
	}
}

void halrf_dpk_reload(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	switch (dm->support_ic_type) {

	default:
		break;
	}
}

enum hal_status
halrf_config_rfk_with_header_file(void *dm_void, u32 config_type)
{
	enum hal_status result = HAL_STATUS_SUCCESS;
	return result;
}

void halrf_txgapk_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	u64 start_time;

	start_time = odm_get_current_time(dm);

	switch (dm->support_ic_type) {

	default:
		break;
	}
	rf->dpk_progressing_time =
		odm_get_progressing_time(dm_void, start_time);
	RF_DBG(dm, DBG_RF_TXGAPK, "[TGGC]TXGAPK progressing_time = %lld ms\n",
	       rf->dpk_progressing_time);
}

void halrf_tssi_init(void *dm_void)
{

}

void halrf_do_tssi(void *dm_void)
{

}

void halrf_set_tssi_value(void *dm_void, u32 tssi_value)
{

}

u32 halrf_query_tssi_value(void *dm_void)
{

	return 0;
}

/*Golbal function*/
void halrf_reload_bp(void *dm_void, u32 *bp_reg, u32 *bp, u32 num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i;

	for (i = 0; i < num; i++)
		odm_write_4byte(dm, bp_reg[i], bp[i]);
}

void halrf_reload_bprf(void *dm_void, u32 *bp_reg, u32 bp[][4], u32 num,
		       u8 ss)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i, path;

	for (i = 0; i < num; i++) {
		for (path = 0; path < ss; path++)
			odm_set_rf_reg(dm, (enum rf_path)path, bp_reg[i],
				       MASK20BITS, bp[i][path]);
	}
}

void halrf_bp(void *dm_void, u32 *bp_reg, u32 *bp, u32 num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i;

	for (i = 0; i < num; i++)
		bp[i] = odm_read_4byte(dm, bp_reg[i]);
}

void halrf_bprf(void *dm_void, u32 *bp_reg, u32 bp[][4], u32 num, u8 ss)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i, path;

	for (i = 0; i < num; i++) {
		for (path = 0; path < ss; path++) {
			bp[i][path] =
				odm_get_rf_reg(dm, (enum rf_path)path,
					       bp_reg[i], MASK20BITS);
		}
	}
}

void halrf_swap(void *dm_void, u32 *v1, u32 *v2)
{
	u32 temp;

	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void halrf_bubble(void *dm_void, u32 *v1, u32 *v2)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (*v1 >= 0x200 && *v2 >= 0x200) {
		if (*v1 > *v2)
			halrf_swap(dm, v1, v2);
	} else if (*v1 < 0x200 && *v2 < 0x200) {
		if (*v1 > *v2)
			halrf_swap(dm, v1, v2);
	} else if (*v1 < 0x200 && *v2 >= 0x200) {
		halrf_swap(dm, v1, v2);
	}
}

void halrf_b_sort(void *dm_void, u32 *iv, u32 *qv)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i, j;

	RF_DBG(dm, DBG_RF_DACK, "[DACK]bubble!!!!!!!!!!!!");
	for (i = 0; i < SN - 1; i++) {
		for (j = 0; j < (SN - 1 - i) ; j++) {
			halrf_bubble(dm, &iv[j], &iv[j + 1]);
			halrf_bubble(dm, &qv[j], &qv[j + 1]);
		}
	}
}

void halrf_minmax_compare(void *dm_void, u32 value, u32 *min,
			  u32 *max)
{

	if (value >= 0x200) {
		if (*min >= 0x200) {
			if (*min > value)
				*min = value;
		} else {
			*min = value;
		}
		if (*max >= 0x200) {
			if (*max < value)
				*max = value;
		}
	} else {
		if (*min < 0x200) {
			if (*min > value)
				*min = value;
		}

		if (*max  >= 0x200) {
			*max = value;
		} else {
			if (*max < value)
				*max = value;
		}
	}
}

u32 halrf_delta(void *dm_void, u32 v1, u32 v2)
{

	if (v1 >= 0x200 && v2 >= 0x200) {
		if (v1 > v2)
			return v1 - v2;
		else
			return v2 - v1;
	} else if (v1 >= 0x200 && v2 < 0x200) {
		return v2 + (0x400 - v1);
	} else if (v1 < 0x200 && v2 >= 0x200) {
		return v1 + (0x400 - v2);
	}

	if (v1 > v2)
		return v1 - v2;
	else
		return v2 - v1;
}

boolean halrf_compare(void *dm_void, u32 value)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	boolean fail = false;

	if (value >= 0x200 && (0x400 - value) > 0x64)
		fail = true;
	else if (value < 0x200 && value > 0x64)
		fail = true;

	if (fail)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]overflow!!!!!!!!!!!!!!!");
	return fail;
}

void halrf_mode(void *dm_void, u32 *i_value, u32 *q_value)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 iv[SN], qv[SN], im[SN], qm[SN], temp;
	u32 p, m, t;
	u32 i_max = 0, q_max = 0, i_min = 0x0, q_min = 0x0, c = 0x0;
	u32 i_delta, q_delta;
	u8 i;
	boolean fail = false;

	ODM_delay_ms(10);
	for (i = 0; i < SN; i++) {
		im[i] = 0;
		qm[i] = 0;
	}
	i = 0;
	c = 0;
	while (i < SN && c < 1000) {
		c++;
		temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
		iv[i] = (temp & 0x3ff000) >> 12;
		qv[i] = temp & 0x3ff;

		fail = false;
		if (halrf_compare(dm, iv[i]))
			fail = true;
		if (halrf_compare(dm, qv[i]))
			fail = true;
		if (!fail)
			i++;
	}
	c = 0;
	do {
		c++;
		i_min = iv[0];
		i_max = iv[0];
		q_min = qv[0];
		q_max = qv[0];
		for (i = 0; i < SN; i++) {
			halrf_minmax_compare(dm, iv[i], &i_min, &i_max);
			halrf_minmax_compare(dm, qv[i], &q_min, &q_max);
		}
		RF_DBG(dm, DBG_RF_DACK, "[DACK]i_min=0x%x, i_max=0x%x",
		       i_min, i_max);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]q_min=0x%x, q_max=0x%x",
		       q_min, q_max);
		if (i_max < 0x200 && i_min < 0x200)
			i_delta = i_max - i_min;
		else if (i_max >= 0x200 && i_min >= 0x200)
			i_delta = i_max - i_min;
		else
			i_delta = i_max + (0x400 - i_min);

		if (q_max < 0x200 && q_min < 0x200)
			q_delta = q_max - q_min;
		else if (q_max >= 0x200 && q_min >= 0x200)
			q_delta = q_max - q_min;
		else
			q_delta = q_max + (0x400 - q_min);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]i_delta=0x%x, q_delta=0x%x",
		       i_delta, q_delta);
		halrf_b_sort(dm, iv, qv);
		if (i_delta > 5 || q_delta > 5) {
			temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
			iv[0] = (temp & 0x3ff000) >> 12;
			qv[0] = temp & 0x3ff;
			temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
			iv[SN - 1] = (temp & 0x3ff000) >> 12;
			qv[SN - 1] = temp & 0x3ff;
		} else {
			break;
		}
	} while (c < 100);
	/*i*/
	m = 0;
	p = 0;
	for (i = 10; i < SN - 10; i++) {
		if (iv[i] > 0x200)
			m = (0x400 - iv[i]) + m;
		else
			p = iv[i] + p;
	}

	if (p > m) {
		t = p - m;
		t = t / (SN - 20);
	} else {
		t = m - p;
		t = t / (SN - 20);
		if (t != 0x0)
			t = 0x400 - t;
	}
	*i_value = t;
	/*q*/
	m = 0;
	p = 0;
	for (i = 10; i < SN - 10; i++) {
		if (qv[i] > 0x200)
			m = (0x400 - qv[i]) + m;
		else
			p = qv[i] + p;
	}
	if (p > m) {
		t = p - m;
		t = t / (SN - 20);
	} else {
		t = m - p;
		t = t / (SN - 20);
		if (t != 0x0)
			t = 0x400 - t;
	}
	*q_value = t;
}

