#include "events/events.hpp"
#include "events/tcp-events.hpp"
#include "log/log.hpp"

#include <iostream>
#include <memory>
#include <thread>

#define TEST_STR "hello\0"

void server()
{
    Log log_("test");
    Log::log_level = 1;
    auto el = EventLoop::getLoop();
    // always use shared pointer for socket owners!
    auto l = std::make_shared<Tcp_listener>(&log_, el, 10000);
    l->on_connected(
        [&](const tcp_socket_ptr_t& sock){
            printf("# server thread connected\n");
            sock->on_read([&](const tcp_socket_ptr_t& sock, circular_buffer_ptr_t buf){
                char * cbuf;
                size_t len;
                buf->read_block((void**)&cbuf, &len);
                buf->commit_read(len);
                printf("# server thread received %s\n", cbuf);
                
                sock->set_events(None);

                sock->write_buffer([](const tcp_socket_ptr_t& sock, circular_buffer_ptr_t buf){
                    buf->write_copy(TEST_STR, sizeof(TEST_STR));
                });
                
                el->getTimerEvent([&](){
                    l->stop();
                    return kEventLoopTimerDone;
                }, 100);
            });
            sock->once_closed([](){
                std::cout << "# server socket signals closing\n";
            });
            sock->set_events(Readable);
        }
    );
    l->on_disconnected(
        [&](tcp_socket_ptr_t sock){
            std::cout << "# listener signals socket closed\n";
            l->stop();
        }
    );
    l->on_state_change(
        [](listener_state_t state) {
            std::cout << "# listener state: " << (state == ListenerStarted?"started":"stopped") << "\n";  
        }
    );
    l->start();
    el->start();
}


int main( int argc, const char* argv[] )
{
    std::thread server_thread(server);

    Log log_("test");
    Log::log_level = 1;

    auto el = EventLoop::getLoop();
    // always use shared pointer for socket owners!

    auto c = std::make_shared<Tcp_client>(&log_, el);

    el->getTimerEvent([&](){
        tcp_socket_ptr_t s = c->connect_blocking("127.0.0.1", 10000);
        assert(s != nullptr);
        
        printf("# main thread connected\n");

        // we want to do a blocking write so set it blocking
        s->set_blocking(true);

        s->write_immediate(TEST_STR, sizeof(TEST_STR), true);

        char buf[sizeof(TEST_STR)];
        ssize_t len = s->read_immediate(buf, sizeof(TEST_STR), true);
        printf("# main thread read returned %zd\n", len);
        assert(sizeof(TEST_STR) == len);
        printf("# main thread read %s\n", buf);

        // during the close process we need the eventloop to manage the socket
        // so make sure to set it non blocking again
        s->set_blocking(false);
        s->close();
        return kEventLoopTimerDone;
    }, 100000);

    el->start();

    server_thread.join();
    c = nullptr;
    std::cout << "Test Passed\n";

    return 0;
}