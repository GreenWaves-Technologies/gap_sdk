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

#include "QUECTEL_BC95_CellularStack.h"
#include "CellularUtil.h"
#include "CellularLog.h"

#define PACKET_SIZE_MAX 1358

using namespace mbed;
using namespace mbed_cellular_util;

QUECTEL_BC95_CellularStack::QUECTEL_BC95_CellularStack(ATHandler &atHandler, int cid, nsapi_ip_stack_t stack_type) : AT_CellularStack(atHandler, cid, stack_type)
{
    _at.set_urc_handler("+NSONMI:", mbed::Callback<void()>(this, &QUECTEL_BC95_CellularStack::urc_nsonmi));
    _at.set_urc_handler("+NSOCLI:", mbed::Callback<void()>(this, &QUECTEL_BC95_CellularStack::urc_nsocli));
}

QUECTEL_BC95_CellularStack::~QUECTEL_BC95_CellularStack()
{
    _at.set_urc_handler("+NSONMI:", NULL);
    _at.set_urc_handler("+NSOCLI:", NULL);
}

nsapi_error_t QUECTEL_BC95_CellularStack::socket_listen(nsapi_socket_t handle, int backlog)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t QUECTEL_BC95_CellularStack::socket_accept(void *server, void **socket, SocketAddress *addr)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t QUECTEL_BC95_CellularStack::socket_connect(nsapi_socket_t handle, const SocketAddress &address)
{
    CellularSocket *socket = (CellularSocket *)handle;

    _at.lock();
    if (!socket->created) {
        const nsapi_error_t error_create = create_socket_impl(socket);
        if (error_create != NSAPI_ERROR_OK) {
            return error_create;
        }
    }

    _at.cmd_start("AT+NSOCO=");
    _at.write_int(socket->id);
    _at.write_string(address.get_ip_address(), false);
    _at.write_int(address.get_port());
    _at.cmd_stop();
    _at.resp_start();
    _at.resp_stop();
    _at.unlock();

    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        socket->remoteAddress = address;
        socket->connected = true;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_NO_CONNECTION;
}

void QUECTEL_BC95_CellularStack::urc_nsonmi()
{
    int sock_id = _at.read_int();

    for (int i = 0; i < get_max_socket_count(); i++) {
        CellularSocket *sock = _socket[i];
        if (sock && sock->id == sock_id) {
            if (sock->_cb) {
                sock->_cb(sock->_data);
            }
            break;
        }
    }
}

void QUECTEL_BC95_CellularStack::urc_nsocli()
{
    int sock_id = _at.read_int();

    const nsapi_error_t err = _at.get_last_error();

    if (err != NSAPI_ERROR_OK) {
        return;
    }

    CellularSocket *sock = find_socket(sock_id);

    if (sock) {
        sock->closed = true;
        if (sock->_cb) {
            sock->_cb(sock->_data);
        }
        tr_info("Socket closed %d", sock_id);
    }
}


int QUECTEL_BC95_CellularStack::get_max_socket_count()
{
    return BC95_SOCKET_MAX;
}

bool QUECTEL_BC95_CellularStack::is_protocol_supported(nsapi_protocol_t protocol)
{
    return (protocol == NSAPI_UDP || protocol == NSAPI_TCP);
}

nsapi_error_t QUECTEL_BC95_CellularStack::socket_close_impl(int sock_id)
{
    CellularSocket *sock = find_socket(sock_id);

    if (sock && sock->closed) {
        return NSAPI_ERROR_OK;
    }
    _at.cmd_start("AT+NSOCL=");
    _at.write_int(sock_id);
    _at.cmd_stop_read_resp();

    tr_info("Close socket: %d error: %d", sock_id, _at.get_last_error());

    return _at.get_last_error();
}

