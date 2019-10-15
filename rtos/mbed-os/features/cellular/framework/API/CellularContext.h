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
#ifndef _CELLULARCONTEXT_H_
#define _CELLULARCONTEXT_H_

#include "CellularInterface.h"
#include "CellularDevice.h"
#include "ControlPlane_netif.h"
#include "PinNames.h"

/** @file CellularContext.h
 * @brief Cellular PDP context class
 *
 */

namespace mbed {

typedef enum pdp_type {
    DEFAULT_PDP_TYPE = DEFAULT_STACK,
    IPV4_PDP_TYPE = IPV4_STACK,
    IPV6_PDP_TYPE = IPV6_STACK,
    IPV4V6_PDP_TYPE = IPV4V6_STACK,
    NON_IP_PDP_TYPE
} pdp_type_t;

/**
 * @addtogroup cellular
 * @{
 */

/// CellularContext is CellularInterface/NetworkInterface with extensions for cellular connectivity
class CellularContext : public CellularInterface {

public:

    // max simultaneous PDP contexts active
    static const int PDP_CONTEXT_COUNT = 4;

    /* authentication type when activating or modifying the pdp context */
    enum AuthenticationType {
        NOAUTH = 0,
        PAP,
        CHAP
    };

    /*  whether the additional exception reports are allowed to be sent when the maximum uplink rate is reached */
    enum RateControlExceptionReports {
        NotAllowedToBeSent = 0,
        AllowedToBeSent
    };

    /* specifies the time unit to be used for the maximum uplink rate */
    enum RateControlUplinkTimeUnit {
        Unrestricted = 0,
        Minute,
        Hour,
        Day,
        Week
    };

    /// PDP Context information
    struct pdpcontext_params_t {
        char apn[MAX_ACCESSPOINT_NAME_LENGTH + 1];
        char local_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char local_subnet_mask[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char gateway_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char dns_primary_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char dns_secondary_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char p_cscf_prim_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        char p_cscf_sec_addr[MAX_IPV6_ADDR_IN_IPV4LIKE_DOTTED_FORMAT + 1];
        int cid;
        int bearer_id;
        int im_signalling_flag;
        int lipa_indication;
        int ipv4_mtu;
        int wlan_offload;
        int local_addr_ind;
        int non_ip_mtu;
        int serving_plmn_rate_control_value;
        pdpcontext_params_t *next;

        pdpcontext_params_t()
        {
            apn[0] = '\0';
            local_addr[0] = '\0';
            local_subnet_mask[0] = '\0';
            gateway_addr[0] = '\0';
            dns_primary_addr[0] = '\0';
            dns_secondary_addr[0] = '\0';
            p_cscf_prim_addr[0] = '\0';
            p_cscf_sec_addr[0] = '\0';
            cid = -1;
            bearer_id = -1;
            im_signalling_flag = -1;
            lipa_indication = -1;
            ipv4_mtu = -1;
            wlan_offload = -1;
            local_addr_ind = -1;
            non_ip_mtu = -1;
            serving_plmn_rate_control_value = -1;
            next = NULL;
        }
    };
    typedef CellularList<pdpcontext_params_t> pdpContextList_t;

    // pointer for next item when used as a linked list
    CellularContext *_next;
protected:
    // friend of CellularDevice, so it's the only way to close or delete this class.
    friend class CellularDevice;
    virtual ~CellularContext() {}
public: // from NetworkInterface
    virtual nsapi_error_t set_blocking(bool blocking) = 0;
    virtual NetworkStack *get_stack() = 0;
    virtual const char *get_ip_address() = 0;

    /** Register callback for status reporting.
     *
     *  The specified status callback function is called on the network, and the cellular device status changes.
     *  The parameters on the callback are the event type and event type dependent reason parameter.
     *
     *  @remark  deleting CellularDevice/CellularContext in callback is not allowed.
     *  @remark  Allocating/adding lots of traces not recommended as callback is called mostly from State machines thread which
     *           is now 2048. You can change to main thread for example via EventQueue.
     *
     *  @param status_cb The callback for status changes.
     */
    virtual void attach(mbed::Callback<void(nsapi_event_t, intptr_t)> status_cb) = 0;
    virtual nsapi_error_t connect() = 0;
    virtual nsapi_error_t disconnect() = 0;

