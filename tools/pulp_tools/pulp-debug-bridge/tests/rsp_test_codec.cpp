#include "events/events.hpp"
#include "events/tcp-events.hpp"
#include "gdb-server/rsp-packet-codec.hpp"

#include <stdlib.h>
#include <string.h>
#include <iostream>

#define INVALID_PACKET1 "$126312365#44"
#define INVALID_PACKET2 "$1263123"

#define TEST_STRING "testing"
#define REPEAT_STRING "AAAABBBBBBCCCCCCCZZEE"

int main( int argc, const char* argv[] )
{
    auto c_buf = std::make_shared<CircularCharBuffer>(1000);

    EventLoop::SpEventLoop _loop = EventLoop::getLoop();

    RspPacketCodec *c = new RspPacketCodec(_loop, std::chrono::milliseconds(100));

    c->encode(TEST_STRING, strlen(TEST_STRING), c_buf, true);
    c->encode(TEST_STRING, strlen(TEST_STRING), c_buf, true);
    c->encode_ack(c_buf);

    c->on_ack([](){ std::cout << "# ACK\n"; });
    c->on_error([](const char * err){ std::cout << "# ERROR " << err << "\n"; });
    c->on_ctrlc([](){ std::cout << "# CTRLC\n"; });
    c->on_packet([](const char * buf, size_t len){ std::cout << "# PACKET [" << std::string(buf,len) << "] \n"; });

    c->decode(c_buf);
    c_buf->write_copy("+", 1);
    c->encode_ack(c_buf);
    c_buf->write_copy("\003", 1);
    c->decode(c_buf);
    c_buf->write_copy(INVALID_PACKET1, sizeof(INVALID_PACKET1));
    c->decode(c_buf);
    c_buf->write_copy(INVALID_PACKET2, sizeof(INVALID_PACKET2));
    c->decode(c_buf);
    _loop->start();
    c->encode(REPEAT_STRING, strlen(REPEAT_STRING), c_buf, false);
    delete(c);
    std::cout << "Test Passed\n";
}
