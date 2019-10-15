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

#include "AT_CellularContext.h"
#include "AT_CellularNetwork.h"
#include "AT_CellularStack.h"
#include "CellularLog.h"
#include "CellularUtil.h"
#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
#include "UARTSerial.h"
#endif // #if DEVICE_SERIAL
#include "mbed_wait_api.h"

#define NETWORK_TIMEOUT 30 * 60 * 1000 // 30 minutes
#define DEVICE_TIMEOUT 5 * 60 * 1000 // 5 minutes
// Timeout to wait for URC indicating ciot optimization support from network
#define CP_OPT_NW_REPLY_TIMEOUT 3000 // 3 seconds


#if NSAPI_PPP_AVAILABLE
#define AT_SYNC_TIMEOUT 1000 // 1 second timeout
#include "nsapi_ppp.h"
#endif

#define USE_APN_LOOKUP (MBED_CONF_CELLULAR_USE_APN_LOOKUP || (NSAPI_PPP_AVAILABLE && MBED_CONF_PPP_CELL_IFACE_APN_LOOKUP))

#if USE_APN_LOOKUP
#include "CellularInformation.h"
#include "APN_db.h"
#endif //USE_APN_LOOKUP

using namespace mbed_cellular_util;
using namespace mbed;

AT_CellularContext::AT_CellularContext(ATHandler &at, CellularDevice *device, const char *apn, bool cp_req, bool nonip_req) :
    AT_CellularBase(at), _is_connected(false), _is_blocking(true),
    _current_op(OP_INVALID), _device(device), _nw(0), _fh(0), _cp_req(cp_req), _nonip_req(nonip_req), _cp_in_use(false)
{
    tr_info("New CellularContext %s (%p)", apn ? apn : "", this);
    _stack = NULL;
    _pdp_type = DEFAULT_PDP_TYPE;
    _authentication_type = CellularContext::CHAP;
    _connect_status = NSAPI_STATUS_DISCONNECTED;
    _is_context_active = false;
    _is_context_activated = false;
    _apn = apn;
    _uname = NULL;
    _pwd = NULL;
    _status_cb = NULL;
    _cid = -1;
    _new_context_set = false;
    _next = NULL;
    _dcd_pin = NC;
    _active_high = false;
    _cp_netif = NULL;
}

AT_CellularContext::~AT_CellularContext()
{
    tr_info("Delete CellularContext with apn: [%s] (%p)", _apn ? _apn : "", this);

    (void)disconnect();

    if (_nw) {
        _device->close_network();
    }

    if (_cp_netif) {
        delete _cp_netif;
    }
}

void AT_CellularContext::set_file_handle(FileHandle *fh)
{
    tr_info("CellularContext filehandle %p", fh);
    _fh = fh;
    _at.set_file_handle(_fh);
}

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
void AT_CellularContext::set_file_handle(UARTSerial *serial, PinName dcd_pin, bool active_high)
{
    tr_info("CellularContext serial %p", serial);
    _dcd_pin = dcd_pin;
    _active_high = active_high;
    _fh = serial;
    _at.set_file_handle(static_cast<FileHandle *>(serial));
    enable_hup(false);
}
#endif // #if DEVICE_SERIAL

void AT_CellularContext::enable_hup(bool enable)
{
    if (_dcd_pin != NC) {
#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
        static_cast<UARTSerial *>(_fh)->set_data_carrier_detect(enable ? _dcd_pin : NC, _active_high);
#endif // #if DEVICE_SERIAL
    }
}

nsapi_error_t AT_CellularContext::connect()
{
    tr_info("CellularContext connect");
    if (_is_connected) {
        return NSAPI_ERROR_IS_CONNECTED;
    }
    call_network_cb(NSAPI_STATUS_CONNECTING);

    nsapi_error_t err = _device->attach_to_network();
    _cb_data.error = check_operation(err, OP_CONNECT);

    if (_is_blocking) {
        if (_cb_data.error == NSAPI_ERROR_OK || _cb_data.error == NSAPI_ERROR_ALREADY) {
            do_connect();
        }
    } else {
        if (_cb_data.error == NSAPI_ERROR_ALREADY) {
            // device is already attached, to be async we must use queue to connect and give proper callbacks
            int id = _device->get_queue()->call_in(0, this, &AT_CellularContext::do_connect);
            if (id == 0) {
                return NSAPI_ERROR_NO_MEMORY;
            }
            return NSAPI_ERROR_OK;
        }
    }

    if (_cb_data.error == NSAPI_ERROR_ALREADY) {
        return NSAPI_ERROR_OK;
    }

    return _cb_data.error;
}

