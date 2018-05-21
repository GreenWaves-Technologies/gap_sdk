// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"
#include <stdlib.h>

#define TASK_SIZE 1024
GAP_FC_DATA __attribute__ ((aligned (8))) unsigned int TASK_STK[TASK_SIZE];

os_thread_t    task_obj;
osThreadAttr_t task_attr;

uint32_t count = 0;

void task () {
    while(1)
    {
        count++;
        printf("CouCou ^-^ !\n");
    }
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
            return -1;
        }
        return 0;
}

int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os test\n");

        create_new_thread();
        
        printf("Main thread is executing, Please Waiting ...\n");

        while(!count) {
            printf("main: Do not disturb me ..\n");
        }

        if(count)
        {
            printf("Test success\n");
            #ifdef JENKINS_TEST_FLAG
            exit(0);
            #else
            return 0;
            #endif
        }
        else
        {
            printf("Test failed\n");
            #ifdef JENKINS_TEST_FLAG
            exit(-1);
            #else
            return -1;
            #endif
        }
    }
}
