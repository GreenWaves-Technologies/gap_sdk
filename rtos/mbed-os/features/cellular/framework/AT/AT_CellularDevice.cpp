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

#include "CellularUtil.h"
#include "AT_CellularDevice.h"
#include "AT_CellularInformation.h"
#include "AT_CellularNetwork.h"
#include "AT_CellularSMS.h"
#include "AT_CellularContext.h"
#include "AT_CellularStack.h"
#include "CellularLog.h"
#include "ATHandler.h"
#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
#include "UARTSerial.h"
#endif // #if DEVICE_SERIAL
#include "FileHandle.h"

using namespace mbed_cellular_util;
using namespace events;
using namespace mbed;

#define DEFAULT_AT_TIMEOUT 1000 // at default timeout in milliseconds
const int MAX_SIM_RESPONSE_LENGTH = 16;

AT_CellularDevice::AT_CellularDevice(FileHandle *fh) : CellularDevice(fh), _network(0), _sms(0),
    _information(0), _context_list(0), _default_timeout(DEFAULT_AT_TIMEOUT),
    _modem_debug_on(false)
{
    MBED_ASSERT(fh);
    _at = get_at_handler(fh);
    MBED_ASSERT(_at);
}

AT_CellularDevice::~AT_CellularDevice()
{
    // make sure that all is deleted even if somewhere close was not called and reference counting is messed up.
    _network_ref_count = 1;
    _sms_ref_count = 1;
    _info_ref_count = 1;

    close_network();
    close_sms();
    close_information();

    AT_CellularContext *curr = _context_list;
    AT_CellularContext *next;
    while (curr) {
        next = (AT_CellularContext *)curr->_next;
        ATHandler *at = &curr->get_at_handler();
        delete curr;
        curr = next;
        release_at_handler(at);
    }

    release_at_handler(_at);
}

nsapi_error_t AT_CellularDevice::hard_power_on()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t AT_CellularDevice::hard_power_off()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t AT_CellularDevice::soft_power_on()
{
    return NSAPI_ERROR_OK;
}

nsapi_error_t AT_CellularDevice::soft_power_off()
{
    return NSAPI_ERROR_OK;
}

// each parser is associated with one filehandle (that is UART)
ATHandler *AT_CellularDevice::get_at_handler(FileHandle *fileHandle)
{
    if (!fileHandle) {
        fileHandle = _fh;
    }

    return ATHandler::get_instance(fileHandle, _queue, _default_timeout,
                                   "\r", get_send_delay(), _modem_debug_on);
}

ATHandler *AT_CellularDevice::get_at_handler()
{
    return get_at_handler(NULL);
}

nsapi_error_t AT_CellularDevice::release_at_handler(ATHandler *at_handler)
{
    if (at_handler) {
        return at_handler->close();
    } else {
        return NSAPI_ERROR_PARAMETER;
    }
}

nsapi_error_t AT_CellularDevice::get_sim_state(SimState &state)
{
    char simstr[MAX_SIM_RESPONSE_LENGTH];
    _at->lock();
    _at->flush();
    _at->cmd_start("AT+CPIN?");
    _at->cmd_stop();
    _at->resp_start("+CPIN:");
    ssize_t len = _at->read_string(simstr, sizeof(simstr));
    if (len != -1) {
        if (len >= 5 && memcmp(simstr, "READY", 5) == 0) {
            state = SimStateReady;
        } else if (len >= 7 && memcmp(simstr, "SIM PIN", 7) == 0) {
            state = SimStatePinNeeded;
        } else if (len >= 7 && memcmp(simstr, "SIM PUK", 7) == 0) {
            state = SimStatePukNeeded;
        } else {
            simstr[len] = '\0';
            tr_error("Unknown SIM state %s", simstr);
            state = SimStateUnknown;
        }
    } else {
        tr_warn("SIM not readable.");
        state = SimStateUnknown; // SIM may not be ready yet or +CPIN may be unsupported command
    }
    _at->resp_stop();
    nsapi_error_t error = _at->get_last_error();
    _at->unlock();
#if MBED_CONF_MBED_TRACE_ENABLE
    switch (state) {
        case SimStatePinNeeded:
            tr_info("SIM PIN required");
            break;
        case SimStatePukNeeded:
            tr_error("SIM PUK required");
            break;
        case SimStateUnknown:
            tr_warn("SIM state unknown");
            break;
        default:
            tr_info("SIM is ready");
            break;
    }
#endif
    return error;
}

