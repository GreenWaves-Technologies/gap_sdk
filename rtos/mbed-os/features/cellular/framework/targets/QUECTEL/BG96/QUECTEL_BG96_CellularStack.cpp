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

#include "QUECTEL/BG96/QUECTEL_BG96_CellularStack.h"
#include "CellularLog.h"

using namespace mbed;

const char *QIURC_RECV = "recv";
const uint8_t QIURC_RECV_LENGTH = 4;
const char *QIURC_CLOSED = "closed";
const uint8_t QIURC_CLOSED_LENGTH = 6;
const uint8_t MAX_QIURC_LENGTH = QIURC_CLOSED_LENGTH;

QUECTEL_BG96_CellularStack::QUECTEL_BG96_CellularStack(ATHandler &atHandler, int cid, nsapi_ip_stack_t stack_type) : AT_CellularStack(atHandler, cid, stack_type)
{
    _at.set_urc_handler("+QIURC:", mbed::Callback<void()>(this, &QUECTEL_BG96_CellularStack::urc_qiurc));
}

QUECTEL_BG96_CellularStack::~QUECTEL_BG96_CellularStack()
{
}

nsapi_error_t QUECTEL_BG96_CellularStack::socket_listen(nsapi_socket_t handle, int backlog)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t QUECTEL_BG96_CellularStack::socket_accept(void *server, void **socket, SocketAddress *addr)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t QUECTEL_BG96_CellularStack::socket_connect(nsapi_socket_t handle, const SocketAddress &address)
{
    CellularSocket *socket = (CellularSocket *)handle;

    int modem_connect_id = -1;
    int request_connect_id = socket->id;
    int err = -1;

    _at.lock();
    if (socket->proto == NSAPI_TCP) {
        _at.cmd_start("AT+QIOPEN=");
        _at.write_int(_cid);
        _at.write_int(request_connect_id);
        _at.write_string("TCP");
        _at.write_string(address.get_ip_address());
        _at.write_int(address.get_port());
        _at.write_int(socket->localAddress.get_port());
        _at.write_int(0);
        _at.cmd_stop();

        handle_open_socket_response(modem_connect_id, err);

        if ((_at.get_last_error() == NSAPI_ERROR_OK) && err) {
            if (err == BG96_SOCKET_BIND_FAIL) {
                socket->created = false;
                _at.unlock();
                return NSAPI_ERROR_PARAMETER;
            }
            _at.cmd_start("AT+QICLOSE=");
            _at.write_int(modem_connect_id);
            _at.cmd_stop();
            _at.resp_start();
            _at.resp_stop();

            _at.cmd_start("AT+QIOPEN=");
            _at.write_int(_cid);
            _at.write_int(request_connect_id);
            _at.write_string("TCP");
            _at.write_string(address.get_ip_address());
            _at.write_int(address.get_port());
            _at.write_int(socket->localAddress.get_port());
            _at.write_int(0);
            _at.cmd_stop();

            handle_open_socket_response(modem_connect_id, err);
        }
    }

    // If opened successfully BUT not requested one, close it
    if (!err && (modem_connect_id != request_connect_id)) {
        _at.cmd_start("AT+QICLOSE=");
        _at.write_int(modem_connect_id);
        _at.cmd_stop();
        _at.resp_start();
        _at.resp_stop();
    }

    nsapi_error_t ret_val = _at.get_last_error();
    _at.unlock();

    if ((ret_val == NSAPI_ERROR_OK) && (modem_connect_id == request_connect_id)) {
        socket->created = true;
        socket->remoteAddress = address;
        socket->connected = true;
        return NSAPI_ERROR_OK;
    }

    return NSAPI_ERROR_NO_CONNECTION;
}