nsapi_error_t QUECTEL_BC95_CellularStack::create_socket_impl(CellularSocket *socket)
{
    int sock_id = -1;
    bool socketOpenWorking = false;

    if (socket->proto == NSAPI_UDP) {
        _at.cmd_start("AT+NSOCR=DGRAM,17,");
    } else if (socket->proto == NSAPI_TCP) {
        _at.cmd_start("AT+NSOCR=STREAM,6,");
    } else {
        return NSAPI_ERROR_PARAMETER;
    }
    _at.write_int(socket->localAddress.get_port());
    _at.write_int(1);
    _at.cmd_stop();
    _at.resp_start();
    sock_id = _at.read_int();
    _at.resp_stop();

    socketOpenWorking = (_at.get_last_error() == NSAPI_ERROR_OK);

    if (!socketOpenWorking) {
        _at.cmd_start("AT+NSOCL=0");
        _at.cmd_stop_read_resp();

        if (socket->proto == NSAPI_UDP) {
            _at.cmd_start("AT+NSOCR=DGRAM,17,");
        } else if (socket->proto == NSAPI_TCP) {
            _at.cmd_start("AT+NSOCR=STREAM,6,");
        }
        _at.write_int(socket->localAddress.get_port());
        _at.write_int(1);
        _at.cmd_stop();
        _at.resp_start();
        sock_id = _at.read_int();
        _at.resp_stop();

        socketOpenWorking = (_at.get_last_error() == NSAPI_ERROR_OK);
    }

    if (!socketOpenWorking || (sock_id == -1)) {
        tr_error("Socket create failed!");
        return NSAPI_ERROR_NO_SOCKET;
    }

    // Check for duplicate socket id delivered by modem
    for (int i = 0; i < BC95_SOCKET_MAX; i++) {
        CellularSocket *sock = _socket[i];
        if (sock && sock->created && sock->id == sock_id) {
            tr_error("Duplicate socket index: %d created:%d, sock_id: %d", i, sock->created, sock_id);
            return NSAPI_ERROR_NO_SOCKET;
        }
    }

    tr_info("Socket create id: %d", sock_id);

    socket->id = sock_id;
    socket->created = true;

    return NSAPI_ERROR_OK;
}

nsapi_size_or_error_t QUECTEL_BC95_CellularStack::socket_sendto_impl(CellularSocket *socket, const SocketAddress &address,
                                                                     const void *data, nsapi_size_t size)
{
    int sent_len = 0;

    if (size > PACKET_SIZE_MAX) {
        return NSAPI_ERROR_PARAMETER;
    }

    char *hexstr = new char[size * 2 + 1];
    int hexlen = char_str_to_hex_str((const char *)data, size, hexstr);
    // NULL terminated for write_string
    hexstr[hexlen] = 0;

    if (socket->proto == NSAPI_UDP) {
        _at.cmd_start("AT+NSOST=");
        _at.write_int(socket->id);
        _at.write_string(address.get_ip_address(), false);
        _at.write_int(address.get_port());
        _at.write_int(size);
    } else if (socket->proto == NSAPI_TCP) {
        _at.cmd_start("AT+NSOSD=");
        _at.write_int(socket->id);
        _at.write_int(size);
    } else {
        delete [] hexstr;
        return NSAPI_ERROR_PARAMETER;
    }

    _at.write_string(hexstr, false);
    _at.cmd_stop();
    _at.resp_start();
    // skip socket id
    _at.skip_param();
    sent_len = _at.read_int();
    _at.resp_stop();

    delete [] hexstr;

    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        return sent_len;
    }

    return _at.get_last_error();
}

nsapi_size_or_error_t QUECTEL_BC95_CellularStack::socket_recvfrom_impl(CellularSocket *socket, SocketAddress *address,
                                                                       void *buffer, nsapi_size_t size)
{
    nsapi_size_or_error_t recv_len = 0;
    int port;
    char ip_address[NSAPI_IP_SIZE];

    _at.cmd_start("AT+NSORF=");
    _at.write_int(socket->id);
    _at.write_int(size <= PACKET_SIZE_MAX ? size : PACKET_SIZE_MAX);
    _at.cmd_stop();
    _at.resp_start();
    // receiving socket id
    _at.skip_param();
    _at.read_string(ip_address, sizeof(ip_address));
    port = _at.read_int();
    recv_len = _at.read_int();
    int hexlen = _at.read_hex_string((char *)buffer, size);
    // remaining length
    _at.skip_param();
    _at.resp_stop();

    if (!recv_len || (recv_len == -1) || (_at.get_last_error() != NSAPI_ERROR_OK)) {
        return NSAPI_ERROR_WOULD_BLOCK;
    }

    if (address) {
        address->set_ip_address(ip_address);
        address->set_port(port);
    }

    if (recv_len != hexlen) {
        tr_error("Not received as much data as expected. Should receive: %d bytes, received: %d bytes", recv_len, hexlen);
    }
    return recv_len;
}
