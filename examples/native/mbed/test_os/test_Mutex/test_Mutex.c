#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

#define TASK_SIZE 1024
GAP_FC_DATA __attribute__ ((aligned (8))) char TASK_STK[TASK_SIZE];

#define MUTEX_SIZE 28
GAP_FC_DATA __attribute__ ((aligned (8))) char MUTEX_STK[MUTEX_SIZE];

os_thread_t    task_obj;
osThreadAttr_t task_attr;

osMutexId_t my_mutex_id;

unsigned int count = 0;

const osMutexAttr_t Thread_Mutex_attr = {
  "myThreadMutex",                          // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  MUTEX_STK,                                // memory for control block
  MUTEX_SIZE                                // size for control block
};

int CreateMutex (void)  {

  my_mutex_id = osMutexNew(&Thread_Mutex_attr);

  if (my_mutex_id == NULL)  {
    printf("New mutex not created\n");
    return -1;
  }
  return 0;
}

int AcquireMutex (osMutexId_t mutex_id)  {
  osStatus_t  status;

  if (mutex_id != NULL)  {
    status  = osMutexAcquire(mutex_id, osWaitForever);
    if (status != osOK)  {
      // handle failure code
      printf("%x => Can not aquire mutex!\n", status);
      return -1;
    }
  }
  return 0;
}

void ReleaseMutex (osMutexId_t mutex_id)  {
  osStatus_t status;

  if (mutex_id != NULL)  {
    status = osMutexRelease(mutex_id);
    if (status != osOK)  {
      // handle failure code
      printf("%x => Can not release mutex!\n", status);
    }
  }
}

void task () {

  // If not aquire mutex, continue asking
  AcquireMutex(my_mutex_id);

  while(count < 50) {
      count++;
      printf("CouCou ^-^ !\n");
  }
  ReleaseMutex(my_mutex_id);
}

int create_new_thread() {
  task_attr.stack_mem  = TASK_STK;
  task_attr.stack_size = sizeof(TASK_STK);
  task_attr.cb_size    = sizeof(task_obj);
  task_attr.cb_mem     = &task_obj;
  task_attr.priority   = osPriorityNormal;
  task_attr.name       = "task_thread";

  osThreadId_t result = osThreadNew ((osThreadFunc_t)task, NULL, &task_attr);

  if ((void *)result == NULL) {
    printf("New thread not created\n");
    return (int)result;
  }
  return 0;
}

int main()
{
    printf("Fabric controller code execution for mbed_os Mutex test\n");

    int error = create_new_thread();
    error = CreateMutex();
    printf("Main thread is executing, Please Waiting ...\n");

    AcquireMutex(my_mutex_id);
    while(count < 100) {
        count++;
        printf("main thread takes the mutex, it will relase after count = 100  ..\n");
    }
    ReleaseMutex(my_mutex_id);

    if (error)
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
