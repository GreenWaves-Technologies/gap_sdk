/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SA
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
 * Authors: Andreas Traber
 *          Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */


#include <list>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <functional>

#include "gdb-server/gdb-server.hpp"

#define REPLY_BUF_LEN 256
#define PACKET_MAX_LEN 4096u

enum mp_type {
  BP_MEMORY   = 0,
  BP_HARDWARE = 1,
  WP_WRITE    = 2,
  WP_READ     = 3,
  WP_ACCESS   = 4
};

#define UNAVAILABLE10 "00000000" "00000000" "00000000" "00000000" "00000000" "00000000" "00000000" "00000000" "00000000" "00000000"
#define UNAVAILABLE33 UNAVAILABLE10 UNAVAILABLE10 UNAVAILABLE10 "00000000" "00000000" "00000000"

// Rsp

Rsp::Rsp(Gdb_server * top, int port, const EventLoop::SpEventLoop &event_loop, int64_t wait_time_usecs) :
  m_top(top), m_port(port), m_event_loop(std::move(event_loop)), m_wait_time_usecs(wait_time_usecs), log("RSP")
{
  init();
}

void Rsp::init()
{
  halt_target();
  main_core = m_top->target->get_threads().front();
  m_thread_init = main_core->get_thread_id();
}

void Rsp::client_connected(const tcp_socket_ptr_t &client)
{
  log.user("RSP: client connected\n");
  if (m_client) {
    log.error("RSP: already connected: disconnecting\n");
    client->close();
    return;
  }
    // Make sure target is halted
  try {
    halt_target();
  } catch (CableException &e) {
    log.user("RSP:cable exception haltimg target\n");
    m_aborted = true;
    stop();
    return;
  }

  m_client = std::make_shared<Rsp::Client>(this->shared_from_this(), client);
}

void Rsp::client_disconnected(const tcp_socket_ptr_t &UNUSED(sock)) {
  log.user("RSP: client disconnected (aborted: %d)\n", m_aborted);

  if (!m_aborted && m_target_state == RSP_TARGET_STOPPED)
  {
    log.debug("RSP: clear breakpoints\n");
    // clear the breakpoints
    m_top->bkp->clear();

    // Start everything
    log.debug("RSP: resume target\n");
    resume_target(false);
  }

  m_stopping = m_stopping||m_target_state == RSP_TARGET_EXITED;
  if (m_stopping) {
    stop_listener();
  }
  m_client = nullptr;
}

void Rsp::abort()
{
  log.debug("RSP: Aborted!\n");
  m_aborted = true;
  stop();
}

void Rsp::stop()
{
  if (m_stopping) return;
  m_stopping = true;
  if (m_client) {
    m_client->stop();
    return;
  }
  stop_listener();
}

void Rsp::stop_listener()
{
  if (m_listener) {
    m_listener->stop();
    m_listener = nullptr;
  }
  m_stopping = false;
  m_running = false;
}

void Rsp::start() {
  m_running = true;
  m_listener = std::make_shared<Tcp_listener>(
    &m_top->log,
    m_event_loop,
    m_port);

  using namespace std::placeholders;

  m_listener->on_connected(std::bind(&Rsp::client_connected, this, _1));
  m_listener->on_disconnected(std::bind(&Rsp::client_disconnected, this, _1));

  m_listener->on_state_change([this](listener_state_t state) {
      log.debug("RSP: Listener status %d\n", state);
      // stop();
    }
  );
  m_listener->start();
}

void Rsp::halt_target()
{
  m_top->emit_started(false);
  m_top->target->halt();
}

void Rsp::resume_target(bool step, int tid)
{
  m_top->target->clear_resume_all();
  if (tid > 0)
    m_top->target->get_thread(tid)->prepare_resume(step);
  else
    m_top->target->prepare_resume_all(step);
  m_top->emit_started(true);
  m_top->target->resume_all();
}

void Rsp::signal_exit(int status)
{
  if (m_client) m_client->signal_exit(status);
}

// Rsp::Client

