/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <vp/itf/jtag.hpp>

#include <vp/vp.hpp>

class Jtag : public vp::component
{
public:
    Jtag(js::config *config);

    int build();

private:
    vp::trace trace;

    void proxy_listener();
    void proxy_loop(int sock);
    bool open_proxy();

    void set_jtag_pads(int tck, int tms, int tdi, int trst);
    static void sync(void *__this, int tdo);

    int port;
    int proxy_socket_in;
    bool listener_error;
    std::thread *listener_thread;
    std::thread *loop_thread;
    vp::jtag_master jtag_itf;

    int tdo;
};

Jtag::Jtag(js::config *config)
    : vp::component(config)
{
}


void Jtag::set_jtag_pads(int tck, int tms, int tdi, int trst)
{
    //fprintf(stderr, "PADS sync (tck: %d, tms: %d, tdi: %d, trst: %d)\n", tck, tms, tdi, trst);
    this->get_time_engine()->lock();
    this->jtag_itf.sync(tck, tdi, tms, trst);
    this->get_time_engine()->unlock();
}


void Jtag::sync(void *__this, int tdo)
{
    Jtag *_this = (Jtag *)__this;
    _this->tdo = tdo;
}


void Jtag::proxy_loop(int sock)
{
    while(1)
    {
        char command;

        int ret = recv(sock, (void *)&command, sizeof(command), 0);

        if (ret != sizeof(command))
            return;

        int reply = 0;

        switch (command)
        {
            case 'B':
                //fprintf(stderr, "Blink ON\n");
                break;
            case 'b':
                //fprintf(stderr, "Blink OFF\n");
                break;
            case 'r':
                //fprintf(stderr, "Reset (trst: %d, srst: %d)\n", 0, 0);
                this->set_jtag_pads(0, 0, 0, 0);
                break;
            case 's':
                //fprintf(stderr, "Reset (trst: %d, srst: %d)\n", 0, 1);
                this->set_jtag_pads(0, 0, 0, 0);
                break;
            case 't':
                //fprintf(stderr, "Reset (trst: %d, srst: %d)\n", 1, 0);
                this->set_jtag_pads(0, 0, 0, 1);
                break;
            case 'u':
                //fprintf(stderr, "Reset (trst: %d, srst: %d)\n", 0, 1);
                this->set_jtag_pads(0, 0, 0, 1);
                break;
            case '0':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 0, 0, 0);
                this->set_jtag_pads(0, 0, 0, 0);
                break;
            case '1':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 0, 0, 1);
                this->set_jtag_pads(0, 0, 1, 0);
                break;
            case '2':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 0, 1, 0);
                this->set_jtag_pads(0, 1, 0, 0);
                break;
            case '3':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 0, 1, 1);
                this->set_jtag_pads(0, 1, 1, 0);
                break;
            case '4':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 1, 0, 0);
                this->set_jtag_pads(1, 0, 0, 0);
                break;
            case '5':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 1, 0, 1);
                this->set_jtag_pads(1, 0, 1, 0);
                break;
            case '6':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 1, 1, 0);
                this->set_jtag_pads(1, 1, 0, 0);
                break;
            case '7':
                //fprintf(stderr, "Write (tck: %d, tms: %d, tdi: %d)\n", 1, 1, 1);
                this->set_jtag_pads(1, 1, 1, 0);
                break;
            case 'R':
                //fprintf(stderr, "Read\n");
                reply = 1;
                break;
            case 'Q':
                fprintf(stderr, "Quit\n");
                break;
            default:
                fprintf(stderr, "Received unknown command %c\n",
                        command);
        }
        if (reply)
        {
            //fprintf(stderr, "SEND TDO %d\n", this->tdo);
            char reply_char = this->tdo ? '1' : '0';
            ::send(sock, (void *)&reply_char, 1, 0);
        }

    }
}


void Jtag::proxy_listener()
{
  int client;

  fprintf(stderr, "Proxy listening on port %d\n", this->port);

  while(1) {
    if((client = accept(proxy_socket_in, NULL, NULL)) == -1) {
      if(errno == EAGAIN) continue;

      fprintf(stderr, "Unable to accept connection: %s\n", strerror(errno));
      listener_error = true;
      return;
    }
    printf("Proxy: Client connected!\n");
    loop_thread = new std::thread(&Jtag::proxy_loop, this, client);
  }

  listener_error = false;
  return;
}


bool Jtag::open_proxy() {
  struct sockaddr_in addr;
  int yes = 1;
  int first_port = port;

  while (port < first_port + 10000)
  {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    proxy_socket_in = socket(PF_INET, SOCK_STREAM, 0);
    if(proxy_socket_in < 0)
    {
      port++;
      continue;
    }

    if(setsockopt(proxy_socket_in, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      port++;
      continue;
    }

    if(bind(proxy_socket_in, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      port++;
      continue;
    }

    if(listen(proxy_socket_in, 1) == -1) {
      port++;
      continue;
    }

    break;
  };

  if (port == first_port + 10000) {
    fprintf(stderr, "Didn't manage to open socket\n");
    return false;
  }

  listener_thread = new std::thread(&Jtag::proxy_listener, this);

  return true;
}

int Jtag::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->jtag_itf.set_sync_meth(&Jtag::sync);
    this->new_master_port("jtag", &this->jtag_itf);

    this->port = this->get_config_int("port");

    if (this->get_js_config()->get_child_bool("enabled"))
    {
        this->open_proxy();
    }

    return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Jtag(config);
}
