/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
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
#include "mbed.h"
#include "greentea-client/test_env.h"
#include "utest/utest.h"
#include "unity/unity.h"

using utest::v1::Case;

#if defined(MBED_RTOS_SINGLE_THREAD)
#error [NOT_SUPPORTED] test not supported
#endif

#if !DEVICE_USTICKER
#error [NOT_SUPPORTED] test not supported
#endif

#define TEST_STACK_SIZE   512
#define MAX_FLAG_POS 30

#define ALL_SIGNALS       0x7fffffff
#define NO_SIGNALS        0x0
#define PROHIBITED_SIGNAL 0x80000000
#define SIGNAL1 0x1
#define SIGNAL2 0x2
#define SIGNAL3 0x4

struct Sync {
    Sync(Semaphore &parent, Semaphore &child): sem_parent(parent), sem_child(child)
    {}

    Semaphore &sem_parent;
    Semaphore &sem_child;
};

template <int32_t signals, uint32_t timeout, int32_t test_val>
void run_signal_wait(void)
{
    osEvent ev = Thread::signal_wait(signals, timeout);
    TEST_ASSERT_EQUAL(test_val, ev.status);
}

template <int32_t signals, uint32_t timeout, int32_t test_val>
void run_release_signal_wait(Semaphore *sem)
{
    sem->release();
    osEvent ev = Thread::signal_wait(signals, timeout);
    TEST_ASSERT_EQUAL(test_val, ev.status);
}

template <int32_t signals, uint32_t timeout, int32_t test_val>
void run_release_wait_signal_wait(Sync *sync)
{
    sync->sem_parent.release();
    sync->sem_child.wait();
    osEvent ev = Thread::signal_wait(signals, timeout);
    TEST_ASSERT_EQUAL(test_val, ev.status);
}

template <int32_t signals, int32_t test_val>
void run_clear(void)
{
    int32_t ret = Thread::signal_clr(signals);
    TEST_ASSERT_EQUAL(test_val, ret);
}

template <int32_t signals, int32_t test_val>
void run_wait_clear(Sync *sync)
{
    sync->sem_parent.release();
    sync->sem_child.wait();
    int32_t ret = ThisThread::flags_clear(signals);
    TEST_ASSERT_EQUAL(test_val, ret);
}

template <int32_t signals1, int32_t signals2, int32_t test_val1, int32_t test_val2>
void run_double_wait_clear(Sync *sync)
{
    int32_t ret;

    sync->sem_parent.release();
    sync->sem_child.wait();
    ret = ThisThread::flags_clear(signals1);
    TEST_ASSERT_EQUAL(test_val1, ret);

    ret = ThisThread::flags_clear(signals2);
    TEST_ASSERT_EQUAL(test_val2, ret);
}

void run_loop_wait_clear(Sync *sync)
{
    int32_t signals = NO_SIGNALS;
    for (int i = 0; i <= MAX_FLAG_POS; i++) {
        int32_t signal = 1 << i;
        signals |= signal;
        sync->sem_child.wait();
        int32_t ret = ThisThread::flags_clear(NO_SIGNALS);
        TEST_ASSERT_EQUAL(signals, ret);
        sync->sem_parent.release();
    }
}


/** Validate that call signal_clr(NO_SIGNALS) doesn't change thread signals and return actual signals

    Given two threads A & B are started, B with all signals already set
    When thread B calls @a signal_clr(NO_SIGNALS)
    Then thread B @a signal_clr status should be ALL_SIGNALS indicating that thread B state is unchanged
 */
void test_clear_no_signals(void)
{
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_double_wait_clear<NO_SIGNALS, NO_SIGNALS, ALL_SIGNALS, ALL_SIGNALS>, &sync));
    sem_parent.wait();
    t.signal_set(ALL_SIGNALS);
    sem_child.release();
    t.join();
}

/** Validate if any signals are set on just created thread

    Given the thread is running
    When thread execute @a signal_clr(NO_SIGNALS)
    Then thread @a signal_clr return status should be NO_SIGNALS(0) indicating no signals set
 */
void test_init_state(void)
{
    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_clear<NO_SIGNALS, NO_SIGNALS>));
    t.join();
}

