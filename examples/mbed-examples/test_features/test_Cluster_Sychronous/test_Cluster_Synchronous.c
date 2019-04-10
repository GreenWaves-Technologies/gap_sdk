// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#define CORE_NUMBER      8

static int starting = 0;

static void cluster_sync_test(void *arg)
{
  printf("Core %d starts first task\n", __core_ID());

  // Hardware mutex - this will be exposed properly in a subsequent release
  EU_MutexLock(0);
  starting++;
  EU_MutexUnlock(0);

  // Do something with a time varying by core
  for (uint32_t i = 0; i < (1000 + (__core_ID() * 10)); i++)
  {
      uint32_t j = 0;
      j++;
  }

  printf("Core %d finished first task\n", __core_ID());
  // Re synchronize with the other cores
  CLUSTER_SynchBarrier();


  EU_MutexLock(0);
  starting++;
  EU_MutexUnlock(0);

  // Do something with a time varying by core
  printf("Core %d starts second task\n", __core_ID());
  for (uint32_t i = 0; i < (1000 + (__core_ID() * 10)); i++)
  {
      uint32_t j = 0;
      j++;
  }
  printf("Core %d finished second task\n", __core_ID());
}

void Master_Entry(void *arg) {
    CLUSTER_CoresFork(cluster_sync_test, arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Cluster Synchronous test\n");

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    #ifdef JENKINS_TEST_FLAG
    exit(error);
    #else
    return error;
    #endif
}
