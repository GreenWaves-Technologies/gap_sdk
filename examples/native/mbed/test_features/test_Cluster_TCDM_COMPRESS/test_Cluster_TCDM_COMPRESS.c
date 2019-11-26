// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#include "compressed_data.h"

/* typedef struct { */
/*   __IOM  uint32_t TCDM_ADDR;         /\*!< Offset: 0x000 (R/W)  CLUSTER_COMPRESS_ENGINE tcdm address *\/ */
/*   __IOM  uint32_t L2_ADDR;           /\*!< Offset: 0x004 (R/W)  CLUSTER_COMPRESS_ENGINE l2 address *\/ */
/*   __IOM  uint32_t CONFIG;            /\*!< Offset: 0x008 (R/W)  CLUSTER_COMPRESS_ENGINE configuration *\/ */
/*   __IOM  uint32_t STATUS;            /\*!< Offset: 0x00C (R/W)  CLUSTER_COMPRESS_ENGINE status *\/ */
/*   __IOM  uint32_t LUT;               /\*!< Offset: 0x010 (R/W)  CLUSTER_COMPRESS_ENGINE look up table *\/ */
/*   __IOM  uint32_t SYMBOL;            /\*!< Offset: 0x014 (R/W)  CLUSTER_COMPRESS_ENGINE symbol *\/ */
/*   __IOM  uint32_t BIT_READ;          /\*!< Offset: 0x018 (R/W)  CLUSTER_COMPRESS_ENGINE bit read *\/ */
/*   __IOM  uint32_t MODE;              /\*!< Offset: 0x01C (R/W)  CLUSTER_COMPRESS_ENGINE mode *\/ */
/*   __IOM  uint32_t SW_RST;            /\*!< Offset: 0x020 (R/W)  CLUSTER_COMPRESS_ENGINE software reset *\/ */
/*   __IOM  uint32_t CLKEN;             /\*!< Offset: 0x024 (R/W)  CLUSTER_COMPRESS_ENGINE clock enable *\/ */
/*   __IOM  uint32_t TRIGGER;           /\*!< Offset: 0x028 (R/W)  CLUSTER_COMPRESS_ENGINE trigger *\/ */
/*   __IOM  uint32_t _reserved0;        /\*!< Offset: 0x02C (R/W)  reserved Register *\/ */
/*   __IOM  uint32_t L2_COUNT;          /\*!< Offset: 0x030 (R/W)  CLUSTER_COMPRESS_ENGINE l2 2D count *\/ */
/*   __IOM  uint32_t L2_STRIDE;         /\*!< Offset: 0x034 (R/W)  CLUSTER_COMPRESS_ENGINE l2 2D stride *\/ */
/*   __IOM  uint32_t TCDM_COUNT;        /\*!< Offset: 0x038 (R/W)  CLUSTER_COMPRESS_ENGINE tcdm 2D count *\/ */
/*   __IOM  uint32_t TCDM_STRIDE;       /\*!< Offset: 0x03C (R/W)  CLUSTER_COMPRESS_ENGINE tcdm 2D stride *\/ */

/* } CLUSTER_DMAMCHAN_COMPRESSOR_Type; */
/* #define CLUSTER_DMAMCHAN_COMPRESSOR_BASE (0x10202000UL)                   /\*!< CLUSTER TCDM compress engine Base Address *\/ */
/* #define CLUSTER_DMAMCHAN_COMPRESSOR ((CLUSTER_DMAMCHAN_COMPRESSOR_Type  *)   CLUSTER_DMAMCHAN_COMPRESSOR_BASE )  /\*!< TCDM/L2 to L2/TCDM compress engine struct *\/ */

#define LUT_PATTERN 0x0F00

