/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__ThreadFlagsMgmt.html
 */
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"


// Thread allocation
#define TASK_SIZE 1024
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned char TASK_STK[TASK_SIZE];

os_thread_t    task_obj;
osThreadAttr_t task_attr;

void threadX (void *argument) {
    uint32_t   flagsX = 0;

    osDelay(1U);
    for (;;) {
        printf("Waiting flag = 1, Return value = %d \n", flagsX);
        flagsX = osThreadFlagsWait(0x0001U, osFlagsWaitAny, 3); /* B */
    }
}

osThreadId_t create_new_thread() {
    task_attr.stack_mem  = TASK_STK;
    task_attr.stack_size = sizeof(TASK_STK);
    task_attr.cb_size    = sizeof(task_obj);
    task_attr.cb_mem     = &task_obj;
    task_attr.priority   = osPriorityNormal;
    task_attr.name       = "task_thread";

    osThreadId_t result = osThreadNew ((osThreadFunc_t)threadX, NULL, &task_attr);

    if ((void *)result == NULL) {
        printf("New thread not created\n");
    }
    return result;
}

/**
  The behavior is the following:
    main starts executing
    statement A sets thread flags to 0x02 (flags = 0x02 – after set)
    main enters delay
    execution switches to threadX
    statement B waits for flag 0x01 and blocks since flag is not set
    execution switches to main
    statement C sets thread flags to 0x07
    threadX wakes-up and clears flag 0x01, thread flags = 0x06, return value set to 0x07 (before clear), note: all this happens during statement C
    statement C returns with flags = 0x06
    main enters delay
    execution switches to threadX
    statement B returns with flagsX = 0x07
    main is finished
    statement B continues executing flagsX = 0xfffffffe (wait for signal)
**/
int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os signal test\n");

        uint32_t   flags;
        // Create a new thread
        osThreadId_t tid = create_new_thread();

        // Set flags
        flags = osThreadFlagsSet(tid, 0x0002U); /* A */
        printf("Setting flag = %d \n", flags);
        osDelay(2U);

        flags = osThreadFlagsSet(tid, 0x0005U); /* C */
        printf("Setting flag = %d \n", flags);
        osDelay(2U);

        printf("Main thread is finished with Test success\n");
        return 0;
    }
}
