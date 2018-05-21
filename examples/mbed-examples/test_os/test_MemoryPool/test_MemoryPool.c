/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__PoolMgmt.html
 */
#include<stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

/*----------------------------------------------------------------------------
 *      Memory Pool creation & usage
 *---------------------------------------------------------------------------*/

#define MEMPOOL_OBJECTS      16                               // number of Memory Pool Objects

typedef struct {                                              // object data type
    uint8_t Buf[32];
    uint8_t Idx;
} MEM_BLOCK_t;

void Thread_MemPool (void *argument);                         // thread function
osThreadId_t tid_Thread_MemPool;                              // thread id

osMemoryPoolId_t mpid_MemPool;                                // memory pool id

int Init_MemPool (void)
{

    mpid_MemPool = osMemoryPoolNew(MEMPOOL_OBJECTS,sizeof(MEM_BLOCK_t), NULL);
    if (mpid_MemPool == NULL) {
        // MemPool object not created, handle failure
        printf("Memory Pool allocation failed\n");
    }

    tid_Thread_MemPool = osThreadNew (Thread_MemPool,NULL , NULL);
    if (tid_Thread_MemPool == NULL) {
        printf("New thread can not be created\n");
        return(-1);
    }

    return(0);
}

void Thread_MemPool (void *argument)
{
    osStatus_t     status;
    MEM_BLOCK_t *pMem = 0;

    while (1) {
        // Insert thread code here...

        pMem = (MEM_BLOCK_t *)osMemoryPoolAlloc (mpid_MemPool, 0);    // get Mem Block
        if (pMem) {                                                      // Mem Block was available
            pMem->Buf[0] = 0x55;                                           // do some work...
            pMem->Idx    = 0;

            status = osMemoryPoolFree (mpid_MemPool, pMem);                // free mem block
            switch (status)  {
            case osOK:
                printf("OK!\n");
                break;
            case osErrorParameter:
                printf("Error!\n");
                break;
            case osErrorNoMemory:
                printf("Error!\n");
                break;
            default:
                break;
            }
        }

        osThreadYield ();                                                // suspend thread
    }
}


int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os Memory Pool test\n");

        if (Init_MemPool() != 0)
        {
            printf("Test failed\n");
            #ifdef JENKINS_TEST_FLAG
            exit(-1);
            #else
            return -1;
            #endif
        }
        else
        {
            printf("Test success\n");
            #ifdef JENKINS_TEST_FLAG
            exit(0);
            #else
            return 0;
            #endif
        }
    }
}
