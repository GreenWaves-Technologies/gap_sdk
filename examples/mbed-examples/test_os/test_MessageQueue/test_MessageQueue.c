/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__Message.html
 */

#include<stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

osThreadId_t tid_Thread_MsgQueue1;                                 // thread id 1
osThreadId_t tid_Thread_MsgQueue2;                                 // thread id 2

// Thread allocation
#define TASK_SIZE 1024

GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK1_STK[TASK_SIZE];
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK2_STK[TASK_SIZE];

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

#define MSGQUEUE_OBJECTS      16                                   // number of Message Queue Objects

typedef struct {                                                   // object data type
    uint8_t Buf[32];
    uint8_t Idx;
} MSGQUEUE_OBJ_t;

osMessageQueueId_t mid_MsgQueue;                                   // message queue id

// Message Queue allocation
#define MSGQ_SIZE 64
GAP_FC_DATA unsigned char MSGQ_STK[MSGQ_SIZE];

#define MSGQ_DATA_SIZE 1024
GAP_FC_DATA unsigned char MSGQ_DATA_STK[MSGQ_DATA_SIZE];

const osMessageQueueAttr_t msgq_attr = {
    "msgq",
    0,
    MSGQ_STK,
    sizeof(MSGQ_STK),
    MSGQ_DATA_STK,
    sizeof(MSGQ_DATA_STK)
};


void Thread_MsgQueue1 (void *argument) {
    MSGQUEUE_OBJ_t msg;
    while (1) {
        // Insert thread code here...
        printf("Thread1 puts data\n");

        msg.Buf[0] = 0x55;                                           // do some work...
        msg.Idx    = 0;
        osMessageQueuePut (mid_MsgQueue, &msg, 0, 0);
        osThreadYield ();                                            // suspend thread
    }
}

void Thread_MsgQueue2 (void *argument) {
    MSGQUEUE_OBJ_t msg;
    osStatus_t status;
    while (1) {
        // Insert thread code here...
        status = osMessageQueueGet (mid_MsgQueue, &msg, NULL, osWaitForever);  // wait for message
        if (status == osOK) {
            // process data
            printf("Thread2 gets data. msg.Idx = %d, msg.Buf[0] = %x\n", msg.Idx, msg.Buf[0]);
            if (msg.Buf[0] != 0x55)
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
}

osThreadId_t create_new_thread(void *task, const osThreadAttr_t *task_attr) {

    osThreadId_t id = osThreadNew((osThreadFunc_t)task, NULL, task_attr);

    if ((void *)id == NULL) {
        printf("Could not create New thread\n");
    }
    return id;
}

int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os Message Queue test\n");

        mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), &msgq_attr);
        if (!mid_MsgQueue) {
            // Message Queue object not created, handle failure
            printf("Could not create Message queue\n");
            return -1;
        }

        // Create new threads
        tid_Thread_MsgQueue1 = create_new_thread(Thread_MsgQueue1, &task1_attr);
        tid_Thread_MsgQueue2 = create_new_thread(Thread_MsgQueue2, &task2_attr);

        printf("Main thread is finished\n");
        return 0;
    }
}
