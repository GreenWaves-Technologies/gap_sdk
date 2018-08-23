/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef BENCH_H
#define BENCH_H

#include "hal/pulp.h"
#include "pulp.h"
#include <stdint.h>

#if defined(TIMER_VERSION) && TIMER_VERSION == 1

static inline void start_timer() {*(volatile int*) START_TIME_ADDR = 1;}

static inline void stop_timer() {*(volatile int*) STOP_TIME_ADDR = 1;}

static inline void reset_timer() {*(volatile int*) RESET_TIME_ADDR = 1;}

static inline int get_time() {return *(volatile int*) GET_TIME_LO_ADDR;}

static inline int get_time_hi() {return *(volatile int*) GET_TIME_HI_ADDR;}

#else

#if !defined(ARCHI_HAS_FC) || defined(ARCHI_HAS_FC_ALIAS)

static inline void start_timer() {plp_timer_conf_high(1, 0, 0, 0, 0, 0, 0, 0);}

static inline void stop_timer() {plp_timer_conf_high(0, 0, 0, 0, 0, 0, 0, 0);}

static inline void reset_timer() {plp_timer_raw_conf_high(plp_timer_raw_conf_high_get() | (1<<PLP_TIMER_RESET_BIT));}

static inline int get_time() {return plp_timer_get_count_high();}
#else

static inline void start_timer() {
  if (hal_is_fc()) plp_fc_timer_conf_high(1, 0, 0, 0, 0, 0, 0, 0);
  else plp_timer_conf_high(1, 0, 0, 0, 0, 0, 0, 0);
}

static inline void stop_timer() {
  if (hal_is_fc()) plp_fc_timer_conf_high(0, 0, 0, 0, 0, 0, 0, 0);
  else plp_timer_conf_high(0, 0, 0, 0, 0, 0, 0, 0);
}

static inline void reset_timer() {
  if (hal_is_fc()) plp_fc_timer_raw_conf_high(plp_fc_timer_raw_conf_high_get() | (1<<PLP_TIMER_RESET_BIT));
  else plp_timer_raw_conf_high(plp_timer_raw_conf_high_get() | (1<<PLP_TIMER_RESET_BIT));
}

static inline int get_time() {
  if (hal_is_fc()) return plp_fc_timer_get_count_high();
  else return plp_timer_get_count_high();
}

#endif

#endif

typedef struct _testresult_t {
  int time;
  int errors;
} testresult_t;

typedef struct _testcase_t {
  char *name;
  void (*test)(testresult_t* result, void (*start)(), void (*stop)());
} testcase_t;

/**
 * @brief Disables the printf ouput of the function print_summary() and
 * run_suite(). These functions are required to run the bench suite and write
 * the result status. Disabling the output of these functions makes it easier
 * for the testing a chip on the tester setup.
 */
void bench_disable_printf(void);

/**
 * @brief Prints the information and the result (success/fail, time elapsed)
 * of a benchmark.
 * @param[in] test the test.
 * @param[in] result the result of the test.
 */
void print_result(testcase_t *test, testresult_t *result);

void print_summary(unsigned int errors);

/**
 * @brief Runs a single benchmark.
 * @param[in] test the benchmark to run.
 * @param[out] result the result of the benchmark.
 */
void run_benchmark(testcase_t *test, testresult_t *result);


/**
 * @brief Runs a series of benchmarks and prints the results.
 * @param[in] tests an array with the benchmarks to run.
 */
int run_suite(testcase_t *tests);

/**
 * @brief Checks if actual == expected and if not, prints fail_msg and
 * increases the error counter in the testresult struct.
 * This function should be used when a fine grained error reporting is needed
 */
void check_uint32(testresult_t* result, const char* fail_msg, uint32_t actual, uint32_t expected);

/**
 * @brief Starts all performance counters
 */
static inline void perf_start(void) {
#ifdef CSR_PCER_ALL_EVENTS_MASK
#ifdef __riscv__
  cpu_perf_conf_events(CSR_PCER_ALL_EVENTS_MASK);
  cpu_perf_conf(CSR_PCMR_ACTIVE | CSR_PCMR_SATURATE);
#else
  cpu_perf_conf_events(SPR_PCER_ALL_EVENTS_MASK);
  cpu_perf_conf(SPR_PCMR_ACTIVE | SPR_PCMR_SATURATE);
#endif
#endif
  // TODO this is failing on most targets, please include that also for specific ones
#if 0
  start_all_icache_stat_regs();
#endif
}

/**
 * @brief Stops all performance counters
 */
static inline void perf_stop(void) {
#ifdef CSR_PCER_ALL_EVENTS_MASK
  cpu_perf_conf(0);
#endif
  // TODO this is failing on most targets, please include that also for specific ones
#if 0
  //stop_all_icache_stat_regs();
#endif
}

/**
 * @brief Resets all performance counters to 0 without stopping them
 */
static inline void perf_reset(void) {
#ifdef CSR_PCER_ALL_EVENTS_MASK
  cpu_perf_setall(0);
#endif
  // TODO this is failing on most targets, please include that also for specific ones
#if 0
  //reset_all_icache_stat_regs();
#endif
}

/**
 * @brief Enable a specific performance counter
 */
static inline void perf_enable_id( int eventid){
#ifdef CSR_PCER_ALL_EVENTS_MASK
#ifdef __riscv__
  cpu_perf_conf_events(CSR_PCER_EVENT_MASK(eventid));
  cpu_perf_conf(CSR_PCMR_ACTIVE | CSR_PCMR_SATURATE);
#else
  cpu_perf_conf_events(SPR_PCER_EVENT_MASK(eventid));
  cpu_perf_conf(SPR_PCMR_ACTIVE | SPR_PCMR_SATURATE);
#endif
#endif
};


/**
 * @brief Prints all performance counters
 */
void perf_print_all(void);

static inline void plp_power_init() {
#if PULP_CHIP == CHIP_PULP4
  set_pin_function(PIN_CAM_I2S_SDI1+2, FUNC_GPIO);
  set_gpio_pin_direction(PIN_CAM_I2S_SDI1+1, DIR_OUT);
  set_gpio_pin_value(PIN_CAM_I2S_SDI1+1, 0);
#endif
}

static inline void plp_power_start() {
#if PULP_CHIP == CHIP_PULP4
	set_gpio_pin_value(PIN_CAM_I2S_SDI1+1, 1);
#endif
}

static inline void plp_power_stop() {
#if PULP_CHIP == CHIP_PULP4
	set_gpio_pin_value(PIN_CAM_I2S_SDI1+1, 0);
#endif
}

#endif