    // from CellularInterface
    virtual void set_plmn(const char *plmn) = 0;
    virtual void set_sim_pin(const char *sim_pin) = 0;
    virtual nsapi_error_t connect(const char *sim_pin, const char *apn = 0, const char *uname = 0,
                                  const char *pwd = 0) = 0;
    virtual void set_credentials(const char *apn, const char *uname = 0, const char *pwd = 0) = 0;
    virtual const char *get_netmask() = 0;
    virtual const char *get_gateway() = 0;
    virtual bool is_connected() = 0;

    /** Same as NetworkInterface::get_default_instance()
     *
     *  @note not to be used if get_default_nonip_instance() was already used
     *
     */
    static CellularContext *get_default_instance();


    /** Instantiates a default Non-IP cellular interface
     *
     *  This function creates a new Non-IP PDP context.
     *
     *  @note not to be used if get_default_instance() was already used
     *
     *  @return         A Non-IP cellular PDP context
     *
     */
    static CellularContext *get_default_nonip_instance();


// Operations, can be sync/async. Also Connect() is this kind of operation, inherited from NetworkInterface above.

    /** Start the interface
     *
     *  Initializes the modem for communication.
     *  By default, this API is synchronous. API can be set to asynchronous with method set_blocking(...).
     *  In synchronous and asynchronous mode application can get result in from callback which is set with
     *  attach(...)
     *
     *  @return         NSAPI_ERROR_OK on success
     *                  NSAPI_ERROR_NO_MEMORY on case of memory failure
     */
    virtual nsapi_error_t set_device_ready() = 0;

    /** Start the interface
     *
     *  Attempts to open the SIM.
     *  By default, this API is synchronous. API can be set to asynchronous with method set_blocking(...).
     *  In synchronous and asynchronous mode, the application can get result in from callback, which is set with
     *  attach(...)
     *
     *  @return         NSAPI_ERROR_OK on success
     *                  NSAPI_ERROR_NO_MEMORY on case of memory failure
     */
    virtual nsapi_error_t set_sim_ready() = 0;

    /** Start the interface
     *
     *  Attempts to register the device to cellular network.
     *  By default, this API is synchronous. API can be set to asynchronous with method set_blocking(...).
     *  In synchronous and asynchronous mode, the application can get result in from callback, which is set with
     *  attach(...)
     *
     *  @return         NSAPI_ERROR_OK on success
     *                  NSAPI_ERROR_NO_MEMORY on case of memory failure
     */
    virtual nsapi_error_t register_to_network() = 0;

    /** Start the interface
     *
     *  Attempts to attach the device to cellular network.
     *  By default, this API is synchronous. API can be set to asynchronous with method set_blocking(...).
     *  In synchronous and asynchronous mode, the application can get result in from callback, which is set with
     *  attach(...)
     *
     *  @return         NSAPI_ERROR_OK on success
     *                  NSAPI_ERROR_NO_MEMORY on case of memory failure
     */
    virtual nsapi_error_t attach_to_network() = 0;

// PDP Context specific functions

    /** Get APN rate control.
     *
     *  @remark optional params are not updated if not received from network, so use good defaults
     *  @param reports       Additional exception reports at maximum rate reached are allowed to be sent [optional]
     *  @param time_unit     Uplink time unit with values 0=unrestricted, 1=minute, 2=hour, 3=day, 4=week [optional]
     *  @param uplink_rate   Maximum number of messages per timeUnit [optional]
     *  @return              NSAPI_ERROR_OK on success
     *                       NSAPI_ERROR_DEVICE_ERROR on case of failure
     */
    virtual nsapi_error_t get_rate_control(CellularContext::RateControlExceptionReports &reports,
                                           CellularContext::RateControlUplinkTimeUnit &time_unit, int &uplink_rate) = 0;

