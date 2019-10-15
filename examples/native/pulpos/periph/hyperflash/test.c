/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define BUFF_SIZE 128
// read address
#define SA      0x0000

static char buff[BUFF_SIZE];
static int done = 0;
static rt_event_sched_t sched;
static rt_flash_t *flash;

/* The function called when RX transfer is finished */
/* It will count the number of finished RX transfers */
static void end_of_rx(void *arg)
{
    printf("End of RX for id %d\n", (int)arg);
    done++;
}

int main()
{
    int err = 0;
    printf("Entering main controller\n");

    /* Initialize a event scheduler which controls the execution of events */
    rt_event_sched_init(&sched);

    /* Allocate 4 event elements in the scheduler */
    if (rt_event_alloc(&sched, 4)) return -1;

    /* Initialize Flash configuration structure */
    /* Here, we use a linux style device driver which supports both SPI Flash and Hyper Flash etc. */
    rt_flash_conf_t flash_conf;
    rt_flash_conf_init(&flash_conf);

    rt_padframe_profile_t *profile_hyper = rt_pad_profile_get("hyper");
    if (profile_hyper == NULL) {
        printf("pad config error\n");
        return 1;
    }
    rt_padframe_set(profile_hyper);
    printf ("pad config done\n");

    /* Open and initialize Hyper Flash device according to device name*/
    printf ("opening hyperflash\n");
    flash = rt_flash_open("hyperflash", &flash_conf, NULL);
    if (flash == NULL) return -1;

    /* Start a RX transfer : read BUFF_SIZE bytes from address 0 to buff[0] address */
    /* When it's finished, it will push function end_of_rx with argument 0 to a event queue */
    rt_flash_read(flash, buff, (void *)SA, BUFF_SIZE, rt_event_get(&sched, end_of_rx, (void *)0));

    /* A event scheduler will always wait and execute if any event being pushed to the queue */
    /* In this test, we have two RX transfers and in the end of the RX transfers, it will count two times. */
    /* So, when all transfers are finished, it will exit */
    while(!done)
        rt_event_execute(&sched, 1);

    /* All transfers are finished, close the device */
    printf ("closing hyperflash\n");
    rt_flash_close(flash, NULL);

    for (int i=0; i<BUFF_SIZE; i++){
        printf("%02x ", buff[i]);
    }
    printf ("\n");

    printf ("Test success\n");

    return 0;
}