Rsp::Client::Client(const std::shared_ptr<Rsp> rsp, tcp_socket_ptr_t client) : m_top(rsp->m_top), m_rsp(std::move(rsp)), m_client(std::move(client)), log("RSPC")
{
  // Wait timer. Initially idle
  m_wait_te = rsp->m_event_loop->getTimerEvent(std::bind(&Rsp::Client::wait_routine, this));

  // Packet codec
  m_codec = std::make_shared<RspPacketCodec>(m_rsp->m_event_loop, std::chrono::milliseconds(m_packet_timeout));
  m_codec->on_ack([this](){
    log.error("RSP: received ack\n");
    // TODO - Should timeout on no ACK
  });
  m_codec->on_error([this](const char * err_str){
    log.error("RSP: packet error: %s\n", err_str);
    stop();
  });
  m_codec->on_packet([this](char * pkt, size_t pkt_len){
    log.protocol("RSP: >> received packet\n");
    m_client->write_buffer([this](const tcp_socket_ptr_t & UNUSED(sock), const circular_buffer_ptr_t &c_buf){
      log.protocol("RSP: send ack\n");
      m_codec->encode_ack(c_buf);
    });
    if (!try_decode(pkt, pkt_len)) stop();
    log.protocol("RSP: << end received packet\n");
  });
  m_codec->on_ctrlc([this](){
    log.protocol("RSP: received ctrl-c\n");
    if (is_running())
      halt_target();
    signal(NULL);
  });

  // socket -> packet codec
  m_client->on_read([this](const tcp_socket_ptr_t& UNUSED(sock), circular_buffer_ptr_t buf){
    m_codec->decode(buf);
  });
  m_client->set_events(Readable);
}

void Rsp::Client::signal_exit(int32_t status)
{
  bool signal_it = is_running();
  m_exit_status = status;
  if (signal_it) {
    halt_target();
    m_rsp->set_target_state(RSP_TARGET_EXITED);
    signal();
  } else {
    m_rsp->set_target_state(RSP_TARGET_EXITED);
  }
}

void Rsp::Client::stop()
{
  m_wait_te->setTimeout(kEventLoopTimerDone);
  log.debug("RSP client stopping\n");
  m_client->close();
}

bool Rsp::Client::run(char * filename)
{
  log.debug("run program %s (new program not supported)\n", (filename?filename:"same again"));
  m_rsp->set_target_state(RSP_TARGET_STOPPED);
  m_top->emit_run(filename);
  return signal();
}

bool Rsp::Client::try_decode(char * pkt, size_t pkt_len)
{
  try {
    return decode(pkt, pkt_len);
  } catch (CableException &e) {
    log.error("Cable error - %s - terminating connection\n", e.what());
  } catch (OffCoreAccessException &e) {
    log.error("Debugger attempted to access an off core - terminating connection\n");
  } catch (std::exception &e) {
    log.error("Unknown exception - %s - terminating connection\n", e.what());
  }
  return false;
}