/** Validate all signals set in one shot

    Given two threads A & B are started
    When thread A call @a signal_set(ALL_SIGNALS) with all possible signals
    Then thread B @a signal_clr(NO_SIGNALS) status should be ALL_SIGNALS indicating all signals set correctly
 */
void test_set_all(void)
{
    int32_t ret;
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_wait_clear<NO_SIGNALS, ALL_SIGNALS>, &sync));

    sem_parent.wait();
    ret = t.signal_set(ALL_SIGNALS);
    TEST_ASSERT_EQUAL(ALL_SIGNALS, ret);

    sem_child.release();
    t.join();
}

/** Validate that call signal_set with prohibited signal doesn't change thread signals

    Given two threads A & B are started, B with all signals set
    When thread A executes @a signal_set(PROHIBITED_SIGNAL) with prohibited signal
    Then thread B @a signal_clr(NO_SIGNALS) status should be ALL_SIGNALS indicating that thread B signals are unchanged

    @note Each signal has up to 31 event flags 0x1, 0x2, 0x4, 0x8, ...,  0x40000000
          Most significant bit is reserved and thereby flag 0x80000000 is prohibited
 */
void test_set_prohibited(void)
{
    int32_t ret;
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_wait_clear<NO_SIGNALS, ALL_SIGNALS>, &sync));

    sem_parent.wait();
    t.signal_set(ALL_SIGNALS);

#if !MBED_TRAP_ERRORS_ENABLED
    ret = t.signal_set(PROHIBITED_SIGNAL);
    TEST_ASSERT_EQUAL(osErrorParameter, ret);
#endif

    sem_child.release();
    t.join();
}

/** Validate all signals clear in one shot

    Given two threads A & B are started, B with all signals set
    When thread B execute @a signal_clr(ALL_SIGNALS) with all possible signals
    Then thread B @a signal_clr(NO_SIGNALS) status should be NO_SIGNALS(0) indicating all signals cleared correctly
 */
void test_clear_all(void)
{
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_double_wait_clear<ALL_SIGNALS, NO_SIGNALS, ALL_SIGNALS, NO_SIGNALS>, &sync));
    sem_parent.wait();
    t.signal_set(ALL_SIGNALS);
    sem_child.release();
    t.join();
}

/** Validate all signals set one by one in loop

    Given two threads A & B are started
    When thread A executes @a signal_set(signal) in loop with all possible signals
 */
void test_set_all_loop(void)
{
    int32_t ret;
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_loop_wait_clear, &sync));

    int32_t signals = 0;
    for (int i = 0; i <= MAX_FLAG_POS; i++) {
        int32_t signal = 1 << i;

        ret = t.signal_set(signal);
        signals |= signal;
        TEST_ASSERT_EQUAL(signals, ret);
        sem_child.release();
        sem_parent.wait();
    }
    t.join();
}

/** Validate signal_wait return status if timeout specified

    Given the thread is running
    When thread executes @a signal_wait(signals, timeout) with specified signals and timeout
    Then thread @a signal_wait status should be osEventTimeout indicating a timeout
         thread @a signal_wait status should be osOK indicating 0[ms] timeout set
 */
template <int32_t signals, uint32_t timeout, int32_t status>
void test_wait_timeout(void)
{
    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_signal_wait<signals, timeout, status>));
    t.join();
}

/** Validate that call of signal_wait return correctly when thread has all signals already set

    Given two threads A & B are started, B with all signals already set
    When thread B executes @a signal_wait(ALL_SIGNALS, osWaitForever),
    Then thread B @a signal_wait return immediately with status osEventSignal indicating all wait signals was already set
 */
void test_wait_all_already_set(void)
{
    Semaphore sem_parent(0, 1);
    Semaphore sem_child(0, 1);
    Sync sync(sem_parent, sem_child);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_release_wait_signal_wait<ALL_SIGNALS, osWaitForever, osEventSignal>, &sync));

    sem_parent.wait();
    TEST_ASSERT_EQUAL(Thread::WaitingSemaphore, t.get_state());
    t.signal_set(ALL_SIGNALS);
    sem_child.release();
    t.join();
}

/** Validate if signal_wait return correctly when all signals set

    Given two threads A & B are started and B waiting for a thread flag to be set
    When thread A executes @a signal_set(ALL_SIGNALS) with all possible signals
    Then thread B @a signal_wait status is osEventSignal indicating all wait signals was set
 */
