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

/* } TCDM_COMPRESS_ENGINE_Type; */
/* #define CLUSTER_TCDM_COMPRESS_ENGINE_BASE (CLUSTER_BASE + 0x00200000UL)                   /\*!< CLUSTER TCDM compress engine Base Address *\/ */
/* #define CLUSTER_TCDM_COMPRESS_ENGINE ((TCDM_COMPRESS_ENGINE_Type  *)   CLUSTER_TCDM_COMPRESS_ENGINE_BASE )  /\*!< TCDM/L2 to L2/TCDM compress engine struct *\/ */

/* Add USER CODE */
static inline unsigned int core_id() {
  int hart_id;
  asm("csrr %0, 0xF14" : "=r" (hart_id) : );
  return hart_id & 0x0ff;
}

#define LUT_PATTERN 0x0F00

static inline void report_conf ( unsigned int *tcdm_addr, unsigned int *l2_addr,    unsigned int *L2_count,
                                 unsigned int *L2_stride, unsigned int *tcdm_count, unsigned int *tcdm_stride, unsigned int *conf_reg  ) {
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

int main(void )
{
   volatile unsigned int *ptr_L2;
   volatile unsigned int *ptr_EOC;

   volatile int testR;
   volatile int cid;
   volatile int error = 0;

   cid = core_id();

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

    TCDM_ADDR     = (unsigned int*)(0x10200000);
    L2_ADDR       = (unsigned int*)(0x10200004);
    CONF_REG      = (unsigned int*)(0x10200008);
    STAT_REG      = (unsigned int*)(0x1020000C);
    LUT_REG       = (unsigned int*)(0x10200010);
    SYMBOL_REG    = (unsigned int*)(0x10200014);
    BIT_READ_REG  = (unsigned int*)(0x10200018);
    MODE_REG      = (unsigned int*)(0x1020001C);

    SW_RST_REG    = (unsigned int*)(0x10200020);
    CLKEN_REG     = (unsigned int*)(0x10200024);
    TRIGGER_REG   = (unsigned int*)(0x10200028);

    L2_COUNT_REG    = (unsigned int*)(0x10200030);
    L2_STRIDE_REG   = (unsigned int*)(0x10200034);
    TCDM_COUNT_REG  = (unsigned int*)(0x10200038);
    TCDM_STRIDE_REG = (unsigned int*)(0x1020003C);

    SW_RST_REG[0] = 1;

    CLKEN_REG[0]  = 1;

    SYMBOL_REG[0]   = 0xABBAABBA;



     printf("PROGRAMMING TX: TCDM --> L2 \n");
     dec_mode    = 0;   // Type1
     ext_mode    = 3;   // 32 bit extension
     bit_width   = 5;   // size of each compressed item
     sign_ext    = 0;   // Use Sign Extension

     start_bit   = 0;
     start_byte  = 0;
     direction   = 1;

     size_transf = 300;   // number of items

     TCDM_ADDR[0]       = 0x10000000;
     L2_ADDR[0]         = 0x1C000000;

     L2_COUNT_REG[0]    = 0;
     L2_STRIDE_REG[0]   = 0;
     TCDM_COUNT_REG[0]  = 0;
     TCDM_STRIDE_REG[0] = 0;
     MODE_REG[0]        = 0;

     CONF_REG[0]  = dec_mode | (ext_mode << 2) |  (bit_width << 4) | (sign_ext << 9) | (start_bit << 10) | (direction << 13) | (start_byte << 14) | (size_transf << 16) ;

     report_conf( (unsigned int *) TCDM_ADDR[0], (unsigned int *) L2_ADDR[0], (unsigned int *) L2_COUNT_REG[0], (unsigned int *) L2_STRIDE_REG[0], (unsigned int *) TCDM_COUNT_REG[0], (unsigned int *) TCDM_STRIDE_REG[0], (unsigned int *) CONF_REG[0]  );


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
     mask = (2 << ((size_transf*bit_width)%32) -1 )-1;

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

   ptr_EOC     = (int*)(0x10201000);
   ptr_EOC[1]  = (error > 0 ) ? 0xDEADBEEF : 0xABBAABBA;
   ptr_EOC[0]  = error;
   ptr_EOC[2]  = 1;

   return error;
}

/* END USER CODE */
