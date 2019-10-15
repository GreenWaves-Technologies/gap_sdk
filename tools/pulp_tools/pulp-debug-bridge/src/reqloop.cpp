
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
 *          Martin Croome, (martin.croome@greenwaves-technologues.com)
 */

#include "reqloop.hpp"

#define PTR_2_INT(__addr) ((unsigned int)(reinterpret_cast<std::uintptr_t>(__addr)&0xffffffff))
#define INT_2_PTR(__addr) (reinterpret_cast<std::uintptr_t>((size_t)__addr))

void ReqLoop::set_debug_struct_addr(unsigned int debug_struct_addr) {
    m_debug_struct_addr = debug_struct_addr;
}

void ReqLoop::target_stopped(bool target_stopped) {
    if (m_stopped) return;
    m_target_stopped = target_stopped;
    if (!target_stopped) {
      m_loop_te->setTimeout(0);
    }
}

void ReqLoop::start(bool target_stopped) {
  m_stopped = false;
  m_target_stopped = target_stopped;
  log.debug("ReqLoop started\n");
  m_loop_te->setTimeout(0);
}

void ReqLoop::stop() {
  if (m_stopped) return;
  log.debug("ReqLoop stopped\n");
  m_stopped = true;
  m_state = REQLOOP_INIT;
  m_loop_te->setTimeout(kEventLoopTimerDone);
}

void ReqLoop::access(bool write, unsigned int addr, int len, char * buf)
{
  if (!m_cable->access(write, addr, len, buf))
    throw LoopCableException();
}

void ReqLoop::activate()
{
  if (this->m_debug_struct == NULL)
  {
    access(false, this->m_debug_struct_addr, 4, (char*)&this->m_debug_struct);

    if (this->m_debug_struct != NULL)
    {
      uint32_t protocol_version;
      access(false, PTR_2_INT(&this->m_debug_struct->protocol_version), 4, (char*)&protocol_version);
      
      if (protocol_version != PROTOCOL_VERSION_4)
      {
        log.error("Protocol version mismatch between bridge and runtime (bridge: %d, runtime: %d)\n", PROTOCOL_VERSION_4, protocol_version);
        throw std::logic_error("Unable to connect to runtime");
      }

      int32_t is_connected;
      access(false, PTR_2_INT(&this->m_debug_struct->target.connected), 4, (char*)&is_connected);
      this->m_target_connected = is_connected;

      // The binary has just started, we need to tell him we want to watch for requests
      unsigned int value = 0;

      uint32_t connected = 1;
      access(true, PTR_2_INT(&this->m_debug_struct->bridge.connected), 4, (char*)&connected);
      if (m_do_printf)
        access(true, PTR_2_INT(&this->m_debug_struct->use_internal_printf), 4, (char*)&value);
    }
  }
}

unsigned int ReqLoop::get_target_state()
{
  unsigned int value;
  this->m_cable->jtag_get_reg(7, 4, &value, this->m_jtag_val);
  value &= 0xf;
  //printf("READ JTAG %x\n", value >> 1);
  return value >> 1;
}

void ReqLoop::send_target_ack()
{
  this->m_jtag_val = 0x7 << 1;
  this->m_cable->jtag_set_reg(7, 4, this->m_jtag_val);
}

void ReqLoop::clear_target_ack()
{
  this->m_jtag_val = 0x0 << 1;
  this->m_cable->jtag_set_reg(7, 4, this->m_jtag_val);
}

