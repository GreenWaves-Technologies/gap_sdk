/*
 * Copyright (c) 2018, Arm Limited and affiliates
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
#include "features/netsocket/TCPSocket.h"
#include "NetworkStack_stub.h"

// Control the rtos EventFlags stub. See EventFlags_stub.cpp
extern std::list<uint32_t> eventFlagsStubNextRetval;

// To test protected functions
class TCPSocketFriend : public TCPSocket {
    friend class TestTCPSocket;
    FRIEND_TEST(TestTCPSocket, get_proto);
};

class TestTCPSocket : public testing::Test {
public:
    unsigned int dataSize = 10;
    char dataBuf[10];
protected:
    TCPSocket *socket;
    NetworkStackstub stack;

    virtual void SetUp()
    {
        socket = new TCPSocket();
    }

    virtual void TearDown()
    {
        stack.return_values.clear();
        eventFlagsStubNextRetval.clear();
        delete socket;
    }
};

TEST_F(TestTCPSocket, get_proto)
{
    TCPSocketFriend tcpFriend;
    EXPECT_EQ(tcpFriend.get_proto(), NSAPI_TCP);
}

TEST_F(TestTCPSocket, constructor)
{
    EXPECT_TRUE(socket);
}

TEST_F(TestTCPSocket, constructor_parameters)
{
    TCPSocket socketParam = TCPSocket();
    socketParam.open(dynamic_cast<NetworkStack *>(&stack));
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socketParam.connect(a), NSAPI_ERROR_OK);
}

/* connect */

TEST_F(TestTCPSocket, connect)
{
    socket->open((NetworkStack *)&stack);

    stack.return_value = NSAPI_ERROR_OK;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_OK);
}

TEST_F(TestTCPSocket, connect_no_open)
{
    stack.return_value = NSAPI_ERROR_OK;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, connect_error_in_progress_no_timeout)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_IN_PROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    eventFlagsStubNextRetval.push_back(osFlagsError); // Break the wait loop
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TestTCPSocket, connect_with_timeout)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_IN_PROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    eventFlagsStubNextRetval.push_back(osFlagsError); // Break the wait loop
    socket->set_blocking(true);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TestTCPSocket, connect_error_is_connected)
{
    socket->open((NetworkStack *)&stack);
    stack.return_values.push_back(NSAPI_ERROR_ALREADY);
    stack.return_values.push_back(NSAPI_ERROR_IS_CONNECTED);
    const SocketAddress a("127.0.0.1", 1024);
    socket->set_timeout(1);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_OK);
}

TEST_F(TestTCPSocket, connect_by_name_and_port)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->connect("testhost", 80), NSAPI_ERROR_OK);
}

TEST_F(TestTCPSocket, connect_by_name_and_port_dns_fail)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_DNS_FAILURE;
    EXPECT_EQ(socket->connect("testhost", 80), NSAPI_ERROR_DNS_FAILURE);
}

/* send */

TEST_F(TestTCPSocket, send_no_open)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->send(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, send_in_one_chunk)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = dataSize;
    EXPECT_EQ(socket->send(dataBuf, dataSize), dataSize);
}

TEST_F(TestTCPSocket, send_in_two_chunks)
{
    socket->open((NetworkStack *)&stack);
    stack.return_values.push_back(4);
    stack.return_values.push_back(dataSize - 4);
    EXPECT_EQ(socket->send(dataBuf, dataSize), dataSize);
}

TEST_F(TestTCPSocket, send_error_would_block)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_WOULD_BLOCK;
    eventFlagsStubNextRetval.push_back(osFlagsError); // Break the wait loop
    EXPECT_EQ(socket->send(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TestTCPSocket, send_error_other)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_NO_MEMORY;
    EXPECT_EQ(socket->send(dataBuf, dataSize), NSAPI_ERROR_NO_MEMORY);
}

TEST_F(TestTCPSocket, send_error_no_timeout)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_WOULD_BLOCK;
    socket->set_blocking(false);
    EXPECT_EQ(socket->send(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TestTCPSocket, send_to)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = 10;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->sendto(a, dataBuf, dataSize), dataSize);
}

/* recv */

TEST_F(TestTCPSocket, recv_no_open)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->recv(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, recv_all_data)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = dataSize;
    EXPECT_EQ(socket->recv(dataBuf, dataSize), dataSize);
}

TEST_F(TestTCPSocket, recv_less_than_expected)
{
    socket->open((NetworkStack *)&stack);
    unsigned int lessThanDataSize = dataSize - 1;
    stack.return_values.push_back(lessThanDataSize);
    EXPECT_EQ(socket->recv(dataBuf, dataSize), lessThanDataSize);
}

TEST_F(TestTCPSocket, recv_would_block)
{
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_WOULD_BLOCK;
    eventFlagsStubNextRetval.push_back(0);
    eventFlagsStubNextRetval.push_back(osFlagsError); // Break the wait loop
    EXPECT_EQ(socket->recv(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TestTCPSocket, recv_from_no_socket)
{
    stack.return_value = NSAPI_ERROR_OK;
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->recvfrom(&a, dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, recv_from)
{
    stack.return_value = NSAPI_ERROR_OK;
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_OK);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_OK);
    SocketAddress b;
    EXPECT_EQ(socket->recvfrom(&b, dataBuf, dataSize), NSAPI_ERROR_OK);
    EXPECT_EQ(a, b);
}

TEST_F(TestTCPSocket, recv_from_null)
{
    stack.return_value = NSAPI_ERROR_OK;
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_OK);
    EXPECT_EQ(socket->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(socket->recvfrom(NULL, dataBuf, dataSize), NSAPI_ERROR_OK);
}

/* listen */

TEST_F(TestTCPSocket, listen_no_open)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->listen(1), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, listen)
{
    stack.return_value = NSAPI_ERROR_OK;
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->listen(1), NSAPI_ERROR_OK);
}

/* these tests will have to be readjusted after TCPServer is deprecated. */

TEST_F(TestTCPSocket, accept_no_open)
{
    nsapi_error_t error;
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->accept(&error), static_cast<TCPSocket *>(NULL));
    EXPECT_EQ(error, NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestTCPSocket, accept)
{
    nsapi_error_t error;
    stack.return_value = NSAPI_ERROR_OK;
    socket->open((NetworkStack *)&stack);
    TCPSocket *sock = socket->accept(&error);
    EXPECT_NE(sock, static_cast<TCPSocket *>(NULL));
    EXPECT_EQ(error, NSAPI_ERROR_OK);
    if (sock) {
        sock->close();
    }
}

TEST_F(TestTCPSocket, accept_would_block)
{
    nsapi_error_t error;
    socket->open((NetworkStack *)&stack);
    stack.return_value = NSAPI_ERROR_WOULD_BLOCK;
    eventFlagsStubNextRetval.push_back(0);
    eventFlagsStubNextRetval.push_back(osFlagsError); // Break the wait loop
    EXPECT_EQ(socket->accept(&error), static_cast<TCPSocket *>(NULL));
    EXPECT_EQ(error, NSAPI_ERROR_WOULD_BLOCK);
}