nsapi_error_t AT_CellularContext::set_device_ready()
{
    nsapi_error_t err = _device->set_device_ready();
    return check_operation(err, OP_DEVICE_READY);
}

nsapi_error_t AT_CellularContext::set_sim_ready()
{
    nsapi_error_t err = _device->set_sim_ready();
    return check_operation(err, OP_SIM_READY);
}

nsapi_error_t AT_CellularContext::register_to_network()
{
    nsapi_error_t err = _device->register_to_network();
    return check_operation(err, OP_REGISTER);
}

nsapi_error_t AT_CellularContext::attach_to_network()
{
    nsapi_error_t err = _device->attach_to_network();
    return check_operation(err, OP_ATTACH);
}

nsapi_error_t AT_CellularContext::check_operation(nsapi_error_t err, ContextOperation op)
{
    _current_op = op;
    if (err == NSAPI_ERROR_IN_PROGRESS || err == NSAPI_ERROR_OK) {
        if (_is_blocking) {
            int sema_err = _semaphore.wait(get_timeout_for_operation(op)); // cellular network searching may take several minutes
            if (sema_err != 1) {
                tr_warning("No cellular connection");
                return NSAPI_ERROR_TIMEOUT;
            }
            return NSAPI_ERROR_OK;
        }
    }

    return err;
}

nsapi_connection_status_t AT_CellularContext::get_connection_status() const
{
    return _connect_status;
}

uint32_t AT_CellularContext::get_timeout_for_operation(ContextOperation op) const
{
    uint32_t timeout = NETWORK_TIMEOUT; // default timeout is 30 minutes as registration and attach may take time
    if (op == OP_SIM_READY || op == OP_DEVICE_READY) {
        timeout = DEVICE_TIMEOUT; // use 5 minutes for device ready and sim
    }
    return timeout;
}

bool AT_CellularContext::is_connected()
{
    return _is_connected;
}

NetworkStack *AT_CellularContext::get_stack()
{
#if NSAPI_PPP_AVAILABLE
    // use lwIP/PPP if modem does not have IP stack
    if (!_stack) {
        _stack = nsapi_ppp_get_stack();
    }
#endif
    return _stack;
}

const char *AT_CellularContext::get_netmask()
{
    return NULL;
}

const char *AT_CellularContext::get_gateway()
{
    return NULL;
}

const char *AT_CellularContext::get_ip_address()
{
#if NSAPI_PPP_AVAILABLE
    return nsapi_ppp_get_ip_addr(_at.get_file_handle());
#else
    if (!_stack) {
        _stack = get_stack();
    }
    if (_stack) {
        return _stack->get_ip_address();
    }
    return NULL;
#endif
}

void AT_CellularContext::attach(Callback<void(nsapi_event_t, intptr_t)> status_cb)
{
    _status_cb = status_cb;
}

nsapi_error_t AT_CellularContext::set_blocking(bool blocking)
{
    nsapi_error_t err = NSAPI_ERROR_OK;
    tr_info("CellularContext set blocking %d", blocking);
#if NSAPI_PPP_AVAILABLE
    err = nsapi_ppp_set_blocking(blocking);
#endif
    _is_blocking = blocking;
    return err;
}

void AT_CellularContext::set_plmn(const char *plmn)
{
    tr_info("CellularContext plmn %s", (plmn ? plmn : "NULL"));
    _device->set_plmn(plmn);
}

void AT_CellularContext::set_sim_pin(const char *sim_pin)
{
    _device->set_sim_pin(sim_pin);
}

nsapi_error_t AT_CellularContext::connect(const char *sim_pin, const char *apn, const char *uname,
                                          const char *pwd)
{
    set_sim_pin(sim_pin);
    set_credentials(apn, uname, pwd);
    return connect();
}

void AT_CellularContext::set_credentials(const char *apn, const char *uname, const char *pwd)
{
    _apn = apn;
    _uname = uname;
    _pwd = pwd;
}

pdp_type_t AT_CellularContext::string_to_pdp_type(const char *pdp_type_str)
{
    pdp_type_t pdp_type = DEFAULT_PDP_TYPE;
    int len = strlen(pdp_type_str);

    if (len == 6 && memcmp(pdp_type_str, "IPV4V6", len) == 0) {
        pdp_type = IPV4V6_PDP_TYPE;
    } else if (len == 4 && memcmp(pdp_type_str, "IPV6", len) == 0) {
        pdp_type = IPV6_PDP_TYPE;
    } else if (len == 2 && memcmp(pdp_type_str, "IP", len) == 0) {
        pdp_type = IPV4_PDP_TYPE;
    } else if (len == 6 && memcmp(pdp_type_str, "Non-IP", len) == 0) {
        pdp_type = NON_IP_PDP_TYPE;
    }
    return pdp_type;
}

