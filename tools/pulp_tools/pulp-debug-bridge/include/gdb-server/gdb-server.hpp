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
 * Authors: Andreas Traber
 */

#ifndef __GDB_SERVER_GDB_SERVER_H__
#define __GDB_SERVER_GDB_SERVER_H__

#include <list>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

#define MAX_ERROR_LEN 256

#include <thread>
#include <unordered_map>
#include <exception>
#include <atomic>

#include "cable.hpp"
#include "json.hpp"

#include "events/tcp-events.hpp"
#include "rsp-packet-codec.hpp"

#define DBG_CTRL_REG  0x0
#define DBG_HIT_REG   0x4
#define DBG_IE_REG    0x8
#define DBG_CAUSE_REG 0xC
#define DBG_NPC_REG   0x2000
#define DBG_PPC_REG   0x2004

#define DBG_CAUSE_BP  0x3

enum exception_causes {
  EXC_CAUSE_NONE         = 0x00,
  EXC_CAUSE_ILLEGAL_INSN = 0x02,
  EXC_CAUSE_BREAKPOINT   = 0x03,
  EXC_CAUSE_ECALL_UMODE  = 0x08,
  EXC_CAUSE_ECALL_MMODE  = 0x0B,
  EXC_CAUSE_DBG_HALT     = 0x1F,
  EXC_CAUSE_MASK         = EXC_CAUSE_DBG_HALT
};

#define EXC_CAUSE_INTERUPT(cause) (cause&(1<<31))

enum target_signal {
  TARGET_SIGNAL_NONE =  0,
  TARGET_SIGNAL_INT  =  2,
  TARGET_SIGNAL_ILL  =  4,
  TARGET_SIGNAL_TRAP =  5,
  TARGET_SIGNAL_FPE  =  8,
  TARGET_SIGNAL_KILL  = 9,
  TARGET_SIGNAL_BUS  = 10,
  TARGET_SIGNAL_SEGV = 11,
  TARGET_SIGNAL_ALRM = 14,
  TARGET_SIGNAL_STOP = 17,
  TARGET_SIGNAL_USR2 = 31,
  TARGET_SIGNAL_PWR  = 32
};

typedef int (*cmd_cb_t)(const char * cmd, char * rsp_buf, int rsp_buf_len);

class Rsp;
class Breakpoints;
class Target;
class Target_cluster_common;
class Target_cluster;
class Target_core;


class CableException : public std::exception 
{
public:
  CableException(const char * format, ...) : std::exception() {
    va_list args;
    va_start(args, format);
    vsnprintf(error_msg, MAX_ERROR_LEN, format, args);
    va_end(args);
  }
  const char * what() const throw() {return error_msg;}
private:
  char error_msg[MAX_ERROR_LEN];
};

class OffCoreAccessException: public std::exception
{
public:
  const char* what() const throw()
  {
    return "Attempt to access core that is switched off";
  }
};

SMART_EMITTER(GdbServerTargetStarted, started)
SMART_EMITTER(GdbRspRunCommand, run)
SMART_EMITTER(GdbMonitorCommand, monitor_command)

class Gdb_server : public GdbServerTargetStartedEmitter<bool>, public GdbMonitorCommandEmitter<const char *, char *, int, int *>, public GdbRspRunCommandEmitter<char *>
{
public:
  Gdb_server(const EventLoop::SpEventLoop &event_loop, const std::shared_ptr<Cable> &cable, js::config *config, int socket_port,
    const char * capabilities);
  void start();
  int stop();
  void abort();
  void print(const char *format, ...);
  int target_is_started();
  void start_target();
  void stop_target();
  void refresh_target();
  void target_update_power();
  void signal_exit(int status);

  Log log;
  EventLoop::SpEventLoop m_event_loop;
  std::shared_ptr<Cable> cable;
  js::config *config;
  int m_socket_port;
  const char * capabilities;

  std::shared_ptr<Target> target;
  std::shared_ptr<Breakpoints> bkp;
  std::shared_ptr<Rsp> rsp;
};

