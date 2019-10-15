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
#include "gtest/gtest.h"
#include "AT_CellularStack.h"
#include <string.h>
#include "AT_CellularNetwork.h"
#include "events/EventQueue.h"
#include "ATHandler.h"
#include "AT_CellularStack.h"
#include "FileHandle_stub.h"
#include "CellularLog.h"
#include "ATHandler_stub.h"
#include "SocketAddress.h"

using namespace mbed;
using namespace events;

class MyStack : public AT_CellularStack {
public:
    bool bool_value;
    bool max_sock_value;
    nsapi_error_t create_error;
    CellularSocket socket;

    MyStack(ATHandler &atr, int cid, nsapi_ip_stack_t typ) : AT_CellularStack(atr, cid, typ)
    {
        bool_value = false;
        max_sock_value = 0;
        create_error = NSAPI_ERROR_OK;
    }

    virtual int get_max_socket_count()
    {
        return max_sock_value;
    }

    virtual bool is_protocol_supported(nsapi_protocol_t protocol)
    {
        return bool_value;
    }

    virtual nsapi_error_t socket_close_impl(int sock_id)
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t create_socket_impl(CellularSocket *socket)
    {
        return create_error;
    }

    virtual nsapi_size_or_error_t socket_sendto_impl(CellularSocket *socket, const SocketAddress &address,
                                                     const void *data, nsapi_size_t size)
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_size_or_error_t socket_recvfrom_impl(CellularSocket *socket, SocketAddress *address,
                                                       void *buffer, nsapi_size_t size)
    {
        return NSAPI_ERROR_OK;
    }

    virtual nsapi_error_t socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto)
    {
        return AT_CellularStack::socket_open(handle, proto);
    }

    virtual nsapi_error_t socket_close(nsapi_socket_t handle)
    {
        return AT_CellularStack::socket_close(handle);
    }

    virtual nsapi_error_t socket_bind(nsapi_socket_t handle, const SocketAddress &address)
    {
        return AT_CellularStack::socket_bind(handle, address);
    }

    virtual nsapi_error_t socket_listen(nsapi_socket_t handle, int backlog)
    {
        return AT_CellularStack::socket_listen(handle, backlog);
    }

    virtual nsapi_error_t socket_connect(nsapi_socket_t handle, const SocketAddress &address)
    {
        return AT_CellularStack::socket_connect(handle, address);
    }

    virtual nsapi_error_t socket_accept(nsapi_socket_t server,
                                        nsapi_socket_t *handle, SocketAddress *address = 0)
    {
        return AT_CellularStack::socket_accept(server, handle, address);
    }

    virtual nsapi_size_or_error_t socket_send(nsapi_socket_t handle,
                                              const void *data, nsapi_size_t size)
    {
        return AT_CellularStack::socket_send(handle, data, size);
    }

    virtual nsapi_size_or_error_t socket_recv(nsapi_socket_t handle,
                                              void *data, nsapi_size_t size)
    {
        return AT_CellularStack::socket_recv(handle, data, size);
    }

    virtual nsapi_size_or_error_t socket_sendto(nsapi_socket_t handle, const SocketAddress &address,
                                                const void *data, nsapi_size_t size)
    {
        return AT_CellularStack::socket_sendto(handle, address, data, size);
    }

    virtual nsapi_size_or_error_t socket_recvfrom(nsapi_socket_t handle, SocketAddress *address,
                                                  void *buffer, nsapi_size_t size)
    {
        return AT_CellularStack::socket_recvfrom(handle, address, buffer, size);
    }

    virtual void socket_attach(nsapi_socket_t handle, void (*callback)(void *), void *data)
    {
        return AT_CellularStack::socket_attach(handle, callback, data);
    }
};

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestAT_CellularStack : public testing::Test {
protected:

    void SetUp()
    {
        ATHandler_stub::return_given_size = false;
        ATHandler_stub::nsapi_error_value = NSAPI_ERROR_OK;
        ATHandler_stub::ssize_value = 0;
        ATHandler_stub::bool_value = false;
        ATHandler_stub::read_string_value = NULL;
    }

    void TearDown()
    {
    }
};
// *INDENT-ON*