void QUECTEL_BG96_CellularStack::urc_qiurc()
{
    char urc_string[MAX_QIURC_LENGTH + 1];
    _at.lock();
    const int urc_string_length = _at.read_string(urc_string, sizeof(urc_string));
    const int sock_id = _at.read_int();
    const nsapi_error_t err = _at.unlock_return_error();

    if (err != NSAPI_ERROR_OK) {
        return;
    }

    bool recv = strcmp(urc_string, "recv") == 0;
    bool closed = strcmp(urc_string, "closed") == 0;

    CellularSocket *sock = find_socket(sock_id);
    if (sock) {
        if (closed) {
            tr_error("Socket closed %d", sock_id);
            sock->closed = true;
        }

        if (recv || closed) {
            if (sock->_cb) {
                sock->_cb(sock->_data);
            }
        }
    }
}

int QUECTEL_BG96_CellularStack::get_max_socket_count()
{
    return BG96_SOCKET_MAX;
}

bool QUECTEL_BG96_CellularStack::is_protocol_supported(nsapi_protocol_t protocol)
{
    return (protocol == NSAPI_UDP || protocol == NSAPI_TCP);
}

nsapi_error_t QUECTEL_BG96_CellularStack::socket_close_impl(int sock_id)
{
    _at.set_at_timeout(BG96_CLOSE_SOCKET_TIMEOUT);
    _at.cmd_start("AT+QICLOSE=");
    _at.write_int(sock_id);
    _at.cmd_stop_read_resp();
    _at.restore_at_timeout();

    return _at.get_last_error();
}

void QUECTEL_BG96_CellularStack::handle_open_socket_response(int &modem_connect_id, int &err)
{
    // OK
    _at.resp_start();
    _at.resp_stop();
    // QIOPEN -> should be handled as URC?
    _at.set_at_timeout(BG96_CREATE_SOCKET_TIMEOUT);
    _at.resp_start("+QIOPEN:");
    _at.restore_at_timeout();
    modem_connect_id = _at.read_int();
    err = _at.read_int();
}

nsapi_error_t QUECTEL_BG96_CellularStack::create_socket_impl(CellularSocket *socket)
{
    int modem_connect_id = -1;
    int request_connect_id = socket->id;
    int remote_port = 0;
    int err = -1;

    if (socket->proto == NSAPI_UDP && !socket->connected) {
        _at.cmd_start("AT+QIOPEN=");
        _at.write_int(_cid);
        _at.write_int(request_connect_id);
        _at.write_string("UDP SERVICE");
        _at.write_string("127.0.0.1");
        _at.write_int(remote_port);
        _at.write_int(socket->localAddress.get_port());
        _at.write_int(0);
        _at.cmd_stop();

        handle_open_socket_response(modem_connect_id, err);

        if ((_at.get_last_error() == NSAPI_ERROR_OK) && err) {
            if (err == BG96_SOCKET_BIND_FAIL) {
                socket->created = false;
                return NSAPI_ERROR_PARAMETER;
            }
            _at.cmd_start("AT+QICLOSE=");
            _at.write_int(modem_connect_id);
            _at.cmd_stop_read_resp();

            _at.cmd_start("AT+QIOPEN=");
            _at.write_int(_cid);
            _at.write_int(request_connect_id);
            _at.write_string("UDP SERVICE");
            _at.write_string("127.0.0.1");
            _at.write_int(remote_port);
            _at.write_int(socket->localAddress.get_port());
            _at.write_int(0);
            _at.cmd_stop();

            handle_open_socket_response(modem_connect_id, err);
        }
    } else if (socket->proto == NSAPI_UDP && socket->connected) {
        _at.cmd_start("AT+QIOPEN=");
        _at.write_int(_cid);
        _at.write_int(request_connect_id);
        _at.write_string("UDP");
        _at.write_string(socket->remoteAddress.get_ip_address());
        _at.write_int(socket->remoteAddress.get_port());
        _at.cmd_stop();

        handle_open_socket_response(modem_connect_id, err);

        if ((_at.get_last_error() == NSAPI_ERROR_OK) && err) {
            if (err == BG96_SOCKET_BIND_FAIL) {
                socket->created = false;
                return NSAPI_ERROR_PARAMETER;
            }
            _at.cmd_start("AT+QICLOSE=");
            _at.write_int(modem_connect_id);
            _at.cmd_stop_read_resp();

            _at.cmd_start("AT+QIOPEN=");
            _at.write_int(_cid);
            _at.write_int(request_connect_id);
            _at.write_string("UDP");
            _at.write_string(socket->remoteAddress.get_ip_address());
            _at.write_int(socket->remoteAddress.get_port());
            _at.cmd_stop();

            handle_open_socket_response(modem_connect_id, err);
        }
    }

    // If opened successfully BUT not requested one, close it
    if (!err && (modem_connect_id != request_connect_id)) {
        _at.cmd_start("AT+QICLOSE=");
        _at.write_int(modem_connect_id);
        _at.cmd_stop_read_resp();
    }

    nsapi_error_t ret_val = _at.get_last_error();

    socket->created = ((ret_val == NSAPI_ERROR_OK) && (modem_connect_id == request_connect_id));

    return ret_val;
}

