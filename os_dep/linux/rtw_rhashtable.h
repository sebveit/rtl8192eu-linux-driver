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
#ifndef __RTW_RHASHTABLE_H__
#define __RTW_RHASHTABLE_H__

#ifdef CONFIG_RTW_MESH /* for now, only promised for kernel versions we support mesh */

/* kernel >= 4.4 always provides rhashtable */
#include <linux/rhashtable.h>

typedef struct rhashtable rtw_rhashtable;
typedef struct rhash_head rtw_rhash_head;
typedef struct rhashtable_params rtw_rhashtable_params;

#define rtw_rhashtable_init(ht, params) rhashtable_init(ht, params)

typedef struct rhashtable_iter rtw_rhashtable_iter;

int rtw_rhashtable_walk_enter(rtw_rhashtable *ht, rtw_rhashtable_iter *iter);
#define rtw_rhashtable_walk_exit(iter) rhashtable_walk_exit(iter)
#define rtw_rhashtable_walk_start(iter) rhashtable_walk_start(iter)
#define rtw_rhashtable_walk_next(iter) rhashtable_walk_next(iter)
#define rtw_rhashtable_walk_stop(iter) rhashtable_walk_stop(iter)

#define rtw_rhashtable_free_and_destroy(ht, free_fn, arg) rhashtable_free_and_destroy((ht), (free_fn), (arg))
#define rtw_rhashtable_lookup_fast(ht, key, params) rhashtable_lookup_fast((ht), (key), (params))
#define rtw_rhashtable_lookup_insert_fast(ht, obj, params) rhashtable_lookup_insert_fast((ht), (obj), (params))
#define rtw_rhashtable_remove_fast(ht, obj, params) rhashtable_remove_fast((ht), (obj), (params))

#endif /* CONFIG_RTW_MESH */

#endif /* __RTW_RHASHTABLE_H__ */

