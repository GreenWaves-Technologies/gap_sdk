/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GAP_BENCH_H_
#define _GAP_BENCH_H_

#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

typedef struct _testresult_t {
    int time;
    int errors;
} testresult_t;

typedef struct _testcase_t {
    char *name;
    void (*test)(testresult_t* result, void (*start)(), void (*stop)());
} testcase_t;

// Flag for disabling the printf output
uint32_t enable_printf = 1;
static uint32_t run_suite_error = 1;

void bench_disable_printf(void) {
  enable_printf = 0;
}

void bench_timer_start(void) {
}

void bench_timer_stop(void) {
}

void bench_timer_reset(void) {
}

void print_result(testcase_t *test, testresult_t *result)
{
  printf("== test: %s -> ", test->name);

  if (result->errors == 0)
    printf("success, ");
  else
    printf("fail, ");

  printf("nr. of errors: %d", result->errors);

  if(result->time == 0)
    printf("\n");
  else
    printf(", execution time: %d\n", result->time);
}

void print_summary(unsigned int errors)
{
  if(enable_printf) {
    printf("==== SUMMARY: ");

    if (errors == 0) {
      printf("SUCCESS\n");
    } else {
      printf("FAIL\n");
    }
  }
}

void run_benchmark(testcase_t *test, testresult_t *result)
{
  result->errors = 0;

  bench_timer_reset();

  test->test(result, bench_timer_start, bench_timer_stop);

  result->time = 0;
}

int run_suite(testcase_t *tests)
{
  // figure out how many tests should be run
  int num = 0;
  while(tests[num].name != 0) num++;

  run_suite_error = 0;
  int i;

  // perform the tests
  for (i = 0; i < num; i++) {
    testresult_t result;
    run_benchmark(&tests[i], &result);
    if(enable_printf) print_result(&tests[i], &result);

    run_suite_error += result.errors;
  }

  print_summary(run_suite_error);

  return run_suite_error;
}

void check_uint32(testresult_t* result, const char* fail_msg, uint32_t actual, uint32_t expected)
{
  if(actual != expected) {
    result->errors += 1;
    if(enable_printf) printf("%s: Actual %X, expected %X\n", fail_msg, (unsigned int) actual, (unsigned int)expected);
  }
}

void test_suite_entry(void *arg) {
    CLUSTER_CoresFork((void *)run_suite, arg);
}

void bench_cluster_forward(testcase_t *testcases) {

    /* Cluster Start - Power on, 9th core seperation disable */
    CLUSTER_Start(0, CLUSTER_CORES_NUM, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, test_suite_entry, (void*)testcases, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);
}

#endif /*_GAP_BENCH_H_*/
