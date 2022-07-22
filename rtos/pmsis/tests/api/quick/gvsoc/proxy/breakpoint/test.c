/* 
 * Copyright (C) 2021 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "stdio.h"


int test_entry()
{

    while(1)
    {
        *(volatile int *)0x1c000000 = 0x12345678;
        gv_stop();

        if (*(volatile int *)0x1c000000 != 0x87654321)
        {
            return -1;
        }
    }

    return 0;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
