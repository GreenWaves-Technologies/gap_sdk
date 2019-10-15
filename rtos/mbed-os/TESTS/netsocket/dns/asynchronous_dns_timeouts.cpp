/*
 * Copyright (c) 2018, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "dns_tests.h"

#include "nsapi_dns.h"

using namespace utest::v1;

namespace {
int result_ok;
int result_no_mem;
int result_dns_failure;
int result_exp_timeout;

const int EXTERNAL_THREAD_SIZE = 2048;
const int EVENT_QUEUE_SIZE = 10;

events::EventQueue *event_queue;
}

static nsapi_error_t event_queue_call(int delay, mbed::Callback<void()> func)
{
    // Run event queue without delays
    if (event_queue->call(func) == 0) {
        return NSAPI_ERROR_NO_MEMORY;
    }

    return NSAPI_ERROR_OK;
}

void ASYNCHRONOUS_DNS_TIMEOUTS()
{
    // Ensures that cache does not contain entries
    do_asynchronous_gethostbyname(dns_test_hosts, MBED_CONF_NSAPI_DNS_CACHE_SIZE, &result_ok, &result_no_mem,
                                  &result_dns_failure, &result_exp_timeout);

    TEST_ASSERT_EQUAL(MBED_CONF_NSAPI_DNS_CACHE_SIZE, result_ok);
    TEST_ASSERT_EQUAL(0, result_no_mem);
    TEST_ASSERT_EQUAL(0, result_dns_failure);
    TEST_ASSERT_EQUAL(0, result_exp_timeout);

    // Dispatch event queue
    Thread eventThread(osPriorityNormal, EXTERNAL_THREAD_SIZE);
    EventQueue queue(EVENT_QUEUE_SIZE * EVENTS_EVENT_SIZE);
    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));
    event_queue = &queue;

    // Set event queue callback to test specific one
    nsapi_dns_call_in_set(event_queue_call);

    do_asynchronous_gethostbyname(dns_test_hosts_second, MBED_CONF_APP_DNS_SIMULT_QUERIES + 1, &result_ok, &result_no_mem,
                                  &result_dns_failure, &result_exp_timeout);

    // Depends on timing, but at least one operation shall fail to timeout
    TEST_ASSERT(result_exp_timeout > 0);

    // Give event queue time to finalise before destructors
    wait(2.0);

    nsapi_dns_call_in_set(0);
}
