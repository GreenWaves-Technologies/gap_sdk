/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "rt/rt_api.h"
#include "stdio.h"
#include "Gap8.h"

#define BUFF_SIZE 100*1024

L2_MEM unsigned char write_buff[2][BUFF_SIZE];
L2_MEM unsigned char read_buff[2][BUFF_SIZE];
L2_MEM unsigned char *hyper_buff[2];
L2_MEM int count = 0;
L2_MEM rt_event_sched_t sched;
L2_MEM rt_hyperram_t *hyper;

/* The function called when RX transfer is finished */
/* It will count the number of finished RX transfers */
static void end_of_rx(void *arg)
{
  printf("End of RX for id %d\n", (int)arg);
  count++;
}

/* The function called when TX transfer is finished */
static void end_of_tx(void *arg)
{
  printf("End of TX for id %d\n", (int)arg);
  int i = (int)arg;

  /* Start a RX transfer : read BUFF_SIZE bytes from hyper_buff[i] address to buff[i] address */
  /* When it's finished, it will push function end_of_rx with argument i to a event queue */
  rt_hyperram_read(hyper, read_buff[i], hyper_buff[i], BUFF_SIZE, rt_event_get(&sched, end_of_rx, (void *)i));
}

int main()
{
  printf("Entering main controller\n");
  //Fill the IO buffers
  for (int i=0;i<BUFF_SIZE;i++){
      //Read buff init a 0
      read_buff[0][i] = 0;
      read_buff[1][i] = 0;
      //Wrtie buffer init with growing numbers
      write_buff[0][i] = i%256;
      write_buff[1][i] = i%256;
  }

  /* Initialize a event scheduler which controls the execution of events */
  rt_event_sched_init(&sched);

  /* Allocate 4 event elements in the scheduler */
  if (rt_event_alloc(&sched, 4)) return -1;

  /* Initialize Hyper RAM configuration structure */
  /* It will configure HyperBus timing, maximum transfer length etc. */
  rt_hyperram_conf_t hyperram_conf;
  rt_hyperram_conf_init(&hyperram_conf);

  rt_padframe_profile_t *profile_hyper = rt_pad_profile_get("hyper");
  if (profile_hyper == NULL) {
      printf("pad config error\n");
      return 1;
  }
  rt_padframe_set(profile_hyper);
  printf ("pad config done\n");

  /* Open and initialize Hyper RAM device according to device name */
  hyper = rt_hyperram_open("hyperram", &hyperram_conf, NULL);
  if (hyper == NULL) return -1;

  /* Allocate Hyper RAM buffer with BUFF_SIZE */
  hyper_buff[0] = rt_hyperram_alloc(hyper, BUFF_SIZE);
  if (hyper_buff[0] == NULL) return -1;

  hyper_buff[1] = rt_hyperram_alloc(hyper, BUFF_SIZE);
  if (hyper_buff[1] == NULL) return -1;
  rt_freq_set(RT_FREQ_DOMAIN_FC, 200000000);

  for (int cpt=0; cpt<10; cpt++) {
      /* Start a TX transfer : write BUFF_SIZE bytes from buff[0] address to hyper_buff[0] address  */
      /* When it's finished, it will push function end_of_tx with argument 0 to a event queue */
      //hal_hyper_ram_enqueue(hyper->channel,write_buff[0], hyper_buff[0], BUFF_SIZE);
      rt_hyperram_write(hyper, write_buff[0], hyper_buff[0], BUFF_SIZE, rt_event_get(&sched, end_of_tx, (void *)0));

      /* Start a TX transfer : write BUFF_SIZE bytes from buff[1] address to hyper_buff[1] address */
      /* When it's finished, it will push function end_of_tx with argument 1 to a event queue */
      rt_hyperram_write(hyper, write_buff[1], hyper_buff[1], BUFF_SIZE, rt_event_get(&sched, end_of_tx, (void *)1));

      /* A event scheduler will always wait and execute if any event coming to the queue */
      /* In this test, we have two TX transfers which cause two RX transfers, then the RX transfers count two times. */
      /* So, when all transfers are finished, it will exit */
      while(count != 2) {
          rt_event_execute(&sched, 1);
      }
      count=0;
  }
  /* Free the hyper_buffer space */
  rt_hyperram_free(hyper, hyper_buff[0], BUFF_SIZE);
  rt_hyperram_free(hyper, hyper_buff[1], BUFF_SIZE);

  /* All transfers are finished, close the device */
  rt_hyperram_close(hyper, NULL);

  int error = 0;
  //Print read and write buffer
  for(int i=0;i<BUFF_SIZE;i++){
      if(read_buff[0][i] != write_buff[0][i]) error++;
      if(read_buff[1][i] != write_buff[1][i]) error++;
      #ifdef DEBUG
      printf("0 : Wrote: %d read: %d\n", write_buff[0][i], read_buff[0][i]);
      printf("1 : Wrote: %d read: %d\n", write_buff[1][i], read_buff[1][i]);
      #endif
  }
  printf("NUmber of errors : %d\n", error) ;

  if (!error)
      printf("Test success\n");

  return error;
}