class Target_core
{
public:
  Target_core(Gdb_server * top, uint32_t dbg_unit_addr, Target_cluster_common * cluster, int core_id);
  void set_power(bool is_on);
  bool get_power();
  bool has_power_state_change();
  void read(uint32_t addr, uint32_t* rdata);
  void write(uint32_t addr, uint32_t wdata);
  void csr_read(unsigned int i, uint32_t *data);
  void csr_write(unsigned int i, uint32_t data);
  void ie_write(uint32_t data);
  int get_thread_id() { return this->thread_id; }
  int get_cluster_id();
  int get_core_id() { return this->core_id; }
  void get_name(char* str, size_t len);
  bool register_available(int num);

  bool actual_pc_read(unsigned int* pc);
  bool is_stopped();
  void read_hit(bool *ss_hit, bool *is_sleeping);
  bool is_stopped_on_trap() { return pc_is_cached && on_trap; }
  bool decide_resume();
  uint32_t get_cause();
  uint32_t check_stopped();

  void stop();
  void halt();
  void prepare_resume(bool step=false);
  void clear_resume() { resume_prepared = false; }
  void commit_resume();
  bool should_resume() { return resume_prepared; }
  void set_step_mode(bool new_step);
  bool get_step_mode() { return step; }
  void commit_step_mode();
  void resume();
  void flush();
  void init();

  void gpr_read_all(uint32_t *data);
  void gpr_read(unsigned int i, uint32_t *data);
  void gpr_write(unsigned int i, uint32_t data);

private:
  Gdb_server * m_top;
  uint32_t dbg_unit_addr;
  Target_cluster_common * cluster;
  int core_id;

  bool is_on = false;
  bool power_state_changed = false;
  uint32_t hartid;
  int thread_id;
  bool pc_is_cached = false;
  uint32_t pc_cached;
  bool stopped = false;
  bool step = false;
  bool commit_step = false;
  bool resume_prepared = false;
  bool on_trap = false;
  bool m_is_sleeping = false;
  bool m_half_stopped = false;
  static int first_free_thread_id;
};

class Target {
public:
  using SpTarget_core = std::shared_ptr<Target_core>;
  Target(Gdb_server * top);
  ~Target();
  inline int get_nb_threads() { return cores.size(); }


  void halt();
  // void resume(bool step=false, int tid=-1);
  void clear_resume_all();
  void prepare_resume_all(bool step);
  void resume_all();
  bool wait();
  void flush();
  void reinitialize();
  void update_power();
  bool is_stopped() { return !started; }
  void mem_read(uint32_t addr, uint32_t length, char * buffer);
  void mem_write(uint32_t addr, uint32_t length, char * buffer);
  bool check_mem_access(uint32_t addr, uint32_t length);

  SpTarget_core check_stopped();

  std::vector<SpTarget_core> get_threads() { return cores; }
  SpTarget_core get_thread(int thread_id) { return cores_from_threadid[thread_id]; }
  SpTarget_core get_thread_from_id(int id) { return cores[id]; }


private:
  Gdb_server * m_top;
  std::vector<std::shared_ptr<Target_cluster_common>> clusters;
  std::vector<SpTarget_core> cores;
  std::map<int, SpTarget_core> cores_from_threadid;
  bool started = true;
};


class Breakpoints {
  public:
    class Breakpoint {
      public:
        Breakpoint(Gdb_server * top, uint32_t addr);
      private:
        friend class Breakpoints;
        void enable();
        void disable();
        Gdb_server * m_top;
        uint32_t addr;
        union {
          uint32_t insn_orig32;
          uint16_t insn_orig16;
        };
        bool is_compressed;
        bool enabled = false;
    };

    Breakpoints(Gdb_server * top);

    void insert(unsigned int addr);
    void remove(unsigned int addr);

    void enable_all();
    void disable_all();

    void clear();
    void clear_history();
    bool have_changed();
    bool at_addr(unsigned int addr);

  private:
    typedef std::shared_ptr<Breakpoint> breakpoint_ptr_t;
    typedef std::map<uint32_t, breakpoint_ptr_t> breakpoints_map_t;

    breakpoints_map_t breakpoints;

    breakpoints_map_t enabled_bps;
    breakpoints_map_t disabled_bps;
    Gdb_server * m_top;

    void remove_it(breakpoints_map_t::iterator it);

    // These do not keep history as yet so should not be used externally
    void disable(unsigned int addr);
    void enable(unsigned int addr);
};

enum capability_support {
  CAPABILITY_NOT_SUPPORTED = 0,
  CAPABILITY_MAYBE_SUPPORTED = 1,
  CAPABILITY_IS_SUPPORTED = 2
};

