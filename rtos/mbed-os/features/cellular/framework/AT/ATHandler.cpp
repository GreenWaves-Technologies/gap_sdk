/*
 * Copyright (c) 2017, Arm Limited and affiliates.
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
#include <stdio.h>
#include <limits.h>
#include "ATHandler.h"
#include "mbed_poll.h"
#include "FileHandle.h"
#include "mbed_wait_api.h"
#include "mbed_debug.h"
#include "rtos/ThisThread.h"
#include "Kernel.h"
#include "CellularUtil.h"

using namespace mbed;
using namespace events;
using namespace mbed_cellular_util;

#include "CellularLog.h"

// URCs should be handled fast, if you add debug traces within URC processing then you also need to increase this time
#define PROCESS_URC_TIME 20

// Suppress logging of very big packet payloads, maxlen is approximate due to write/read are cached
#define DEBUG_MAXLEN 60
#define DEBUG_END_MARK "..\r"

const char *mbed::OK = "OK\r\n";
const uint8_t OK_LENGTH = 4;
const char *mbed::CRLF = "\r\n";
const uint8_t CRLF_LENGTH = 2;
const char *CME_ERROR = "+CME ERROR:";
const uint8_t CME_ERROR_LENGTH = 11;
const char *CMS_ERROR = "+CMS ERROR:";
const uint8_t CMS_ERROR_LENGTH = 11;
const char *ERROR_ = "ERROR\r\n";
const uint8_t ERROR_LENGTH = 7;
const uint8_t MAX_RESP_LENGTH = CMS_ERROR_LENGTH;
const char DEFAULT_DELIMITER = ',';

static const uint8_t map_3gpp_errors[][2] =  {
    { 103, 3 },  { 106, 6 },  { 107, 7 },  { 108, 8 },  { 111, 11 }, { 112, 12 }, { 113, 13 }, { 114, 14 },
    { 115, 15 }, { 122, 22 }, { 125, 25 }, { 172, 95 }, { 173, 96 }, { 174, 97 }, { 175, 99 }, { 176, 111 },
    { 177, 8 },  { 126, 26 }, { 127, 27 }, { 128, 28 }, { 129, 29 }, { 130, 30 }, { 131, 31 }, { 132, 32 },
    { 133, 33 }, { 134, 34 }, { 140, 40 }, { 141, 41 }, { 142, 42 }, { 143, 43 }, { 144, 44 }, { 145, 45 },
    { 146, 46 }, { 178, 65 }, { 179, 66 }, { 180, 48 }, { 181, 83 }, { 171, 49 },
};

ATHandler::ATHandler(FileHandle *fh, EventQueue &queue, uint32_t timeout, const char *output_delimiter, uint16_t send_delay) :
    _nextATHandler(0),
    _fileHandle(NULL), // filehandle is set by set_file_handle()
    _queue(queue),
    _last_err(NSAPI_ERROR_OK),
    _last_3gpp_error(0),
    _oob_string_max_length(0),
    _oobs(NULL),
    _at_timeout(timeout),
    _previous_at_timeout(timeout),
    _at_send_delay(send_delay),
    _last_response_stop(0),
    _oob_queued(false),
    _ref_count(1),
    _is_fh_usable(false),
    _stop_tag(NULL),
    _delimiter(DEFAULT_DELIMITER),
    _prefix_matched(false),
    _urc_matched(false),
    _error_found(false),
    _max_resp_length(MAX_RESP_LENGTH),
    _debug_on(MBED_CONF_CELLULAR_DEBUG_AT),
    _cmd_start(false),
    _use_delimiter(true),
    _start_time(0)
{
    clear_error();

    if (output_delimiter) {
        _output_delimiter = new char[strlen(output_delimiter) + 1];
        memcpy(_output_delimiter, output_delimiter, strlen(output_delimiter) + 1);
    } else {
        _output_delimiter = NULL;
    }

    reset_buffer();
    memset(_recv_buff, 0, sizeof(_recv_buff));
    memset(_info_resp_prefix, 0, sizeof(_info_resp_prefix));

    _current_scope = NotSet;
    set_tag(&_resp_stop, OK);
    set_tag(&_info_stop, CRLF);
    set_tag(&_elem_stop, ")");

    set_file_handle(fh);
}

void ATHandler::set_debug(bool debug_on)
{
    _debug_on = debug_on;
}

bool ATHandler::get_debug() const
{
    return _debug_on;
}

ATHandler::~ATHandler()
{
    set_file_handle(NULL);

    while (_oobs) {
        struct oob_t *oob = _oobs;
        _oobs = oob->next;
        delete oob;
    }
    if (_output_delimiter) {
        delete [] _output_delimiter;
    }
}

void ATHandler::inc_ref_count()
{
    _ref_count++;
}

void ATHandler::dec_ref_count()
{
    _ref_count--;
}

int ATHandler::get_ref_count()
{
    return _ref_count;
}

FileHandle *ATHandler::get_file_handle()
{
    return _fileHandle;
}

void ATHandler::set_file_handle(FileHandle *fh)
{
    if (_fileHandle) {
        set_is_filehandle_usable(false);
    }
    _fileHandle = fh;
    if (_fileHandle) {
        set_is_filehandle_usable(true);
    }
}

void ATHandler::set_is_filehandle_usable(bool usable)
{
    if (_fileHandle) {
        if (usable) {
            _fileHandle->set_blocking(false);
            _fileHandle->sigio(Callback<void()>(this, &ATHandler::event));
        } else {
            _fileHandle->set_blocking(true); // set back to default state
            _fileHandle->sigio(NULL);
        }
        _is_fh_usable = usable;
    }
}

void ATHandler::set_urc_handler(const char *prefix, Callback<void()> callback)
{
    if (!callback) {
        remove_urc_handler(prefix);
        return;
    }

    if (find_urc_handler(prefix)) {
        tr_warn("URC already added with prefix: %s", prefix);
        return;
    }

    struct oob_t *oob = new struct oob_t;
    size_t prefix_len = strlen(prefix);
    if (prefix_len > _oob_string_max_length) {
        _oob_string_max_length = prefix_len;
        if (_oob_string_max_length > _max_resp_length) {
            _max_resp_length = _oob_string_max_length;
        }
    }

    oob->prefix = prefix;
    oob->prefix_len = prefix_len;
    oob->cb = callback;
    oob->next = _oobs;
    _oobs = oob;
}

void ATHandler::remove_urc_handler(const char *prefix)
{
    struct oob_t *current = _oobs;
    struct oob_t *prev = NULL;
    while (current) {
        if (strcmp(prefix, current->prefix) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                _oobs = current->next;
            }
            delete current;
            break;
        }
        prev = current;
        current = prev->next;
    }
}

bool ATHandler::find_urc_handler(const char *prefix)
{
    struct oob_t *oob = _oobs;
    while (oob) {
        if (strcmp(prefix, oob->prefix) == 0) {
            return true;
        }
        oob = oob->next;
    }

    return false;
}

void ATHandler::event()
{
    if (!_oob_queued) {
        _oob_queued = true;
        (void) _queue.call(Callback<void(void)>(this, &ATHandler::process_oob));
    }
}

void ATHandler::lock()
{
#ifdef AT_HANDLER_MUTEX
    _fileHandleMutex.lock();
#endif
    clear_error();
    _start_time = rtos::Kernel::get_ms_count();
}

void ATHandler::unlock()
{
#ifdef AT_HANDLER_MUTEX
    _fileHandleMutex.unlock();
#endif
    if (_fileHandle->readable() || (_recv_pos < _recv_len)) {
        (void) _queue.call(Callback<void(void)>(this, &ATHandler::process_oob));
    }
}

nsapi_error_t ATHandler::unlock_return_error()
{
    nsapi_error_t err = _last_err;
    unlock();
    return err;
}

void ATHandler::set_at_timeout(uint32_t timeout_milliseconds, bool default_timeout)
{
    lock();
    if (default_timeout) {
        _previous_at_timeout = timeout_milliseconds;
        _at_timeout = timeout_milliseconds;
    } else if (timeout_milliseconds != _at_timeout) {
        _previous_at_timeout = _at_timeout;
        _at_timeout = timeout_milliseconds;
    }
    unlock();
}

void ATHandler::restore_at_timeout()
{
    lock();
    if (_previous_at_timeout != _at_timeout) {
        _at_timeout = _previous_at_timeout;
    }
    unlock();
}

void ATHandler::process_oob()
{
    if (!_is_fh_usable) {
        tr_debug("process_oob, filehandle is not usable, return...");
        return;
    }
    lock();
    _oob_queued = false;
    if (_fileHandle->readable() || (_recv_pos < _recv_len)) {
        tr_debug("AT OoB readable %d, len %u", _fileHandle->readable(), _recv_len - _recv_pos);
        _current_scope = NotSet;
        uint32_t timeout = _at_timeout;
        while (true) {
            _at_timeout = timeout;
            if (match_urc()) {
                if (!(_fileHandle->readable() || (_recv_pos < _recv_len))) {
                    break; // we have nothing to read anymore
                }
            } else if (mem_str(_recv_buff, _recv_len, CRLF, CRLF_LENGTH)) { // If no match found, look for CRLF and consume everything up to CRLF
                _at_timeout = PROCESS_URC_TIME;
                consume_to_tag(CRLF, true);
            } else {
                _at_timeout = PROCESS_URC_TIME;
                if (!fill_buffer()) {
                    reset_buffer(); // consume anything that could not be handled
                    break;
                }
                _start_time = rtos::Kernel::get_ms_count();
            }
        }
        _at_timeout = timeout;
        tr_debug("AT OoB done");
    }
    unlock();
}

void ATHandler::reset_buffer()
{
    _recv_pos = 0;
    _recv_len = 0;
}

void ATHandler::rewind_buffer()
{
    if (_recv_pos > 0 && _recv_len >= _recv_pos) {
        _recv_len -= _recv_pos;
        // move what is not read to beginning of buffer
        memmove(_recv_buff, _recv_buff + _recv_pos, _recv_len);
        _recv_pos = 0;
    }
}

int ATHandler::poll_timeout(bool wait_for_timeout)
{
    int timeout;
    if (wait_for_timeout) {
        uint64_t now = rtos::Kernel::get_ms_count();
        if (now >= _start_time + _at_timeout) {
            timeout = 0;
        } else if (_start_time + _at_timeout - now > INT_MAX) {
            timeout = INT_MAX;
        } else {
            timeout = _start_time + _at_timeout - now;
        }
    } else {
        timeout = 0;
    }
    return timeout;
}

bool ATHandler::fill_buffer(bool wait_for_timeout)
{
    // Reset buffer when full
    if (sizeof(_recv_buff) == _recv_len) {
        tr_error("AT overflow");
        debug_print(_recv_buff, _recv_len, AT_ERR);
        reset_buffer();
    }

    pollfh fhs;
    fhs.fh = _fileHandle;
    fhs.events = POLLIN;
    int count = poll(&fhs, 1, poll_timeout(wait_for_timeout));
    if (count > 0 && (fhs.revents & POLLIN)) {
        ssize_t len = _fileHandle->read(_recv_buff + _recv_len, sizeof(_recv_buff) - _recv_len);
        if (len > 0) {
            debug_print(_recv_buff + _recv_len, len, AT_RX);
            _recv_len += len;
            return true;
        }
    }

    return false;
}

int ATHandler::get_char()
{
    if (_recv_pos == _recv_len) {
        reset_buffer(); // try to read as much as possible
        if (!fill_buffer()) {
            tr_warn("AT timeout");
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            return -1; // timeout to read
        }
    }

    return _recv_buff[_recv_pos++];
}

void ATHandler::skip_param(uint32_t count)
{
    if (_last_err || !_stop_tag || _stop_tag->found) {
        return;
    }

    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }

    for (uint32_t i = 0; (i < count && !_stop_tag->found); i++) {
        size_t match_pos = 0;
        while (true) {
            int c = get_char();
            if (c == -1) {
                set_error(NSAPI_ERROR_DEVICE_ERROR);
                return;
            } else if (c == _delimiter) {
                break;
            } else if (_stop_tag->len && c == _stop_tag->tag[match_pos]) {
                match_pos++;
                if (match_pos == _stop_tag->len) {
                    _stop_tag->found = true;
                    break;
                }
            } else if (match_pos) {
                match_pos = 0;
            }
        }
    }
    return;
}

void ATHandler::skip_param(ssize_t len, uint32_t count)
{
    if (_last_err || !_stop_tag || _stop_tag->found) {
        return;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }

    for (uint32_t i = 0; i < count; i++) {
        ssize_t read_len = 0;
        while (read_len < len) {
            int c = get_char();
            if (c == -1) {
                set_error(NSAPI_ERROR_DEVICE_ERROR);
                return;
            }
            read_len++;
        }
    }
    return;
}

ssize_t ATHandler::read_bytes(uint8_t *buf, size_t len)
{
    if (_last_err) {
        return -1;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return -1;
    }

    bool debug_on = _debug_on;
    size_t read_len = 0;
    for (; read_len < len; read_len++) {
        int c = get_char();
        if (c == -1) {
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            _debug_on = debug_on;
            return -1;
        }
        buf[read_len] = c;
        if (_debug_on && read_len >= DEBUG_MAXLEN) {
            _debug_on = false;
        }
    }
    _debug_on = debug_on;
    return read_len;
}

ssize_t ATHandler::read_string(char *buf, size_t size, bool read_even_stop_tag)
{
    if (_last_err || !_stop_tag || (_stop_tag->found && read_even_stop_tag == false)) {
        return -1;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return -1;
    }

    unsigned int len = 0;
    size_t match_pos = 0;
    bool delimiter_found = false;

    for (; len < (size - 1 + match_pos); len++) {
        int c = get_char();
        if (c == -1) {
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            return -1;
        } else if (c == _delimiter) {
            buf[len] = '\0';
            delimiter_found = true;
            break;
        } else if (c == '\"') {
            match_pos = 0;
            len--;
            continue;
        } else if (_stop_tag->len && c == _stop_tag->tag[match_pos]) {
            match_pos++;
            if (match_pos == _stop_tag->len) {
                _stop_tag->found = true;
                // remove tag from string if it was matched
                len -= (_stop_tag->len - 1);
                buf[len] = '\0';
                break;
            }
        } else if (match_pos) {
            match_pos = 0;
        }

        buf[len] = c;
    }

    if (len && (len == size - 1 + match_pos)) {
        buf[len] = '\0';
    }

    // Consume to delimiter or stop_tag
    if (!delimiter_found && !_stop_tag->found) {
        match_pos = 0;
        while (1) {
            int c = get_char();
            if (c == -1) {
                set_error(NSAPI_ERROR_DEVICE_ERROR);
                break;
            } else if (c == _delimiter) {
                break;
            } else if (_stop_tag->len && c == _stop_tag->tag[match_pos]) {
                match_pos++;
                if (match_pos == _stop_tag->len) {
                    _stop_tag->found = true;
                    break;
                }
            }
        }
    }

    return len;
}

ssize_t ATHandler::read_hex_string(char *buf, size_t size)
{
    if (_last_err || !_stop_tag || _stop_tag->found) {
        return -1;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return -1;
    }

    size_t match_pos = 0;

    consume_char('\"');

    if (_last_err) {
        return -1;
    }

    size_t read_idx = 0;
    size_t buf_idx = 0;
    char hexbuf[2];

    bool debug_on = _debug_on;
    for (; read_idx < size * 2 + match_pos; read_idx++) {
        int c = get_char();

        if (_debug_on && read_idx >= DEBUG_MAXLEN) {
            _debug_on = false;
        }

        if (match_pos) {
            buf_idx++;
        } else {
            buf_idx = read_idx / 2;
        }

        if (c == -1) {
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            return -1;
        }
        if (c == _delimiter) {
            break;
        } else if (c == '\"') {
            match_pos = 0;
            read_idx--;
            continue;
        } else if (_stop_tag->len && c == _stop_tag->tag[match_pos]) {
            match_pos++;
            if (match_pos == _stop_tag->len) {
                _stop_tag->found = true;
                // remove tag from string if it was matched
                buf_idx -= (_stop_tag->len - 1);
                break;
            }
        } else if (match_pos) {
            match_pos = 0;
        }

        if (match_pos) {
            buf[buf_idx] = c;
        } else {
            hexbuf[read_idx % 2] = c;
            if (read_idx % 2 == 1) {
                hex_str_to_char_str(hexbuf, 2, buf + buf_idx);
            }
        }
    }
    _debug_on = debug_on;

    if (read_idx && (read_idx == size * 2 + match_pos)) {
        buf_idx++;
    }

    return buf_idx;
}

int32_t ATHandler::read_int()
{
    if (_last_err || !_stop_tag || _stop_tag->found) {
        return -1;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return -1;
    }

    char buff[BUFF_SIZE];
    if (read_string(buff, sizeof(buff)) == 0) {
        return -1;
    }

    return std::strtol(buff, NULL, 10);
}

void ATHandler::set_delimiter(char delimiter)
{
    _delimiter = delimiter;
}

void ATHandler::set_default_delimiter()
{
    _delimiter = DEFAULT_DELIMITER;
}

void ATHandler::use_delimiter(bool use_delimiter)
{
    _use_delimiter = use_delimiter;
}

void ATHandler::set_tag(tag_t *tag_dst, const char *tag_seq)
{
    if (tag_seq) {
        size_t tag_len = strlen(tag_seq);
        set_string(tag_dst->tag, tag_seq, tag_len);
        tag_dst->len = tag_len;
        tag_dst->found = false;
    } else {
        _stop_tag = NULL;
    }
}

void ATHandler::set_stop_tag(const char *stop_tag_seq)
{
    if (_last_err || !_stop_tag) {
        return;
    }

    set_tag(_stop_tag, stop_tag_seq);
}

void ATHandler::set_scope(ScopeType scope_type)
{
    if (_current_scope != scope_type) {
        _current_scope = scope_type;
        switch (_current_scope) {
            case RespType:
                _stop_tag = &_resp_stop;
                _stop_tag->found = false;
                break;
            case InfoType:
                _stop_tag = &_info_stop;
                _stop_tag->found = false;
                consume_char(' ');
                break;
            case ElemType:
                _stop_tag = &_elem_stop;
                _stop_tag->found = false;
                break;
            case NotSet:
                _stop_tag = NULL;
                return;
            default:
                break;
        }
    }
}

// should match from recv_pos?
bool ATHandler::match(const char *str, size_t size)
{
    rewind_buffer();

    if ((_recv_len - _recv_pos) < size) {
        return false;
    }

    if (str && memcmp(_recv_buff + _recv_pos, str, size) == 0) {
        // consume matching part
        _recv_pos += size;
        return true;
    }
    return false;
}

bool ATHandler::match_urc()
{
    rewind_buffer();
    size_t prefix_len = 0;
    for (struct oob_t *oob = _oobs; oob; oob = oob->next) {
        prefix_len = oob->prefix_len;
        if (_recv_len >= prefix_len) {
            if (match(oob->prefix, prefix_len)) {
                set_scope(InfoType);
                if (oob->cb) {
                    oob->cb();
                }
                information_response_stop();
                return true;
            }
        }
    }
    return false;
}

bool ATHandler::match_error()
{
    if (match(CME_ERROR, CME_ERROR_LENGTH)) {
        at_error(true, DeviceErrorTypeErrorCME);
        return true;
    } else if (match(CMS_ERROR, CMS_ERROR_LENGTH)) {
        at_error(true, DeviceErrorTypeErrorCMS);
        return true;
    } else if (match(ERROR_, ERROR_LENGTH)) {
        at_error(false, DeviceErrorTypeNoError);
        return true;
    }

    return false;
}

void ATHandler::clear_error()
{
    _last_err = NSAPI_ERROR_OK;
    _last_at_err.errCode = 0;
    _last_at_err.errType = DeviceErrorTypeNoError;
    _last_3gpp_error = 0;
}

nsapi_error_t ATHandler::get_last_error() const
{
    return _last_err;
}

device_err_t ATHandler::get_last_device_error() const
{
    return _last_at_err;
}

void ATHandler::set_error(nsapi_error_t err)
{
    if (err != NSAPI_ERROR_OK) {
        tr_debug("AT error %d", err);
    }
    if (_last_err == NSAPI_ERROR_OK) {
        _last_err = err;
    }

}

int ATHandler::get_3gpp_error()
{
    return _last_3gpp_error;
}

void ATHandler::set_3gpp_error(int err, DeviceErrorType error_type)
{
    if (_last_3gpp_error) { // don't overwrite likely root cause error
        return;
    }

    if (error_type == DeviceErrorTypeErrorCMS && err < 128) {
        // CMS errors 0-127 maps straight to 3GPP errors
        _last_3gpp_error = err;
    } else {
        for (size_t i = 0; i < sizeof(map_3gpp_errors) / sizeof(map_3gpp_errors[0]); i++) {
            if (map_3gpp_errors[i][0] == err) {
                _last_3gpp_error = map_3gpp_errors[i][1];
                tr_debug("AT3GPP error code %d", get_3gpp_error());
                break;
            }
        }
    }
}

void ATHandler::at_error(bool error_code_expected, DeviceErrorType error_type)
{
    if (error_code_expected && (error_type == DeviceErrorTypeErrorCMS || error_type == DeviceErrorTypeErrorCME)) {
        set_scope(InfoType);
        int32_t err = read_int();

        if (err != -1) {
            set_3gpp_error(err, error_type);
            _last_at_err.errCode = err;
            _last_at_err.errType = error_type;
            tr_error("AT error code %ld", err);
        } else {
            tr_warn("ATHandler ERROR reading failed");
        }
    }

    set_error(NSAPI_ERROR_DEVICE_ERROR);
}

void ATHandler::resp(const char *prefix, bool check_urc)
{
    _prefix_matched = false;
    _urc_matched = false;
    _error_found = false;

    while (!get_last_error()) {

        match(CRLF, CRLF_LENGTH);

        if (match(OK, OK_LENGTH)) {
            set_scope(RespType);
            _stop_tag->found = true;
            return;
        }

        if (match_error()) {
            _error_found = true;
            return;
        }

        if (prefix && strlen(prefix) && match(prefix, strlen(prefix))) {
            _prefix_matched = true;
            return;
        }

        if (check_urc && match_urc()) {
            _urc_matched = true;
            clear_error();
            continue;
        }

        // If no match found, look for CRLF and consume everything up to and including CRLF
        if (mem_str(_recv_buff, _recv_len, CRLF, CRLF_LENGTH)) {
            // If no prefix, return on CRLF - means data to read
            if (!prefix || (prefix && !strlen(prefix))) {
                return;
            }
            consume_to_tag(CRLF, true);
        } else {
            // If no prefix, no CRLF and no more chance to match for OK, ERROR or URC(since max resp length is already in buffer)
            // return so data could be read
            if ((!prefix || (prefix && !strlen(prefix))) && ((_recv_len - _recv_pos) >= _max_resp_length)) {
                return;
            }
            if (!fill_buffer()) {
                // if we don't get any match and no data within timeout, set an error to indicate need for recovery
                set_error(NSAPI_ERROR_DEVICE_ERROR);
            }
        }
    }

    return;
    // something went wrong so application need to recover and retry
}

void ATHandler::resp_start(const char *prefix, bool stop)
{
    if (_last_err) {
        return;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }

    set_scope(NotSet);
    // Try get as much data as possible
    rewind_buffer();
    (void)fill_buffer(false);

    if (prefix) {
        MBED_ASSERT(strlen(prefix) < BUFF_SIZE);
        strcpy(_info_resp_prefix, prefix); // copy prefix so we can later use it without having to provide again for info_resp
    }

    set_scope(RespType);

    resp(prefix, true);

    if (!stop && prefix && _prefix_matched) {
        set_scope(InfoType);
    }
}

// check urc because of error as urc
bool ATHandler::info_resp()
{
    if (_last_err || _resp_stop.found) {
        return false;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return false;
    }

    if (_prefix_matched) {
        _prefix_matched = false;
        return true;
    }

    // If coming here after another info response was started(looping), stop the previous one.
    // Trying to handle stopping in this level instead of doing it in upper level.
    if (get_scope() == InfoType) {
        information_response_stop();
    }

    resp(_info_resp_prefix, false);

    if (_prefix_matched) {
        set_scope(InfoType);
        _prefix_matched = false;
        return true;
    }

    // On mismatch go to response scope
    set_scope(RespType);
    return false;
}

bool ATHandler::info_elem(char start_tag)
{
    if (_last_err) {
        return false;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return false;
    }

    // If coming here after another info response element was started(looping), stop the previous one.
    // Trying to handle stopping in this level instead of doing it in upper level.
    if (get_scope() == ElemType) {
        information_response_element_stop();
    }

    consume_char(_delimiter);

    if (consume_char(start_tag)) {
        _prefix_matched = true;
        set_scope(ElemType);
        return true;
    }

    // On mismatch go to information response scope
    set_scope(InfoType);
    return false;
}

bool ATHandler::consume_char(char ch)
{
    int read_char = get_char();
    if (read_char == -1) {
        return false;
    }
    // If we read something else than ch, recover it
    if (read_char != ch) {
        _recv_pos--;
        return false;
    }
    return true;
}

bool ATHandler::consume_to_tag(const char *tag, bool consume_tag)
{
    size_t match_pos = 0;
    size_t tag_length = strlen(tag);

    while (true) {
        int c = get_char();
        if (c == -1) {
            tr_debug("consume_to_tag not found");
            return false;
        }
        if (c == tag[match_pos]) {
            match_pos++;
        } else if (match_pos != 0) {
            match_pos = 0;
            if (c == tag[match_pos]) {
                match_pos++;
            }
        }
        if (match_pos == tag_length) {
            break;
        }
    }

    if (!consume_tag) {
        _recv_pos -= tag_length;
    }
    return true;
}

bool ATHandler::consume_to_stop_tag()
{
    if (!_stop_tag || (_stop_tag && _stop_tag->found) || _error_found) {
        return true;
    }

    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return true;
    }

    if (consume_to_tag((const char *)_stop_tag->tag, true)) {
        return true;
    }

    tr_debug("AT stop tag not found");
    set_error(NSAPI_ERROR_DEVICE_ERROR);
    return false;
}

// consume by size needed?

void ATHandler::resp_stop()
{
    if (_is_fh_usable) {
        // Do not return on error so that we can consume whatever there is in the buffer

        if (_current_scope == ElemType) {
            information_response_element_stop();
            set_scope(InfoType);
        }

        if (_current_scope == InfoType) {
            information_response_stop();
        }

        // Go for response stop_tag
        if (_stop_tag && !_stop_tag->found && !_error_found) {
            // Check for URC for every new line
            while (!get_last_error()) {

                if (match(_stop_tag->tag, _stop_tag->len)) {
                    break;
                }

                if (match_urc()) {
                    continue;
                }

                // If no URC nor stop_tag found, look for CRLF and consume everything up to and including CRLF
                if (mem_str(_recv_buff, _recv_len, CRLF, CRLF_LENGTH)) {
                    consume_to_tag(CRLF, true);
                    // If stop tag is CRLF we have to stop reading/consuming the buffer
                    if (!strncmp(CRLF, _stop_tag->tag, _stop_tag->len)) {
                        break;
                    }
                    // If no URC nor CRLF nor stop_tag -> fill buffer
                } else {
                    if (!fill_buffer()) {
                        // if we don't get any match and no data within timeout, set an error to indicate need for recovery
                        set_error(NSAPI_ERROR_DEVICE_ERROR);
                    }
                }
            }
        }
    } else {
        _last_err = NSAPI_ERROR_BUSY;
    }

    set_scope(NotSet);

    // Restore stop tag to OK
    set_tag(&_resp_stop, OK);
    // Reset info resp prefix
    memset(_info_resp_prefix, 0, sizeof(_info_resp_prefix));

    _last_response_stop = rtos::Kernel::get_ms_count();
}

void ATHandler::information_response_stop()
{
    if (consume_to_stop_tag()) {
        set_scope(RespType);
    }
}

void ATHandler::information_response_element_stop()
{
    if (consume_to_stop_tag()) {
        set_scope(InfoType);
    }
}

ATHandler::ScopeType ATHandler::get_scope()
{
    return _current_scope;
}

void ATHandler::set_string(char *dest, const char *src, size_t src_len)
{
    memcpy(dest, src, src_len);
    dest[src_len] = '\0';
}

const char *ATHandler::mem_str(const char *dest, size_t dest_len, const char *src, size_t src_len)
{
    if (dest_len >= src_len) {
        for (size_t i = 0; i < dest_len - src_len + 1; ++i) {
            if (memcmp(dest + i, src, src_len) == 0) {
                return dest + i;
            }
        }
    }
    return NULL;
}

void ATHandler::cmd_start(const char *cmd)
{
    if (_last_err != NSAPI_ERROR_OK) {
        return;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }

    if (_at_send_delay) {
        rtos::ThisThread::sleep_until(_last_response_stop + _at_send_delay);
    }

    (void)write(cmd, strlen(cmd));

    _cmd_start = true;
}

void ATHandler::write_int(int32_t param)
{
    // do common checks before sending subparameter
    if (check_cmd_send() == false) {
        return;
    }

    // write the integer subparameter
    const int32_t str_len = 12;
    char number_string[str_len];
    int32_t result = sprintf(number_string, "%ld", param);
    if (result > 0 && result < str_len) {
        (void)write(number_string, strlen(number_string));
    }
}

void ATHandler::write_string(const char *param, bool useQuotations)
{
    // do common checks before sending subparameter
    if (check_cmd_send() == false) {
        return;
    }

    // we are writing string, surround it with quotes
    if (useQuotations && write("\"", 1) != 1) {
        return;
    }

    (void)write(param, strlen(param));

    if (useQuotations) {
        // we are writing string, surround it with quotes
        (void)write("\"", 1);
    }
}

void ATHandler::cmd_stop()
{
    if (_last_err != NSAPI_ERROR_OK) {
        return;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }
    // Finish with CR
    (void)write(_output_delimiter, strlen(_output_delimiter));
}

void ATHandler::cmd_stop_read_resp()
{
    cmd_stop();
    resp_start();
    resp_stop();
}

size_t ATHandler::write_bytes(const uint8_t *data, size_t len)
{
    if (_last_err != NSAPI_ERROR_OK) {
        return 0;
    }
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return 0;
    }

    return write(data, len);
}

size_t ATHandler::write(const void *data, size_t len)
{
    pollfh fhs;
    fhs.fh = _fileHandle;
    fhs.events = POLLOUT;
    size_t write_len = 0;
    bool debug_on = _debug_on;
    for (; write_len < len;) {
        int count = poll(&fhs, 1, poll_timeout());
        if (count <= 0 || !(fhs.revents & POLLOUT)) {
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            _debug_on = debug_on;
            return 0;
        }
        ssize_t ret = _fileHandle->write((uint8_t *)data + write_len, len - write_len);
        if (ret < 0) {
            set_error(NSAPI_ERROR_DEVICE_ERROR);
            _debug_on = debug_on;
            return 0;
        }
        if (_debug_on && write_len < DEBUG_MAXLEN) {
            if (write_len + ret < DEBUG_MAXLEN) {
                debug_print((char *)data + write_len, ret, AT_TX);
            } else {
                _debug_on = false;
            }
        }
        write_len += (size_t)ret;
    }
    _debug_on = debug_on;

    return write_len;
}

// do common checks before sending subparameters
bool ATHandler::check_cmd_send()
{
    if (_last_err != NSAPI_ERROR_OK) {
        return false;
    }

    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return false;
    }

    // Don't write delimiter if flag was set so
    if (!_use_delimiter) {
        return true;
    }

    // Don't write delimiter if this is the first subparameter
    if (_cmd_start) {
        _cmd_start = false;
    } else {
        if (write(&_delimiter, 1) != 1) {
            // writing of delimiter failed, return. write() already have set the _last_err
            return false;
        }
    }

    return true;
}

void ATHandler::flush()
{
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return;
    }
    tr_debug("AT flush");
    reset_buffer();
    while (fill_buffer(false)) {
        reset_buffer();
    }
}

void ATHandler::debug_print(const char *p, int len, ATType type)
{
#if MBED_CONF_CELLULAR_DEBUG_AT
    if (_debug_on) {
        const int buf_size = len * 4 + 1; // x4 -> reserve space for extra characters, +1 -> terminating null
        char *buffer = new char [buf_size];
        if (buffer) {
            memset(buffer, 0, buf_size);

            char *pbuf = buffer;
            for (ssize_t i = 0; i < len; i++) {
                const char c = *p++;
                if (isprint(c)) {
                    *pbuf++ = c;
                } else if (c == '\r') {
                    sprintf(pbuf, "<cr>");
                    pbuf += 4;
                } else if (c == '\n') {
                    sprintf(pbuf, "<ln>");
                    pbuf += 4;
                } else {
                    sprintf(pbuf, "<%02X>", c);
                    pbuf += 4;
                }
            }
            MBED_ASSERT((int)(pbuf - buffer) <= buf_size); // Check for buffer overflow

            if (type == AT_RX) {
                tr_info("AT RX (%2d): %s", len, buffer);
            } else if (type == AT_TX) {
                tr_info("AT TX (%2d): %s", len, buffer);
            } else {
                tr_info("AT ERR (%2d): %s", len, buffer);
            }

            delete [] buffer;
        } else {
            tr_error("AT trace unable to allocate buffer!");
        }
    }
#endif // MBED_CONF_CELLULAR_DEBUG_AT
}

bool ATHandler::sync(int timeout_ms)
{
    if (!_is_fh_usable) {
        _last_err = NSAPI_ERROR_BUSY;
        return false;
    }
    tr_debug("AT sync");
    lock();
    uint32_t timeout = _at_timeout;
    _at_timeout = timeout_ms;
    // poll for 10 seconds
    for (int i = 0; i < 10; i++) {
        // For sync use an AT command that is supported by all modems and likely not used frequently,
        // especially a common response like OK could be response to previous request.
        clear_error();
        _start_time = rtos::Kernel::get_ms_count();
        cmd_start("AT+CMEE?");
        cmd_stop();
        resp_start("+CMEE:");
        resp_stop();
        if (!_last_err) {
            _at_timeout = timeout;
            unlock();
            return true;
        }
    }
    tr_error("AT sync failed");
    _at_timeout = timeout;
    unlock();
    return false;
}