bool Rsp::Client::v_packet(char* data, size_t len)
{
  log.debug("V Packet\n");
  if (strncmp ("vKill", data, std::min(strlen ("vKill"), len)) == 0)
  {
    m_rsp->set_target_state(RSP_TARGET_KILLED);
    halt_target();
    return send_str("OK");
  }
  else if (strncmp ("vRun", data, strlen ("vRun")) == 0)
  {
    return run(&data[5]);
  }
  else if (strncmp ("vCont?", data, std::min(strlen ("vCont?"), len)) == 0)
  {
    return send_str("vCont;c;s;C;S");
  }
  else if (strncmp ("vCont", data, std::min(strlen ("vCont"), len)) == 0)
  {

    m_top->target->clear_resume_all();

    // vCont can contains several commands, handle them in sequence
    char *str = strtok(&data[6], ";");
    bool thread_is_started=false;
    int stepped_tid = -1;
    while(str != NULL) {
      // Extract command and thread ID
      int tid = -1;
      char *delim = strchr(str, ':');
      if (delim != NULL) {
        tid = atoi(delim+1);
        if (tid == 0) {
          tid = 1;
        } else {
          tid = tid - 1;
        }
        *delim = 0;
        m_thread_sel = tid;
      }

      bool cont = false;
      bool step = false;

      if (str[0] == 'C' || str[0] == 'c') {
        cont = true;
        step = false;
      } else if (str[0] == 'S' || str[0] == 's') {
        cont = true;
        step = true;
      } else {
        log.error("Unsupported command in vCont packet: %s\n", str);
        exit(-1);
      }

      if (cont) {
        if (tid == -1) {
          thread_is_started = true;
          m_top->target->prepare_resume_all(step);
        } else {
          auto core = m_top->target->get_thread(tid);
          if (core->get_power()) {
            thread_is_started = true;
            core->prepare_resume(step);
          } else if (step) stepped_tid = tid;
        }
      }

      str = strtok(NULL, ";");
    }

    if (!thread_is_started) {
      if (stepped_tid != -1) {
        // Core is off but gdb is trying to step it
        // We'll say we did the step
        size_t len;
        char str[128];
        len = snprintf(str, 128, "T%02xthread:%1x;", TARGET_SIGNAL_TRAP, stepped_tid + 1);
        return send(str, len);
      } else
        return send_str("N");
    }
    m_top->emit_started(true);
    m_rsp->set_target_state(RSP_TARGET_RUNNING);
    m_top->target->resume_all();

    return wait();
  }

  return send_str("");
}



bool Rsp::Client::query(char* data, size_t len)
{
  int ret;
  char reply[REPLY_BUF_LEN];
  log.debug("Query packet\n");
  if (strncmp ("qSupported", data, strlen ("qSupported")) == 0)
  {
    log.detail("qSupported packet\n");
    Rsp_capability::parse(data, len, &m_remote_caps);
    log.debug("swbreak: %d\n", remote_capability("swbreak"));
    if (strlen(m_top->capabilities) > 0) {
      snprintf(reply, REPLY_BUF_LEN, "PacketSize=%x;%s", REPLY_BUF_LEN, m_top->capabilities);
    } else {
      snprintf(reply, REPLY_BUF_LEN, "PacketSize=%x", REPLY_BUF_LEN);
    }
    return send_str(reply);
  }
  else if (strncmp ("qTStatus", data, strlen ("qTStatus")) == 0)
  {
    // not supported, send empty packet
    return send_str("");
  }
  else if (strncmp ("qfThreadInfo", data, strlen ("qfThreadInfo")) == 0)
  {
    reply[0] = 'm';
    ret = 1;
    for (auto &thread : m_top->target->get_threads())
    {
      ret += snprintf(&reply[ret], REPLY_BUF_LEN - ret, "%u,", thread->get_thread_id()+1);
    }

    return send(reply, ret-1);
  }
  else if (strncmp ("qsThreadInfo", data, strlen ("qsThreadInfo")) == 0)
  {
    return send_str("l");
  }
  else if (strncmp ("qThreadExtraInfo", data, strlen ("qThreadExtraInfo")) == 0)
  {
    const char* str_default = "Unknown Core";
    char str[REPLY_BUF_LEN];
    unsigned int thread_id;
    if (sscanf(data, "qThreadExtraInfo,%x", &thread_id) != 1) {
      log.error("Could not parse qThreadExtraInfo packet\n");
      return send_str("E01");
    }
    auto thread = m_top->target->get_thread(thread_id - 1);
    {
      if (thread != NULL)
        thread->get_name(str, REPLY_BUF_LEN);
      else
        strcpy(str, str_default);

      ret = 0;
      for(size_t i = 0; i < strlen(str); i++)
        ret += snprintf(&reply[ret], REPLY_BUF_LEN - ret, "%02X", str[i]);
    }

    return send(reply, ret);
  }
  else if (strncmp ("qAttached", data, strlen ("qAttached")) == 0)
  {
    if (m_top->target->is_stopped()) {
      return send_str("0");
    } else {
      return send_str("1");
    }
  }
  else if (strncmp ("qC", data, strlen ("qC")) == 0)
  {
    snprintf(reply, 64, "0.%u", m_top->target->get_thread(m_thread_sel)->get_thread_id()+1);
    return send_str(reply);
  }
  else if (strncmp ("qSymbol", data, strlen ("qSymbol")) == 0)
  {
    return send_str("OK");
  }
  else if (strncmp ("qOffsets", data, strlen ("qOffsets")) == 0)
  {
    auto current_core = m_top->target->get_thread(m_thread_sel);
    return send_str("Text=0;Data=0;Bss=0");
  }
  else if (strncmp ("qT", data, strlen ("qT")) == 0)
  {
    // not supported, send empty packet
    return send_str("");
  }
  else if (strncmp ("qRcmd", data, strlen ("qRcmd")) == 0||strncmp ("qXfer", data, strlen ("qXfer")) == 0)
  {
    int ret;
    m_top->emit_monitor_command(data, reply, REPLY_BUF_LEN, &ret);
    if (ret > 0) {
      return send_str(reply);
    } else {
      return send_str("");
    }
  }

  log.error("Unknown query packet\n");

  return send_str("");
}