// PDP Context handling
nsapi_error_t AT_CellularContext::delete_current_context()
{
    tr_info("Delete context %d", _cid);
    _at.clear_error();
    _at.cmd_start("AT+CGDCONT=");
    _at.write_int(_cid);
    _at.cmd_stop_read_resp();

    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        _cid = -1;
        _new_context_set = false;
    }

    return _at.get_last_error();
}

nsapi_error_t AT_CellularContext::do_user_authentication()
{
    // if user has defined user name and password we need to call CGAUTH before activating or modifying context
    if (_pwd && _uname) {
        if (!get_property(PROPERTY_AT_CGAUTH)) {
            return NSAPI_ERROR_UNSUPPORTED;
        }
        _at.cmd_start("AT+CGAUTH=");
        _at.write_int(_cid);
        _at.write_int(_authentication_type);

        const bool stored_debug_state = _at.get_debug();
        _at.set_debug(false);

        _at.write_string(_uname);
        _at.write_string(_pwd);

        _at.set_debug(stored_debug_state);

        _at.cmd_stop_read_resp();
        if (_at.get_last_error() != NSAPI_ERROR_OK) {
            return NSAPI_ERROR_AUTH_FAILURE;
        }
    }

    return NSAPI_ERROR_OK;
}

AT_CellularBase::CellularProperty AT_CellularContext::pdp_type_t_to_cellular_property(pdp_type_t pdp_type)
{
    AT_CellularBase::CellularProperty prop = PROPERTY_IPV4_PDP_TYPE;
    if (pdp_type == IPV6_PDP_TYPE) {
        prop = PROPERTY_IPV6_PDP_TYPE;
    } else if (pdp_type == IPV4V6_PDP_TYPE) {
        prop = PROPERTY_IPV4V6_PDP_TYPE;
    } else if (pdp_type == NON_IP_PDP_TYPE) {
        prop = PROPERTY_NON_IP_PDP_TYPE;
    }

    return prop;
}

bool AT_CellularContext::get_context()
{
    bool modem_supports_ipv6 = get_property(PROPERTY_IPV6_PDP_TYPE);
    bool modem_supports_ipv4 = get_property(PROPERTY_IPV4_PDP_TYPE);
    _at.cmd_start("AT+CGDCONT?");
    _at.cmd_stop();
    _at.resp_start("+CGDCONT:");
    _cid = -1;
    int cid_max = 0; // needed when creating new context
    char apn[MAX_ACCESSPOINT_NAME_LENGTH];
    int apn_len = 0;

    while (_at.info_resp()) {
        int cid = _at.read_int();
        if (cid > cid_max) {
            cid_max = cid;
        }
        char pdp_type_from_context[10];
        int pdp_type_len = _at.read_string(pdp_type_from_context, sizeof(pdp_type_from_context));
        if (pdp_type_len > 0) {
            apn_len = _at.read_string(apn, sizeof(apn));
            if (apn_len >= 0) {
                if (_apn && (strcmp(apn, _apn) != 0)) {
                    continue;
                }

                // APN matched -> Check PDP type
                pdp_type_t pdp_type = string_to_pdp_type(pdp_type_from_context);

                // Accept exact matching PDP context type or dual PDP context for IPv4/IPv6 only modems
                if (get_property(pdp_type_t_to_cellular_property(pdp_type)) ||
                        ((pdp_type == IPV4V6_PDP_TYPE && (modem_supports_ipv4 || modem_supports_ipv6)) && !_nonip_req)) {
                    _pdp_type = pdp_type;
                    _cid = cid;
                }
            }
        }
    }

    _at.resp_stop();
    if (_cid == -1) { // no suitable context was found so create a new one
        if (!set_new_context(cid_max + 1)) {
            return false;
        }
    }

    // save the apn
    if (apn_len > 0 && !_apn) {
        memcpy(_found_apn, apn, apn_len + 1);
    }

    tr_info("Found PDP context %d", _cid);

    return true;
}

