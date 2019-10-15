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
#include "features/netsocket/InternetSocket.h"
#include "NetworkStack_stub.h"

extern std::list<uint32_t> eventFlagsStubNextRetval;

// InternetSocket is an abstract class, so we have to test it via its child.
class stubInternetSocket : public InternetSocket {
protected:
    nsapi_error_t return_value;
public:
    stubInternetSocket()
    {
        return_value = 0;
    }
    virtual nsapi_error_t connect(const SocketAddress &address)
    {
        _remote_peer = address;
        return return_value;
    }
    virtual nsapi_size_or_error_t send(const void *data, nsapi_size_t size)
    {
        return return_value;
    }
    virtual nsapi_size_or_error_t recv(void *data, nsapi_size_t size)
    {
        return return_value;
    }
    virtual nsapi_size_or_error_t sendto(const SocketAddress &address,
                                         const void *data, nsapi_size_t size)
    {
        return return_value;
    }
    virtual nsapi_size_or_error_t recvfrom(SocketAddress *address,
                                           void *data, nsapi_size_t size)
    {
        return return_value;
    }
    virtual Socket *accept(nsapi_error_t *error = NULL)
    {
        return NULL;
    }
    virtual nsapi_error_t listen(int backlog = 1)
    {
        return return_value;
    }

    // Testing functions
    void add_reader(void)
    {
        _readers++;
    }
    void rem_reader(void)
    {
        _readers--;
    }
    void add_writer(void)
    {
        _writers++;
    }
    void rem_writer(void)
    {
        _writers--;
    }

protected:
    virtual nsapi_protocol_t get_proto()
    {
        return NSAPI_TCP;
    }
};

static bool callback_is_called;
static void my_callback()
{
    callback_is_called = true;
}

class TestInternetSocket : public testing::Test {
protected:
    stubInternetSocket *socket;
    NetworkStackstub stack;

    virtual void SetUp()
    {
        socket = new stubInternetSocket;
    }

    virtual void TearDown()
    {
        delete socket;
    }
};

TEST_F(TestInternetSocket, constructor)
{
    EXPECT_TRUE(socket);
}

TEST_F(TestInternetSocket, open_null_stack)
{
    EXPECT_EQ(socket->open(NULL), NSAPI_ERROR_PARAMETER);
}

TEST_F(TestInternetSocket, open_error)
{
    stack.return_value = NSAPI_ERROR_PARAMETER;
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_PARAMETER);
}

TEST_F(TestInternetSocket, open)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_OK);
}

TEST_F(TestInternetSocket, open_twice)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_OK);
    EXPECT_EQ(socket->open((NetworkStack *)&stack), NSAPI_ERROR_PARAMETER);
}

TEST_F(TestInternetSocket, close)
{
    stack.return_value = NSAPI_ERROR_OK;
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->close(), NSAPI_ERROR_OK);
}

TEST_F(TestInternetSocket, close_no_open)
{
    stack.return_value = NSAPI_ERROR_OK;
    EXPECT_EQ(socket->close(), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestInternetSocket, close_during_read)
{
    stack.return_value = NSAPI_ERROR_OK;
    socket->open((NetworkStack *)&stack);
    // when c++11 is available use something like the code below to test the blocking behavior
    // socket->add_reader();
    // std::async(c[](){std::this_thread::sleep_for(1ms); socket->rem_reader()});
    EXPECT_EQ(socket->close(), NSAPI_ERROR_OK);
}

TEST_F(TestInternetSocket, modify_multicast_group)
{
    SocketAddress a("127.0.0.1", 1024);
    stack.return_value = NSAPI_ERROR_OK;
    socket->open((NetworkStack *)&stack);
    // when c++11 is available use something like the code below to test the blocking behavior
    // socket->add_reader();
    // std::async(c[](){std::this_thread::sleep_for(1ms); socket->rem_reader()});
    EXPECT_EQ(socket->join_multicast_group(a), NSAPI_ERROR_UNSUPPORTED);
    EXPECT_EQ(socket->leave_multicast_group(a), NSAPI_ERROR_UNSUPPORTED);
}

// set_blocking and set_timeout are tested within TCPSocket.

TEST_F(TestInternetSocket, bind_no_socket)
{
    EXPECT_EQ(socket->bind(1), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestInternetSocket, bind)
{
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->bind("127.0.0.1", 80), NSAPI_ERROR_OK);
}

TEST_F(TestInternetSocket, bind_nullstring)
{
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->bind(NULL, 80), NSAPI_ERROR_OK);
}

// setsockopt and getsockopt are really just calling the underlying stack functions

TEST_F(TestInternetSocket, setsockopt_no_stack)
{
    EXPECT_EQ(socket->setsockopt(0, 0, 0, 0), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestInternetSocket, setsockopt)
{
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->setsockopt(0, 0, 0, 0), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestInternetSocket, getsockopt_no_stack)
{
    EXPECT_EQ(socket->getsockopt(0, 0, 0, 0), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TestInternetSocket, getsockopt)
{
    socket->open((NetworkStack *)&stack);
    EXPECT_EQ(socket->getsockopt(0, 0, 0, 0), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestInternetSocket, sigio)
{
    callback_is_called = false;
    socket->open((NetworkStack *)&stack);
    socket->sigio(mbed::callback(my_callback));
    socket->close(); // Trigger event;
    EXPECT_EQ(callback_is_called, true);
}

TEST_F(TestInternetSocket, getpeername)
{
    SocketAddress peer;
    SocketAddress zero;

    stack.return_value = NSAPI_ERROR_OK;

    EXPECT_EQ(socket->getpeername(&peer), NSAPI_ERROR_NO_SOCKET);

    socket->open((NetworkStack *)&stack);
    socket->connect(zero);

    EXPECT_EQ(socket->getpeername(&peer), NSAPI_ERROR_NO_CONNECTION);

    const nsapi_addr_t saddr = {NSAPI_IPv4, {192, 168, 0, 1} };
    const SocketAddress remote(saddr, 1024);
    socket->connect(remote);

    EXPECT_EQ(socket->getpeername(&peer), NSAPI_ERROR_OK);
    EXPECT_EQ(remote, peer);
}
