/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define STACK_SIZE 2048

static volatile int end = 0;
static rt_event_sched_t sched;

static void event_callback(void *arg) {
  printf("Entered event entry with arg 0x%x\n", (int)arg);
  if ((int)arg == -1) end = 1;
}

static void *thread_entry(void *arg) {
  printf("Entered thread entry with arg 0x%x\n", (int)arg);

  for (int i=0; i<10; i++) {
    printf("Thread iter\n");
    // Half the time push an event
    if (i & 1) rt_event_push_callback(&sched, event_callback, (void *)i);

    printf("Thread iter\n");
    // Yield this thread to allow the main thread to run
    rt_thread_yield();
  }

  // Now push an event that will cause the main thread event loop to exit
  rt_event_push_callback(&sched, event_callback, (void *)-1);

  return (void *)0x87654321;
}

int main()
{
  printf("Entering main controller\n");

  rt_thread_t thread;

  // Create a stack for the new thread
  int stack = (int)rt_alloc(RT_ALLOC_FC_DATA, STACK_SIZE);
  if (stack == 0) return -1;

  // Initialize a new event scheduler
  rt_event_sched_init(&sched);
  if (rt_event_alloc(&sched, 4)) return -1;

  // Create a new thread
  if (rt_thread_create(&thread, thread_entry, (void *)0x12345678, stack, STACK_SIZE)) return -1;

  // Execute events until we end
  while(!end) {
    rt_event_execute(&sched, 1);
  }

  // Join back with the spawned thread
  void *status = rt_thread_join(&thread);
  printf("Test success: Thread has finished with value 0x%x\n", (int)status);

  return 0;
}
