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

#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>


#include "dpi/models.hpp"
#include "debug_bridge/proxy.hpp"

typedef struct {
  int tdi;
  int tms;
  int trstn;
  int tdo;
} jtag_cycle_t;

class Proxy : public Dpi_model
{

public:
  Proxy(js::config *config, void *handle);

  void start();



private:
  void proxy_listener();
  void proxy_loop(int sock);
  bool open_proxy();
  void jtag_buff_cycle(int tdi, int tms, int trstn);
  void jtag_buff_flush();
  void dpi_task();
  static void dpi_task_stub(Proxy *proxy);
  void reset_req(int value, int duration);
  void config_req(int value);

  Jtag_itf *jtag;
  Ctrl_itf *ctrl;

  int verbose;
  int port;
  std::thread *listener_thread;
  std::thread *loop_thread;
  int proxy_socket_in;
  bool listener_error;

  jtag_cycle_t *jtag_buff = NULL;
  int jtag_buff_size = 0;
  int jtag_buff_current = 0;
  bool jtag_has_buff = false;
  bool has_req = false;
  bool has_str = false;
  bool reset_set = false;
  int reset_duration;
  int reset_value;
  bool config_set = false;
  int config_value;

  pthread_mutex_t mutex;
  pthread_cond_t cond;
};



Proxy::Proxy(js::config *config, void *handle) : Dpi_model(config, handle)
{
  port = config->get("port")->get_int();
  verbose = config->get("verbose")->get_bool();
  print("Creating proxy model (port: 0x%d)", port);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  jtag = new Jtag_itf();
  create_itf("jtag", static_cast<Jtag_itf *>(jtag));
  ctrl = new Ctrl_itf();
  create_itf("ctrl", static_cast<Ctrl_itf *>(ctrl));

  if (config->get("active")->get_bool())
    open_proxy();
}

void Proxy::start()
{
  create_task((void *)&Proxy::dpi_task_stub, this);
}

void Proxy::jtag_buff_cycle(int tdi, int tms, int trstn)
{
  if (jtag_buff_current >= jtag_buff_size) {
    if (jtag_buff_size == 0) jtag_buff_size = 256;
    else jtag_buff_size *= 2;
    jtag_buff = (jtag_cycle_t *)realloc(jtag_buff, jtag_buff_size*sizeof(jtag_cycle_t));
  }
  jtag_buff[jtag_buff_current].tdi = tdi;
  jtag_buff[jtag_buff_current].tms = tms;
  jtag_buff[jtag_buff_current].trstn = trstn;
  jtag_buff_current++;
}

void Proxy::jtag_buff_flush()
{
  pthread_mutex_lock(&mutex);
  while(has_req || jtag_has_buff) pthread_cond_wait(&cond, &mutex);
  jtag_has_buff = true;
  pthread_mutex_unlock(&mutex);
  raise_event_from_ext();
  pthread_mutex_lock(&mutex);
  while(jtag_has_buff) pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
  jtag_buff_current = 0;
}

void Proxy::reset_req(int value, int duration)
{
  pthread_mutex_lock(&mutex);
  while(has_req || jtag_has_buff) {
    pthread_cond_wait(&cond, &mutex);
  }
  has_req = true;
  reset_set = true;
  reset_duration = duration;
  reset_value = value;
  pthread_mutex_unlock(&mutex);
  raise_event_from_ext();
  pthread_mutex_lock(&mutex);
  while(reset_set) pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
}

void Proxy::config_req(int value)
{
  pthread_mutex_lock(&mutex);
  while(has_req || jtag_has_buff) {
    pthread_cond_wait(&cond, &mutex);
  }
  has_req = true;
  config_set = true;
  config_value = value;
  pthread_mutex_unlock(&mutex);
  raise_event_from_ext();
  pthread_mutex_lock(&mutex);
  while(config_set) pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
}

void Proxy::proxy_loop(int sock)
{
  while(1) {

    proxy_req_t req;

    int ret = recv(sock, (void *)&req, sizeof(req), 0);

    if(ret != sizeof(req)) {
      return;
    }

    if (req.type == DEBUG_BRIDGE_JTAG_REQ)
    {
      uint8_t buffer[req.jtag.bits];
      uint8_t out_buffer[(req.jtag.bits + 7) / 8];
      uint8_t value = 0;
      uint8_t *out_ptr = out_buffer;

      ::recv(sock, (void *)buffer, req.jtag.bits, 0);


      for (int i=0; i<req.jtag.bits; i++)
      {
        uint8_t value = buffer[i];
        int trst = (value >> DEBUG_BRIDGE_JTAG_TRST) & 1;
        int tms = (value >> DEBUG_BRIDGE_JTAG_TMS) & 1;
        int tdi = (value >> DEBUG_BRIDGE_JTAG_TDI) & 1;
        //fprintf(stderr, "%d: VALUE %x TDI: %d, TMS: %d, TRST: %d\n", i, value, tdi, tms, trst);
        jtag_buff_cycle(tdi, tms, trst);
      }

      jtag_buff_flush();

      if (req.jtag.tdo)
      {
        for (int i=0; i<req.jtag.bits; i++)
        {
          // Set TDO to 0 in case the platform reported another value than 0
          // or 1 (e.g. X)
          unsigned int tdo = jtag_buff[i].tdo;
          tdo = tdo <= 1 ? tdo : 0;
          value |= (tdo << (i % 8));
          if ((i % 8) == 7 || i == req.jtag.bits - 1)
          {
            *out_ptr = value;
            out_ptr++;
            value = 0;
          }
        }
        ::send(sock, (void *)out_buffer, (req.jtag.bits + 7) / 8, 0);
      }

    }
    else if (req.type == DEBUG_BRIDGE_RESET_REQ)
    {
      reset_req(req.reset.active, req.reset.duration);
    }
    else if (req.type == DEBUG_BRIDGE_CONFIG_REQ)
    {
      config_req(req.config.value);
    }
    else
    {
      fprintf(stderr, "Received unknown debug bridge request: %d\n", req.type);
    }
  }
}