nsapi_error_t AT_CellularDevice::set_pin(const char *sim_pin)
{
    // if SIM is already in ready state then settings the PIN
    // will return error so let's check the state before settings the pin.
    SimState state;
    if (get_sim_state(state) == NSAPI_ERROR_OK && state == SimStateReady) {
        return NSAPI_ERROR_OK;
    }

    if (sim_pin == NULL) {
        return NSAPI_ERROR_PARAMETER;
    }

    _at->lock();
    _at->cmd_start("AT+CPIN=");

    const bool stored_debug_state = _at->get_debug();
    _at->set_debug(false);

    _at->write_string(sim_pin);

    _at->set_debug(stored_debug_state);

    _at->cmd_stop_read_resp();
    return _at->unlock_return_error();
}

CellularContext *AT_CellularDevice::get_context_list() const
{
    return _context_list;
}

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
CellularContext *AT_CellularDevice::create_context(UARTSerial *serial, const char *const apn, PinName dcd_pin,
                                                   bool active_high, bool cp_req, bool nonip_req)
{
    // Call FileHandle base version - explict upcast to avoid recursing into ourselves
    CellularContext *ctx = create_context(static_cast<FileHandle *>(serial), apn, cp_req, nonip_req);
    if (serial) {
        ctx->set_file_handle(serial, dcd_pin, active_high);
    }
    return ctx;
}
#endif // #if DEVICE_SERIAL

CellularContext *AT_CellularDevice::create_context(FileHandle *fh, const char *apn, bool cp_req, bool nonip_req)
{
    AT_CellularContext *ctx = create_context_impl(*get_at_handler(fh), apn, cp_req, nonip_req);
    AT_CellularContext *curr = _context_list;

    if (_context_list == NULL) {
        _context_list = ctx;
        return ctx;
    }

    AT_CellularContext *prev = NULL;
    while (curr) {
        prev = curr;
        curr = (AT_CellularContext *)curr->_next;
    }

    prev->_next = ctx;
    return ctx;
}

AT_CellularContext *AT_CellularDevice::create_context_impl(ATHandler &at, const char *apn, bool cp_req, bool nonip_req)
{
    if (cp_req) {

    }
    return new AT_CellularContext(at, this, apn, cp_req, nonip_req);
}

void AT_CellularDevice::delete_context(CellularContext *context)
{
    AT_CellularContext *curr = _context_list;
    AT_CellularContext *prev = NULL;
    while (curr) {
        if (curr == context) {
            if (prev == NULL) {
                _context_list = (AT_CellularContext *)curr->_next;
            } else {
                prev->_next = curr->_next;
            }
        }
        prev = curr;
        curr = (AT_CellularContext *)curr->_next;
    }
    curr = (AT_CellularContext *)context;
    ATHandler *at = NULL;
    if (curr) {
        at = &curr->get_at_handler();
    }
    delete (AT_CellularContext *)context;
    release_at_handler(at);
}

CellularNetwork *AT_CellularDevice::open_network(FileHandle *fh)
{
    if (!_network) {
        _network = open_network_impl(*get_at_handler(fh));
    }
    _network_ref_count++;
    return _network;
}

CellularSMS *AT_CellularDevice::open_sms(FileHandle *fh)
{
    if (!_sms) {
        _sms = open_sms_impl(*get_at_handler(fh));
    }
    _sms_ref_count++;
    return _sms;
}

