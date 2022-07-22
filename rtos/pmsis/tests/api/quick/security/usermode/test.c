/* 
 * Copyright (C) 2020 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Antoine Faravelon, GWT (antoine.faravelon@greenwaves-technologies.com)
 */

#include "pmsis.h"

volatile uint32_t user_done = 0;
void usermode_entry(void *arg)
{
    volatile char user_string[] = "Hello from user thread\n";
    // syscall from user mode
    syscall(ECALL_PUTS,(uint32_t)user_string,0,0,0);
    
    user_done = 1;
    while(1);
}


static int test_entry()
{
    // syscall from kernel mode
    uint32_t id = syscall(ECALL_CLUSTERID,0,0,0,0);
    printf("Hello from main thread %d\n",id);

    pi_user_task_create(usermode_entry, NULL, "user_thread",
            MAIN_APP_STACK_SIZE/sizeof(configSTACK_DEPTH_TYPE),tskIDLE_PRIORITY + 1);

    while(!user_done)
    {
        pi_time_wait_us(1000);
    }
    return 0;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