bool Rsp::Client::mem_read(char* data, size_t)
{
  unsigned char buffer[512];
  char reply[1024];
  uint32_t addr;
  uint32_t length;
  uint32_t i;

  if (sscanf(data, "%x,%x", &addr, &length) != 2) {
    log.error("Could not parse packet\n");
    return false;
  }

  if (length >= 512) {
    return send_str("E01");
  }

  if (m_top->target->check_mem_access(addr, length)) {
    m_top->target->mem_read(addr, length, (char *)buffer);

    for(i = 0; i < length; i++) {
      snprintf(&reply[i * 2], 3, "%02x", (uint32_t)buffer[i]);
    }
  } else {
    log.detail("Filtered memory read attempt - area is inaccessible\n");
    memset(reply, (int)'0', length * 2);
    reply[length * 2] = '\0';
  }

  return send(reply, length*2);
}



bool Rsp::Client::mem_write_ascii(char* data, size_t len)
{
  uint32_t addr;
  int length;
  uint32_t wdata;
  size_t i, j;

  char* buffer;
  int buffer_len;

  if (sscanf(data, "%x,%d:", &addr, &length) != 2) {
    log.error("Could not parse packet\n");
    return false;
  }

  for(i = 0; i < len; i++) {
    if (data[i] == ':') {
      break;
    }
  }

  if (i == len)
    return false;

  // align to hex data
  data = &data[i+1];
  len = len - i - 1;

  buffer_len = len/2;
  buffer = (char*)malloc(buffer_len);
  if (buffer == NULL) {
    log.error("Failed to allocate buffer\n");
    return false;
  }

  for(j = 0; j < len/2; j++) {
    wdata = 0;
    for(i = 0; i < 2; i++) {
      char c = data[j * 2 + i];
      uint32_t hex = 0;
      if (c >= '0' && c <= '9')
        hex = c - '0';
      else if (c >= 'a' && c <= 'f')
        hex = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F')
        hex = c - 'A' + 10;

      wdata |= hex << (4 * i);
    }

    buffer[j] = wdata;
  }

  if (!m_top->target->check_mem_access(addr, buffer_len))
    return send_str("E03");

  m_top->target->mem_write(addr, buffer_len, buffer);

  free(buffer);

  return send_str("OK");
}

bool Rsp::Client::mem_write(char* data, size_t len)
{
  uint32_t addr;
  unsigned int length;
  size_t i;

  if (sscanf(data, "%x,%x:", &addr, &length) != 2) {
    log.error("Could not parse packet\n");
    return false;
  }

  for(i = 0; i < len; i++) {
    if (data[i] == ':') {
      break;
    }
  }

  if (i == len)
    return false;

  // align to hex data
  data = &data[i+1];
  len = len - i - 1;

  if (!m_top->target->check_mem_access(addr, len))
    return send_str("E03");

  m_top->target->mem_write(addr, len, data);
  return send_str("OK");
}

// This defines the valid CSR registers on GAP8
// TODO - Move this to config
typedef std::pair<uint32_t, uint32_t> csr_range_t;

