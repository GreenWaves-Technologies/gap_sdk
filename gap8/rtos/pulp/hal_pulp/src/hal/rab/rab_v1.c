/*
 * Copyright (C) 2017 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal/rab/rab_v1.h"
#include "archi-host/pgtable_hwdef.h"   // virt_pfn2addr()
#include <inttypes.h>   // printf() identifiers
#include "stdio.h"      // printf()

int print_rab_cfg_val(const rab_cfg_val_t* const cfg_val)
{
    printf("va_start: ");
    print_virt_addr(&(cfg_val->va_start));
    printf(", va_end: ");
    print_virt_addr(&(cfg_val->va_end));
    printf(", offset: ");
    print_phys_addr(&(cfg_val->offset));
    printf(", flags: 0x%" PRIx8, cfg_val->flags);
    return 0;
}

int print_rab_cfg(const rab_cfg_t* const begin, const rab_cfg_t* const end,
        const unsigned only_valid)
{
    unsigned i = 0;
    for (const rab_cfg_t* cfg_rptr = begin; cfg_rptr < end; ++cfg_rptr, ++i) {
        rab_cfg_val_t cfg_val;
        read_rab_cfg_val(&cfg_val, cfg_rptr);
        if (!only_valid || cfg_val.flags != 0) {
            printf("[%02u]: ", i);
            print_rab_cfg_val(&cfg_val);
            printf("\n");
        }
    }

    return 0;
}

int print_rab_cfg_l2_val(const rab_cfg_l2_varam_t* const varam_ptr,
        const rab_cfg_l2_param_t* const param_ptr, const rab_cfg_l2_val_t* const cfg_val,
        const unsigned char entry)
{
    const virt_addr_t virt_addr = virt_pfn2addr(cfg_val->virt_pfn);
    phys_addr_t phys_addr;
    phys_pfn2addr(&phys_addr, cfg_val->phys_pfn);

    printf("va: ");
    print_virt_addr(&virt_addr);
    printf(", set %d, entry %d", (int)cfg_val->set, (int)entry);
    printf(" @ varam: 0x%" PRIx32, (unsigned long)varam_ptr);
    printf(", pa: ");
    print_phys_addr(&phys_addr);
    printf(" @ param: 0x%" PRIx32, (unsigned long)param_ptr);
    printf(", flags: 0x%" PRIx8, cfg_val->flags);
    return 0;
}