bool AT_CellularContext::set_new_context(int cid)
{
    bool modem_supports_ipv6 = get_property(PROPERTY_IPV6_PDP_TYPE);
    bool modem_supports_ipv4 = get_property(PROPERTY_IPV4_PDP_TYPE);
    bool modem_supports_nonip = get_property(PROPERTY_NON_IP_PDP_TYPE);

    char pdp_type_str[8 + 1] = {0};
    pdp_type_t pdp_type = IPV4_PDP_TYPE;

    if (_nonip_req && _cp_in_use && modem_supports_nonip) {
        strncpy(pdp_type_str, "Non-IP", sizeof(pdp_type_str));
        pdp_type = NON_IP_PDP_TYPE;
    } else if (modem_supports_ipv6 && modem_supports_ipv4) {
        strncpy(pdp_type_str, "IPV4V6", sizeof(pdp_type_str));
        pdp_type = IPV4V6_PDP_TYPE;
    } else if (modem_supports_ipv6) {
        strncpy(pdp_type_str, "IPV6", sizeof(pdp_type_str));
        pdp_type = IPV6_PDP_TYPE;
    } else if (modem_supports_ipv4) {
        strncpy(pdp_type_str, "IP", sizeof(pdp_type_str));
        pdp_type = IPV4_PDP_TYPE;
    } else {
        return false;
    }

    //apn: "If the value is null or omitted, then the subscription value will be requested."
    bool success = false;
    _at.cmd_start("AT+CGDCONT=");
    _at.write_int(cid);
    _at.write_string(pdp_type_str);
    _at.write_string(_apn);
    _at.cmd_stop_read_resp();
    success = (_at.get_last_error() == NSAPI_ERROR_OK);

    if (success) {
        _pdp_type = pdp_type;
        _cid = cid;
        _new_context_set = true;
        tr_info("New PDP context %d, type %d", _cid, pdp_type);
    }

    return success;
}

nsapi_error_t AT_CellularContext::do_activate_context()
{
    if (_nonip_req && _cp_in_use) {
        return activate_non_ip_context();
    }

    // In IP case but also when Non-IP is requested and
    // control plane optimization is not established -> activate ip context
    _nonip_req = false;
    return activate_ip_context();
}

nsapi_error_t AT_CellularContext::activate_ip_context()
{
    return find_and_activate_context();
}

nsapi_error_t AT_CellularContext::activate_non_ip_context()
{
    return find_and_activate_context();
}

void AT_CellularContext::activate_context()
{
    tr_info("Activate PDP context %d", _cid);
    _at.cmd_start("AT+CGACT=1,");
    _at.write_int(_cid);
    _at.cmd_stop_read_resp();
    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        _is_context_activated = true;
    }
}

nsapi_error_t AT_CellularContext::find_and_activate_context()
{
    _at.lock();

    nsapi_error_t err = NSAPI_ERROR_OK;

    // try to find or create context of suitable type
    if (get_context()) {
#if NSAPI_PPP_AVAILABLE
        _at.unlock();
        // in PPP we don't activate any context but leave it to PPP stack
        return err;
#else
        // try to authenticate user before activating or modifying context
        err = do_user_authentication();
#endif // NSAPI_PPP_AVAILABLE
    } else {
        err = NSAPI_ERROR_NO_CONNECTION;
    }

    if (err != NSAPI_ERROR_OK) {
        _at.unlock();
        tr_error("Failed to activate network context! (%d)", err);
        return err;
    }

    // do check for stack to validate that we have support for stack
    if (!get_stack()) {
        _at.unlock();
        tr_error("No cellular stack!");
        return NSAPI_ERROR_UNSUPPORTED;
    }

    _is_context_active = false;
    _is_context_activated = false;

    _is_context_active = _nw->is_active_context(NULL, _cid);

    if (!_is_context_active) {
        activate_context();
    }

    err = (_at.get_last_error() == NSAPI_ERROR_OK) ? NSAPI_ERROR_OK : NSAPI_ERROR_NO_CONNECTION;

    // If new PDP context was created and failed to activate, delete it
    if (err != NSAPI_ERROR_OK && _new_context_set) {
        delete_current_context();
    } else if (err == NSAPI_ERROR_OK) {
        _is_context_active = true;
    }

    _at.unlock();

    return err;
}

void AT_CellularContext::do_connect()
{
    if (!_is_context_active) {
        _cb_data.error = do_activate_context();
    } else {
        _cb_data.error = NSAPI_ERROR_OK;
    }

#if !NSAPI_PPP_AVAILABLE
    // in PPP mode we did not activate any context, just searched the correct _cid
    if (_status_cb) {
        _status_cb((nsapi_event_t)CellularActivatePDPContext, (intptr_t)&_cb_data);
    }
#endif // !NSAPI_PPP_AVAILABLE

    if (_cb_data.error != NSAPI_ERROR_OK) {
        _is_connected = false;
        return;
    }
#if NSAPI_PPP_AVAILABLE
    if (_cb_data.error == NSAPI_ERROR_OK) {
        _at.lock();
        _cb_data.error = open_data_channel();
        _at.unlock();
        if (_cb_data.error != NSAPI_ERROR_OK) {
            tr_error("Failed to open data channel!");
            call_network_cb(NSAPI_STATUS_DISCONNECTED);
            _is_connected = false;
        } else {
            _is_context_activated = true;
        }
    }
#else
    _is_connected = true;
    call_network_cb(NSAPI_STATUS_GLOBAL_UP);
#endif
}

