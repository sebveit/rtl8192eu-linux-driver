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

#ifdef CONFIG_RTW_MESH /* for now, only promised for kernel versions we support mesh */

#include <drv_types.h>

int rtw_rhashtable_walk_enter(rtw_rhashtable *ht, rtw_rhashtable_iter *iter)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
        return rhashtable_walk_init(ht, iter, GFP_ATOMIC);
#else /* 4.4 <= kernel < 4.7 */
        /* Older API lacks GFP parameter and requires manual setup */
        iter->ht = ht;
        iter->p = NULL;
        iter->slot = 0;
        iter->skip = 0;

        iter->walker = kmalloc(sizeof(*iter->walker), GFP_ATOMIC);
        if (!iter->walker)
                return -ENOMEM;

        spin_lock(&ht->lock);
        iter->walker->tbl = rcu_dereference_protected(ht->tbl,
                                                     lockdep_is_held(&ht->lock));
        list_add(&iter->walker->list, &iter->walker->tbl->walkers);
        spin_unlock(&ht->lock);

        return 0;
#endif
}


#endif /* CONFIG_RTW_MESH */