void Proxy::proxy_listener()
{
  int client;

  fprintf(stderr, "Proxy listening on port %d\n", port);

  while(1) {
    if((client = accept(proxy_socket_in, NULL, NULL)) == -1) {
      if(errno == EAGAIN) continue;

      fprintf(stderr, "Unable to accept connection: %s\n", strerror(errno));
      listener_error = true;
      return;
    }
    printf("Proxy: Client connected!\n");
    loop_thread = new std::thread(&Proxy::proxy_loop, this, client);
  }

  listener_error = false;
  return;
}

bool Proxy::open_proxy() {
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
      //fprintf(stderr, "Unable to create comm socket: %s\n", strerror(errno));
      port++;
      continue;
    }

    if(setsockopt(proxy_socket_in, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      //fprintf(stderr, "Unable to setsockopt on the socket: %s\n", strerror(errno));
      port++;
      continue;
    }

    if(bind(proxy_socket_in, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      //fprintf(stderr, "Unable to bind the socket: %s\n", strerror(errno));
      port++;
      continue;
    }

    if(listen(proxy_socket_in, 1) == -1) {
      //fprintf(stderr, "Unable to listen: %s\n", strerror(errno));
      port++;
      continue;
    }

    break;
  };

  if (port == first_port + 10000) {
    fprintf(stderr, "Didn't manage to open socket\n");
    return false;
  }

  listener_thread = new std::thread(&Proxy::proxy_listener, this);

  return true;
}



void Proxy::dpi_task_stub(Proxy *_this)
{
  _this->dpi_task();
}

void Proxy::dpi_task()
{
  //dpi_reset_set(0);
  //dpi_jtag_set(0, 0, 0, 1);
  //dpi_wait(1000000);
  while(1) {

    if (has_req || has_str || jtag_has_buff) {
      pthread_mutex_lock(&mutex);

      if (reset_set)
      {
        reset_set = false;
        has_req = false;
        ctrl->reset_edge(reset_value);

        wait(reset_duration);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        continue;
      }

      if (config_set)
      {
        config_set = false;
        has_req = false;
        ctrl->config_edge(config_value);
        wait(1000000);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        continue;
      }

#if 0
      if (stop_set)
      {
        stop_set = false;
        has_req = false;
        dpi_stop(stop_value);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        continue;
      }
#endif

      if (jtag_has_buff) {
        for (int i=0; i<jtag_buff_current; i++) {
          int dummy;
          jtag_cycle_t *elem = &jtag_buff[i];
          //fprintf(stderr, "JTAG_DBG: JTAG buff cycle (trstn: %d, tdi: %d, tms: %d)\n", elem->trstn, elem->tdi, elem->tms);
          jtag->tck_edge(0, elem->tdi, elem->tms, elem->trstn, &elem->tdo);
          wait(50);
          jtag->tck_edge(1, elem->tdi, elem->tms, elem->trstn, &elem->tdo);
          wait(50);
          jtag->tck_edge(0, elem->tdi, elem->tms, elem->trstn, &dummy);
          //fprintf(stderr, "JTAG_DBG: TDO=%d\n", elem->tdo);
        }
        jtag_has_buff = false;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        continue;
      }

#if 0
      if (has_req) {
        int _tdi = tdi, _tms = tms, _trstn = trstn;
        //fprintf(stderr, "JTAG_DBG: JTAG cycle (trstn: %d, tdi: %d, tms: %d)\n", _trstn, _tdi, _tms);
        has_req = false;
        pthread_cond_signal(&cond);
        dpi_jtag_set(0, _tdi, _tms, _trstn);
        dpi_wait(50);
        dpi_jtag_set(1, _tdi, _tms, _trstn);
        dpi_wait(50);
        dpi_jtag_set(0, _tdi, _tms, _trstn);
        tdo = dpi_jtag_get_tdo();
        //fprintf(stderr, "JTAG_DBG: TDO=%d\n", tdo);
        pthread_mutex_unlock(&mutex);
        continue;
      }
#endif

#if 0
      if (has_str) {
        dpi_print(handle, pending_str);
        pending_str = NULL;
        pthread_cond_signal(&cond);
        has_str = false;
        pthread_mutex_unlock(&mutex);
        continue;
      }
#endif
      pthread_mutex_unlock(&mutex);
    } else {
      wait_event();
    }

  }
}



extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new Proxy(config, handle);
}