#if NSAPI_PPP_AVAILABLE
nsapi_error_t AT_CellularContext::open_data_channel()
{
    // If Non-IP in use fail
    if (_pdp_type == NON_IP_PDP_TYPE) {
        tr_error("Attempt of PPP connect over NON-IP: failed to CONNECT");
        return NSAPI_ERROR_PARAMETER;
    }

    tr_info("CellularContext PPP connect");
    if (get_property(PROPERTY_AT_CGDATA)) {
        _at.cmd_start("AT+CGDATA=\"PPP\",");
        _at.write_int(_cid);
    } else {
        MBED_ASSERT(_cid >= 0 && _cid <= 99);
        _at.cmd_start("ATD*99***");
        _at.use_delimiter(false);
        _at.write_int(_cid);
        _at.write_string("#", false);
        _at.use_delimiter(true);
    }
    _at.cmd_stop();

    _at.resp_start("CONNECT", true);
    if (_at.get_last_error()) {
        tr_error("Failed to CONNECT");
        return _at.get_last_error();
    }

    _at.set_is_filehandle_usable(false);
    enable_hup(true);
    /* Initialize PPP
     * If blocking: mbed_ppp_init() is a blocking call, it will block until
                  connected, or timeout after 30 seconds*/
    nsapi_error_t err = nsapi_ppp_connect(_at.get_file_handle(), callback(this, &AT_CellularContext::ppp_status_cb), _uname, _pwd, (nsapi_ip_stack_t)_pdp_type);
    if (err) {
        ppp_disconnected();
    }

    return err;
}

void AT_CellularContext::ppp_status_cb(nsapi_event_t ev, intptr_t ptr)
{
    tr_debug("ppp_status_cb: event %d, ptr %d", ev, ptr);
    if (ev == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_GLOBAL_UP) {
        _is_connected = true;
    } else {
        _is_connected = false;
    }

    _connect_status = (nsapi_connection_status_t)ptr;

    // catch all NSAPI_STATUS_DISCONNECTED events but send to device only when we did not ask for disconnect.
    if (ev == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_DISCONNECTED) {
        if (_is_connected) {
            ppp_disconnected();
            _device->cellular_callback(ev, ptr, this);
        }
        return;
    }

    // call device's callback, it will broadcast this to here (cellular_callback)
    _device->cellular_callback(ev, ptr, this);
}

void AT_CellularContext::ppp_disconnected()
{
    enable_hup(false);

    // after ppp disconnect if we wan't to use same at handler we need to set filehandle again to athandler so it
    // will set the correct sigio and nonblocking
    _at.lock();
    _at.set_is_filehandle_usable(true);
    if (!_at.sync(AT_SYNC_TIMEOUT)) { // consume extra characters after ppp disconnect, also it may take a while until modem listens AT commands
        tr_error("AT sync failed after PPP Disconnect");
    }
    _at.unlock();
}

#endif //#if NSAPI_PPP_AVAILABLE

nsapi_error_t AT_CellularContext::disconnect()
{
    tr_info("CellularContext disconnect()");
    if (!_nw || !_is_connected) {
        if (_new_context_set) {
            delete_current_context();
        }
        _cid = -1;
        return NSAPI_ERROR_NO_CONNECTION;
    }

    // set false here so callbacks know that we are not connected and so should not send DISCONNECTED
    _is_connected = false;
#if NSAPI_PPP_AVAILABLE
    nsapi_error_t err = nsapi_ppp_disconnect(_at.get_file_handle());
    if (err != NSAPI_ERROR_OK) {
        tr_error("CellularContext disconnect failed!");
        // continue even in failure due to ppp disconnect in any case releases filehandle
    }
    ppp_disconnected();
#endif // NSAPI_PPP_AVAILABLE
    _at.lock();

    // deactivate a context only if we have activated
    if (_is_context_activated) {
        if (_nonip_req && _cp_in_use) {
            deactivate_non_ip_context();
        } else {
            deactivate_ip_context();
        }
    }
    _is_context_active = false;
    _connect_status = NSAPI_STATUS_DISCONNECTED;

    // call device's callback, it will broadcast this to here (cellular_callback)
    _device->cellular_callback(NSAPI_EVENT_CONNECTION_STATUS_CHANGE, NSAPI_STATUS_DISCONNECTED, this);

    if (_new_context_set) {
        delete_current_context();
    }
    _cid = -1;

    return _at.unlock_return_error();
}

