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
#ifndef AT_CELLULARCONTEXT_H_
#define AT_CELLULARCONTEXT_H_

#include "CellularContext.h"
#include "AT_CellularBase.h"
#include "Semaphore.h"

const int MAX_APN_LENGTH = 63 + 1;

namespace mbed {

class AT_CellularContext : public CellularContext, public AT_CellularBase {
public:
    AT_CellularContext(ATHandler &at, CellularDevice *device, const char *apn = 0, bool cp_req = false, bool nonip_req = false);
    virtual ~AT_CellularContext();

// from CellularBase/NetworkInterface
    virtual nsapi_error_t set_blocking(bool blocking);
    virtual NetworkStack *get_stack();
    virtual const char *get_ip_address();
    virtual void attach(mbed::Callback<void(nsapi_event_t, intptr_t)> status_cb);
    virtual nsapi_error_t connect();
    virtual nsapi_error_t disconnect();
    virtual nsapi_connection_status_t get_connection_status() const;
    virtual bool is_connected();
    // from CellularBase
    virtual void set_plmn(const char *plmn);
    virtual void set_sim_pin(const char *sim_pin);
    virtual nsapi_error_t connect(const char *sim_pin, const char *apn = 0, const char *uname = 0,
                                  const char *pwd = 0);
    virtual void set_credentials(const char *apn, const char *uname = 0, const char *pwd = 0);
    virtual const char *get_netmask();
    virtual const char *get_gateway();

// from CellularContext
    virtual nsapi_error_t get_pdpcontext_params(pdpContextList_t &params_list);
    virtual nsapi_error_t get_rate_control(CellularContext::RateControlExceptionReports &reports,
                                           CellularContext::RateControlUplinkTimeUnit &time_unit, int &uplink_rate);
    virtual nsapi_error_t get_apn_backoff_timer(int &backoff_timer);
    virtual nsapi_error_t set_device_ready();
    virtual nsapi_error_t set_sim_ready();
    virtual nsapi_error_t register_to_network();
    virtual nsapi_error_t attach_to_network();
    virtual void set_file_handle(FileHandle *fh);
#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
    virtual void set_file_handle(UARTSerial *serial, PinName dcd_pin = NC, bool active_high = false);
#endif // #if DEVICE_SERIAL
    virtual void enable_hup(bool enable);

    virtual ControlPlane_netif *get_cp_netif();

protected:
    virtual void cellular_callback(nsapi_event_t ev, intptr_t ptr);

    /** Does the authentication for the PDP Context if user name and password are provided.
     *  Can be overridden by the modem target if 3GPP default implementation if not an option
     *
     *  @return NSAPI_ERROR_OK if no credentials provided or authentication was successful
     *          NSAPI_ERROR_AUTH_FAILURE if authentication failed
     *          NSAPI_ERROR_DEVICE_ERROR if communication with the modemm failed
     */
    virtual nsapi_error_t do_user_authentication();

    /** Activates PDP context or in PPP mode opens data channel.
     *  Can be overridden by the modem target if 3GPP default implementation if not an option
     */
    virtual void do_connect();

    /** Get the operation specific timeout. Used in synchronous mode when setting the maximum
     *   waiting time. Modem specific implementation can override this to provide different timeouts.
     *
     *  @param  op  current operation
     *  @return     timeout in milliseconds
     */
    virtual uint32_t get_timeout_for_operation(ContextOperation op) const;

    /** Helper method to call callback function if it is provided
     *
     *  @param status connection status which is parameter in callback function
     */
    void call_network_cb(nsapi_connection_status_t status);

    virtual nsapi_error_t activate_non_ip_context();
    virtual nsapi_error_t setup_control_plane_opt();
    virtual void deactivate_non_ip_context();
    virtual void deactivate_ip_context();
    virtual void set_disconnect();
    virtual void deactivate_context();
private:
#if NSAPI_PPP_AVAILABLE
    nsapi_error_t open_data_channel();
    void ppp_status_cb(nsapi_event_t ev, intptr_t ptr);
    void ppp_disconnected();
#endif // #if NSAPI_PPP_AVAILABLE
    nsapi_error_t do_activate_context();
    virtual void activate_context();
    nsapi_error_t find_and_activate_context();
    nsapi_error_t activate_ip_context();
    void check_and_deactivate_context();
    bool set_new_context(int cid);
    bool get_context();
    nsapi_error_t delete_current_context();
    pdp_type_t string_to_pdp_type(const char *pdp_type);
    nsapi_error_t check_operation(nsapi_error_t err, ContextOperation op);
    AT_CellularBase::CellularProperty pdp_type_t_to_cellular_property(pdp_type_t pdp_type);
    void ciot_opt_cb(mbed::CellularNetwork::CIoT_Supported_Opt ciot_opt);

private:
    bool _is_connected;
    bool _is_blocking;
    ContextOperation  _current_op;
    char _found_apn[MAX_APN_LENGTH];
    CellularDevice *_device;
    CellularNetwork *_nw;
    FileHandle *_fh;
    rtos::Semaphore _semaphore;
    rtos::Semaphore _cp_opt_semaphore;

protected:
    // flag indicating if CP was requested to be setup
    bool _cp_req;
    // flag indicating if Non-IP context was requested to be setup
    bool _nonip_req;

    // tells if CCIOTOPTI received green from network for CP optimization use
    bool _cp_in_use;
};

} // namespace mbed

#endif // AT_CELLULARCONTEXT_H_