using std::unique_ptr;
class Rsp_capability;
using Rsp_capabilities = std::unordered_map<std::string,unique_ptr<Rsp_capability>>;

class Rsp_capability
{
public:
  Rsp_capability(const char * name, capability_support support);
  Rsp_capability(const char * name, const char * value);
  static void parse(char * buf, size_t len, Rsp_capabilities *caps);
  bool is_supported() { return support == CAPABILITY_IS_SUPPORTED; }
private:
  std::string name, value;
  capability_support support;
};


class Rsp : public std::enable_shared_from_this<Rsp> {
  public:
    Rsp(Gdb_server * top, int port, const EventLoop::SpEventLoop &event_loop, int64_t wait_time_usecs = 50000);
    ~Rsp() {}
    void start();
    void stop();
    void abort();
    void init();
    void signal_exit(int32_t status);

    enum RspTargetState {
      RSP_TARGET_RUNNING,
      RSP_TARGET_STOPPED,
      RSP_TARGET_EXITED,
      RSP_TARGET_KILLED
    };

    void set_target_state(RspTargetState state) { m_target_state = state; }
    RspTargetState get_target_state() { return m_target_state; }

    class Client
    {
      friend Rsp;
      public:
        Client(std::shared_ptr<Rsp> rsp, tcp_socket_ptr_t client);
        void stop();
        bool is_running() { return m_rsp->get_target_state() == RSP_TARGET_RUNNING; };
        bool send_str(const char* data);
        void signal_exit(int32_t status);
      private:

        void halt_target();
        bool try_decode(char * pkt, size_t pkt_len);
        bool remote_capability(const char * name) {
          Rsp_capabilities::const_iterator it = m_remote_caps.find (name);
          return it != m_remote_caps.end() && it->second.get()->is_supported();
        }
        int cause_to_signal(uint32_t cause, int * int_num = nullptr);
        int get_signal(std::shared_ptr<Target_core> core);
        bool decode(char* data, size_t len);
        size_t get_packet(char* data, size_t len);

        void client_routine();

        bool regs_send();
        bool signal(const std::shared_ptr<Target_core> &core=nullptr);

        bool multithread(char* data, size_t len);

        bool v_packet(char* data, size_t len);

        bool run(char * filename = NULL);

        bool query(char* data, size_t len);

        bool send(const char* data, size_t len);

        bool cont(char* data, size_t len); // continue, reserved keyword, thus not used as function name
        bool wait();
        int64_t wait_routine();
        bool step(char* data, size_t len);

        // internal helper functions

        bool reg_read(char* data, size_t len);
        bool reg_write(char* data, size_t len);

        bool mem_read(char* data, size_t len);
        bool mem_write_ascii(char* data, size_t len);
        bool mem_write(char* data, size_t len);

        bool bp_insert(char* data, size_t len);
        bool bp_remove(char* data, size_t len);

        int32_t m_exit_status = 0;

        std::shared_ptr<RspPacketCodec> m_codec;
        EventLoop::SpTimerEvent m_wait_te;

        int m_thread_sel;

        Rsp_capabilities m_remote_caps;
        Gdb_server * m_top;

        std::shared_ptr<Rsp>  m_rsp = nullptr;
        tcp_socket_ptr_t m_client = nullptr;

        int m_packet_timeout = 2000;
        Log log;
    };

    typedef std::shared_ptr<Rsp::Client> rsp_client_ptr_t;
    rsp_client_ptr_t get_client() { return m_client; }

  private:
    void client_connected(const tcp_socket_ptr_t &client);
    void client_disconnected(const tcp_socket_ptr_t &client);
    void rsp_client_finished();
    void notify_finished();
    void resume_target(bool step=false, int tid=-1);
    void halt_target();
    void stop_listener();

    std::shared_ptr<Target_core> main_core = nullptr;
    rsp_client_ptr_t m_client = nullptr;
    Gdb_server * m_top = nullptr;
    std::shared_ptr<Tcp_listener> m_listener;

    RspTargetState m_target_state = RSP_TARGET_STOPPED;
    int m_thread_init;
    int m_port;
    EventLoop::SpEventLoop m_event_loop;
    int64_t m_wait_time_usecs;
    int m_conn_cnt = 0;
    bool m_running = false, m_aborted = false, m_stopping = false;
    Log log;
};

#endif