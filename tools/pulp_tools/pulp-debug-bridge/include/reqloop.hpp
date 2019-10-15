/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies SA
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
 * Authors: Martin Croome, GWT (martin.croome@greenwaves.technologies.com)
 */

#ifndef __REQLOOP_HPP__
#define __REQLOOP_HPP__

#include <stdint.h>
#include <list>
#include <queue>
#include <memory>
#include <functional>
#include <exception>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "debug_bridge/debug_bridge.h"
#include "cables/cable.hpp"
#include "events/events.hpp"
#include "events/emitter.hpp"

#define LOOP_DEFAULT_LOOP_USECS 10000
#define LOOP_DEFAULT_SLOW_LOOP_USECS 10000000

typedef enum 
{
  TARGET_SYNC_FSM_STATE_INIT,
  TARGET_SYNC_FSM_STATE_WAIT_INIT,
  TARGET_SYNC_FSM_STATE_WAIT_AVAILABLE,
  TARGET_SYNC_FSM_STATE_WAIT_REQUEST,
  TARGET_SYNC_FSM_STATE_WAIT_ACK
} target_sync_fsm_state_e;

class LoopCableException: public std::exception
{
public:
  const char* what() const throw()
  {
    return "Exception accessing cable";
  }
};

typedef enum {
  REQLOOP_INIT,
  REQLOOP_IDLE,
  REQLOOP_PRINTF
} reqloop_state_t;

SMART_EMITTER(ProgramExit, exit)
SMART_EMITTER(Available, available)
SMART_EMITTER(ReqEvent, req_event)

class ReqLoop : public ProgramExitEmitter<int32_t>, public AvailableEmitter<bool>, public ReqEventEmitter<bool> {
public:
  ReqLoop(const EventLoop::SpEventLoop &event_loop, std::shared_ptr<Cable> cable, unsigned int debug_struct_addr, bool do_printf = true, bool check_exit = true);
  ~ReqLoop();

  void set_debug_struct_addr(unsigned int debug_struct_addr);
  void start(bool target_stopped);
  void target_stopped(bool target_stopped);
  void stop();
  void access(bool write, unsigned int addr, int len, char * buf);
  void set_printf_loops(int loops) { m_printf_loops = loops; }
  int64_t reqloop_routine();

  Log log;
  bool get_target_available() {
    return m_target_sync_fsm_state == TARGET_SYNC_FSM_STATE_WAIT_AVAILABLE ||
      m_target_sync_fsm_state == TARGET_SYNC_FSM_STATE_WAIT_REQUEST;
  }

private:
  bool wait_target_request();
  void send_target_ack();
  void clear_target_ack();
  void activate();
  unsigned int get_target_state();

  reqloop_state_t m_state = REQLOOP_INIT;

  EventLoop::SpTimerEvent m_loop_te;
  std::shared_ptr<Cable> m_cable;     

  unsigned int m_debug_struct_addr, m_jtag_val;
  hal_debug_struct_t *m_debug_struct = NULL;

  int64_t m_slow_usecs, m_fast_usecs, m_cur_usecs;

  bool m_stopped = true, m_target_stopped = false, m_target_connected = false, m_do_printf, m_check_exit, m_target_jtag_sync;

  target_sync_fsm_state_e m_target_sync_fsm_state;
  int m_printf_loops = 10;
};

#endif