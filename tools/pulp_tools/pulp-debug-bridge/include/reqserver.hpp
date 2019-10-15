/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies SAS
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

/* 
 * Authors: Martin Croome (martin.croome@greenwaves-technologies.com)
 */

#ifndef __REQSERVER_HPP__
#define __REQSERVER_HPP__

#include <queue>
#include <vector>
#include <memory>

#include "cables/cable.hpp"
#include "events/events.hpp"
#include "events/tcp-events.hpp"
#include "debug_bridge/reqserver.h"

#define REQSERVER_MAX_REQ 2048

typedef enum {
  REQSERVER_CLIENT_CONNECTED,
  REQSERVER_CLIENT_DISCONNECTED
} req_event_t;

SMART_EMITTER(ClientEvent, req_event)

class ReqServer : public std::enable_shared_from_this<ReqServer>, public ClientEventEmitter<req_event_t> {
  public:
    ReqServer(const EventLoop::SpEventLoop &event_loop, std::shared_ptr<Cable> cable, int port, int max_access = REQSERVER_MAX_REQ);
    ~ReqServer() {}
    void start();
    void stop();
    void target_alert();
    void target_reset();
    void stop_listener();
    void soc_power(bool enabled);
    bool soc_is_powered() { return m_soc_power; }

    typedef enum {
      CLIENT_IDLE,
      CLIENT_REQ,
      CLIENT_DATA
    } client_state_t;

    class Request
    {
      public:
        bool receive(circular_buffer_ptr_t buf, bool * clear_timer);
        bool execute(std::shared_ptr<Cable> cable); // returns true if request has completed
        bool send(circular_buffer_ptr_t buf);
        void reset();
        bool is_in_progress() { return m_in_progress; }
        bool is_in_error_state() { return m_error; }

      private:
        reqserver_req_t m_req;
        int m_pos = 0;
        int m_start_buff;
        std::vector<char> m_buf;
        bool m_error = false, m_in_progress = false;
        Log log;
    };

    class Client
    {
      friend ReqServer;
      public:
        Client(std::shared_ptr<ReqServer> srv, tcp_socket_ptr_t client);
        ~Client();
        void stop();
        void target_alert();
        void target_reset();
        void soc_power(bool enabled);
      private:
        bool send_data(const char* data, int len);
        bool send_alert();
        bool decode(char* data, size_t len);
        int64_t process_transaction();
        int64_t packet_timeout();
        void on_read(circular_buffer_ptr_t buf);
        void on_write(circular_buffer_ptr_t buf);
        
        bool m_send_alert = false, m_send_reset = false;

        client_state_t m_state;
        Request m_cur_req;
        std::queue<Request> m_pending_reqs, m_completed_reqs;
        std::shared_ptr<ReqServer>  m_reqserver = nullptr;
        tcp_socket_ptr_t m_client = nullptr;
        EventLoop::SpTimerEvent m_trans_te;
        EventLoop::SpTimerEvent m_pkt_to_te;

        int m_packet_timeout = 2000;
        Log log;
    };

    typedef std::shared_ptr<ReqServer::Client> req_srv_client_ptr_t;
    req_srv_client_ptr_t get_client() { return m_client; }

  private:
    void client_connected(const tcp_socket_ptr_t &client);
    void client_disconnected(const tcp_socket_ptr_t &client);
    int64_t process_transaction();

    bool m_started = false, m_stopping = false, m_soc_power = false;

    req_srv_client_ptr_t m_client = nullptr;
    std::shared_ptr<Tcp_listener> m_listener;

    EventLoop::SpEventLoop m_event_loop;

    std::shared_ptr<Cable> m_cable;
    int m_port, m_max_access;
    Log log;
};

#endif