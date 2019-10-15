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

#ifndef AT_CELLULAR_STACK_H_
#define AT_CELLULAR_STACK_H_

#include "AT_CellularBase.h"
#include "NetworkStack.h"

namespace mbed {

// <PDP_addr_1> and <PDP_addr_2>: each is a string type that identifies the MT in the address space applicable to the PDP.
// The string is given as dot-separated numeric (0-255) parameter of the form:
// a1.a2.a3.a4 for IPv4 and
// a1.a2.a3.a4.a5.a6.a7.a8.a9.a10.a11.a12.a13.a14.a15.a16 for IPv6.
#define PDP_IPV6_SIZE 63+1

/**
 * Class AT_CellularStack.
 *
 * Implements NetworkStack and introduces interface for modem specific stack implementations.
 */
class AT_CellularStack : public NetworkStack, public AT_CellularBase {

public:
    AT_CellularStack(ATHandler &at, int cid, nsapi_ip_stack_t stack_type);
    virtual ~AT_CellularStack();

public: // NetworkStack

    virtual const char *get_ip_address();
protected: // NetworkStack

    /**
     * Modem specific socket stack initialization
     *
     *  @return 0 on success
     */
    virtual nsapi_error_t socket_stack_init();

    /**
      * Note: Socket_open does not actually open socket on all drivers, but that's deferred until calling `sendto`.
      * The reason is that IP stack implementations are very much modem specific and it's quite common that when a
      * socket is created (via AT commands) it must also be given remote IP address, and that is usually known
      * only when calling `sendto`.
      */
    virtual nsapi_error_t socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto);

    virtual nsapi_error_t socket_close(nsapi_socket_t handle);

    virtual nsapi_error_t socket_bind(nsapi_socket_t handle, const SocketAddress &address);

    virtual nsapi_error_t socket_listen(nsapi_socket_t handle, int backlog);

    virtual nsapi_error_t socket_connect(nsapi_socket_t handle, const SocketAddress &address);

    virtual nsapi_error_t socket_accept(nsapi_socket_t server,
                                        nsapi_socket_t *handle, SocketAddress *address = 0);

    virtual nsapi_size_or_error_t socket_send(nsapi_socket_t handle,
                                              const void *data, nsapi_size_t size);

    virtual nsapi_size_or_error_t socket_recv(nsapi_socket_t handle,
                                              void *data, nsapi_size_t size);

    virtual nsapi_size_or_error_t socket_sendto(nsapi_socket_t handle, const SocketAddress &address,
                                                const void *data, nsapi_size_t size);

    virtual nsapi_size_or_error_t socket_recvfrom(nsapi_socket_t handle, SocketAddress *address,
                                                  void *buffer, nsapi_size_t size);

    virtual void socket_attach(nsapi_socket_t handle, void (*callback)(void *), void *data);

protected:

    class CellularSocket {
    public:
        CellularSocket() :
            id(-1),
            connected(false),
            proto(NSAPI_UDP),
            remoteAddress("", 0),
            localAddress("", 0),
            _cb(NULL),
            _data(NULL),
            created(false),
            closed(false),
            started(false),
            tx_ready(false),
            rx_avail(false),
            pending_bytes(0)
        {
        }
        // Socket identifier, generally it will be the socket ID assigned by the
        // modem. In a few special cases, modems may take that as an input argument.
        int id;
        // Being connected means remote ip address and port are set
        bool connected;
        nsapi_protocol_t proto;
        SocketAddress remoteAddress;
        SocketAddress localAddress;
        void (*_cb)(void *);
        void *_data;
        bool created; // socket has been created on modem stack
        bool closed; // socket has been closed by a peer
        bool started; // socket has been opened on modem stack
        bool tx_ready; // socket is ready for sending on modem stack
        bool rx_avail; // socket has data for reading on modem stack
        nsapi_size_t pending_bytes; // The number of received bytes pending
    };

    /**
    * Gets maximum number of sockets modem supports
    */
    virtual int get_max_socket_count() = 0;

    /**
    * Checks if modem supports the given protocol
    *
    * @param protocol   Protocol type
    */
    virtual bool is_protocol_supported(nsapi_protocol_t protocol) = 0;

    /**
    * Implements modem specific AT command set for socket closing
    *
    * @param sock_id   Socket id
    */
    virtual nsapi_error_t socket_close_impl(int sock_id) = 0;

    /**
    * Implements modem specific AT command set for creating socket
    *
    * @param socket   Cellular socket handle
    */
    virtual nsapi_error_t create_socket_impl(CellularSocket *socket) = 0;

    /**
    * Implements modem specific AT command set for sending data
    *
    * @param socket   Cellular socket handle
    * @param address  The SocketAddress of the remote host
    * @param data     Buffer of data to send to the host
    * @param size     Size of the buffer in bytes
    * @return         Number of sent bytes on success, negative error
    *                 code on failure
    */
    virtual nsapi_size_or_error_t socket_sendto_impl(CellularSocket *socket, const SocketAddress &address,
                                                     const void *data, nsapi_size_t size) = 0;

    /**
     *  Implements modem specific AT command set for receiving data
     *
     *  @param socket   Socket handle
     *  @param address  Destination for the source address or NULL
     *  @param buffer   Destination buffer for data received from the host
     *  @param size     Size of the buffer in bytes
     *  @return         Number of received bytes on success, negative error
     *                  code on failure
     */
    virtual nsapi_size_or_error_t socket_recvfrom_impl(CellularSocket *socket, SocketAddress *address,
                                                       void *buffer, nsapi_size_t size) = 0;

    /**
     *  Find the socket handle based on socket identifier
     *
     *  @param sock_id  Socket identifier
     *  @return         Socket handle, NULL on error
     */
    CellularSocket *find_socket(int sock_id);

    // socket container
    CellularSocket **_socket;

    // number of socket slots allocated in socket container
    int _socket_count;

    // IP address
    char _ip[PDP_IPV6_SIZE];

    // PDP context id
    int _cid;

    // stack type from PDP context
    nsapi_ip_stack_t _stack_type;

private:
    int find_socket_index(nsapi_socket_t handle);

    int get_socket_index_by_port(uint16_t port);

    // mutex for write/read to a _socket array, needed when multiple threads may open sockets simultaneously
    PlatformMutex _socket_mutex;
};

} // namespace mbed

#endif // AT_CELLULAR_STACK_H_
