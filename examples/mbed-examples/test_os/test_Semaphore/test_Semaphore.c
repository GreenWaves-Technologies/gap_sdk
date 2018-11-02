/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__SemaphoreMgmt.html
 */
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

#define TASK_SIZE 1024
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK1_STK[TASK_SIZE];
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK2_STK[TASK_SIZE];
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK3_STK[TASK_SIZE];

#define SEMAPHORE_SIZE 16
GAP_FC_DATA unsigned int SEMAPHORE_STK[SEMAPHORE_SIZE];

// TASK1
os_thread_t    task1_obj;
const osThreadAttr_t task1_attr = {
  "task_thread",
  0,
  &task1_obj,
  sizeof(task1_obj),
  TASK1_STK,
  sizeof(TASK1_STK),
  osPriorityNormal
};

// TASK2
os_thread_t    task2_obj;
const osThreadAttr_t task2_attr = {
  "task_thread",
  0,
  &task2_obj,
  sizeof(task2_obj),
  TASK2_STK,
  sizeof(TASK2_STK),
  osPriorityNormal
};

// TASK3
os_thread_t    task3_obj;
const osThreadAttr_t task3_attr = {
  "task_thread",
  0,
  &task3_obj,
  sizeof(task3_obj),
  TASK3_STK,
  sizeof(TASK3_STK),
  osPriorityNormal
};

osSemaphoreId_t sid_Thread_Semaphore;                      // semaphore id

const osSemaphoreAttr_t Thread_Semaphore_attr = {
  "myThreadSem",                          // human readable mutex name
  0,                                      // attr_bits
  SEMAPHORE_STK,                          // memory for control block
  SEMAPHORE_SIZE                          // size for control block
};

int CreateSemaphore (void)
{

  sid_Thread_Semaphore = osSemaphoreNew(2, 2, &Thread_Semaphore_attr);
  if (!sid_Thread_Semaphore) {
    // Semaphore object not created, handle failure
    printf("New semaphore can not create\n");
  }

  return(0);
}

void task1 () {
  osStatus_t val;
  while(1) {
    val = osSemaphoreAcquire (sid_Thread_Semaphore, 10);   // wait 10 mSec
    printf("Sem1 = %x, ", (int)val);

    switch (val) {
    case osOK:
      printf("Task1 got a count, release after 1ms\n");
      osDelay(1U);
      osSemaphoreRelease (sid_Thread_Semaphore);           // Return a token back to a semaphore
      break;
    case osErrorResource:
      printf("Task1 can not get a count\n");
      break;
    case osErrorParameter:
      printf("Task1 can not get a count\n");
      break;
    default:
      break;
    }

    osThreadYield ();                                      // suspend thread
  }
}

void task2 () {
  osStatus_t val;
  while(1) {
    val = osSemaphoreAcquire (sid_Thread_Semaphore, 10);   // wait 10 mSec
    printf("Sem2 = %x, ", (int)val);

    switch (val) {
    case osOK:
      printf("Task2 got a count, release after 2ms\n");
      osDelay(2U);
      osSemaphoreRelease (sid_Thread_Semaphore);           // Return a token back to a semaphore
      break;
    case osErrorResource:
      printf("Task2 can not get a count\n");
      break;
    case osErrorParameter:
      printf("Task2 can not get a count\n");
      break;
    default:
      break;
    }

    osThreadYield ();                                      // suspend thread
  }
}

void task3 () {
  osStatus_t val;
  while(1) {
    val = osSemaphoreAcquire (sid_Thread_Semaphore, 10);   // wait 10 mSec
    printf("Sem3 = %x, ", (int)val);

    switch (val) {
    case osOK:
      printf("Task3 got a count, release after 3ms\n");
      osDelay(3U);
      osSemaphoreRelease (sid_Thread_Semaphore);           // Return a token back to a semaphore
      break;
    case osErrorResource:
      printf("Task3 can not get a count\n");
      break;
    case osErrorParameter:
      printf("Task3 can not get a count\n");
      break;
    default:
      break;
    }

    osThreadYield ();                                      // suspend thread
  }
}

int create_new_thread(void *task, const osThreadAttr_t *task_attr) {

  osThreadId_t result = osThreadNew ((osThreadFunc_t)task, NULL, task_attr);

  if ((void *)result == NULL) {
    printf("New thread not created\n");
    return -1;
  }
  return 0;
}


int main()
{
  if (__is_FC()) {

    printf("Fabric controller code execution for mbed_os test\n");

    int error = CreateSemaphore();
    error = create_new_thread(task1, &task1_attr);
    error = create_new_thread(task2, &task2_attr);
    error = create_new_thread(task3, &task3_attr);
    printf("Created semaphore of 2 count\n");

    while(1) {
      printf("main: Do not disturb me ..\n");
      osDelay(3);
    }

    if (error)
    {
      printf("Test failed\n");
      return -1;
    }
    else
    {
      printf("Test success\n");
      printf("Main thread is finished, then go to idle thread, sleep() function is not implemented ...\n");
      return 0;
    }
  }
}
