/* ESP8266 implementation of NetworkInterfaceAPI
 * Copyright (c) 2015 ARM Limited
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

#ifndef ESP8266_INTERFACE_H
#define ESP8266_INTERFACE_H

#if DEVICE_SERIAL && DEVICE_INTERRUPTIN && defined(MBED_CONF_EVENTS_PRESENT) && defined(MBED_CONF_NSAPI_PRESENT) && defined(MBED_CONF_RTOS_PRESENT)
#include "drivers/DigitalOut.h"
#include "drivers/Timer.h"
#include "ESP8266/ESP8266.h"
#include "events/EventQueue.h"
#include "events/mbed_shared_queues.h"
#include "features/netsocket/NetworkInterface.h"
#include "features/netsocket/NetworkStack.h"
#include "features/netsocket/nsapi_types.h"
#include "features/netsocket/SocketAddress.h"
#include "features/netsocket/WiFiAccessPoint.h"
#include "features/netsocket/WiFiInterface.h"
#include "platform/Callback.h"
#include "rtos/ConditionVariable.h"
#include "rtos/Mutex.h"

#define ESP8266_SOCKET_COUNT 5

#define ESP8266_INTERFACE_CONNECT_INTERVAL_MS (5000)
#define ESP8266_INTERFACE_CONNECT_TIMEOUT_MS (2 * ESP8266_CONNECT_TIMEOUT + ESP8266_INTERFACE_CONNECT_INTERVAL_MS)

#ifdef TARGET_FF_ARDUINO
#ifndef MBED_CONF_ESP8266_TX
#define MBED_CONF_ESP8266_TX D1
#endif

#ifndef MBED_CONF_ESP8266_RX
#define MBED_CONF_ESP8266_RX D0
#endif
#endif /* TARGET_FF_ARDUINO */

/** ESP8266Interface class
 *  Implementation of the NetworkStack for the ESP8266
 */
class ESP8266Interface : public NetworkStack, public WiFiInterface {
public:
#if defined MBED_CONF_ESP8266_TX && defined MBED_CONF_ESP8266_RX
    /**
     * @brief ESP8266Interface default constructor
     *        Will use values defined in mbed_lib.json
     */
    ESP8266Interface();
#endif

    /** ESP8266Interface lifetime
     * @param tx        TX pin
     * @param rx        RX pin
     * @param debug     Enable debugging
     */
    ESP8266Interface(PinName tx, PinName rx, bool debug = false, PinName rts = NC, PinName cts = NC, PinName rst = NC);

    /**
     * @brief ESP8266Interface default destructor
     */
    virtual ~ESP8266Interface();

    /** Start the interface
     *
     *  Attempts to connect to a WiFi network. Requires ssid and passphrase to be set.
     *  If passphrase is invalid, NSAPI_ERROR_AUTH_ERROR is returned.
     *
     *  @return         0 on success, negative error code on failure
     */
    virtual int connect();

    /** Start the interface
     *
     *  Attempts to connect to a WiFi network.
     *
     *  If interface is configured blocking it will timeout after up to
     *  ESP8266_INTERFACE_CONNECT_TIMEOUT_MS + ESP8266_CONNECT_TIMEOUT ms.
     *
     *  @param ssid      Name of the network to connect to
     *  @param pass      Security passphrase to connect to the network
     *  @param security  Type of encryption for connection (Default: NSAPI_SECURITY_NONE)
     *  @param channel   This parameter is not supported, setting it to anything else than 0 will result in NSAPI_ERROR_UNSUPPORTED
     *  @return          0 on success, or error code on failure
     */
    virtual int connect(const char *ssid, const char *pass, nsapi_security_t security = NSAPI_SECURITY_NONE,
                        uint8_t channel = 0);

    /** Set the WiFi network credentials
     *
     *  @param ssid      Name of the network to connect to
     *  @param pass      Security passphrase to connect to the network
     *  @param security  Type of encryption for connection
     *                   (defaults to NSAPI_SECURITY_NONE)
     *  @return          0 on success, or error code on failure
     */
    virtual int set_credentials(const char *ssid, const char *pass, nsapi_security_t security = NSAPI_SECURITY_NONE);

    /** Set the WiFi network channel - NOT SUPPORTED
     *
     * This function is not supported and will return NSAPI_ERROR_UNSUPPORTED
     *
     *  @param channel   Channel on which the connection is to be made, or 0 for any (Default: 0)
     *  @return          Not supported, returns NSAPI_ERROR_UNSUPPORTED
     */
    virtual int set_channel(uint8_t channel);

    /** Stop the interface
     *  @return             0 on success, negative on failure
     */
    virtual int disconnect();

    /** Get the internally stored IP address
     *  @return             IP address of the interface or null if not yet connected
     */
    virtual const char *get_ip_address();

