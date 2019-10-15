#include "events/events.hpp"
#include "events/tcp-events.hpp"
#include "log/log.hpp"

#include <iostream>
#include <memory>

int main( int argc, const char* argv[] )
{
    Log log_("test");
    Log::log_level = 1;

    auto el = EventLoop::getLoop();
    // always use shared pointer for socket owners!
    auto l = std::make_shared<Tcp_listener>(&log_, el, 10000);
    l->on_connected(
        [](const tcp_socket_ptr_t& sock){
            sock->on_read([](const tcp_socket_ptr_t& UNUSED(sock), circular_buffer_ptr_t buf){
                char cbuf[1024];
                size_t clen = buf->read_copy(cbuf, 1024);
                cbuf[clen] = 0;
                printf("# %s\n", cbuf);
            });
            sock->on_closed([](){
                std::cout << "# Reading socket signals closing\n";
            });
            sock->set_events(Readable);
        }
    );
    l->on_disconnected(
        [&](tcp_socket_ptr_t sock){
            std::cout << "# Listener signals socket closed\n";
            l->stop();
        }
    );
    l->on_state_change(
        [](listener_state_t state) {
            std::cout << "# Listener state: " << (state == ListenerStarted?"started":"stopped") << "\n";  
        }
    );
    l->start();

    int pkt_cnt = 8;
    int *ppkt_cnt = &pkt_cnt;

    // always use shared pointer for socket owners!
    auto c = std::make_shared<Tcp_client>(&log_, el);

    c->on_connected(
        [ppkt_cnt](tcp_socket_ptr_t sock){
            sock->on_write([ppkt_cnt](const tcp_socket_ptr_t& sock, circular_buffer_ptr_t buf){
                std::cout << "# Write packet\n";

                const char *cbuf = "testing";
                #ifndef NDEBUG
                assert(buf->write_copy(cbuf, 8) == 8);
                #else
                buf->write_copy(cbuf, 8);
                #endif
                if (((*ppkt_cnt)--)<=0) {
                    // sock->set_events(None);
                    sock->close();
                }
            });

            sock->once_closed([](){
                std::cout << "# Writing socket signals closing\n";
            });

            // enable writing on the socket
            sock->set_events(Writable);
        }
    );

    c->on_disconnected(
        [](tcp_socket_ptr_t sock){
            std::cout << "# Client signals socket closed\n";
        }
    );
    c->connect("127.0.0.1", 10000);
    el->start();
    std::cout << "# Loop exited\n";
    el = nullptr;
    c = nullptr;
    l = nullptr;
    std::cout << "Test Passed\n";

    return 0;
}