#define CSR(__x) csr_range_t(__x, __x)
#define CSRR(__x, __y) csr_range_t(__x, __y)

const std::vector<csr_range_t> csr_ranges = {
  CSR(0x000),             // USTATUS
  CSR(0x014),             // UHartID
  CSRR(0x041, 0x042),     // UEPC, UCAUSE
  CSR(0x300),             // MSTATUS
  CSR(0x301),             // MISA
  CSR(0x305),             // MTVEC
  CSRR(0x341, 0x342),     // MEPC, MCAUSE
  CSRR(0x780, 0x79f),     // PCCRs
  CSRR(0x7a0, 0x7a1),     // PCER, PCMR
  CSRR(0x7b0, 0x7b7),     // hw loops
  CSR(0xc10),             // Priv level
  CSR(0xf14),             // MHartID
};

bool valid_csr(uint32_t csr_offset)
{
  for (auto r : csr_ranges) {
    if (csr_offset<r.first) return false;
    if (csr_offset>=r.first&&csr_offset<=r.second) return true;
  }
  return false;
}

bool Rsp::Client::reg_read(char* data, size_t)
{
  uint32_t addr;
  uint32_t rdata = 0;
  char data_str[10];

  if (sscanf(data, "%x", &addr) != 1) {
    log.error("Could not parse packet\n");
    return false;
  }

  // Note: if invalid registers are read return "xx" (not available) not ""
  // otherwise gdb gives up reading more.
  // Letting gdb do what it wants results in a core crash

  auto core = m_top->target->get_thread(m_thread_sel);
  if (!core->get_power()) {
    return send_str("xx");
  } else if (addr < 32) {
    log.debug("Read register 0x%02x\n", addr);
    if (core->register_available(addr))
      core->gpr_read(addr, &rdata);
    else
      return send_str("xx");
  } else if (addr == 0x20) {
    log.debug("Read PC\n");
    core->actual_pc_read(&rdata);
  } else if (addr >= 0x41) { // Read CSR
    uint32_t csr_num = addr - 0x41;
    if (valid_csr(csr_num) && core->register_available(addr)) {
      log.debug("Read CSR 0x%04x\n", csr_num);
      core->csr_read(csr_num, &rdata);
    } else {
      return send_str("xx");
    }
  } else {
    return send_str("xx");
  }

  rdata = htonl(rdata);
  snprintf(data_str, 9, "%08x", rdata);
  return send_str(data_str);
}

bool Rsp::Client::reg_write(char* data, size_t)
{
  uint32_t addr;
  uint32_t wdata;

  if (sscanf(data, "%x=%08x", &addr, &wdata) != 2) {
    log.error("Could not parse packet\n");
    return false;
  }

  wdata = ntohl(wdata);
  auto core = m_top->target->get_thread(m_thread_sel);
  if (core->register_available(addr)) {
    if (addr < 32) {
      log.debug("Write register 0x%02x 0x08x\n", addr, wdata);
      core->gpr_write(addr, wdata);
    } else if (addr == 32) {
      log.debug("Write NPC 0x08x\n", wdata);
      core->write(DBG_NPC_REG, wdata);
    } else if (addr >= 0x41) {
      uint32_t csr_num = addr - 0x41;
      if (valid_csr(csr_num)) {
        log.debug("Write CSR 0x%04x 0x08x\n", csr_num, wdata);
        m_top->target->get_thread(m_thread_sel)->csr_write(csr_num, wdata);
      } else {
        return send_str("E01");
      }
    } else {
      return send_str("E01");
    }
  } else {
    return send_str("E02");
  }

  return send_str("OK");
}