void test_wait_all(void)
{
    Semaphore sem(0, 1);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_release_signal_wait<ALL_SIGNALS, osWaitForever, osEventSignal>, &sem));

    sem.wait();
    TEST_ASSERT_EQUAL(Thread::WaitingThreadFlag, t.get_state());

    t.signal_set(ALL_SIGNALS);
    t.join();
}

/** Validate if signal_wait accumulate signals and return correctly when all signals set

    Given two threads A & B are started and B waiting for a thread signals to be set
    When thread A executes @a signal_set setting all signals in loop
    Then thread B @a signal_wait status is osEventSignal indicating that all wait signals was set
 */
void test_wait_all_loop(void)
{
    int32_t ret;
    Semaphore sem(0, 1);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_release_signal_wait<ALL_SIGNALS, osWaitForever, osEventSignal>, &sem));

    sem.wait();
    TEST_ASSERT_EQUAL(Thread::WaitingThreadFlag, t.get_state());

    for (int i = 0; i < MAX_FLAG_POS; i++) {
        int32_t signal = 1 << i;
        ret = t.signal_set(signal);
    }
    ret = t.signal_set(1 << MAX_FLAG_POS);
    TEST_ASSERT_EQUAL(NO_SIGNALS, ret);
    t.join();
}

/** Validate if setting same signal twice cause any unwanted behaviour

    Given two threads A & B are started and B waiting for a thread signals to be set
    When thread A executes @a signal_set twice for the same signal
    Then thread A @a signal_set status is current signal set
         thread B @a signal_wait status is osEventSignal indicating that all wait signals was set
 */
void test_set_double(void)
{
    int32_t ret;
    Semaphore sem(0, 1);

    Thread t(osPriorityNormal, TEST_STACK_SIZE);
    t.start(callback(run_release_signal_wait < SIGNAL1 | SIGNAL2 | SIGNAL3, osWaitForever, osEventSignal >, &sem));

    sem.wait();
    TEST_ASSERT_EQUAL(Thread::WaitingThreadFlag, t.get_state());

    ret = t.signal_set(SIGNAL1);
    TEST_ASSERT_EQUAL(SIGNAL1, ret);

    ret = t.signal_set(SIGNAL2);
    TEST_ASSERT_EQUAL(SIGNAL1 | SIGNAL2, ret);

    ret = t.signal_set(SIGNAL2);
    TEST_ASSERT_EQUAL(SIGNAL1 | SIGNAL2, ret);
    TEST_ASSERT_EQUAL(Thread::WaitingThreadFlag, t.get_state());

    ret = t.signal_set(SIGNAL3);
    TEST_ASSERT_EQUAL(NO_SIGNALS, ret);
    t.join();
}


utest::v1::status_t test_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(10, "default_auto");
    return utest::v1::verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("Validate that call signal_clr(NO_SIGNALS) doesn't change thread signals and return actual signals", test_clear_no_signals),
    Case("Validate if any signals are set on just created thread", test_init_state),
    Case("Validate all signals set in one shot", test_set_all),
    Case("Validate that call signal_set with prohibited signal doesn't change thread signals", test_set_prohibited),
    Case("Validate all signals clear in one shot", test_clear_all),
    Case("Validate all signals set one by one in loop", test_set_all_loop),
    Case("Validate signal_wait return status if timeout specified: 0[ms] no signals", test_wait_timeout<0, 0, osOK>),
    Case("Validate signal_wait return status if timeout specified: 0[ms] all signals", test_wait_timeout<ALL_SIGNALS, 0, osOK>),
    Case("Validate signal_wait return status if timeout specified: 1[ms] no signals", test_wait_timeout<0, 1, osEventTimeout>),
    Case("Validate signal_wait return status if timeout specified: 1[ms] all signals", test_wait_timeout<ALL_SIGNALS, 1, osEventTimeout>),
    Case("Validate that call of signal_wait return correctly when thread has all signals already set", test_wait_all_already_set),
    Case("Validate if signal_wait return correctly when all signals set", test_wait_all),
    Case("Validate if signal_wait accumulate signals and return correctly when all signals set", test_wait_all_loop),
    Case("Validate if setting same signal twice cause any unwanted behaviour", test_set_double)
};

utest::v1::Specification specification(test_setup, cases);

int main()
{
    return !utest::v1::Harness::run(specification);
}