void AT_CellularContext::deactivate_ip_context()
{
    check_and_deactivate_context();
}

void AT_CellularContext::deactivate_non_ip_context()
{
    check_and_deactivate_context();
}

void AT_CellularContext::deactivate_context()
{
    _at.cmd_start("AT+CGACT=0,");
    _at.write_int(_cid);
    _at.cmd_stop_read_resp();
}

void AT_CellularContext::check_and_deactivate_context()
{
    // CGACT and CGATT commands might take up to 3 minutes to respond.
    _at.set_at_timeout(180 * 1000);
    int active_contexts_count = 0;
    _is_context_active = _nw->is_active_context(&active_contexts_count, _cid);

    CellularNetwork::RadioAccessTechnology rat = CellularNetwork::RAT_GSM;
    // always return NSAPI_ERROR_OK
    CellularNetwork::registration_params_t reg_params;
    _nw->get_registration_params(reg_params);
    rat = reg_params._act;
    // 3GPP TS 27.007:
    // For EPS, if an attempt is made to disconnect the last PDN connection, then the MT responds with ERROR
    if (_is_context_active && (rat < CellularNetwork::RAT_E_UTRAN || active_contexts_count > 1)) {
        _at.clear_error();
        deactivate_context();
    }

    if (_new_context_set) {
        _at.clear_error();
        _at.cmd_start("AT+CGDCONT=");
        _at.write_int(_cid);
        _at.cmd_stop_read_resp();
    }
}

nsapi_error_t AT_CellularContext::get_apn_backoff_timer(int &backoff_timer)
{
    // If apn is set
    if (_apn) {
        _at.lock();
        _at.cmd_start("AT+CABTRDP=");
        _at.write_string(_apn);
        _at.cmd_stop();
        _at.resp_start("+CABTRDP:");
        if (_at.info_resp()) {
            _at.skip_param();
            backoff_timer = _at.read_int();
        }
        _at.resp_stop();
        return _at.unlock_return_error();
    }

    return NSAPI_ERROR_PARAMETER;
}

nsapi_error_t AT_CellularContext::get_rate_control(
    CellularContext::RateControlExceptionReports &reports,
    CellularContext::RateControlUplinkTimeUnit &timeUnit, int &uplinkRate)
{
    _at.lock();

    _at.cmd_start("AT+CGAPNRC=");
    _at.write_int(_cid);
    _at.cmd_stop();

    _at.resp_start("+CGAPNRC:");
    _at.read_int();
    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        bool comma_found = true;
        int next_element = _at.read_int();
        if (next_element >= 0) {
            reports = (RateControlExceptionReports)next_element;
            next_element = _at.read_int();
        } else {
            comma_found = false;
        }

        if (comma_found && next_element >= 0) {
            timeUnit = (RateControlUplinkTimeUnit)next_element;
            next_element = _at.read_int();
        } else {
            comma_found = false;
        }

        if (comma_found && next_element >= 0) {
            uplinkRate = next_element;
        }
        if (_at.get_last_error() == NSAPI_ERROR_OK) {
            tr_debug("CGAPNRC: reports %d, time %d, rate %d", reports, timeUnit, uplinkRate);
        }
    }
    _at.resp_stop();

    return _at.unlock_return_error();
}

