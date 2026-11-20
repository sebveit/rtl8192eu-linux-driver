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

/*************************************************************
 * include files
 ************************************************************/
#include "mp_precomp.h"
#include "phydm_precomp.h"

#ifdef CONFIG_PATH_DIVERSITY


#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
void phydm_tx_path_diversity_init_jgr3(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u32 i;

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		p_div->path_sel[i] = BB_PATH_A; /* TxInfo default at path-A */
}

void phydm_tx_by_mac_or_reg_jgr3(void *dm_void, enum phydm_path_ctrl ctrl)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (ctrl == TX_PATH_BY_REG) {
		odm_set_bb_reg(dm, R_0x1e24, BIT(16), 0);
	} else {
		odm_set_bb_reg(dm, R_0x1e24, BIT(16), 1);
	}
}

void phydm_set_resp_tx_path_by_fw(void *dm_void, u8 macid, enum bb_path path,
				  boolean enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 h2c_para[6] = {0};
	u8 path_map[4];
	u8 num_enable_path = 0;
	u8 n_tx_path_ctrl_map = 0;
	u8 i = 0, n_sts = 0;

	/*Response TX is controlled in FW ctrl info*/

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] =====>\n", __func__);

	if (enable) {
		n_tx_path_ctrl_map = path;

		for (i = 0; i < 4; i++) {
			path_map[i] = 0;
			if (path & BIT(i))
				num_enable_path++;
		}

		for (i = 0; i < 4; i++) {
			if (path & BIT(i)) {
				path_map[i] = n_sts;
				n_sts++;

				if (n_sts == num_enable_path)
					break;
			}
		}
	}

	PHYDM_DBG(dm, DBG_PATH_DIV, "ctrl_map=0x%x Map[D:A]={%d, %d, %d, %d}\n",
		  n_tx_path_ctrl_map,
		  path_map[3], path_map[2], path_map[1], path_map[0]);

	h2c_para[0] = macid;
	h2c_para[1] = n_tx_path_ctrl_map;
	h2c_para[2] = (path_map[3] << 6) | (path_map[2] << 4) |
		      (path_map[1] << 2) | path_map[0];

	odm_fill_h2c_cmd(dm, PHYDM_H2C_DYNAMIC_TX_PATH, 6, h2c_para);
}
#endif

void phydm_set_tx_path_by_bb_reg(void *dm_void, enum bb_path path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] path=%s\n",
		  __func__, (path == BB_PATH_A) ? "A" : "B");

	switch (dm->support_ic_type) {

	default:
		break;
	}
}

u8 phydm_get_tx_path_txdesc(void *dm_void, u8 macid)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 rpt_val = 0;

	if (p_div->path_sel[macid] == BB_PATH_A)
		rpt_val = 1;
	else if (p_div->path_sel[macid] == BB_PATH_B)
		rpt_val = 2;

	return rpt_val;
}

void phydm_tx_path_diversity_2ss(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	struct cmn_sta_info *sta;
	enum bb_path default_path = BB_PATH_A, path = BB_PATH_A;
	u32 rssi_a = 0, rssi_b = 0;
	u32 local_max_rssi, min_rssi = 0xff;
	u8 i = 0;

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] =======>\n", __func__);

	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	if (dm->support_ic_type & ODM_IC_JGR3_SERIES) {
		if (dm->is_one_entry_only)
			phydm_tx_by_mac_or_reg_jgr3(dm, TX_PATH_BY_REG);
		else
			phydm_tx_by_mac_or_reg_jgr3(dm, TX_PATH_BY_DESC);
	}
	#endif

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		sta = dm->phydm_sta_info[i];
		if (is_sta_active(sta))
			continue;

		/* 2 Caculate RSSI per path */
		rssi_a = (p_div->path_a_cnt[i]) ?
			 (p_div->path_a_sum[i] / p_div->path_a_cnt[i]) : 0;
		rssi_b = (p_div->path_b_cnt[i]) ?
			 (p_div->path_b_sum[i] / p_div->path_b_cnt[i]) : 0;

		if (rssi_a == rssi_b)
			path =  p_div->default_tx_path;
		else
			path = (rssi_a > rssi_b) ? BB_PATH_A : BB_PATH_B;

		local_max_rssi = (rssi_a > rssi_b) ? rssi_a : rssi_b;

		PHYDM_DBG(dm, DBG_PATH_DIV,
			  "[%d]PathA sum=%d, cnt=%d, avg_rssi=%d\n",
			  i, p_div->path_a_sum[i],
			  p_div->path_a_cnt[i], rssi_a);
		PHYDM_DBG(dm, DBG_PATH_DIV,
			  "[%d]PathB sum=%d, cnt=%d, avg_rssi=%d\n",
			  i, p_div->path_b_sum[i],
			  p_div->path_b_cnt[i], rssi_b);

		/*Select default Tx path */
		if (local_max_rssi < min_rssi) {
			min_rssi = local_max_rssi;
			default_path = path;
		}

		if (p_div->path_sel[i] != path) {
			p_div->path_sel[i] = path;
			#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
			if (dm->support_ic_type & ODM_IC_JGR3_SERIES)
				phydm_set_resp_tx_path_by_fw(dm, i, path, true);
			#endif
		}

		p_div->path_a_cnt[i] = 0;
		p_div->path_a_sum[i] = 0;
		p_div->path_b_cnt[i] = 0;
		p_div->path_b_sum[i] = 0;
	}

	/* 2 Update default Tx path */
	if (default_path != p_div->default_tx_path)
		phydm_set_tx_path_by_bb_reg(dm, default_path);
}