bool ReqLoop::wait_target_request()
{
  // In case the target does not support synchronization through the JTAG
  // register, just consider that the target is always available.
  if (!this->m_target_jtag_sync)
    return true;

  switch (this->m_target_sync_fsm_state)
  {
    case TARGET_SYNC_FSM_STATE_INIT: {
      // This state is used in case the bridge is not yet connected to the
      // target (e.g. if it boots from flash).
      // Wait until the target becomes available and ask for the connection.
      unsigned int state = this->get_target_state();
      if (state == 4)
      {
        // Target is asking for connection, acknowledge it and go to next step
        this->send_target_ack();
        this->m_target_sync_fsm_state = TARGET_SYNC_FSM_STATE_WAIT_INIT;
      }
      return false;
    }

    case TARGET_SYNC_FSM_STATE_WAIT_INIT: {
      // This state is used to wait for the target acknoledgment, after
      // it asked for connection
      unsigned int state = this->get_target_state();
      if (state != 4)
      {
        // Once the target acknowledged the connection, activate the bridge
        // clear the acknowledge and wait for target availability
        this->activate();
        this->clear_target_ack();
        this->m_target_sync_fsm_state = TARGET_SYNC_FSM_STATE_WAIT_REQUEST;
      }
      return true;
    }

    case TARGET_SYNC_FSM_STATE_WAIT_AVAILABLE: {
      // We are in the state when the target cannot be accessed.
      // Wait until we see something different from 0.
      // We also need to clear the request acknowledgement to let the target
      // know that we got it, in case we come from state
      // TARGET_SYNC_FSM_STATE_WAIT_ACK.
      this->clear_target_ack();
      unsigned int state = this->get_target_state();

      if (state == 0)
        return false;

      this->m_target_sync_fsm_state = TARGET_SYNC_FSM_STATE_WAIT_REQUEST;
      emit_available(true);
      return false;
    }

    case TARGET_SYNC_FSM_STATE_WAIT_REQUEST: {
      // The target became available, now wait for a request
      unsigned int state = this->get_target_state();

      if (state == 1)
      {
        // The target is still available with no request, stay in this state
        return false;
      }

      if (state == 2)
      {
        // The target is requesting a shutdown, acknowledge it and stop
        // accessing the target
        emit_available(false);
        this->send_target_ack();
        this->m_target_sync_fsm_state = TARGET_SYNC_FSM_STATE_WAIT_ACK;
        return false;
      }

      // The target is requesting something else, just report true so that
      // the caller now checks what is requested through JTAG and stay in
      // this state.
      return true;
    }

    case TARGET_SYNC_FSM_STATE_WAIT_ACK: {
      // The chip is not accessible, just wait until it becomes available
      // again
      unsigned int state = this->get_target_state();
      if (state != 0)
        return false;

      this->m_target_sync_fsm_state = TARGET_SYNC_FSM_STATE_WAIT_AVAILABLE;
      return false;
    }

    default:
      return false;
  }
}

int64_t ReqLoop::reqloop_routine()
{
  if (m_stopped) return kEventLoopTimerDone;

  switch (this->m_state) {

    case REQLOOP_INIT: {
      this->activate();
      // In case the birdge is not yet connected, do extra init steps to
      // connect once the target becomes available
      this->m_target_sync_fsm_state = this->m_debug_struct ? TARGET_SYNC_FSM_STATE_WAIT_AVAILABLE : TARGET_SYNC_FSM_STATE_INIT;

      this->m_jtag_val = 0;
      this->m_state = REQLOOP_IDLE;
      return 0;
    }

    case REQLOOP_IDLE: {
      if (!this->m_debug_struct_addr) {
        log.warning("Trying to launch request loop (command reqloop) while no binary is provided");
        return kEventLoopTimerDone;
      }


      // Wait until the target is available and has a request.
      // This will poll the target through the JTAG register.
      if (!this->wait_target_request())
      {
        return 100;
      }

      if (m_check_exit) {
        uint32_t value;
        access(false, PTR_2_INT(&this->m_debug_struct->exit_status), 4, (char*)&value);
        if (value >> 31) {
          int32_t status = ((int32_t)value << 1) >> 1;
          log.user("Detected end of application, exiting with status: %d\n", status);
          emit_exit(status);
          return kEventLoopTimerDone;
        }
      }
    }

    // Fallthrough.
    case REQLOOP_PRINTF: {
      if (m_do_printf) {
        uint32_t value;
        access(false, PTR_2_INT(&this->m_debug_struct->pending_putchar), 4, (char*)&value);
        int cnt = 0;
        while(value > 0) {
          if (cnt++ > m_printf_loops) {
            this->m_state = REQLOOP_PRINTF;
            return 0;
          }
          char buff[value+1];
          access(false, PTR_2_INT(&this->m_debug_struct->putc_buffer), value, &(buff[0]));
          buff[value] = 0;
          unsigned int zero = 0;
          access(true, PTR_2_INT(&this->m_debug_struct->pending_putchar), 4, (char*)&zero);
          fputs(buff, stdout);
          fflush(NULL);
          access(false, PTR_2_INT(&this->m_debug_struct->pending_putchar), 4, (char*)&value);
        }
        this->m_state = REQLOOP_IDLE;
      }
      emit_req_event(true);
      if (m_target_stopped) {
        // If target has stopped we have now flushed printfs so slow down
        return 1000000;
      } else {
        // If we cannot sync then slow the loop down
        return (this->m_target_jtag_sync ? 0 : 500);
      }
    }

    default: {
      log.error("unexpected reqloop state");
      return kEventLoopTimerDone;
    }
  }
}

ReqLoop::ReqLoop(
    const EventLoop::SpEventLoop &event_loop, std::shared_ptr<Cable> cable, unsigned int debug_struct_addr, bool do_printf, bool check_exit) : 
      log("REQLOOP"), m_cable(std::move(cable)), m_debug_struct_addr(debug_struct_addr), m_do_printf(do_printf), m_check_exit(check_exit) {

  js::config *config = m_cable->get_config();  
  this->m_target_jtag_sync = (config?config->get_child_bool("**/debug_bridge/target_jtag_sync"):false);
  m_loop_te = event_loop->getTimerEvent(std::bind(&ReqLoop::reqloop_routine, this));
}

ReqLoop::~ReqLoop() {
}

