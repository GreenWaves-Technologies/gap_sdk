/*
 * Copyright (c) 2018, Arm Limited and affiliates.
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
#ifndef MY_CELLULARDEVICE_H_
#define MY_CELLULARDEVICE_H_

#include "CellularDevice.h"
#include "AT_CellularNetwork.h"
#include "FileHandle_stub.h"
#include "ATHandler_stub.h"
#include "AT_CellularContext.h"

using namespace events;

namespace mbed {

class CellularSMS;
class CellularInformation;
class CellularContext;
class FileHandle;

class myCellularDevice : public CellularDevice {
public:
    myCellularDevice(FileHandle *fh) : CellularDevice(fh), _context_list(0), _network(0) {}
    ~myCellularDevice()
    {
        delete _context_list;
        delete _network;
    }

    virtual nsapi_error_t set_pin(const char *sim_pin)
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t get_sim_state(SimState &state)
    {
        return NSAPI_ERROR_OK;
    }

    virtual CellularContext *create_context(UARTSerial *serial, const char *const apn, PinName dcd_pin,
                                            bool active_high, bool cp_req = false, bool nonip_req = false)
    {
        return NULL;
    }

    virtual CellularContext *create_context(FileHandle *fh = NULL, const char *apn = NULL, bool cp_req = false, bool nonip_req = false)
    {
        EventQueue que;
        FileHandle_stub fh1;
        ATHandler at(&fh1, que, 0, ",");
        _context_list = new AT_CellularContext(at, NULL);
        return _context_list;
    }
    virtual void delete_context(CellularContext *context)
    {
        delete _context_list;
    }

    virtual CellularNetwork *open_network(FileHandle *fh = NULL)
    {
        EventQueue que;
        FileHandle_stub fh1;
        ATHandler at(&fh1, que, 0, ",");
        _network = new AT_CellularNetwork(at);
        return _network;
    }

    virtual CellularSMS *open_sms(FileHandle *fh = NULL)
    {
        return NULL;
    }

    virtual CellularInformation *open_information(FileHandle *fh = NULL)
    {
        return NULL;
    }

    virtual void close_network()
    {
        delete _network;
    }

    virtual void close_sms() {}

    virtual void close_information() {}

    virtual void set_timeout(int timeout) {}

    virtual uint16_t get_send_delay() const
    {
        return 0;
    }

    virtual void modem_debug_on(bool on) {}

    virtual nsapi_error_t init()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t shutdown()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t is_ready()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t hard_power_on()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t hard_power_off()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t soft_power_on()
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t soft_power_off()
    {
        return NSAPI_ERROR_OK;
    }

    virtual void set_ready_cb(Callback<void()> callback)
    {
    }

    nsapi_error_t set_power_save_mode(int periodic_time, int active_time)
    {
        return NSAPI_ERROR_OK;
    }

    virtual CellularContext *get_context_list() const
    {
        return _context_list;
    }
    void cellular_callback(nsapi_event_t ev, intptr_t ptr)
    {
        CellularDevice::cellular_callback(ev, ptr);
    }
    virtual ATHandler *get_at_handler()
    {
        return NULL;
    }
    virtual nsapi_error_t release_at_handler(ATHandler *at_handler)
    {
        return NSAPI_ERROR_OK;
    }
    AT_CellularNetwork *_network;
    AT_CellularContext *_context_list;
};

} // namespace
#endif /* MY_CELLULARDEVICE_H_ */
