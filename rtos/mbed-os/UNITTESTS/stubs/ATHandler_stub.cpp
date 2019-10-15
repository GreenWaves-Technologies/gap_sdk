/*
 * Copyright (c) , Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
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

#include <ctype.h>
#include "nsapi_types.h"
#include "ATHandler.h"
#include "events/EventQueue.h"
#include "ATHandler_stub.h"

using namespace mbed;
using namespace events;

#include "CellularLog.h"

const int DEFAULT_AT_TIMEOUT = 1000; // at default timeout in milliseconds

nsapi_error_t ATHandler_stub::nsapi_error_value = 0;
uint8_t ATHandler_stub::nsapi_error_ok_counter = 0;
int ATHandler_stub::int_value = -1;
int ATHandler_stub::ref_count = 0;
int ATHandler_stub::timeout = 0;
bool ATHandler_stub::default_timeout = 0;
bool ATHandler_stub::debug_on = 0;
ssize_t ATHandler_stub::ssize_value = 0;
const char *ATHandler_stub::read_string_value = NULL;
size_t ATHandler_stub::size_value = 0;
size_t ATHandler_stub::return_given_size = false;
bool ATHandler_stub::bool_value = false;
uint8_t ATHandler_stub::uint8_value = 0;
FileHandle_stub *ATHandler_stub::fh_value = NULL;
device_err_t ATHandler_stub::device_err_value;
bool ATHandler_stub::call_immediately = false;
uint8_t ATHandler_stub::resp_info_true_counter = false;
uint8_t ATHandler_stub::resp_info_true_counter2 = false;
uint8_t ATHandler_stub::resp_info_false_counter = false;
uint8_t ATHandler_stub::info_elem_true_counter = false;
int ATHandler_stub::int_valid_count_table[kRead_int_table_size];
int ATHandler_stub::int_count = kRead_int_table_size;
bool ATHandler_stub::process_oob_urc = false;

int ATHandler_stub::read_string_index = kRead_string_table_size;
const char *ATHandler_stub::read_string_table[kRead_string_table_size] = {'\0'};
int ATHandler_stub::resp_stop_success_count = kResp_stop_count_default;
int ATHandler_stub::urc_amount = 0;
mbed::Callback<void()> ATHandler_stub::callback[kATHandler_urc_table_max_size];
char *ATHandler_stub::urc_string_table[kATHandler_urc_table_max_size] = {NULL};

bool ATHandler_stub::get_debug_flag = false;
uint8_t ATHandler_stub::set_debug_call_count = 0;

bool ATHandler_stub::is_get_debug_run()
{
    return ATHandler_stub::get_debug_flag;
}

void ATHandler_stub::get_debug_clear()
{
    ATHandler_stub::get_debug_flag = false;
}

uint8_t ATHandler_stub::set_debug_call_count_get()
{
    return ATHandler_stub::set_debug_call_count;
}

void ATHandler_stub::debug_call_count_clear()
{
    ATHandler_stub::set_debug_call_count = 0;
}

ATHandler::ATHandler(FileHandle *fh, EventQueue &queue, uint32_t timeout, const char *output_delimiter, uint16_t send_delay) :
    _nextATHandler(0),
    _fileHandle(fh),
    _queue(queue),
    _ref_count(1)
{
    ATHandler_stub::ref_count = 1;

    ATHandler_stub::process_oob_urc = false;
    ATHandler_stub::urc_amount = 0;
    int i = 0;
    while (i < kATHandler_urc_table_max_size) {
        ATHandler_stub::callback[i] = NULL;
        ATHandler_stub::urc_string_table[i++] = NULL;
    }
}

void ATHandler::set_debug(bool debug_on)
{
    ++ATHandler_stub::set_debug_call_count;
    ATHandler_stub::debug_on = debug_on;
}

bool ATHandler::get_debug() const
{
    ATHandler_stub::get_debug_flag = true;

    return ATHandler_stub::debug_on;
}

ATHandler::~ATHandler()
{
    ATHandler_stub::ref_count = kATHandler_destructor_ref_ount;

    int i = 0;
    while (i < kATHandler_urc_table_max_size) {
        if (ATHandler_stub::urc_string_table[i]) {
            delete [] ATHandler_stub::urc_string_table[i];
            i++;
        } else {
            break;
        }
    }
}

void ATHandler::inc_ref_count()
{
    _ref_count++;
    ATHandler_stub::ref_count = _ref_count;
}

void ATHandler::dec_ref_count()
{
    _ref_count--;
    ATHandler_stub::ref_count = _ref_count;
}

int ATHandler::get_ref_count()
{
    return _ref_count;
}

FileHandle *ATHandler::get_file_handle()
{
    ATHandler_stub::fh_value = (FileHandle_stub *)_fileHandle;
    return _fileHandle;
}

void ATHandler::set_file_handle(FileHandle *fh)
{
}

void ATHandler::set_urc_handler(const char *urc, mbed::Callback<void()> cb)
{
    if (ATHandler_stub::urc_amount < kATHandler_urc_table_max_size) {
        ATHandler_stub::callback[ATHandler_stub::urc_amount] = cb;
        if (urc) {
            ATHandler_stub::urc_string_table[ATHandler_stub::urc_amount] = new char[kATHandler_urc_string_max_size];
            memset(ATHandler_stub::urc_string_table[ATHandler_stub::urc_amount], 0, sizeof(ATHandler_stub::urc_string_table[ATHandler_stub::urc_amount]));
            int bytes_to_copy = strlen(urc) < kATHandler_urc_string_max_size ? strlen(urc) : kATHandler_urc_string_max_size;
            memcpy(ATHandler_stub::urc_string_table[ATHandler_stub::urc_amount], urc, bytes_to_copy);
        }
        ATHandler_stub::urc_amount++;
    } else {
        ATHandler_stub::callback[0] = cb;
        MBED_ASSERT("ATHandler URC amount limit reached");
    }
    if (ATHandler_stub::call_immediately) {
        cb();
    }
}

void ATHandler::remove_urc_handler(const char *prefix)
{
}

nsapi_error_t ATHandler::get_last_error() const
{
    if (ATHandler_stub::nsapi_error_ok_counter) {
        ATHandler_stub::nsapi_error_ok_counter--;
        return NSAPI_ERROR_OK;
    }
    return ATHandler_stub::nsapi_error_value;
}

void ATHandler::lock()
{
}

void ATHandler::unlock()
{
}

nsapi_error_t ATHandler::unlock_return_error()
{
    return ATHandler_stub::nsapi_error_value;
}

void ATHandler::set_at_timeout(uint32_t timeout_milliseconds, bool default_timeout)
{
    ATHandler_stub::timeout = timeout_milliseconds;
    ATHandler_stub::default_timeout = default_timeout;
}

void ATHandler::restore_at_timeout()
{
}

void ATHandler::process_oob()
{
    if (ATHandler_stub::process_oob_urc) {
        int i = 0;
        while (i < ATHandler_stub::urc_amount) {
            if (ATHandler_stub::read_string_index >= 0) {
                int len = 0;
                if (ATHandler_stub::urc_string_table[i]) {
                    len = strlen(ATHandler_stub::urc_string_table[i]);
                }
                if (!memcmp(ATHandler_stub::urc_string_table[i],
                            ATHandler_stub::read_string_table[ATHandler_stub::read_string_index],
                            len)) {
                    ATHandler_stub::callback[i]();
                    break;
                }
            }
            i++;
        }
    }
}

void ATHandler::clear_error()
{
    ATHandler_stub::nsapi_error_ok_counter++;
}

void ATHandler::skip_param(uint32_t count)
{

}

void ATHandler::skip_param(ssize_t len, uint32_t count)
{
}

ssize_t ATHandler::read_bytes(uint8_t *buf, size_t len)
{
    return ATHandler_stub::ssize_value;
}

ssize_t ATHandler::read_string(char *buf, size_t size, bool read_even_stop_tag)
{

    if (ATHandler_stub::read_string_index == kRead_string_table_size) {
        if (ATHandler_stub::read_string_value && ATHandler_stub::ssize_value >= 0) {
            memcpy(buf, ATHandler_stub::read_string_value, ATHandler_stub::ssize_value + 1);
            buf[ATHandler_stub::ssize_value] = '\0';
        }
        return ATHandler_stub::ssize_value;
    }

    ATHandler_stub::read_string_index--;
    if (ATHandler_stub::read_string_index >= 0) {
        const char *tmp = ATHandler_stub::read_string_table[ATHandler_stub::read_string_index];
        ssize_t len = strlen(tmp);
        memcpy(buf, tmp, len + 1);
        buf[len] = '\0';
        return len;
    }

    ATHandler_stub::nsapi_error_value = NSAPI_ERROR_DEVICE_ERROR;
    return -1;
}

int32_t ATHandler::read_int()
{
    if (ATHandler_stub::nsapi_error_value != NSAPI_ERROR_OK) {
        return -1;
    }

    if (ATHandler_stub::int_count == kRead_int_table_size) {
        return ATHandler_stub::int_value;
    }

    //printf("ATHandler_stub::int_count: %d", ATHandler_stub::int_count);
    ATHandler_stub::int_count--;
    if (ATHandler_stub::int_count < kRead_int_table_size && ATHandler_stub::int_count >= 0) {
        return ATHandler_stub::int_valid_count_table[ATHandler_stub::int_count];
    }

    ATHandler_stub::nsapi_error_value = NSAPI_ERROR_DEVICE_ERROR;
    return -1;
}

void ATHandler::set_delimiter(char delimiter)
{
}

void ATHandler::set_default_delimiter()
{
}

void ATHandler::set_stop_tag(const char *stop_tag_seq)
{
}

int ATHandler::get_3gpp_error()
{
    return ATHandler_stub::int_value;
}

void ATHandler::resp_start(const char *prefix, bool stop)
{
}

bool ATHandler::info_resp()
{
    //3 counter variables available here now so that in a test
    //case it is possible to have at least two while loops checking
    //specified amount of info_resps.
    //
    //For example:
    //while(athandler.info_resp())
    //{
    //   resp_info_true_counter responses handled in this loop
    //}
    //   resp_info_false_counter set to 1 to break out from the 1st loop
    //while(athandler.info_resp())
    //{
    //   resp_info_true_counter2 responses handled in this loop
    //}
    if (ATHandler_stub::resp_info_true_counter) {
        ATHandler_stub::resp_info_true_counter--;
        return true;
    }
    if (ATHandler_stub::resp_info_false_counter) {
        ATHandler_stub::resp_info_false_counter--;
        return false;
    }
    if (ATHandler_stub::resp_info_true_counter2) {
        ATHandler_stub::resp_info_true_counter2--;
        return true;
    }
    return ATHandler_stub::bool_value;
}

bool ATHandler::info_elem(char start_tag)
{
    if (ATHandler_stub::info_elem_true_counter) {
        ATHandler_stub::info_elem_true_counter--;
        return true;
    }
    return ATHandler_stub::bool_value;
}

bool ATHandler::consume_to_stop_tag()
{
    return ATHandler_stub::bool_value;
}

void ATHandler::resp_stop()
{
    if (ATHandler_stub::resp_stop_success_count > 0) {
        ATHandler_stub::resp_stop_success_count--;
        return;
    }
    ATHandler_stub::nsapi_error_value = NSAPI_ERROR_DEVICE_ERROR;
}

void ATHandler::cmd_start(const char *cmd)
{
}

void ATHandler::write_int(int32_t param)
{
}

void ATHandler::write_string(const char *param, bool useQuotations)
{
}

size_t ATHandler::write_bytes(const uint8_t *param, size_t len)
{
    if (ATHandler_stub::return_given_size) {
        return len;
    }
    return ATHandler_stub::size_value;
}

void ATHandler::cmd_stop()
{
}

void ATHandler::cmd_stop_read_resp()
{
    cmd_stop();
    resp_start();
    resp_stop();
}

device_err_t ATHandler::get_last_device_error() const
{
    return ATHandler_stub::device_err_value;
}

void ATHandler::flush()
{

}
