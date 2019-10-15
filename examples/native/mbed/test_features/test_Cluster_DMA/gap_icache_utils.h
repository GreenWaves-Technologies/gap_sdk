#ifndef __GAP_ICACHE_UTILS_H__
#define __GAP_ICACHE_UTILS_H__

#include <stdlib.h>
#include "gap_common.h"

volatile int PowerBenchActive;

static inline int read_hit_icache_reg_i(int i)
{
    volatile int reg_status;
    reg_status = *(volatile int*) (CLUSTER_SCBC_BASE + (12 + i*4) * 4);
    return reg_status;
}

static inline int read_trans_icache_reg_i(int i)
{
    volatile int reg_status;
    reg_status = *(volatile int*) (CLUSTER_SCBC_BASE + (13 + i*4) * 4);
    return reg_status;
}

static inline int read_miss_icache_reg_i(int i)
{
    volatile int reg_status;
    reg_status = *(volatile int*) (CLUSTER_SCBC_BASE + (14 + i*4) * 4);
    return reg_status;
}

static inline int read_cong_icache_reg_i(int i)
{
    volatile int reg_status;
    reg_status = *(volatile int*) (CLUSTER_SCBC_BASE + (15 + i*4) * 4);
    return reg_status;
}

void ICACHE_CNTS_Printf() {
    int core_id = 0;

    printf("\nICacheHit");
    for(core_id = 0; core_id < 8; ++core_id)
        printf(",%d", (int)read_hit_icache_reg_i(core_id));

    printf("\nICacheTrans");
    for(core_id = 0; core_id < 8; ++core_id)
        printf(",%d", (int)read_trans_icache_reg_i(core_id));

    printf("\nICacheMiss");
    for(core_id = 0; core_id < 8; ++core_id)
        printf(",%d", (int)read_miss_icache_reg_i(core_id));

    printf("\nICacheCong");
    for(core_id = 0; core_id < 8; ++core_id)
        printf(",%d", (int)read_cong_icache_reg_i(core_id));
    printf("\n");
}

void ICACHE_Start(int iter) {
    // 0x10201404, Icache Flush
//    SCBC->ICACHE_FLUSH       = 0xC1A0FFFFU;
    // 0x10201410, Clear counter
    CLUSTER_SCBC->ICACHE_CNTS_CLEAR  = 0xC1A0FFFFU;
    // 0x10201414, Enable counter Start
    CLUSTER_SCBC->ICACHE_CNTS_ENABLE = 0xCA550FFFU;

    if(iter == 1) {
        PowerBenchActive  = 0XABBAABBA;
    }
}

void ICACHE_Stop(int iter) {
    // 0x10201414, Enable counter Stop
    CLUSTER_SCBC->ICACHE_CNTS_ENABLE = 0xF16A0000U;
    if(iter == 1) {
        PowerBenchActive = 0xABBACACA;
    }

    /* ICACHE printf counters */
    ICACHE_CNTS_Printf();
}

#endif