TEST_F(TestAT_CellularStack, Create)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack *st = new MyStack(at, 0, IPV4_STACK);

    EXPECT_TRUE(st != NULL);
    delete st;
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_get_ip_address)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    EXPECT_EQ(strlen(st.get_ip_address()), 0);

    char table[] = "1.2.3.4.5.65.7.8.9.10.11\0";
    ATHandler_stub::ssize_value = -1;
    ATHandler_stub::bool_value = true;
    ATHandler_stub::read_string_value = table;
    EXPECT_TRUE(st.get_ip_address() == NULL);

    ATHandler_stub::ssize_value = strlen(table);
    ATHandler_stub::bool_value = true;
    ATHandler_stub::read_string_value = table;
    EXPECT_TRUE(st.get_ip_address());
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_open)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    st.bool_value = false;
    EXPECT_EQ(st.socket_open(NULL, NSAPI_TCP), NSAPI_ERROR_UNSUPPORTED);

    st.bool_value = true;
    st.max_sock_value = 0;
    nsapi_socket_t sock = &st.socket;
    EXPECT_EQ(st.socket_open(&sock, NSAPI_TCP), NSAPI_ERROR_NO_SOCKET);

    MyStack st2(at, 0, IPV6_STACK);
    st2.bool_value = true;
    st2.max_sock_value = 1;
    sock = &st2.socket;
    EXPECT_EQ(st2.socket_open(&sock, NSAPI_TCP), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_close)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    EXPECT_EQ(st.socket_close(&st.socket), NSAPI_ERROR_DEVICE_ERROR);

    nsapi_socket_t sock = &st.socket;
    st.bool_value = true;
    st.max_sock_value = 1;
    EXPECT_EQ(st.socket_open(&sock, NSAPI_TCP), NSAPI_ERROR_OK);
    st.max_sock_value = 0;
    EXPECT_EQ(st.socket_close(sock), NSAPI_ERROR_DEVICE_ERROR);

    MyStack st2(at, 0, IPV6_STACK);
    st2.max_sock_value = 1;
    st2.bool_value = true;
    sock = &st2.socket;
    EXPECT_EQ(st2.socket_open(&sock, NSAPI_TCP), NSAPI_ERROR_OK);
    EXPECT_EQ(st2.socket_close(sock), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_bind)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    SocketAddress addr;
    ATHandler_stub::nsapi_error_value = NSAPI_ERROR_ALREADY;
    EXPECT_EQ(st.socket_bind(NULL, addr), NSAPI_ERROR_DEVICE_ERROR);

    EXPECT_EQ(st.socket_bind(&st.socket, addr), NSAPI_ERROR_ALREADY);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_listen)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    EXPECT_EQ(st.socket_listen(&st.socket, 4), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_connect)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    SocketAddress addr;
    EXPECT_EQ(st.socket_connect(NULL, addr), NSAPI_ERROR_DEVICE_ERROR);

    EXPECT_EQ(st.socket_connect(&st.socket, addr), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_accept)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    nsapi_socket_t sock = &st.socket;
    EXPECT_EQ(st.socket_accept(NULL, &sock), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_send)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    EXPECT_EQ(st.socket_send(NULL, "addr", 4), NSAPI_ERROR_DEVICE_ERROR);

    EXPECT_EQ(st.socket_send(&st.socket, "addr", 4), NSAPI_ERROR_DEVICE_ERROR);

    SocketAddress addr;
    st.max_sock_value = 1;
    st.bool_value = true;
    nsapi_socket_t sock = &st.socket;
    st.socket_open(&sock, NSAPI_TCP);
    st.socket_connect(sock, addr);
    EXPECT_EQ(st.socket_send(sock, "addr", 4), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_sendto)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);

    SocketAddress addr;
    EXPECT_EQ(st.socket_sendto(NULL, addr, "addr", 4), NSAPI_ERROR_DEVICE_ERROR);

    st.max_sock_value = 1;
    st.bool_value = true;
    nsapi_socket_t sock = &st.socket;
    st.socket_open(&sock, NSAPI_TCP);
    st.socket_connect(sock, addr);
    st.create_error = NSAPI_ERROR_CONNECTION_LOST;
    EXPECT_EQ(st.socket_sendto(sock, addr, "addr", 4), NSAPI_ERROR_CONNECTION_LOST);

    st.create_error = NSAPI_ERROR_OK;
    EXPECT_EQ(st.socket_sendto(sock, addr, "addr", 4), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_recv)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    char table[4];
    EXPECT_EQ(st.socket_recv(NULL, table, 4), NSAPI_ERROR_DEVICE_ERROR);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_recvfrom)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);
    char table[4];
    EXPECT_EQ(st.socket_recvfrom(NULL, NULL, table, 4), NSAPI_ERROR_DEVICE_ERROR);

    SocketAddress addr;
    st.max_sock_value = 1;
    st.bool_value = true;
    nsapi_socket_t sock = &st.socket;
    st.socket_open(&sock, NSAPI_TCP);
    st.socket_connect(sock, addr);
    st.create_error = NSAPI_ERROR_CONNECTION_LOST;
    EXPECT_EQ(st.socket_recvfrom(sock, &addr, table, 4), NSAPI_ERROR_CONNECTION_LOST);

    st.create_error = NSAPI_ERROR_OK;
    EXPECT_EQ(st.socket_recvfrom(sock, &addr, table, 4), NSAPI_ERROR_OK);
}

TEST_F(TestAT_CellularStack, test_AT_CellularStack_socket_attach)
{
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler at(&fh1, que, 0, ",");

    MyStack st(at, 0, IPV6_STACK);

    st.socket_attach(NULL, NULL, NULL);
    st.max_sock_value = 1;
    st.bool_value = true;
    nsapi_socket_t sock = &st.socket;
    st.socket_open(&sock, NSAPI_TCP);
    st.socket_attach(sock, NULL, NULL);
}
