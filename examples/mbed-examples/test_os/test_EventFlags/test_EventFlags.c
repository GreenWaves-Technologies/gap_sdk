/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__EventFlags.html
 */
#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

osThreadId_t tid_Thread_EventSender;                          // thread id 1
osThreadId_t tid_Thread_EventReceiver;                        // thread id 2

osEventFlagsId_t evt_id;                                      // message queue id

#define FLAGS_MSK1 0x00000001ul

// Thread allocation
#define TASK_SIZE 1024
GAP_FC_DATA __attribute__ ((aligned (8))) char TASK1_STK[TASK_SIZE];
GAP_FC_DATA __attribute__ ((aligned (8))) char TASK2_STK[TASK_SIZE];

// TASK1
os_thread_t    task1_obj;
const osThreadAttr_t task1_attr = {
  "task_thread1",
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
  "task_thread2",
  0,
  &task2_obj,
  sizeof(task2_obj),
  TASK2_STK,
  sizeof(TASK2_STK),
  osPriorityNormal
};

// Event Flags allocation
#define EVFLAGS_SIZE 64
GAP_FC_DATA unsigned int EVFLAGS_STK[EVFLAGS_SIZE];

const osEventFlagsAttr_t evflags_attr = {
  "event_flags",
  0,
  EVFLAGS_STK,
  sizeof(EVFLAGS_STK)
};


void Thread_EventSender (void *argument)
{
    evt_id = osEventFlagsNew(&evflags_attr);
    while (1) {
        osEventFlagsSet(evt_id, FLAGS_MSK1);
        printf("Sended flags = 0x%x\n", FLAGS_MSK1);
        osDelay (10);                                           // suspend thread
    }
}

void Thread_EventReceiver (void *argument)
{
    uint32_t flags;
    while (1) {
        flags = osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        printf("Received flags = 0x%x\n", flags);//handle event

        if (flags != FLAGS_MSK1)
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

osThreadId_t create_new_thread(void *task, const osThreadAttr_t *task_attr) {

    osThreadId_t id = osThreadNew((osThreadFunc_t)task, NULL, task_attr);

    if ((void *)id == NULL) {
        printf("Could not create New thread\n");
    }
    return id;
}

/**
   The behavior is the following:

   Event sender sends 1
   Event receiver receives 1
   Wait for 10ms

   Event sender sends 1
   Event receiver receives 1
   Wait for 10ms

   ...
**/
int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os event flags test\n");

        // Create new threads
        tid_Thread_EventSender   = create_new_thread(Thread_EventSender, &task1_attr);
        tid_Thread_EventReceiver = create_new_thread(Thread_EventReceiver, &task2_attr);

        printf("Main thread is finished\n");
        return 0;
    }
}