nsapi_error_t AT_CellularContext::get_pdpcontext_params(pdpContextList_t &params_list)
{
    const int ipv6_subnet_size = 128;
    const int max_ipv6_size = 64;
    char *ipv6_and_subnetmask = new char[ipv6_subnet_size];
    char *temp = new char[max_ipv6_size];

    _at.lock();

    _at.cmd_start("AT+CGCONTRDP=");
    _at.write_int(_cid);
    _at.cmd_stop();

    _at.resp_start("+CGCONTRDP:");
    pdpcontext_params_t *params = NULL;
    while (_at.info_resp()) { // response can be zero or many +CGDCONT lines
        params = params_list.add_new();
        params->cid = _at.read_int();
        params->bearer_id = _at.read_int();
        _at.read_string(params->apn, sizeof(params->apn));

        // rest are optional params
        ipv6_and_subnetmask[0] = '\0';
        temp[0] = '\0';
        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->local_addr, sizeof(params->local_addr), params->local_subnet_mask, sizeof(params->local_subnet_mask));
        ipv6_and_subnetmask[0] = '\0';

        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->gateway_addr, sizeof(params->gateway_addr), temp, max_ipv6_size);
        prefer_ipv6(params->gateway_addr, sizeof(params->gateway_addr), temp, max_ipv6_size);
        ipv6_and_subnetmask[0] = '\0';
        temp[0] = '\0';

        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->dns_primary_addr, sizeof(params->dns_primary_addr), temp, max_ipv6_size);
        prefer_ipv6(params->dns_primary_addr, sizeof(params->dns_primary_addr), temp, max_ipv6_size);
        ipv6_and_subnetmask[0] = '\0';
        temp[0] = '\0';

        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->dns_secondary_addr, sizeof(params->dns_secondary_addr), temp, max_ipv6_size);
        prefer_ipv6(params->dns_secondary_addr, sizeof(params->dns_secondary_addr), temp, max_ipv6_size);
        ipv6_and_subnetmask[0] = '\0';
        temp[0] = '\0';

        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->p_cscf_prim_addr, sizeof(params->p_cscf_prim_addr), temp, max_ipv6_size);
        prefer_ipv6(params->p_cscf_prim_addr, sizeof(params->p_cscf_prim_addr), temp, max_ipv6_size);
        ipv6_and_subnetmask[0] = '\0';
        temp[0] = '\0';

        _at.read_string(ipv6_and_subnetmask, ipv6_subnet_size);
        separate_ip_addresses(ipv6_and_subnetmask, params->p_cscf_sec_addr, sizeof(params->p_cscf_sec_addr), temp, max_ipv6_size);
        prefer_ipv6(params->p_cscf_sec_addr, sizeof(params->p_cscf_sec_addr), temp, max_ipv6_size);

        params->im_signalling_flag = _at.read_int();
        params->lipa_indication = _at.read_int();
        params->ipv4_mtu = _at.read_int();
        params->wlan_offload = _at.read_int();
        params->local_addr_ind = _at.read_int();
        params->non_ip_mtu = _at.read_int();
        params->serving_plmn_rate_control_value = _at.read_int();
    }
    _at.resp_stop();

    delete [] temp;
    delete [] ipv6_and_subnetmask;

    return _at.unlock_return_error();
}

// Called by CellularDevice for network and cellular device changes
void AT_CellularContext::cellular_callback(nsapi_event_t ev, intptr_t ptr)
{
    if (ev >= NSAPI_EVENT_CELLULAR_STATUS_BASE && ev <= NSAPI_EVENT_CELLULAR_STATUS_END) {
        cell_callback_data_t *data = (cell_callback_data_t *)ptr;
        cellular_connection_status_t st = (cellular_connection_status_t)ev;
        _cb_data.error = data->error;
#if USE_APN_LOOKUP
        if (st == CellularSIMStatusChanged && data->status_data == CellularDevice::SimStateReady &&
                _cb_data.error == NSAPI_ERROR_OK) {
            if (!_apn) {
                char imsi[MAX_IMSI_LENGTH + 1];
                wait(1); // need to wait to access SIM in some modems
                _cb_data.error = _device->open_information()->get_imsi(imsi, sizeof(imsi));
                if (_cb_data.error == NSAPI_ERROR_OK) {
                    const char *apn_config = apnconfig(imsi);
                    if (apn_config) {
                        const char *apn = _APN_GET(apn_config);
                        const char *uname = _APN_GET(apn_config);
                        const char *pwd = _APN_GET(apn_config);
                        tr_info("Looked up APN %s", apn);
                        set_credentials(apn, uname, pwd);
                    }
                } else {
                    tr_error("APN lookup failed");
                    _device->stop();
                    if (_is_blocking) {
                        // operation failed, release semaphore
                        _semaphore.release();
                    }
                }
                _device->close_information();
            }
        }
#endif // USE_APN_LOOKUP

        if (!_nw && st == CellularDeviceReady && data->error == NSAPI_ERROR_OK) {
            _nw = _device->open_network(_fh);
        }

        if (_cp_req && !_cp_in_use && (data->error == NSAPI_ERROR_OK) &&
                (st == CellularSIMStatusChanged && data->status_data == CellularDevice::SimStateReady)) {
            if (setup_control_plane_opt() != NSAPI_ERROR_OK) {
                tr_error("Control plane SETUP failed!");
            } else {
                tr_info("Control plane SETUP success!");
            }
        }

        if (_is_blocking) {
            if (data->error != NSAPI_ERROR_OK) {
                // operation failed, release semaphore
                _current_op = OP_INVALID;
                _semaphore.release();
            } else {
                if ((st == CellularDeviceReady && _current_op == OP_DEVICE_READY) ||
                        (st == CellularSIMStatusChanged && _current_op == OP_SIM_READY &&
                         data->status_data == CellularDevice::SimStateReady)) {
                    // target reached, release semaphore
                    _current_op = OP_INVALID;
                    _semaphore.release();
                } else if (st == CellularRegistrationStatusChanged && (data->status_data == CellularNetwork::RegisteredHomeNetwork ||
                                                                       data->status_data == CellularNetwork::RegisteredRoaming || data->status_data == CellularNetwork::AlreadyRegistered) && _current_op == OP_REGISTER) {
                    // target reached, release semaphore
                    _current_op = OP_INVALID;
                    _semaphore.release();
                } else if (st == CellularAttachNetwork && (_current_op == OP_ATTACH || _current_op == OP_CONNECT) &&
                           data->status_data == CellularNetwork::Attached) {
                    // target reached, release semaphore
                    _current_op = OP_INVALID;
                    _semaphore.release();
                }
            }
        } else {
            // non blocking
            if (st == CellularAttachNetwork && _current_op == OP_CONNECT && data->error == NSAPI_ERROR_OK &&
                    data->status_data == CellularNetwork::Attached) {
                _current_op = OP_INVALID;
                // forward to application
                if (_status_cb) {
                    _status_cb(ev, ptr);
                }
                do_connect();
                return;
            }
        }
    } else {
#if NSAPI_PPP_AVAILABLE
        if (_is_blocking) {
            if (ev == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_GLOBAL_UP) {
                tr_info("CellularContext IP %s", get_ip_address());
                _cb_data.error = NSAPI_ERROR_OK;
            } else if (ev == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_DISCONNECTED) {
                tr_info("PPP disconnected");
                _cb_data.error = NSAPI_ERROR_NO_CONNECTION;
            }
        }
#else
#if MBED_CONF_MBED_TRACE_ENABLE
        if (ev == NSAPI_EVENT_CONNECTION_STATUS_CHANGE && ptr == NSAPI_STATUS_DISCONNECTED) {
            tr_info("cb: CellularContext disconnected");
        }
#endif // MBED_CONF_MBED_TRACE_ENABLE
#endif // NSAPI_PPP_AVAILABLE
    }

    // forward to application
    if (_status_cb) {
        _status_cb(ev, ptr);
    }
}