    /** Get the internally stored MAC address
     *  @return             MAC address of the interface
     */
    virtual const char *get_mac_address();

    /** Get the local gateway
    *
    *  @return         Null-terminated representation of the local gateway
    *                  or null if no network mask has been recieved
    */
    virtual const char *get_gateway();

    /** Get the local network mask
     *
     *  @return         Null-terminated representation of the local network mask
     *                  or null if no network mask has been recieved
     */
    virtual const char *get_netmask();

    /** Gets the current radio signal strength for active connection
     *
     * @return          Connection strength in dBm (negative value)
     */
    virtual int8_t get_rssi();

    /** Scan for available networks
     *
     * This function will block.
     *
     * @param  ap       Pointer to allocated array to store discovered AP
     * @param  count    Size of allocated @a res array, or 0 to only count available AP
     * @param  timeout  Timeout in milliseconds; 0 for no timeout (Default: 0)
     * @return          Number of entries in @a, or if @a count was 0 number of available networks, negative on error
     *                  see @a nsapi_error
     */
    virtual int scan(WiFiAccessPoint *res, unsigned count);

    /** Translates a hostname to an IP address with specific version
     *
     *  The hostname may be either a domain name or an IP address. If the
     *  hostname is an IP address, no network transactions will be performed.
     *
     *  If no stack-specific DNS resolution is provided, the hostname
     *  will be resolve using a UDP socket on the stack.
     *
     *  @param address  Destination for the host SocketAddress
     *  @param host     Hostname to resolve
     *  @param version  IP version of address to resolve, NSAPI_UNSPEC indicates
     *                  version is chosen by the stack (defaults to NSAPI_UNSPEC)
     *  @return         0 on success, negative error code on failure
     */
    using NetworkInterface::gethostbyname;

    /** Add a domain name server to list of servers to query
     *
     *  @param addr     Destination for the host address
     *  @return         0 on success, negative error code on failure
     */
    using NetworkInterface::add_dns_server;

    /** @copydoc NetworkStack::setsockopt
     */
    virtual nsapi_error_t setsockopt(nsapi_socket_t handle, int level,
                                     int optname, const void *optval, unsigned optlen);

    /** @copydoc NetworkStack::getsockopt
     */
    virtual nsapi_error_t getsockopt(nsapi_socket_t handle, int level, int optname,
                                     void *optval, unsigned *optlen);

    /** Register callback for status reporting
     *
     *  The specified status callback function will be called on status changes
     *  on the network. The parameters on the callback are the event type and
     *  event-type dependent reason parameter.
     *
     *  In ESP8266 the callback will be called when processing OOB-messages via
     *  AT-parser. Do NOT call any ESP8266Interface -functions or do extensive
     *  processing in the callback.
     *
     *  @param status_cb The callback for status changes
     */
    virtual void attach(mbed::Callback<void(nsapi_event_t, intptr_t)> status_cb);

    /** Get the connection status
     *
     *  @return         The connection status according to ConnectionStatusType
     */
    virtual nsapi_connection_status_t get_connection_status() const;

protected:
    /** Open a socket
     *  @param handle       Handle in which to store new socket
     *  @param proto        Type of socket to open, NSAPI_TCP or NSAPI_UDP
     *  @return             0 on success, negative on failure
     */
    virtual int socket_open(void **handle, nsapi_protocol_t proto);

    /** Close the socket
     *  @param handle       Socket handle
     *  @return             0 on success, negative on failure
     *  @note On failure, any memory associated with the socket must still
     *        be cleaned up
     */
    virtual int socket_close(void *handle);

    /** Bind a server socket to a specific port
     *  @param handle       Socket handle
     *  @param address      Local address to listen for incoming connections on
     *  @return             0 on success, negative on failure.
     */
    virtual int socket_bind(void *handle, const SocketAddress &address);

    /** Start listening for incoming connections
     *  @param handle       Socket handle
     *  @param backlog      Number of pending connections that can be queued up at any
     *                      one time [Default: 1]
     *  @return             0 on success, negative on failure
     */
    virtual int socket_listen(void *handle, int backlog);

    /** Connects this TCP socket to the server
     *  @param handle       Socket handle
     *  @param address      SocketAddress to connect to
     *  @return             0 on success, negative on failure
     */
    virtual int socket_connect(void *handle, const SocketAddress &address);

    /** Accept a new connection.
     *  @param handle       Handle in which to store new socket
     *  @param server       Socket handle to server to accept from
     *  @return             0 on success, negative on failure
     *  @note This call is not-blocking, if this call would block, must
     *        immediately return NSAPI_ERROR_WOULD_WAIT
     */
    virtual int socket_accept(void *handle, void **socket, SocketAddress *address);