CellularInformation *AT_CellularDevice::open_information(FileHandle *fh)
{
    if (!_information) {
        _information = open_information_impl(*get_at_handler(fh));
    }
    _info_ref_count++;
    return _information;
}

AT_CellularNetwork *AT_CellularDevice::open_network_impl(ATHandler &at)
{
    return new AT_CellularNetwork(at);
}

AT_CellularSMS *AT_CellularDevice::open_sms_impl(ATHandler &at)
{
    return new AT_CellularSMS(at);
}

AT_CellularInformation *AT_CellularDevice::open_information_impl(ATHandler &at)
{
    return new AT_CellularInformation(at);
}

void AT_CellularDevice::close_network()
{
    if (_network) {
        _network_ref_count--;
        if (_network_ref_count == 0) {
            ATHandler *atHandler = &_network->get_at_handler();
            delete _network;
            _network = NULL;
            release_at_handler(atHandler);
        }
    }
}

void AT_CellularDevice::close_sms()
{
    if (_sms) {
        _sms_ref_count--;
        if (_sms_ref_count == 0) {
            ATHandler *atHandler = &_sms->get_at_handler();
            delete _sms;
            _sms = NULL;
            release_at_handler(atHandler);
        }
    }
}

void AT_CellularDevice::close_information()
{
    if (_information) {
        _info_ref_count--;
        if (_info_ref_count == 0) {
            ATHandler *atHandler = &_information->get_at_handler();
            delete _information;
            _information = NULL;
            release_at_handler(atHandler);
        }
    }
}

void AT_CellularDevice::set_timeout(int timeout)
{
    _default_timeout = timeout;

    ATHandler::set_at_timeout_list(_default_timeout, true);
}

uint16_t AT_CellularDevice::get_send_delay() const
{
    return 0;
}

void AT_CellularDevice::modem_debug_on(bool on)
{
    _modem_debug_on = on;

    ATHandler::set_debug_list(_modem_debug_on);
}

nsapi_error_t AT_CellularDevice::init()
{
    _at->lock();
    _at->flush();
    _at->cmd_start("ATE0"); // echo off
    _at->cmd_stop_read_resp();

    _at->cmd_start("AT+CMEE=1"); // verbose responses
    _at->cmd_stop_read_resp();

    _at->cmd_start("AT+CFUN=1"); // set full functionality
    _at->cmd_stop_read_resp();

    return _at->unlock_return_error();
}

nsapi_error_t AT_CellularDevice::shutdown()
{
    _at->lock();
    if (_state_machine) {
        _state_machine->reset();
    }
    CellularDevice::shutdown();
    _at->cmd_start("AT+CFUN=0");// set to minimum functionality
    _at->cmd_stop_read_resp();
    return _at->unlock_return_error();
}

nsapi_error_t AT_CellularDevice::is_ready()
{
    _at->lock();
    _at->cmd_start("AT");
    _at->cmd_stop_read_resp();

    // we need to do this twice because for example after data mode the first 'AT' command will give modem a
    // stimulus that we are back to command mode.
    _at->clear_error();
    _at->cmd_start("AT");
    _at->cmd_stop_read_resp();

    return _at->unlock_return_error();
}

void AT_CellularDevice::set_ready_cb(Callback<void()> callback)
{
}

