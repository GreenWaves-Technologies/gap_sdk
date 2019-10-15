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

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <functional>

#include "json.hpp"
#include "log/log.hpp"
#include "cables/adv_dbg_itf/adv_dbg_itf.hpp"
#include "cables/jtag-proxy/jtag-proxy.hpp"
#include "reqloop.hpp"
#ifdef __USE_FTDI__
#include "cables/ftdi/ftdi.hpp"
#endif
#include "gdb-server/gdb-server.hpp"
#include "bridge-commands.hpp"

using namespace std;

static Log s_log("wrapper");
static BridgeState *bridge = NULL;

extern "C" bool cable_new(const char *config_string, const char *system_config_string, cable_cb_t state_cb)
{
  const char *cable_name = NULL;
  js::config *config = NULL;
  js::config *system_config = js::import_config_from_string(std::string(system_config_string));

  if (config_string != NULL)
  {
    config = js::import_config_from_string(std::string(config_string));
    js::config *type_config = config->get("type");
    if (type_config != NULL)
    {
      cable_name = type_config->get_str().c_str();
    }
  }

  if (cable_name == NULL) {
    s_log.error("No cable name specified\n");
    return false;
  }

  if (strncmp(cable_name, "ftdi", 4) == 0)
  {
#ifdef __USE_FTDI__
    Ftdi::FTDIDeviceID id = Ftdi::Olimex;
    if (strcmp(cable_name, "ftdi@digilent") == 0) id = Ftdi::Digilent;
    auto adu = std::make_shared<Adv_dbg_itf>(system_config, std::make_shared<Ftdi>(system_config, id, state_cb));
    if (!adu->connect(config)) return false;
    int tap = 0;
    if (config->get("tap")) tap = config->get("tap")->get_int();
    adu->device_select(tap);
    bridge->m_adu = adu;
    return true;
#else
    s_log.error("Debug bridge has not been compiled with FTDI support\n");
    return false;
#endif
  }
  else if (strcmp(cable_name, "jtag-proxy") == 0)
  {
    auto adu = std::make_shared<Adv_dbg_itf>(system_config, std::make_shared<Jtag_proxy>(bridge->m_event_loop, state_cb));
    if (!adu->connect(config)) return false;
    int tap = 0;
    if (config->get("tap")) tap = config->get("tap")->get_int();
    adu->device_select(tap);
    bridge->m_adu = adu;
    return true;
  }
  else
  {
    s_log.error("Unknown cable: %s\n", cable_name);
    return false;
  }

  return false;
}

extern "C" bool cable_write(unsigned int addr, int size, const char *data)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->access(true, addr, size, (char *)data);
}

extern "C" bool cable_read(unsigned int addr, int size, const char *data)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->access(false, addr, size, (char *)data);
}

extern "C" bool chip_reset(bool active)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->chip_reset(active);
}

#ifdef __USE_FTDI__
extern "C" bool cable_set_VQPS(bool active)
{
  if (!bridge->m_adu) return false;
  auto dev = bridge->m_adu->get_device();
  if (!dev) return false;
  auto ftdi = std::dynamic_pointer_cast<Ftdi>(dev);
  if (!ftdi) {
    s_log.error("bad cable type");
    return false;
  }
  return ftdi->chip_setVQPS(active);
}
#endif

extern "C" bool jtag_reset(bool active)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->jtag_reset(active);
}

extern "C" bool jtag_soft_reset()
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->jtag_soft_reset();
}


extern "C" bool cable_jtag_set_reg(unsigned int reg, int width, unsigned int value)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->jtag_set_reg(reg, width, value);
}

extern "C" bool cable_jtag_get_reg(unsigned int reg, int width, unsigned int *out_value, unsigned int value)
{
  if (!bridge->m_adu) return false;
  return bridge->m_adu->jtag_get_reg(reg, width, out_value, value);
}

static void init_sigint_handler(int) {
  printf("sig int raised\n");
  raise(SIGTERM);
}

extern "C" char * bridge_get_error()
{
  return strdup(Log::last_error);
}

extern "C" void bridge_set_log_level(int level)
{
  Log::log_level = level;
}

extern "C" void bridge_init(const char *config_string, int log_level)
{
  // This should be the first C method called by python.
  // As python is not catching SIGINT where we are in C world, we have to
  // setup a  sigint handler to exit in case control+C is hit.
  signal (SIGINT, init_sigint_handler);

  printf("Bridge init - log level %d\n", log_level);

  bridge = new BridgeState(config_string);
  Log::log_level = log_level;
}

extern "C" void bridge_add_execute(int(*bridge_proc)(void * state))
{
  bridge->m_bridge_commands->add_execute(bridge_proc);
}

extern "C" void bridge_add_repeat_start(int64_t delay, int count)
{
  bridge->m_bridge_commands->add_repeat_start(std::chrono::microseconds(delay), count);
}

extern "C" void bridge_add_repeat_end()
{
  bridge->m_bridge_commands->add_repeat_end();
}

extern "C" void bridge_add_delay(int64_t delay)
{
  bridge->m_bridge_commands->add_delay(std::chrono::microseconds(delay));
}

extern "C" void bridge_add_wait_exit()
{
  bridge->m_bridge_commands->add_wait_exit();
}

extern "C" void bridge_loop_timeout(int64_t(*cb)(), int64_t timeout)
{
  bridge->m_event_loop->getTimerEvent(cb, timeout);
}