    /** Send data to the remote host
     *  @param handle       Socket handle
     *  @param data         The buffer to send to the host
     *  @param size         The length of the buffer to send
     *  @return             Number of written bytes on success, negative on failure
     *  @note This call is not-blocking, if this call would block, must
     *        immediately return NSAPI_ERROR_WOULD_WAIT
     */
    virtual int socket_send(void *handle, const void *data, unsigned size);

    /** Receive data from the remote host
     *  @param handle       Socket handle
     *  @param data         The buffer in which to store the data received from the host
     *  @param size         The maximum length of the buffer
     *  @return             Number of received bytes on success, negative on failure
     *  @note This call is not-blocking, if this call would block, must
     *        immediately return NSAPI_ERROR_WOULD_WAIT
     */
    virtual int socket_recv(void *handle, void *data, unsigned size);

    /** Send a packet to a remote endpoint
     *  @param handle       Socket handle
     *  @param address      The remote SocketAddress
     *  @param data         The packet to be sent
     *  @param size         The length of the packet to be sent
     *  @return             The number of written bytes on success, negative on failure
     *  @note This call is not-blocking, if this call would block, must
     *        immediately return NSAPI_ERROR_WOULD_WAIT
     */
    virtual int socket_sendto(void *handle, const SocketAddress &address, const void *data, unsigned size);

    /** Receive a packet from a remote endpoint
     *  @param handle       Socket handle
     *  @param address      Destination for the remote SocketAddress or null
     *  @param buffer       The buffer for storing the incoming packet data
     *                      If a packet is too long to fit in the supplied buffer,
     *                      excess bytes are discarded
     *  @param size         The length of the buffer
     *  @return             The number of received bytes on success, negative on failure
     *  @note This call is not-blocking, if this call would block, must
     *        immediately return NSAPI_ERROR_WOULD_WAIT
     */
    virtual int socket_recvfrom(void *handle, SocketAddress *address, void *buffer, unsigned size);

    /** Register a callback on state change of the socket
     *  @param handle       Socket handle
     *  @param callback     Function to call on state change
     *  @param data         Argument to pass to callback
     *  @note Callback may be called in an interrupt context.
     */
    virtual void socket_attach(void *handle, void (*callback)(void *), void *data);

    /** Provide access to the NetworkStack object
     *
     *  @return The underlying NetworkStack object
     */
    virtual NetworkStack *get_stack()
    {
        return this;
    }

    /** Set blocking status of connect() which by default should be blocking.
     *
     *  @param blocking Use true to make connect() blocking.
     *  @return         NSAPI_ERROR_OK on success, negative error code on failure.
     */
    virtual nsapi_error_t set_blocking(bool blocking);

private:
    // AT layer
    ESP8266 _esp;
    void refresh_conn_state_cb();

    // HW reset pin
    class ResetPin {
    public:
        ResetPin(PinName rst_pin);
        void rst_assert();
        void rst_deassert();
        bool is_connected();
    private:
        mbed::DigitalOut  _rst_pin;
    } _rst_pin;


    // Credentials
    static const int ESP8266_SSID_MAX_LENGTH = 32; /* 32 is what 802.11 defines as longest possible name */
    char ap_ssid[ESP8266_SSID_MAX_LENGTH + 1]; /* The longest possible name; +1 for the \0 */
    static const int ESP8266_PASSPHRASE_MAX_LENGTH = 63; /* The longest allowed passphrase */
    static const int ESP8266_PASSPHRASE_MIN_LENGTH = 8; /* The shortest allowed passphrase */
    char ap_pass[ESP8266_PASSPHRASE_MAX_LENGTH + 1]; /* The longest possible passphrase; +1 for the \0 */
    nsapi_security_t _ap_sec;

    bool _if_blocking; // NetworkInterface, blocking or not
    rtos::ConditionVariable _if_connected;

    // connect status reporting
    nsapi_error_t _conn_status_to_error();
    mbed::Timer _conn_timer;

    // Drivers's socket info
    struct _sock_info {
        bool open;
        uint16_t sport;
    };
    struct _sock_info _sock_i[ESP8266_SOCKET_COUNT];

    // Driver's state
    int _initialized;
    nsapi_error_t _connect_retval;
    bool _get_firmware_ok();
    nsapi_error_t _init(void);
    nsapi_error_t _reset();

    //sigio
    struct {
        void (*callback)(void *);
        void *data;
        uint8_t deferred;
    } _cbs[ESP8266_SOCKET_COUNT];
    void event();
    void event_deferred();

    // Connection state reporting to application
    nsapi_connection_status_t _conn_stat;
    mbed::Callback<void(nsapi_event_t, intptr_t)> _conn_stat_cb;

    // Background OOB processing
    // Use global EventQueue
    events::EventQueue *_global_event_queue;
    int _oob_event_id;
    int _connect_event_id;
    void proc_oob_evnt();
    void _connect_async();
    rtos::Mutex _cmutex; // Protect asynchronous connection logic

};
#endif
#endif
