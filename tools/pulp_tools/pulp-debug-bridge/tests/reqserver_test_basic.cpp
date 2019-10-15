
#include <memory>
#include <vector>
#include <thread>

#include "mock_cable.hpp"
#include "reqserver.hpp"
#include "debug_bridge/reqserver.h"
#include "events/events.hpp"

void client()
{
    Log log_("test");
    Log::log_level = LOG_DEBUG;
    auto el = EventLoop::getLoop();
    // always use shared pointer for socket owners!
    auto c = std::make_shared<Tcp_client>(&log_, el);

    int trans_id = 1;
    int *ptrans_id = &trans_id;

    c->on_connected(
        [ptrans_id](tcp_socket_ptr_t sock){
            sock->on_write([ptrans_id](const tcp_socket_ptr_t& sock, circular_buffer_ptr_t buf){
                std::cout << "# Writing one\n";
                if (*ptrans_id < 10) {
                    if (*ptrans_id %2 == 0) {
                        reqserver_req_t req;
                        req.trans_id = (*ptrans_id)++;
                        req.addr = 1000;
                        req.len = 100;
                        req.type = (*ptrans_id == 9?10:REQSERVER_READMEM_REQ);
                        buf->write_copy(&req, sizeof(req));
                    } else {
                        std::vector<char> vdata(sizeof(reqserver_req_t) + 100);
                        reqserver_write_req_t * preq = (reqserver_write_req_t *) &vdata[0];
                        preq->req.trans_id = (*ptrans_id)++;
                        preq->req.addr = 1000;
                        preq->req.len = 100;
                        preq->req.type = (*ptrans_id == 9?10:REQSERVER_WRITEMEM_REQ);
                        buf->write_copy(preq, sizeof(reqserver_req_t) + 100);
                    }
                    sock->set_events(Readable);
                } else {
                    std::cout << "# close socket\n";
                    sock->close();
                }
                
            });

            sock->on_read([](const tcp_socket_ptr_t& sock, circular_buffer_ptr_t buf){
                std::cout << "# Read one\n";
                reqserver_rsp_t rsp;
                buf->read_copy(&rsp, sizeof(rsp));
                switch (rsp.type) {
                    case REQSERVER_READMEM_RSP: {
                        int len;
                        buf->read_copy(&len, sizeof(len));
                        std::vector<char> data(len);
                        buf->read_copy(&data[0], len);
                        break;
                    }
                    default:
                        break;
                }
                sock->set_events(Writable);
            });

            sock->once_closed([](){
                std::cout << "# Writing socket signals closing\n";
            });

            std::cout << "# Enable write events\n";
            // enable writing on the socket
            sock->set_events(Writable);
        }
    );

    c->on_disconnected(
        [](tcp_socket_ptr_t sock){
            std::cout << "# Client signals socket closed\n";
        }
    );
    c->connect("127.0.0.1", 9999);
    el->start();
}

int main( int argc, const char* argv[] )
{

    auto el = EventLoop::getLoop();
    auto cable = std::make_shared<MockCable>();

    auto req_srv = std::make_shared<ReqServer>(el, cable, 9999, 10);
    req_srv->on_req_event([req_srv](req_event_t e){
        std::cout << "# Server event\n";
        if (e == REQSERVER_CLIENT_DISCONNECTED) {
            std::cout << "# Stop listener\n";
            req_srv->stop();
        }
    });
    req_srv->soc_power(true);
    req_srv->start();
    std::thread client_thread(client);
    el->start();
    client_thread.join();
    std::cout << "Test Passed\n";
}
