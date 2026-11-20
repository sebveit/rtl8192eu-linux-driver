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

#ifndef __PHYDM_MATH_LIB_H__
#define __PHYDM_MATH_LIB_H__

#define AUTO_MATH_LIB_VERSION "1.0" /* @2017.06.06*/

/*@
 * 1 ============================================================
 * 1  Definition
 * 1 ============================================================
 */

#define DIVIDED_2(X) ((X) >> 1)
/*@1/3 ~ 11/32*/
#define DIVIDED_3(X) (((X) + ((X) << 1) + ((X) << 3)) >> 5)
#define DIVIDED_4(X) ((X) >> 2)

/*Store Ori Value*/
#define WEIGHTING_AVG(v1, w1, v2, w2) \
	(((v1) * (w1) + (v2) * (w2)) / ((w2) + (w1)))

/*Store 2^ma x Value*/
#define MA_ACC(old, new_val, ma) ((old) - ((old) >> (ma)) + (new_val))
#define GET_MA_VAL(val, ma) (((val) + (1 << ((ma) - 1))) >> (ma))
#define FRAC_BITS 3
/*@
 * 1 ============================================================
 * 1  enumeration
 * 1 ============================================================
 */

/*@
 * 1 ============================================================
 * 1  structure
 * 1 ============================================================
 */

/*@
 * 1 ============================================================
 * 1  function prototype
 * 1 ============================================================
 */

s32 odm_pwdb_conversion(s32 X, u32 total_bit, u32 decimal_bit);

s32 odm_sign_conversion(s32 value, u32 total_bit);

u16 phydm_find_intrvl(void *dm_void, u16 val, u16 *threshold, u16 th_len);

void phydm_seq_sorting(void *dm_void, u32 *value, u32 *rank_idx, u32 *idx_out,
		       u8 seq_length);

u32 odm_convert_to_db(u64 value);

u64 phydm_db_2_linear(u32 value);

u16 phydm_show_fraction_num(u32 frac_val, u8 bit_num);

u32 phydm_gen_bitmask(u8 mask_num);

s32 phydm_cnvrt_2_sign(u32 val, u8 bit_num);
#endif