static inline void report_conf ( unsigned int tcdm_addr, unsigned int l2_addr,    unsigned int L2_count,
                                 unsigned int L2_stride, unsigned int tcdm_count, unsigned int tcdm_stride, unsigned int conf_reg  ) {
    printf("/************************************************/\n");
    printf("TCDM_ADDR       = 0x%8X; \n",  tcdm_addr);
    printf("L2_ADDR         = 0x%8X; \n",  l2_addr);
    printf("L2_COUNT_REG    = 0x%8X; \n",  L2_count);
    printf("L2_STRIDE_REG   = 0x%8X; \n",  L2_stride);
    printf("TCDM_COUNT_REG  = 0x%8X; \n",  tcdm_count);
    printf("TCDM_STRIDE_REG = 0x%8X; \n",  tcdm_stride);
    printf("CONF_REG        = 0x%1X; \n",  conf_reg);
    printf("/************************************************/\n");
}

int test_compressor(void )
{
    volatile unsigned int *ptr_L2;

    volatile int testR;
    volatile int error = 0;

    volatile unsigned int *TCDM_ADDR;
    volatile unsigned int *L2_ADDR;
    volatile unsigned int *CONF_REG;
    volatile unsigned int *STAT_REG;
    volatile unsigned int *LUT_REG;
    volatile unsigned int *SYMBOL_REG;
    volatile unsigned int *BIT_READ_REG;
    volatile unsigned int *MODE_REG;

    volatile unsigned int *SW_RST_REG;
    volatile unsigned int *CLKEN_REG;
    volatile unsigned int *TRIGGER_REG;

    volatile unsigned int *L2_COUNT_REG;
    volatile unsigned int *L2_STRIDE_REG;
    volatile unsigned int *TCDM_COUNT_REG;
    volatile unsigned int *TCDM_STRIDE_REG;

    unsigned int dec_mode    = 0;  // Type1
    unsigned int ext_mode    = 0;  // 32 bit extension
    unsigned int bit_width   = 0;  // size of each compressed item
    unsigned int sign_ext    = 0;  // Use Sign Extension

    unsigned int start_bit   = 0;
    unsigned int start_byte  = 0;

    unsigned int direction = 0;  // 0 Decompression (L2 to TCDM);  1 compression (TCDM to L2)

    unsigned int size_transf = 0; // 32 Byte/Item



    printf("StartTest!\n");

    ptr_L2            = (unsigned int*)(0x1C000000);

    // | ID  |  ADDR       Note           | Bit| Attr
    //-----------------------------------------------
    // | 0:  | 0x0000 --> TCDM_ADDR_REG   | 32 | RW |
    // | 1:  | 0x0004 --> L2_ADDR_REG     | 32 | RW |
    // | 2:  | 0x0008 --> CONF_REG        | 32 | RW |
    // | 3:  | 0x000C --> STATUS_REG      |  4 | R  |
    // | 4:  | 0x0010 --> LUT WRITE       | 32 | RW |
    // | 5:  | 0x0014 --> SPECIAL_SYMBOL  |    |    |
    // | 6:  | 0x0018 --> BIT_READ        | 32 | R  |
    // | 7:  | 0x001C --> MODE_REG        |  2 | RW |
    // | 8:  | 0x0020 --> SOFT_RESET      | 0  | W  |
    // | 9:  | 0x0024 --> CLOCK ENABLE    | 1  | RW |
    // | 10: | 0x0028 --> PUSH_CMD        | 0  | W  |
    // | 11: | 0x002C --> ---------       |    |    |
    // | 12: | 0x0030 --> L2_COUNT_REG    | 16 | RW |
    // | 13: | 0x0034 --> L2_STRIDE_REG   | 16 | RW |
    // | 14: | 0x0038 --> TCDM_COUNT_REG  | 16 | RW |
    // | 15: | 0x003C --> TCDM_STRIDE_REG | 16 | RW |

    TCDM_ADDR     = (unsigned int*)(0x10202000);
    L2_ADDR       = (unsigned int*)(0x10202004);
    CONF_REG      = (unsigned int*)(0x10202008);
    STAT_REG      = (unsigned int*)(0x1020200C);
    LUT_REG       = (unsigned int*)(0x10202010);
    SYMBOL_REG    = (unsigned int*)(0x10202014);
    BIT_READ_REG  = (unsigned int*)(0x10202018);
    MODE_REG      = (unsigned int*)(0x1020201C);

    SW_RST_REG    = (unsigned int*)(0x10202020);
    CLKEN_REG     = (unsigned int*)(0x10202024);
    TRIGGER_REG   = (unsigned int*)(0x10202028);

    L2_COUNT_REG    = (unsigned int*)(0x10202030);
    L2_STRIDE_REG   = (unsigned int*)(0x10202034);
    TCDM_COUNT_REG  = (unsigned int*)(0x10202038);
    TCDM_STRIDE_REG = (unsigned int*)(0x1020203C);

    SW_RST_REG[0] = 1;

    CLKEN_REG[0]  = 1;

    SYMBOL_REG[0]   = 0xABBAABBA;



    printf("PROGRAMMING TX: TCDM --> L2 \n");
    dec_mode    = GLOB_dec_mode;   // Type1
    ext_mode    = GLOB_ext_mode;   // 32 bit extension
    bit_width   = GLOB_bit_width;  // size of each compressed item
    sign_ext    = GLOB_sign_ext;   // Use Sign Extension

    start_bit   = GLOB_start_bit;
    start_byte  = GLOB_start_byte;

    direction   = 1;

    size_transf = GLOB_size_transf;   // number of items

    TCDM_ADDR[0] = ADDR_DEC_DATA;
    L2_ADDR[0]   = 0x1C000000;

    L2_COUNT_REG[0]    = 0;
    L2_STRIDE_REG[0]   = 0;
    TCDM_COUNT_REG[0]  = 0;
    TCDM_STRIDE_REG[0] = 0;
    MODE_REG[0]        = 0;

    CONF_REG[0]  = dec_mode | (ext_mode << 2) |  (bit_width << 4) | (sign_ext << 9) | (start_bit << 10) | (direction << 13) | (start_byte << 14) | (size_transf << 16) ;

    report_conf( (unsigned int) TCDM_ADDR[0], (unsigned int) L2_ADDR[0], (unsigned int) L2_COUNT_REG[0], (unsigned int ) L2_STRIDE_REG[0], (unsigned int) TCDM_COUNT_REG[0], (unsigned int) TCDM_STRIDE_REG[0], (unsigned int) CONF_REG[0]  );


    for(unsigned int j=0; j<size_transf; j++)
    {
        //printf(" ptr_TCDM_expect[%d] = %8X\n ",j,tcdm_expect_data[j]);
        ptr_TCDM[j] = tcdm_expect_data[j];
    }


    TRIGGER_REG[0] = 1;

    printf("POLLING \n");
    while(STAT_REG[0] == 8)
    {
        for(unsigned int j=0; j<16; j++)
        {
            testR = 12;
        }
    }



    unsigned int got_data, expected_data;


    printf("Checking! initial Errors on L2 = %d\n", error);
    int unsigned n_count, mask;
    n_count = ((size_transf*bit_width)%32 == 0) ?  (size_transf*bit_width)/32 : size_transf*bit_width/32+1;
    mask = (2 << ((size_transf*bit_width)%32 -1) )-1;

    for(unsigned int j=0; j<n_count; j++)
    {
        expected_data = (j == n_count-1) ? l2_compr_data[j] & mask : l2_compr_data[j];
        got_data      = (j == n_count-1) ? ptr_L2[j] & mask        : ptr_L2[j];
        if ( expected_data != got_data )
        {
            printf("ERROR on Iteration %d [DEK, GOLDEN]: %8X != %8X \n" , j , got_data, expected_data );
            error++;
        }
    }


    printf("Read %d bit!\n", BIT_READ_REG[0]);
    printf("TEST DONE! with %d errors! \n", error);
    return error;
}

void Master_Entry(void *arg) {
    test_compressor();
}

int main(void )
{
    printf("Fabric controller code execution for mbed_os Cluster Compressor test\n");

    /* Cluster Start - Power on, 9th core seperation disable */
    CLUSTER_Start(0, CLUSTER_CORES_NUM, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}

/* END USER CODE */