void phydm_tx_path_diversity(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (!(dm->support_ability & ODM_BB_PATH_DIV))
		return;

	switch (dm->support_ic_type) {

	}
}

void phydm_tx_path_diversity_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (!(dm->support_ability & ODM_BB_PATH_DIV))
		return;

	switch (dm->support_ic_type) {


	}
}

void phydm_process_rssi_for_path_div(void *dm_void, void *phy_info_void,
				     void *pkt_info_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 id = 0;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;

	if (!(pktinfo->is_packet_to_self || pktinfo->is_packet_match_bssid))
		return;

	if (pktinfo->data_rate <= ODM_RATE11M)
		return;

	id = pktinfo->station_id;
	p_div->path_a_sum[id] += phy_info->rx_mimo_signal_strength[0];
	p_div->path_a_cnt[id]++;

	p_div->path_b_sum[id] += phy_info->rx_mimo_signal_strength[1];
	p_div->path_b_cnt[id]++;
}

void phydm_pathdiv_debug(void *dm_void, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val[10] = {0};
	u8 i, input_idx = 0;

	if (input_idx == 0)
		return;

	PHYDM_SSCANF(input[1], DCMD_HEX, &val[0]);
	PHYDM_SSCANF(input[2], DCMD_HEX, &val[1]);

	if ((strcmp(input[1], "-h") == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{1:TX Ctrl Sig} {0:BB, 1:MAC}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{2:BB Default TX REG} {path}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{3:MAC DESC TX} {path} {macid}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{4:MAC Resp TX} {path} {macid}\n");
	} else if (val[0] == 1) {
		phydm_tx_by_mac_or_reg_jgr3(dm, (enum phydm_path_ctrl)val[1]);
	} else if (val[0] == 2) {
		phydm_set_tx_path_by_bb_reg(dm, (enum bb_path)val[1]);
	} else if (val[0] == 3) {
		p_div->path_sel[val[2]] = (enum bb_path)val[1];
	} else if (val[0] == 4) {
		phydm_set_resp_tx_path_by_fw(dm, (u8)val[2],
					     (enum bb_path)val[1], true);
	}

	*_used = used;
	*_out_len = out_len;
#endif
}

void phydm_c2h_dtp_handler(void *dm_void, u8 *cmd_buf, u8 cmd_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;

	u8 macid = cmd_buf[0];
	u8 target = cmd_buf[1];
	u8 nsc_1 = cmd_buf[2];
	u8 nsc_2 = cmd_buf[3];
	u8 nsc_3 = cmd_buf[4];

	PHYDM_DBG(dm, DBG_PATH_DIV, "Target_candidate = (( %d ))\n", target);
/*@
	if( (nsc_1 >= nsc_2) &&  (nsc_1 >= nsc_3))
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_1);
	}
	else	if( nsc_2 >= nsc_3)
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_2);
	}
	else
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_3);
	}
	*/
}

#endif /*  @#ifdef CONFIG_PATH_DIVERSITY */