extern "C" int bridge_start()
{
  int ret = bridge->m_bridge_commands->start_bridge();
  bridge->m_event_loop->start();
  // It would be nice to clean up json-config properly but not possible at present
  // if (bridge->m_system_config) delete bridge->m_system_config;
  delete bridge;
  return ret;
}

extern "C" void bridge_stop()
{
  bridge->m_bridge_commands->stop_bridge();
}

extern "C" void bridge_target_stopped(int stopped)
{
  if (bridge->m_req_loop)
    bridge->m_req_loop->target_stopped(stopped);
}

extern "C" void bridge_reqloop_init(unsigned int debug_struct_addr, int do_printf)
{
  if (!bridge->m_req_loop) {
    bridge->m_req_loop = std::make_shared<ReqLoop>(bridge->m_event_loop, std::static_pointer_cast<Cable>(bridge->m_adu), debug_struct_addr, do_printf);
    bridge->m_req_loop->on_exit(std::bind(&BridgeCommands::stop_bridge, bridge->m_bridge_commands));
  } else
    bridge->m_req_loop->set_debug_struct_addr(debug_struct_addr);
}

extern "C" void bridge_reqserver_init(int port, int max_access)
{
  if (!bridge->m_req_server) {
    bridge->m_req_server = std::make_shared<ReqServer>(bridge->m_event_loop, std::static_pointer_cast<Cable>(bridge->m_adu), port, max_access);
    using namespace std::placeholders;
    bridge->m_req_loop->on_available(std::bind(&ReqServer::soc_power, bridge->m_req_server, _1));
    bridge->m_req_loop->on_req_event(std::bind(&ReqServer::target_alert, bridge->m_req_server));
  }
}

extern "C" void bridge_reqloop_start(bool start_reqserver)
{
  if (bridge->m_req_loop) {
    if (start_reqserver && bridge->m_req_server)
      bridge->m_req_server->start();
    bridge->m_req_loop->start(false);
  }
}

extern "C" void bridge_reqloop_stop(bool stop_reqserver)
{
  if (bridge->m_req_loop) {
    if (bridge->m_req_server) {
      if (stop_reqserver) {
        bridge->m_req_server->stop();
      }
      else
      {
        bridge->m_req_server->soc_power(false);
        bridge->m_req_server->target_reset();
      }
    }
    bridge->m_req_loop->stop();
  }
}

extern "C" void bridge_deinit()
{
  if (bridge) {
    delete bridge;
    bridge = NULL;
  }
}

extern "C" bool gdb_server_open(int socket_port, cmd_cb_t cmd_cb, const char * capabilities)
{
  // slow down printf if gdb is working so as not to starve it
  if (bridge->m_req_loop) bridge->m_req_loop->set_printf_loops(2);
  try {
    bridge->m_gdb_server = std::make_shared<Gdb_server>(bridge->m_event_loop, bridge->m_adu, bridge->m_system_config, socket_port, capabilities);
    if (bridge->m_req_loop) {
      bridge->m_req_loop->once_exit([](int32_t status){
        bridge->m_gdb_server->signal_exit(status);
      });
    }
    bridge->m_gdb_server->on_monitor_command([cmd_cb](const char * data, char * reply, int reply_len, int * retval) {
      *retval = cmd_cb(data, reply, reply_len);
    });
    bridge->m_gdb_server->on_run([cmd_cb](char *){
        cmd_cb("__gdb_tgt_run", NULL, 0);
    });
    bridge->m_gdb_server->on_started([cmd_cb](bool is_started){
      if (is_started)
        cmd_cb("__gdb_tgt_res", NULL, 0);
      else
        cmd_cb("__gdb_tgt_hlt", NULL, 0);
    });
    bridge->m_gdb_server->start();
  } catch (CableException &e) {
    s_log.error("error initializing GDB server %s\n", e.what());
    bridge->m_gdb_server = nullptr;
    return false;
  }
  return true;
}

extern "C" void gdb_server_close()
{
  if (bridge->m_gdb_server) bridge->m_gdb_server->stop();
}

extern "C" void gdb_server_abort()
{
  if (bridge->m_gdb_server) bridge->m_gdb_server->abort();
}

extern "C" bool gdb_send_str(const char * str)
{
  if (bridge->m_gdb_server && bridge->m_gdb_server->rsp) {
    auto client = bridge->m_gdb_server->rsp->get_client();
    if (client) {
      return client->send_str(str);
    }
  }
  return false;
}

extern "C" bool gdb_server_refresh_target()
{
  if (bridge->m_gdb_server) {
    bridge->m_gdb_server->refresh_target();
    return true;
  }
  return false;
}

#if 0

extern "C" void plt_exit(void *_bridge, bool status)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getMemIF()->exit(status);
}

extern "C" bool jtag_reset(void *_bridge)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getJtagIF()->jtag_reset();
}

extern "C" bool jtag_idle(void *_bridge)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getJtagIF()->jtag_idle();
}

extern "C" bool jtag_shift_ir(void *_bridge)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getJtagIF()->jtag_shift_ir();
}

extern "C" bool jtag_shift_dr(void *_bridge)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getJtagIF()->jtag_shift_dr();
}

extern "C" void jtag_shift(void *_bridge, int width, const char *datain, const char *dataout, int noex)
{
  Bridge *bridge = (Bridge *)_bridge;
  bridge->getJtagIF()->jtag_shift(width, (unsigned char *)datain, (unsigned char *)dataout, noex);
}

#endif
