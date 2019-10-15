#ifndef __COMMOM_H__
#define __COMMOM_H__

#include <stdint.h>
#include <string.h>
#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"

performance_t perf;

#define l2malloc malloc
#define l2free   free

#define l1malloc L1_Malloc
#define l1free   L1_Free

#define ANSI_COLOR_RED     "\x1b[1m\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[1m\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SHARED_ICACHE    (0x0U)
#define PRIVATE_ICACHE   (0x1U)
#define MULTIPORT_ICACHE (0x2U)
#define L2_MEM_BASE_ADDR (0x1C000000)

volatile int PowerBenchActive __attribute__( ( aligned ( 8 ) ) );

int cycles[3] = {0,0,0};
int reg[3][20];

static inline uint32_t checksum_vector(void *v, uint32_t nbElements, uint32_t elementSize)
{
    uint32_t cnt = 0ULL;
    uint32_t i;
    for(i=0; i<nbElements; i++)
        cnt += ((uint8_t *) v) [i*elementSize];

    return cnt;
}

static inline void memcpy_async(void *dst, void *src, uint32_t size, dma_req_t *req)
{
    if (L2_MEM_BASE_ADDR <= (uint32_t) dst) {
        // Copy from shared L1 to L2
        DMAMCHAN_Memcpy_1D((uint32_t)dst, (uint32_t)src, size, GAP_DMA_TCDM2L2, req);
    } else {
        // copy from L2 to shared L1
        DMAMCHAN_Memcpy_1D((uint32_t)src, (uint32_t)dst, size, GAP_DMA_L22TCDM, req);
    }
}

static inline void memcpy_async2D(void *dst, void *src, uint32_t size, uint32_t stride, uint32_t count, dma_req_t *req)
{
    if (L2_MEM_BASE_ADDR <= (uint32_t) dst) {
        // Copy from shared L1 to L2
        DMAMCHAN_Memcpy_2D((uint32_t) dst, (uint32_t)src, size, stride, count, GAP_DMA_TCDM2L2, req);
    }
    else {
        // copy from L2 to shared L1
        DMAMCHAN_Memcpy_2D((uint32_t) src, (uint32_t) dst, size, stride, count, GAP_DMA_L22TCDM, req);
    }
}

static inline void memcpy_wait(dma_req_t *req)
{
    DMAMCHAN_WaitRequestEnd(req);
}

static inline void perf_save_reg(int iter) {
    for (int i = 4; i < 20; i++)
        reg[iter][i] = *(volatile int*) (CLUSTER_SCBC_BASE + (i) * 4);
}

static inline void perf_reg_print(int iter) {
    printf("Time: %d\n", cycles[iter]);

    for(int i = 4; i < 8; ++i)
        printf("%d,", (int)reg[iter][i]);
    printf("  PRI\n");

    for(int i = 8; i < 12; ++i)
        printf("%d,", (int)reg[iter][i]);
    printf("  SP\n");

    for(int i = 12; i < 16; ++i)
        printf("%d,", (int)reg[iter][i]);
    printf("  HIER1\n");

    for(int i = 16; i < 20; ++i)
        printf("%d,", (int)reg[iter][i]);
    printf("  MP-HIER2\n");
}

static inline void
profile_start(int iter)
{
    // 0x10201410, Clear counter
    CLUSTER_SCBC->ICACHE_CNTS_CLEAR  = 0xC1A0FFFFU;
    // 0x10201414, Enable counter Start
    CLUSTER_SCBC->ICACHE_CNTS_ENABLE = 0xCA550FFFU;

    #pragma omp master
    {
        CLUSTER_TIMERL->COMPARE = 0xFFFFFFFF;
        /* Reset Enable Counter */
        CLUSTER_TIMERL->CTRL =
            (1 << TIMERL_CFG_REG_LOW_ENABLE_Pos)
            | (1 << TIMERL_CFG_REG_LOW_RESET_Pos)
            | (1 << TIMERL_CFG_REG_LOW_IRQE_Pos)
            | (0 << TIMERL_CFG_REG_LOW_IEM_Pos)
            | (1 << TIMERL_CFG_REG_LOW_CMP_CLR_Pos)
            | (0 << TIMERL_CFG_REG_LOW_ONE_SHOT_Pos)
            | (0 << TIMERL_CFG_REG_LOW_PRESCALERE_Pos)
            | (0 << TIMERL_CFG_REG_LOW_PRESCALER_Pos)
            | (0 << TIMERL_CFG_REG_LOW_CLKS_Pos)
            | (0 << TIMERL_CFG_REG_LOW_64BIT_Pos);
        /* Reset value */
        CLUSTER_TIMERL->VALUE = 0;

        if(iter == 2) {
            PowerBenchActive  = 0XABBAABBA;
        }
    }
}

static inline void
profile_stop(int iter)
{
    #pragma omp master
    {
        if(iter == 2) {
            PowerBenchActive = 0xABBACACA;
        }
        cycles[iter] = CLUSTER_TIMERL->VALUE;

        CLUSTER_TIMERL->CTRL =  0;

        perf_save_reg(iter);
    }
}

static inline void
profile_show()
{
    perf_reg_print(0);
    perf_reg_print(1);
    perf_reg_print(2);
}
#endif