    /** Get the relevant information for an active nonsecondary PDP context.
     *
     *  @remark optional params are not updated if not received from network.
     *  @param params_list   reference to linked list, which is filled on successful call
     *  @return              NSAPI_ERROR_OK on success
     *                       NSAPI_ERROR_NO_MEMORY on memory failure
     *                       NSAPI_ERROR_DEVICE_ERROR on other failures
     */
    virtual nsapi_error_t get_pdpcontext_params(pdpContextList_t &params_list) = 0;

    /** Get backoff timer value
     *
     *  @param backoff_timer Backoff timer value associated with PDP APN in seconds
     *  @return              NSAPI_ERROR_OK on success
     *                       NSAPI_ERROR_PARAMETER if no access point is set or found when activating context
     *                       NSAPI_ERROR_DEVICE_ERROR on failure
     */
    virtual nsapi_error_t get_apn_backoff_timer(int &backoff_timer) = 0;

    /** Set the file handle used to communicate with the modem. You can use this to change the default file handle.
     *
     *  @param fh   file handle for communicating with the modem
     */
    virtual void set_file_handle(FileHandle *fh) = 0;

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN) || defined(DOXYGEN_ONLY)
    /** Set the UART serial used to communicate with the modem. Can be used to change default file handle.
     *  File handle set with this method will use data carrier detect to be able to detect disconnection much faster in PPP mode.
     *
     *  @param serial       UARTSerial used in communication to modem. If null then the default file handle is used.
     *  @param dcd_pin      Pin used to set data carrier detect on/off for the given UART
     *  @param active_high  a boolean set to true if DCD polarity is active low
     */
    virtual void set_file_handle(UARTSerial *serial, PinName dcd_pin = NC, bool active_high = false) = 0;
#endif // #if DEVICE_SERIAL

    /** Returns the control plane AT command interface
     */
    virtual ControlPlane_netif *get_cp_netif() = 0;

protected: // Device specific implementations might need these so protected
    enum ContextOperation {
        OP_INVALID      = -1,
        OP_DEVICE_READY = 0,
        OP_SIM_READY    = 1,
        OP_REGISTER     = 2,
        OP_ATTACH       = 3,
        OP_CONNECT      = 4,
        OP_MAX          = 5
    };

    /** The CellularDevice calls the status callback function on status changes on the network or CellularDevice.
    *
    *  @param ev   event type
    *  @param ptr  event-type dependent reason parameter
    */
    virtual void cellular_callback(nsapi_event_t ev, intptr_t ptr) = 0;

    /** Enable or disable hang-up detection
     *
     *  When in PPP data pump mode, it is helpful if the FileHandle will signal hang-up via
     *  POLLHUP, e.g., if the DCD line is deasserted on a UART. During command mode, this
     *  signaling is not desired. enable_hup() controls whether this function should be
     *  active.
     */
    virtual void enable_hup(bool enable) = 0;

    /** Triggers control plane's operations needed when control plane data is received,
     *  like socket event, for example.
     */
    void cp_data_received();

    // member variables needed in target override methods
    NetworkStack *_stack; // must be pointer because of PPP
    pdp_type_t _pdp_type;
    CellularContext::AuthenticationType _authentication_type;
    nsapi_connection_status_t _connect_status;
    cell_callback_data_t _cb_data;
    Callback<void(nsapi_event_t, intptr_t)> _status_cb;
    int _cid;
    bool _new_context_set;
    bool _is_context_active;
    bool _is_context_activated; // did we activate the context
    const char *_apn;
    const char *_uname;
    const char *_pwd;
    PinName _dcd_pin;
    bool _active_high;

    ControlPlane_netif *_cp_netif;
};

/**
 * @}
 */

} // namespace mbed


#endif /* _CELLULARCONTEXT_H_ */
