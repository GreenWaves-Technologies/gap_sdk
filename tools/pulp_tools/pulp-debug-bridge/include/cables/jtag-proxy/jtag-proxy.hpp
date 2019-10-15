/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "adv_dbg_itf/adv_dbg_itf.hpp"
#include "proxy.hpp"
#include "events/tcp-events.hpp"

class Jtag_proxy : public Cable {
  public:

    Jtag_proxy(EventLoop::SpEventLoop event_loop, cable_cb_t cable_state_cb);
    ~Jtag_proxy() {};
    
    bool connect(js::config *config);

    bool bit_inout(char* inbit, char outbit, bool last);

    bool stream_inout(char* instream, char* outstream, unsigned int n_bits, bool last);

    bool jtag_reset(bool active);

    int flush();

    bool chip_reset(bool active);

  private:
    std::shared_ptr<Tcp_client> m_client;

    int m_port = 0;
    const char *m_server;

    bool proxy_stream(char* instream, char* outstream, unsigned int n_bits, bool last, int bit);
    Log log;
    cable_cb_t cable_state_cb;
    std::shared_ptr<Tcp_client> m_tcp_client;
    tcp_socket_ptr_t m_socket;
};