bool Rsp::Client::regs_send()
{
  uint32_t gpr[32];
  uint32_t pc = 0;
  char regs_str[512];
  int i;

  auto core = m_top->target->get_thread(m_thread_sel);

  if (core->get_power()) {
    core->gpr_read_all(gpr);

    // now build the string to send back
    for(i = 0; i < 32; i++) {
      if (core->register_available(i))
        snprintf(&regs_str[i * 8], 9, "%08x", (unsigned int)htonl(gpr[i]));
      else
        snprintf(&regs_str[i * 8], 9, "xxxxxxxx");
    }

    if (core->actual_pc_read(&pc)) {
      snprintf(&regs_str[32 * 8], 9, "%08x", (unsigned int)htonl(pc));
      return send_str(regs_str);
    } else
      return send_str("E02");
  } else {
    // core is off so indicate that none of its registers are available
    return send_str(UNAVAILABLE33);
  }
}

bool Rsp::Client::signal(const std::shared_ptr<Target_core> &core)
{
  char str[128];
  int len;
  RspTargetState state = m_rsp->get_target_state();
  if (state == RSP_TARGET_KILLED) {
    len = snprintf(str, 128, "X%02x", 9);
  } if (state == RSP_TARGET_EXITED) {
    len = snprintf(str, 128, "W%02x", (uint32_t) m_exit_status);
  } else if (core == NULL) {
    auto current_core = m_top->target->get_thread(m_thread_sel);
    len = snprintf(str, 128, "S%02x", get_signal(current_core));
  } else {
    int sig = get_signal(core);
    len = snprintf(str, 128, "T%02xthread:%1x;", sig, core->get_thread_id()+1);
  }

  return send(str, len);
}

int Rsp::Client::cause_to_signal(uint32_t cause, int * int_num)
{
  int res;
  if (EXC_CAUSE_INTERUPT(cause)) {
    if (int_num) {
      *int_num = cause & 0x1f;
    }
    res = TARGET_SIGNAL_INT;
  } else {
    cause &= EXC_CAUSE_MASK;
    if (cause == EXC_CAUSE_BREAKPOINT)
      res = TARGET_SIGNAL_TRAP;
    else if (cause == EXC_CAUSE_ILLEGAL_INSN)
      res = TARGET_SIGNAL_ILL;
    // else if ((cause & 0x1f) == 5) - There is no definition for this in the RTL
    //   return TARGET_SIGNAL_BUS;
    else
      res = TARGET_SIGNAL_STOP;
  }
  return res;
}

int Rsp::Client::get_signal(std::shared_ptr<Target_core> core)
{
  if (core->is_stopped()) {
    bool is_hit, is_sleeping;
    core->read_hit(&is_hit, &is_sleeping);
    if (is_hit)
      return TARGET_SIGNAL_TRAP;
    else if (is_sleeping)
      return TARGET_SIGNAL_NONE;
    else
      return cause_to_signal(core->get_cause());
  } else {
    return TARGET_SIGNAL_NONE;
  }
}

bool Rsp::Client::cont(char* data, size_t)
{
  uint32_t sig;
  uint32_t addr;
  uint32_t npc;
  bool npc_found = false;

  // strip signal first
  if (data[0] == 'C') {
    if (sscanf(data, "C%X;%X", &sig, &addr) == 2)
      npc_found = true;
  } else {
    if (sscanf(data, "c%X", &addr) == 1)
      npc_found = true;
  }

  if (npc_found) {
    auto core = m_top->target->get_thread(m_thread_sel);
    // only when we have received an address
    core->read(DBG_NPC_REG, &npc);

    if (npc != addr)
      core->write(DBG_NPC_REG, addr);
  }

  m_thread_sel = m_rsp->m_thread_init;

  m_rsp->set_target_state(RSP_TARGET_RUNNING);
  m_rsp->resume_target(false);
  return wait();
}

// This should not be used and is probably wrong
// but GDB should use vCont anyway
bool Rsp::Client::step(char* data, size_t len)
{
  uint32_t addr;
  uint32_t npc;
  size_t i;

  if (len < 1) return false;

  // strip signal first
  if (data[0] == 'S') {
    for (i = 0; i < len; i++) {
      if (data[i] == ';') {
        data = &data[i+1];
        break;
      }
    }
  }

  if (sscanf(data, "%x", &addr) == 1) {
    auto core = m_top->target->get_thread(m_thread_sel);
    // only when we have received an address
    core->read(DBG_NPC_REG, &npc);

    if (npc != addr)
      core->write(DBG_NPC_REG, addr);
  }

  m_thread_sel = m_rsp->m_thread_init;

  m_rsp->set_target_state(RSP_TARGET_RUNNING);
  m_rsp->resume_target(true);
  return wait();
}