nsapi_size_or_error_t QUECTEL_BG96_CellularStack::socket_sendto_impl(CellularSocket *socket, const SocketAddress &address,
                                                                     const void *data, nsapi_size_t size)
{
    if (size > BG96_MAX_SEND_SIZE) {
        return NSAPI_ERROR_PARAMETER;
    }

    int sent_len = 0;
    int sent_len_before = 0;
    int sent_len_after = 0;

    // Get the sent count before sending
    _at.cmd_start("AT+QISEND=");
    _at.write_int(socket->id);
    _at.write_int(0);
    _at.cmd_stop();

    _at.resp_start("+QISEND:");
    sent_len_before = _at.read_int();
    _at.resp_stop();

    // Send
    _at.cmd_start("AT+QISEND=");
    _at.write_int(socket->id);
    _at.write_int(size);
    if (socket->proto == NSAPI_UDP) {
        _at.write_string(address.get_ip_address());
        _at.write_int(address.get_port());
    }
    _at.cmd_stop();

    _at.resp_start(">");
    _at.write_bytes((uint8_t *)data, size);
    _at.resp_start();
    _at.set_stop_tag("\r\n");
    _at.resp_stop();

    // Get the sent count after sending
    _at.cmd_start("AT+QISEND=");
    _at.write_int(socket->id);
    _at.write_int(0);
    _at.cmd_stop();

    _at.resp_start("+QISEND:");
    sent_len_after = _at.read_int();
    _at.resp_stop();

    if (_at.get_last_error() == NSAPI_ERROR_OK) {
        sent_len = sent_len_after - sent_len_before;
        return sent_len;
    }

    return _at.get_last_error();
}

nsapi_size_or_error_t QUECTEL_BG96_CellularStack::socket_recvfrom_impl(CellularSocket *socket, SocketAddress *address,
                                                                       void *buffer, nsapi_size_t size)
{
    nsapi_size_or_error_t recv_len = 0;
    int port;
    char ip_address[NSAPI_IP_SIZE + 1];

    _at.cmd_start("AT+QIRD=");
    _at.write_int(socket->id);
    if (socket->proto == NSAPI_TCP) {
        // do not read more than max size
        size = size > BG96_MAX_RECV_SIZE ? BG96_MAX_RECV_SIZE : size;
        _at.write_int(size);
    }
    _at.cmd_stop();

    _at.resp_start("+QIRD:");
    recv_len = _at.read_int();
    _at.read_string(ip_address, sizeof(ip_address));
    port = _at.read_int();
    if (recv_len > 0) {
        // do not read more than buffer size
        recv_len = recv_len > (nsapi_size_or_error_t)size ? size : recv_len;
        _at.read_bytes((uint8_t *)buffer, recv_len);
    }
    _at.resp_stop();

    // We block only if 0 recv length really means no data.
    // If 0 is followed by ip address and port can be an UDP 0 length packet
    if (!recv_len && port < 0) {
        return NSAPI_ERROR_WOULD_BLOCK;
    }

    if (address) {
        address->set_ip_address(ip_address);
        address->set_port(port);
    }

    return recv_len;
}