void AT_CellularContext::call_network_cb(nsapi_connection_status_t status)
{
    if (_connect_status != status) {
        _connect_status = status;
        if (_status_cb) {
            _status_cb(NSAPI_EVENT_CONNECTION_STATUS_CHANGE, _connect_status);
        }
        if (_nw && _connect_status == NSAPI_STATUS_DISCONNECTED) {
            tr_info("CellularContext disconnected");
        }
    }
}

ControlPlane_netif *AT_CellularContext::get_cp_netif()
{
    tr_error("No control plane interface available from base context!");
    return NULL;
}

nsapi_error_t AT_CellularContext::setup_control_plane_opt()
{
    // check if control plane optimization already set
    mbed::CellularNetwork::CIoT_Supported_Opt supported_network_opt;

    if (_nw->get_ciot_network_optimization_config(supported_network_opt)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    if (supported_network_opt == mbed::CellularNetwork::CIOT_OPT_CONTROL_PLANE  ||
            supported_network_opt == mbed::CellularNetwork::CIOT_OPT_BOTH) {
        _cp_in_use = true;
        return NSAPI_ERROR_OK;
    }

    // ciot optimization not set by app so need to set it now
    nsapi_error_t ciot_opt_ret;
    ciot_opt_ret = _nw->set_ciot_optimization_config(mbed::CellularNetwork::CIOT_OPT_CONTROL_PLANE,
                                                     mbed::CellularNetwork::PREFERRED_UE_OPT_CONTROL_PLANE,
                                                     callback(this, &AT_CellularContext::ciot_opt_cb));

    if (ciot_opt_ret != NSAPI_ERROR_OK) {
        return ciot_opt_ret;
    }

    //wait for control plane opt call back to release semaphore
    _cp_opt_semaphore.wait(CP_OPT_NW_REPLY_TIMEOUT);

    if (_cp_in_use) {
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_DEVICE_ERROR;
}

void AT_CellularContext::ciot_opt_cb(mbed::CellularNetwork::CIoT_Supported_Opt  ciot_opt)
{
    if (ciot_opt == mbed::CellularNetwork::CIOT_OPT_CONTROL_PLANE ||
            ciot_opt == mbed::CellularNetwork::CIOT_OPT_BOTH) {
        _cp_in_use = true;
    }
    _cp_opt_semaphore.release();
}

void AT_CellularContext::set_disconnect()
{
    tr_debug("AT_CellularContext::set_disconnect()");
    _is_connected = false;
    _device->cellular_callback(NSAPI_EVENT_CONNECTION_STATUS_CHANGE, NSAPI_STATUS_DISCONNECTED, this);
}