void Rsp::Client::halt_target() {
  m_wait_te->setTimeout(kEventLoopTimerDone);
  m_rsp->halt_target();
  m_rsp->set_target_state(RSP_TARGET_STOPPED);
}

int64_t Rsp::Client::wait_routine()
{
  std::shared_ptr<Target_core> stopped_core;
  try {
    stopped_core = m_top->target->check_stopped();
  } catch (CableException &e) {
    log.error("cable exception checking stop status %s\n", e.what());
    stop();
    return kEventLoopTimerDone;
  }

  if (stopped_core) {
    // move to thread of stopped core
    m_thread_sel = stopped_core->get_thread_id();
    log.debug("found stopped core - thread %d\n", m_thread_sel + 1);
    m_rsp->halt_target();
    m_rsp->set_target_state(RSP_TARGET_STOPPED);
    if (!signal(stopped_core)) stop();
    return kEventLoopTimerDone;
  } else {
    return m_rsp->m_wait_time_usecs;
  }
}

bool Rsp::Client::wait()
{
  // run immediately the first time around
  m_wait_te->setTimeout(0);
  m_client->set_events(Readable);
  return true;
}

bool Rsp::Client::multithread(char* data, size_t len)
{
  int thread_id;
  log.debug("Subsequent %c operations on thread %s\n", data[0], &data[1]);

  if (len < 1) return false;

  switch (data[0]) {
    case 'c':
    case 'g':

      if (sscanf(&data[1], "%d", &thread_id) != 1)
        return false;

      if (thread_id == -1) // affects all threads
        return send_str("OK");

      if (thread_id != 0)
        thread_id = thread_id - 1;

      // we got the thread id, now let's look for this thread in our list
      if (m_top->target->get_thread(thread_id) != NULL) {
        m_thread_sel = thread_id;
        return send_str("OK");
      }

      return send_str("E01");
  }

  return false;
}

bool Rsp::Client::decode(char* data, size_t len)
{
  // if (data[0] == 0x03) {
  //   log.print(LOG_DEBUG, "Received break\n");
  //   return signal();
  // }

  log.detail("Received packet: '%s' (len: %d)\n", data, len);
  switch (data[0]) {
    case 'q':
      log.detail("call query\n");
      return query(data, len);

    case 'g':
      return regs_send();

    case 'p':
      return reg_read(&data[1], len-1);

    case 'P':
      return reg_write(&data[1], len-1);

    case 'c':
    case 'C':
      return cont(&data[0], len);

    case 'R':
      return run();

    case 's':
    case 'S':
      return step(&data[0], len);

    case 'H':
      return multithread(&data[1], len-1);

    case 'm':
      return mem_read(&data[1], len-1);

    case '?':
      return signal();

    case 'v':
      return v_packet(&data[0], len);

    case 'M':
      return mem_write_ascii(&data[1], len-1);

    case 'X':
      return mem_write(&data[1], len-1);

    case 'z':
      return bp_remove(&data[0], len);

    case 'Z':
      return bp_insert(&data[0], len);

    case 'T':
      return send_str("OK"); // threads are always alive

    case 'D':
      send_str("OK");
      return false;

    case '!':
      return send_str("OK"); // extended mode supported

    default:
      log.error("Unknown packet: starts with %c\n", data[0]);
      break;
  }

  return false;
}

#ifdef _WIN32
inline void timersub(const timeval* tvp, const timeval* uvp, timeval* vvp)
{
  vvp->tv_sec = tvp->tv_sec - uvp->tv_sec;
  vvp->tv_usec = tvp->tv_usec - uvp->tv_usec;
  if (vvp->tv_usec < 0)
  {
     --vvp->tv_sec;
     vvp->tv_usec += 1000000;
  }
}
#endif

