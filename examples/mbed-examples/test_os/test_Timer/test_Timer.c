/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__TimerMgmt.html
 */
#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

// Timer allocation
#define TIMER_SIZE 64
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned int TIMER0_STK[TIMER_SIZE];
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned int TIMER1_STK[TIMER_SIZE];

const osTimerAttr_t timer0_attr = {
    "timer0",
    0,
    TIMER0_STK,
    sizeof(TIMER0_STK)
};

const osTimerAttr_t timer1_attr = {
    "timer1",
    0,
    TIMER1_STK,
    sizeof(TIMER1_STK)
};

int32_t arg;

static void one_shot_Callback (void *argument) {
    arg = (int32_t)argument; // cast back argument '0'
    printf("one_shot_Callback(%d)\n", arg);
}

static void periodic_Callback (void *argument) {
    arg = (int32_t)argument; // cast back argument '5'
    printf("periodic_Callback(%d)\n", arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Timer test\n");

    osTimerId_t one_shot_id, periodic_id;
    // creates a one-shot timer:
    // (void*)0 is passed as an argument
    // to the callback function
    one_shot_id = osTimerNew(one_shot_Callback, osTimerOnce, (void *)10, &timer0_attr);

    if (one_shot_id == NULL)  {
        printf("One-shoot timer can not be created\n");
    }
    printf("Timer ID = %x \n", one_shot_id);

    // creates a periodic timer:
    // (void*)5 is passed as an argument
    // to the callback function
    periodic_id = osTimerNew(periodic_Callback, osTimerPeriodic, (void *)5, &timer1_attr);

    if (periodic_id == NULL)  {
        printf("Periodic timer created can not be created\n");
    }
    printf("Timer ID = %x \n", periodic_id);

    osTimerStart(one_shot_id, 100);
    osTimerStart(periodic_id, 50);
    osDelay(200U);

    if (arg == 0)
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