nsapi_error_t AT_CellularDevice::set_power_save_mode(int periodic_time, int active_time)
{
    _at->lock();

    if (periodic_time == 0 && active_time == 0) {
        // disable PSM
        _at->cmd_start("AT+CPSMS=");
        _at->write_int(0);
        _at->cmd_stop_read_resp();
    } else {
        const int PSMTimerBits = 5;

        /**
            Table 10.5.163a/3GPP TS 24.008: GPRS Timer 3 information element

            Bits 5 to 1 represent the binary coded timer value.

            Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:
            8 7 6
            0 0 0 value is incremented in multiples of 10 minutes
            0 0 1 value is incremented in multiples of 1 hour
            0 1 0 value is incremented in multiples of 10 hours
            0 1 1 value is incremented in multiples of 2 seconds
            1 0 0 value is incremented in multiples of 30 seconds
            1 0 1 value is incremented in multiples of 1 minute
            1 1 0 value is incremented in multiples of 320 hours (NOTE 1)
            1 1 1 value indicates that the timer is deactivated (NOTE 2).
         */
        char pt[8 + 1]; // timer value encoded as 3GPP IE
        const int ie_value_max = 0x1f;
        uint32_t periodic_timer = 0;
        if (periodic_time <= 2 * ie_value_max) { // multiples of 2 seconds
            periodic_timer = periodic_time / 2;
            strcpy(pt, "01100000");
        } else {
            if (periodic_time <= 30 * ie_value_max) { // multiples of 30 seconds
                periodic_timer = periodic_time / 30;
                strcpy(pt, "10000000");
            } else {
                if (periodic_time <= 60 * ie_value_max) { // multiples of 1 minute
                    periodic_timer = periodic_time / 60;
                    strcpy(pt, "10100000");
                } else {
                    if (periodic_time <= 10 * 60 * ie_value_max) { // multiples of 10 minutes
                        periodic_timer = periodic_time / (10 * 60);
                        strcpy(pt, "00000000");
                    } else {
                        if (periodic_time <= 60 * 60 * ie_value_max) { // multiples of 1 hour
                            periodic_timer = periodic_time / (60 * 60);
                            strcpy(pt, "00100000");
                        } else {
                            if (periodic_time <= 10 * 60 * 60 * ie_value_max) { // multiples of 10 hours
                                periodic_timer = periodic_time / (10 * 60 * 60);
                                strcpy(pt, "01000000");
                            } else { // multiples of 320 hours
                                int t = periodic_time / (320 * 60 * 60);
                                if (t > ie_value_max) {
                                    t = ie_value_max;
                                }
                                periodic_timer = t;
                                strcpy(pt, "11000000");
                            }
                        }
                    }
                }
            }
        }

        uint_to_binary_str(periodic_timer, &pt[3], sizeof(pt) - 3, PSMTimerBits);
        pt[8] = '\0';

        /**
            Table 10.5.172/3GPP TS 24.008: GPRS Timer information element

            Bits 5 to 1 represent the binary coded timer value.

            Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:

            8 7 6
            0 0 0  value is incremented in multiples of 2 seconds
            0 0 1  value is incremented in multiples of 1 minute
            0 1 0  value is incremented in multiples of decihours
            1 1 1  value indicates that the timer is deactivated.

            Other values shall be interpreted as multiples of 1 minute in this version of the protocol.
        */
        char at[8 + 1];
        uint32_t active_timer; // timer value encoded as 3GPP IE
        if (active_time <= 2 * ie_value_max) { // multiples of 2 seconds
            active_timer = active_time / 2;
            strcpy(at, "00000000");
        } else {
            if (active_time <= 60 * ie_value_max) { // multiples of 1 minute
                active_timer = (1 << 5) | (active_time / 60);
                strcpy(at, "00100000");
            } else { // multiples of decihours
                int t = active_time / (6 * 60);
                if (t > ie_value_max) {
                    t = ie_value_max;
                }
                active_timer = t;
                strcpy(at, "01000000");
            }
        }

        uint_to_binary_str(active_timer, &at[3], sizeof(at) - 3, PSMTimerBits);
        at[8] = '\0';

        // request for both GPRS and LTE
        _at->cmd_start("AT+CPSMS=");
        _at->write_int(1);
        _at->write_string(pt);
        _at->write_string(at);
        _at->write_string(pt);
        _at->write_string(at);
        _at->cmd_stop_read_resp();

        if (_at->get_last_error() != NSAPI_ERROR_OK) {
            tr_warn("Power save mode not enabled!");
        } else {
            // network may not agree with power save options but
            // that should be fine as timeout is not longer than requested
        }
    }

    return _at->unlock_return_error();
}