bool time_has_expired(const timeval* start, const timeval* max_delay)
{
  struct timeval now, used;
  ::gettimeofday(&now, NULL);
  timersub(&now, start, &used);
  return timercmp(max_delay, &used, <);
}

bool Rsp::Client::send(const char* data, size_t len)
{
  log.debug("Sending \"%s\" (%d)\n", data, len);
  m_client->write_buffer([&](const tcp_socket_ptr_t & UNUSED(sock), const circular_buffer_ptr_t &c_buf){
    // disabled run length encoding since GDB didn't seem to like it
    m_codec->encode(data, len, c_buf, true);
  });
  return true;
}

bool Rsp::Client::send_str(const char* data)
{
  return send(data, strlen(data));
}

bool Rsp::Client::bp_insert(char* data, size_t len)
{
  enum mp_type type;
  uint32_t addr;
  int bp_len;

  if (len < 1) return false;

  if (3 != sscanf(data, "Z%1d,%x,%1d", (int *)&type, &addr, &bp_len)) {
    log.error("Could not get three arguments\n");
    return send_str("E01");
  }

  if (type != BP_MEMORY) {
    log.error("ERROR: Not a memory bp\n");
    return send_str("");
  }

  m_top->bkp->insert(addr);

  log.debug("Breakpoint inserted at 0x%08x\n", addr);
  return send_str("OK");
}



bool Rsp::Client::bp_remove(char* data, size_t len)
{
  enum mp_type type;
  uint32_t addr;
  int bp_len;

  data[len] = 0;

  if (3 != sscanf(data, "z%1d,%x,%1d", (int *)&type, &addr, &bp_len)) {
    log.error("Could not get three arguments\n");
    return false;
  }

  if (type != BP_MEMORY) {
    log.error("Not a memory bp\n");
    return send_str("");
  }

  m_top->bkp->remove(addr);

  return send_str("OK");
}

Rsp_capability::Rsp_capability(const char * name, capability_support support) : name(name), support(support)
{
}

Rsp_capability::Rsp_capability(const char * name, const char * value) : name(name), value(value), support(CAPABILITY_IS_SUPPORTED)
{
}

char *strnstr(char *str, const char *substr, size_t n)
{
    char *p = str, *pEnd = str+n;
    size_t substr_len = strlen(substr);

    if(0 == substr_len)
        return str; // the empty string is contained everywhere.

    pEnd -= (substr_len - 1);
    for(;p < pEnd; ++p)
    {
        if(0 == strncmp(p, substr, substr_len))
            return p;
    }
    return NULL;
}

void Rsp_capability::parse(char * buf, size_t len, Rsp_capabilities * caps)
{
  char * caps_buf = strnstr(buf, ":", len);
  if (!caps_buf)
    return;

  caps_buf++;
  // ensure terminated
  len = strnlen(caps_buf, len - (caps_buf - buf));
  caps_buf[len - 1] = 0;

  char * cap = strtok (caps_buf, ";");

  while (cap != NULL) {
    int last = (strlen(cap)-1);
    char cap_type = cap[last];

    switch (cap_type) {
      case '+':
        cap[last] = 0;
        caps->insert(
          std::make_pair(
            cap,
            unique_ptr<Rsp_capability>(new Rsp_capability(cap, CAPABILITY_IS_SUPPORTED))
          )
        );
        break;
      case '-':
        cap[last] = 0;
        caps->insert(
          std::make_pair(
            cap,
            unique_ptr<Rsp_capability>(new Rsp_capability(cap, CAPABILITY_NOT_SUPPORTED))
          )
        );
        break;
      case '?':
        cap[last] = 0;
        caps->insert(
          std::make_pair(
            cap,
            unique_ptr<Rsp_capability>(new Rsp_capability(cap, CAPABILITY_MAYBE_SUPPORTED))
          )
        );
        break;
      default:
        char * value = strstr(cap, "=");
        if (value) {
          *value = 0;
          value++;
          caps->insert(
            std::make_pair(
              cap,
              unique_ptr<Rsp_capability>(new Rsp_capability(cap, value))
            )
          );
        }
        break;
    }
    cap = strtok (NULL, ";");
  }
}
