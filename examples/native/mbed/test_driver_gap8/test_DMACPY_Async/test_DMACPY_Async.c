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
        {
            (*errSum)++;
            if( (*errSum) == 1 )
                printf("Errors : \n");
            printf(" [%x - %x] ", cmpA[i], cmpB[i]);
        }
    }
}

void callback( void *arg )
{
    printf("Callback transfer done !\n");
}

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Non Blocking DMACPY Test ***\n\n");

    uint32_t errors = 1, fc_src[BUF_SIZE], fc_dst[BUF_SIZE];
    dmacpy_handle_t handle;

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
        fc_src[i] = i;

    printf("UDMA Dmacpy test FC( fc_src %x ) -> L2( src_L2 %x ) -> L2( dst_L2 %x) -> FC( fc_dst %x ).\n\n",
           &fc_src, &src_L2, &dst_L2, &fc_dst);

    DMACPY_Init( udmaCopy );

    DMACPY_TransferCreateHandle( &handle, callback, NULL );
    DMACPY_NonBlockingTransfer( udmaCopy, fc_src, src_L2, sizeof( fc_src ), uDMACPY_FC2L2, &handle );

    DMACPY_TransferCreateHandle( &handle, callback, NULL );
    DMACPY_NonBlockingTransfer( udmaCopy, src_L2, dst_L2, sizeof( fc_src ), uDMACPY_L22L2, &handle );

    DMACPY_TransferCreateHandle( &handle, callback, NULL );
    DMACPY_NonBlockingTransfer( udmaCopy, dst_L2, fc_dst, sizeof( fc_src ), uDMACPY_L22FC, &handle );

    if( errors )
    {
        printf("\t FC SRC\n");
        printData( fc_src, BUF_SIZE );
        printf("\t L2 SRC\n");
        printData( src_L2, BUF_SIZE );
        printf("\t L2 DST\n");
        printData( dst_L2, BUF_SIZE );
        printf("\t FC DST\n");
        printData( fc_dst, BUF_SIZE );
        errors = 0;
    }

    checkData( fc_src, src_L2, BUF_SIZE, &errors );
    checkData( fc_src, dst_L2, BUF_SIZE, &errors );
    checkData( fc_src, fc_dst, BUF_SIZE, &errors );

    DMACPY_Deinit( udmaCopy );

    printf("Test %s with %d errors.\n", ( errors ? "failed": "success" ), errors);
}
