#include "gap_common.h"


/* Variables used. */
#define BUF_SIZE 256
GAP_L2_DATA uint32_t src_L2[BUF_SIZE], dst_L2[BUF_SIZE];

static DMACPY_Type *udmaCopy = DMACPY_BASE_PTRS;

static void printData( uint32_t *buf, uint32_t size )
{
    for( uint32_t i = 0; i <  size; i++ )
        printf(" %2x ", buf[i]);
    printf("\n");
}

static void checkData( uint32_t *cmpA, uint32_t *cmpB, uint32_t size, uint32_t *errSum )
{
    for( uint32_t i = 0; i <  size; i++ )
    {
        if( cmpA[i] != cmpB[i] )
            (*errSum)++;
    }
}

int main( void )
{
    printf("\n\n\t *** DMACPY Test ***\n\n");

    uint32_t errors = 1, src[BUF_SIZE], dst[BUF_SIZE];

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
        src[i] = i;

    printf("UDMA Dmacpy test FC( src %x ) -> L2( src_L2 %x ) -> L2( dst_L2 %x) -> FC( dst %x ).\n\n",
           &src, &src_L2, &dst_L2, &dst);

    DMACPY_Init( udmaCopy );

    DMACPY_BlockingTransfer( udmaCopy, src, src_L2, sizeof( src ), uDMACPY_FC2L2 );

    DMACPY_BlockingTransfer( udmaCopy, src_L2, dst_L2, sizeof( src ), uDMACPY_L22L2 );

    DMACPY_BlockingTransfer( udmaCopy, dst_L2, dst, sizeof( src ), uDMACPY_L22FC );

    DMACPY_Deinit( udmaCopy );

    if( errors )
    {
        printf("\t FC SRC\n");
        printData( src, BUF_SIZE );
        printf("\t L2 SRC\n");
        printData( src_L2, BUF_SIZE );
        printf("\t L2 DST\n");
        printData( dst_L2, BUF_SIZE );
        printf("\t FC DST\n");
        printData( dst, BUF_SIZE );
        errors = 0;
    }

    checkData( src, src_L2, BUF_SIZE, &errors );
    checkData( src, dst_L2, BUF_SIZE, &errors );
    checkData( src, dst, BUF_SIZE, &errors );

    printf("Test %s with %d errors.\n", ( errors ? "failed": "success" ), errors);
}
