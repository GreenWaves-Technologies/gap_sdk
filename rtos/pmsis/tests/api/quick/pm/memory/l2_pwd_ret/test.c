/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"



#define BUFF_SIZE (1<<18)
#define NB_ACCESSES (64)
#define STEP (BUFF_SIZE / NB_ACCESSES)


static int test_entry()
{
    int errors = 0;

    printf("STA\n");

    // Allocate a buffer and fill with some values. Note that this should trigger the power up of some banks
    uint32_t *chunk = (uint32_t *)pi_l2_malloc(BUFF_SIZE);


    for (int i=0; i<BUFF_SIZE / 4; i+=STEP)
    {
        chunk[i] = i;
    }


    // Now put it retentive
    pi_l2_ret_ctrl(chunk, BUFF_SIZE, 1);


    // Wait for some time, to see if values are kept
    pi_time_wait_us(500);


    // Deactivate retention, the banks should go back to normal state
    pi_l2_ret_ctrl(chunk, BUFF_SIZE, 0);


    // And check that the values were kept
    for (int i=0; i<BUFF_SIZE / 4; i+=STEP)
    {
        if (chunk[i] != i)
        {
            printf("Detected error (index: %d, expected: 0x%x, got: 0x%x)\n", i, i, chunk[i]);
            errors++;
        }
    }


    // Free the buffer which should trigger some bank power off
    pi_l2_free(chunk, BUFF_SIZE);

    if (errors)
    {
        printf("TKO\n");
    }
    else
    {
        printf("TOK\n");
    }

    return errors;
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