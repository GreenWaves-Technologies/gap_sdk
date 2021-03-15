/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>
#include <archi/eu/eu_v4.h>

class Core_event_unit;
class Event_unit;
class Dispatch_unit;
class Mutex_unit;

// Timing constants

// Cycles required by the core to really wakeup after his clock is back
// Could actually be split into 2 latencies:
//   - THe one required by the event unit to grant the access.
//   - The one required by the core to continue after the grant is back.
//     This one should be 2 or 3 cycles and should be moved to the core.
#define EU_WAKEUP_REQ_LATENCY 6

// Cycles needed by the event unit to send back the clock once
// a core is waken-up
#define EU_WAKEUP_LATENCY 2


class Soc_event_unit {
public:

  Soc_event_unit(Event_unit *top);
  void reset();

  int nb_fifo_events;
  int nb_free_events;
  int fifo_event_head;
  int fifo_event_tail;
  int *fifo_event;
  int fifo_soc_event;

  vp::wire_slave<int>     soc_event_itf;

  vp::io_req_status_e ioReq(uint32_t offset, bool is_write, uint32_t *data);

  void check_state();

private:
  static void sync(void *__this, int event);

  Event_unit *top;
  vp::trace     trace;

};

class Semaphore {
public:
  void reset();

  uint32_t value;
  uint32_t waiting_mask;
  int elected_core;
  int width;
  vp::io_req *waiting_reqs[32];
  vp::trace     trace_value;
  vp::trace     trace_waiting_cores;

  void set_value(uint32_t value);
};

class Semaphore_unit {
public:
  Semaphore_unit(Event_unit *top);

  void reset();

  Semaphore *semaphores;
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core);

private:
  vp::io_req_status_e enqueue_sleep(Semaphore *mutex, vp::io_req *req, int core_id) ;
  Event_unit *top;
  vp::trace     trace;
  int nb_semaphores;
  int semaphore_event;
  int width;
};

class Bitfield {
public:
  void reset();

  uint32_t value;
};

class Bitfield_unit {
public:
  Bitfield_unit(Event_unit *top);

  void reset();

  Bitfield *bitfields;
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core);

private:
  Event_unit *top;
  vp::trace     trace;
  int nb_bitfields;
};


class Mutex {
public:
  void reset();

  //Plp3_ckg *top;
  bool locked;
  uint32_t waiting_mask;
  uint32_t value;
  vp::io_req *waiting_reqs[32];
  //void sleepCancel(int coreId);
  //function<void (int)> sleepCancelCallback;
};

class Mutex_unit {
public:
  Mutex_unit(Event_unit *top);

  void reset();

  //Plp3_ckg *top;
  //gv::trace trace;
  Mutex *mutexes;
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core);

private:
  vp::io_req_status_e enqueue_sleep(Mutex *mutex, vp::io_req *req, int core_id) ;
  Event_unit *top;
  vp::trace     trace;
  int nb_mutexes;
  int mutex_event;
};



class Dispatch {
public:
  //Plp3_ckg *top;
  //DispatchUnit *dispatch;
  vp::reg_32 value;
  vp::reg_32 status_mask;     // Cores that must get the value before it can be written again
  vp::reg_32 config_mask;     // Cores that will get a valid value
  vp::reg_32 waiting_mask;
  vp::io_req *waiting_reqs[32];
//
//  //gv::ioSlave_ioReq stallRetryCallbackPtr;
//  //function<void (int)> sleepCancelCallback;
//
//  //bool stallRetryCallback(gv::ioReq *req);
  //void sleepCancel(int coreId);
};


class Dispatch_core
{
public:
  int tail;
};


class Dispatch_unit {
public:
  Dispatch_unit(Event_unit *top);

  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core);
  void reset();

  vp::io_req_status_e enqueue_sleep(Dispatch *dispatch, vp::io_req *req, int core_id, bool is_caller=true);

  //Plp3_ckg *top;
  //gv::trace trace;
  //Dispatch *dispatches;
  //unsigned int globalFifoId;
  //unsigned int fifoId[32];
  unsigned int config;
  //unsigned int teamConfig;
  //bool ioReq(gv::ioReq *req, uint32_t offset, bool isRead, uint32_t *data, int coreId);
  int dispatch_event;
private:
  Event_unit *top;
  Dispatch_core *core;
  Dispatch *dispatches;
  int size;
  int fifo_head;
};



class Barrier {
public:
    uint32_t      core_mask;
    vp::reg_32    status;
    uint32_t      target_mask;
    vp::trace     trace_core_mask;
    vp::trace     trace_target_mask;
};


class Barrier_unit {
public:
  Barrier_unit(Event_unit *top);

  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core);
  void reset();

private:
  void check_barrier(int barrier_id);

  Event_unit *top;
  vp::trace     trace;
  Barrier *barriers;
  int nb_barriers;
  int barrier_event;
};


typedef enum
{
  CORE_STATE_NONE,
  CORE_STATE_WAITING_EVENT,
  CORE_STATE_WAITING_BARRIER,
  CORE_STATE_SKIP_ELW
} Event_unit_core_state_e;

class Event_unit : public vp::component
{

  friend class Core_event_unit;
  friend class Dispatch_unit;
  friend class Barrier_unit;
  friend class Semaphore_unit;
  friend class Bitfield_unit;
  friend class Mutex_unit;
  friend class Soc_event_unit;

public:

  Event_unit(js::config *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  static vp::io_req_status_e demux_req(void *__this, vp::io_req *req, int core);
  static void irq_ack_sync(void *__this, int irq, int core);

protected:

  vp::trace     trace;

  vp::io_slave in;

  Mutex_unit *mutex;
  Semaphore_unit *semaphore;
  Bitfield_unit *bitfield;
  Core_event_unit *core_eu;
  Dispatch_unit *dispatch;
  Barrier_unit *barrier_unit;
  Soc_event_unit *soc_event_unit;

  int nb_core;


  vp::io_req_status_e sw_events_req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data);
  void trigger_event(int event, uint32_t core_mask);
  void send_event(int core, uint32_t mask);
  static void in_event_sync(void *__this, bool active, int id);

};



class Core_event_unit
{
public:
  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  void build(Event_unit *top, int core_id);
  void set_status(uint32_t new_value);
  void clear_status(uint32_t mask);
  void reset();
  void check_state();
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data);
  void check_wait_mask();
  void check_pending_req();
  void cancel_pending_req();
  vp::io_req_status_e wait_event(vp::io_req *req, Event_unit_core_state_e wait_state=CORE_STATE_WAITING_EVENT);
  vp::io_req_status_e put_to_sleep(vp::io_req *req, Event_unit_core_state_e wait_state=CORE_STATE_WAITING_EVENT);
  Event_unit_core_state_e get_state() { return (Event_unit_core_state_e)state.get(); }
  void set_state(Event_unit_core_state_e state) { this->top->trace.msg("Changing state (core: %d, state: %d)\n", core_id, state); this->state.set(state); }
  void irq_ack_sync(int irq, int core);
  void wakeup_handler();
  void irq_wakeup_handler();

  vp::io_slave demux_in;

  vp::reg_32 status;
  vp::reg_32 evt_mask;
  vp::reg_32 irq_mask;
  uint32_t clear_evt_mask;
  uint32_t interrupt_elw_value;

  int sync_irq;
  int interrupted_elw;       // True if an ELW was interrupted to handle an IRQ

  vp::reg_1  is_active;

private:
  Event_unit *top;
  int core_id;
  vp::reg_32 state;
  vp::io_req *pending_req;

  vp::wire_master<bool> barrier_itf;
  vp::wire_slave<bool> in_event_itf[32];

  vp::wire_master<bool>    clock_itf;

  vp::wire_master<int>    irq_req_itf;
  vp::wire_slave<int>     irq_ack_itf;

  vp::clock_event *wakeup_event;
  vp::clock_event *irq_wakeup_event;

};



Event_unit::Event_unit(js::config *config)
: vp::component(config)
{
  nb_core = get_config_int("nb_core");
}

void Event_unit::reset(bool active)
{
  if (active)
  {
    for (int i=0; i<nb_core; i++)
    {
      core_eu[i].reset();
    }
    dispatch->reset();
    barrier_unit->reset();
    mutex->reset();
    soc_event_unit->reset();
    semaphore->reset();
  }
}

vp::io_req_status_e Event_unit::req(void *__this, vp::io_req *req)
{
  Event_unit *_this = (Event_unit *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Event_unit access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4)
  {
    _this->trace.warning("Only 32 bits accesses are allowed\n");
    return vp::IO_REQ_INVALID;
  }

  if (offset >= EU_CORES_AREA_OFFSET && offset < EU_CORES_AREA_OFFSET + EU_CORES_AREA_SIZE)
  {
    unsigned int core_id = EU_CORE_AREA_COREID_GET(offset - EU_CORES_AREA_OFFSET);
    if (core_id >= _this->nb_core) return vp::IO_REQ_INVALID;
    return _this->core_eu[core_id].req(req, offset - EU_CORE_AREA_OFFSET_GET(core_id), is_write, (uint32_t *)data);
  }
  else if (offset >= EU_SOC_EVENTS_AREA_OFFSET && offset < EU_SOC_EVENTS_AREA_OFFSET + EU_SOC_EVENTS_AREA_SIZE)
  {
    return _this->soc_event_unit->ioReq(offset - EU_SOC_EVENTS_AREA_OFFSET, is_write, (uint32_t *)data);
  }
  else if (offset >= EU_EXT_EVENT_AREA_OFFSET && offset < EU_EXT_EVENT_AREA_OFFSET + EU_EXT_EVENT_AREA_SIZE)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
  }
  else if (offset >= EU_SW_EVENTS_AREA_BASE && offset < EU_SW_EVENTS_AREA_BASE + EU_SW_EVENTS_AREA_SIZE)
  {
    return _this->sw_events_req(req, offset - EU_SW_EVENTS_AREA_BASE, is_write, (uint32_t *)data);
  }
  else if (offset >= EU_BARRIER_AREA_OFFSET && offset < EU_BARRIER_AREA_OFFSET + EU_BARRIER_AREA_SIZE)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //return barrierUnit.barrierIoReq(req, offset - EU_BARRIER_AREA_OFFSET, isRead, data, -1, false);
  }

  return vp::IO_REQ_INVALID;

}



vp::io_req_status_e Event_unit::sw_events_req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data)
{
  if (offset >= EU_CORE_TRIGG_SW_EVENT && offset <  EU_CORE_TRIGG_SW_EVENT_SIZE)
  {
    if (!is_write) return vp::IO_REQ_INVALID;

    int event = (offset - EU_CORE_TRIGG_SW_EVENT) >> 2;
    trace.msg("SW event trigger (event: %d, coreMask: 0x%x)\n", event, *data);
    trigger_event(1<<event, *data);
  }
  else if (offset >= EU_CORE_TRIGG_SW_EVENT_WAIT && offset <  EU_CORE_TRIGG_SW_EVENT_WAIT_SIZE)
  {
    trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) {
    //  trace.msg("Writing event trigger target core mask (coreMask: 0x%x)\n", *data);
    //  targetCoreMask = *data;
    //}
    //else
    //{
    //  int event = (offset - EU_CORE_TRIGG_SW_EVENT_WAIT) >> 2;
    //  trace.msg("Event trigger and wait (event: %d, coreMask: 0x%x)\n", event, *data);
    //  top->triggerEvent(1<<event, targetCoreMask);
    //  *data = waitEvent(req);
    //  return req->error != 0;
    //}
  }
  else if (offset >= EU_CORE_TRIGG_SW_EVENT_WAIT_CLEAR && offset <  EU_CORE_TRIGG_SW_EVENT_WAIT_CLEAR_SIZE)
  {
    trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) {
    //  trace.msg("Writing event trigger target core mask (coreMask: 0x%x)\n", *data);
    //  targetCoreMask = *data;
    //}
    //else
    //{
    //  int event = (offset - EU_CORE_TRIGG_SW_EVENT_WAIT) >> 2;
    //  trace.msg("Event trigger, wait and clear (event: %d, coreMask: 0x%x)\n", event, *data);
    //  top->triggerEvent(1<<event, targetCoreMask);
    //  clearEvtMask = evtMask;
    //  *data = waitEvent(req);
    //  return req->error != 0;
    //}
  }
  else
  {
    trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    return vp::IO_REQ_INVALID;    
  }

  return vp::IO_REQ_OK;
}


void Event_unit::trigger_event(int event_mask, uint32_t core_mask)
{
  for (unsigned int i=0; i<nb_core; i++) {
    if (core_mask == 0 || (core_mask & (1 << i))) {
      send_event(i, event_mask);
    }
  }
}

void Event_unit::send_event(int core, uint32_t mask)
{
  trace.msg("Triggering event (core: %d, mask: 0x%x)\n", core, mask);
  Core_event_unit *eu = &core_eu[core];
  eu->set_status(eu->status.get() | mask);
  eu->check_state();
}


void Core_event_unit::build(Event_unit *top, int core_id)
{
  this->top = top;
  this->core_id = core_id;

  this->top->new_reg("core_" + std::to_string(core_id) + "/active", &this->is_active, 1);
  this->top->new_reg("core_" + std::to_string(core_id) + "/status", &this->status, 32);
  this->top->new_reg("core_" + std::to_string(core_id) + "/evt_mask", &this->evt_mask, 32);
  this->top->new_reg("core_" + std::to_string(core_id) + "/irq_mask", &this->irq_mask, 32);
  this->top->new_reg("core_" + std::to_string(core_id) + "/state", &this->state, 32);

  demux_in.set_req_meth_muxed(&Event_unit::demux_req, core_id);
  top->new_slave_port("demux_in_" + std::to_string(core_id), &demux_in);

  //wakeup_event = top->event_new((void *)this, Core_event_unit::wakeup_handler);
  //irq_wakeup_event = top->event_new((void *)this, Core_event_unit::irq_wakeup_handler);

  top->new_master_port("irq_req_" + std::to_string(core_id), &irq_req_itf);

  top->new_master_port("clock_" + std::to_string(core_id), &clock_itf);

  irq_ack_itf.set_sync_meth_muxed(&Event_unit::irq_ack_sync, core_id);
  top->new_slave_port("irq_ack_" + std::to_string(core_id), &irq_ack_itf);

  for (int i=0; i<32; i++)
  {
    in_event_itf[i].set_sync_meth_muxed(&Event_unit::in_event_sync, (core_id << 16 | i));
    top->new_slave_port("in_event_" + std::to_string(i) + "_pe_" + std::to_string(core_id), &in_event_itf[i]);
  }

}

vp::io_req_status_e Core_event_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data)
{
  if (offset == EU_CORE_BUFFER)
  {
    if (is_write) return vp::IO_REQ_INVALID;
    *data = status.get();
    return vp::IO_REQ_OK;
  }
  else if (offset == EU_CORE_BUFFER_MASKED)
  {
    if (is_write) return vp::IO_REQ_INVALID;
    *data = status.get() & evt_mask.get();
    return vp::IO_REQ_OK;
  }
  else if (offset == EU_CORE_BUFFER_CLEAR)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    clear_status(*data);
    top->trace.msg("Clearing buffer status (mask: 0x%x, newValue: 0x%x)\n", *data, status.get());
    check_state();
    return vp::IO_REQ_OK;
  }
  else if (offset == EU_CORE_MASK)
  {
    if (!is_write) *data = evt_mask.get();
    else {
      evt_mask.set(*data);
      top->trace.msg("Updating event mask (newValue: 0x%x)\n", *data);
      check_state();
    }
  }
  else if (offset == EU_CORE_MASK_AND)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    evt_mask.set(evt_mask.get() & ~*data);
    top->trace.msg("Clearing event mask (mask: 0x%x, newValue: 0x%x)\n", *data, evt_mask);
    check_state();
  }
  else if (offset == EU_CORE_MASK_OR)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    evt_mask.set(evt_mask.get() | *data);
    top->trace.msg("Setting event mask (mask: 0x%x, newValue: 0x%x)\n", *data, evt_mask);
    check_state();
  }
  else if (offset == EU_CORE_STATUS)
  {
    top->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //*data = active;
    return vp::IO_REQ_INVALID;
  }
  else if (offset == EU_CORE_EVENT_WAIT)
  {
    if (is_write) return vp::IO_REQ_INVALID;
    top->trace.msg("Wait\n");
    vp::io_req_status_e err = wait_event(req);
    *data = evt_mask.get() & status.get();
    return err;
  }

  else if (offset == EU_CORE_EVENT_WAIT_CLEAR)
  {
    top->trace.msg("Wait and clear\n");
    clear_evt_mask = evt_mask.get();
    vp::io_req_status_e err = wait_event(req);
    *data = evt_mask.get() & status.get();
    return err;
  }
  else if (offset == EU_CORE_MASK_IRQ)
  {
    if (!is_write) *data = irq_mask.get();
    else {
      top->trace.msg("Updating irq mask (newValue: 0x%x)\n", *data);
      irq_mask.set(*data);
      check_state();
    }
  }
  else if (offset == EU_CORE_BUFFER_IRQ_MASKED)
  {
    if (is_write) return vp::IO_REQ_INVALID;
    *data = status.get() & irq_mask.get();
  }
  else if (offset == EU_CORE_MASK_IRQ_AND)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    irq_mask.set(irq_mask.get() & ~*data);
    top->trace.msg("Clearing irq mask (mask: 0x%x, newValue: 0x%x)\n", *data, irq_mask);
    check_state();
  }
  else if (offset == EU_CORE_MASK_IRQ_OR)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    irq_mask.set(irq_mask.get() | *data);
    top->trace.msg("Setting irq mask (mask: 0x%x, newValue: 0x%x)\n", *data, irq_mask);
    check_state();
  }
  else
  {
    return vp::IO_REQ_INVALID;
  }

  return vp::IO_REQ_OK;
}

void Event_unit::irq_ack_sync(void *__this, int irq, int core)
{
  Event_unit *_this = (Event_unit *)__this;

  _this->trace.msg("Received IRQ acknowledgement (core: %d, irq: %d)\n", core, irq);

  _this->core_eu[core].irq_ack_sync(irq, core);
}

vp::io_req_status_e Event_unit::demux_req(void *__this, vp::io_req *req, int core)
{
  Event_unit *_this = (Event_unit *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Demux event_unit access (core: %d, offset: 0x%x, size: 0x%x, is_write: %d)\n", core, offset, size, is_write);

  Core_event_unit *core_eu = &_this->core_eu[core];

  core_eu->interrupted_elw = false;

  if (size != 4)
  {
    _this->trace.warning("Only 32 bits accesses are allowed\n");
    return vp::IO_REQ_INVALID;
  }

  if (core_eu->get_state() == CORE_STATE_SKIP_ELW)
  {
    core_eu->set_state(CORE_STATE_NONE);
    *(uint32_t *)data = core_eu->interrupt_elw_value;
    return vp::IO_REQ_OK;
  }

  if (offset >= EU_CORE_DEMUX_OFFSET && offset < EU_CORE_DEMUX_OFFSET + EU_CORE_DEMUX_SIZE)
  {
    return _this->core_eu[core].req(req, offset - EU_CORE_DEMUX_OFFSET, is_write, (uint32_t *)data);
  }
  else if (offset >= EU_MUTEX_DEMUX_OFFSET && offset < EU_MUTEX_DEMUX_OFFSET + EU_MUTEX_DEMUX_SIZE)
  {
    return _this->mutex->req(req, offset - EU_MUTEX_DEMUX_OFFSET, is_write, (uint32_t *)data, core);
  }
  else if (offset >= EU_SEM_DEMUX_OFFSET && offset < EU_SEM_DEMUX_OFFSET + EU_SEM_DEMUX_SIZE)
  {
    return _this->semaphore->req(req, offset - EU_SEM_DEMUX_OFFSET, is_write, (uint32_t *)data, core);
  }
  else if (offset >= EU_BITFIELD_DEMUX_OFFSET && offset < EU_BITFIELD_DEMUX_OFFSET + EU_BITFIELD_DEMUX_SIZE)
  {
    return _this->bitfield->req(req, offset - EU_BITFIELD_DEMUX_OFFSET, is_write, (uint32_t *)data, core);
  }
  else if (offset >= EU_DISPATCH_DEMUX_OFFSET && offset < EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_DEMUX_SIZE)
  {
    return _this->dispatch->req(req, offset - EU_DISPATCH_DEMUX_OFFSET, is_write, (uint32_t *)data, core);
  }
  else if (offset >= EU_LOOP_DEMUX_OFFSET && offset < EU_LOOP_DEMUX_OFFSET + EU_LOOP_DEMUX_SIZE)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //return loopUnit.ioReq(req, offset - EU_LOOP_DEMUX_OFFSET, isRead, data, coreId);
  }
  else if (offset >= EU_SW_EVENTS_DEMUX_OFFSET && offset < EU_SW_EVENTS_DEMUX_OFFSET + EU_SW_EVENTS_DEMUX_SIZE)
  {
    return _this->sw_events_req(req, offset - EU_SW_EVENTS_DEMUX_OFFSET, is_write, (uint32_t *)data);
  }
  else if (offset >= EU_BARRIER_DEMUX_OFFSET && offset < EU_BARRIER_DEMUX_OFFSET + EU_BARRIER_DEMUX_SIZE)
  {
    return _this->barrier_unit->req(req, offset - EU_BARRIER_DEMUX_OFFSET, is_write, (uint32_t *)data, core);
  }

  return vp::IO_REQ_INVALID;
}

void Event_unit::in_event_sync(void *__this, bool active, int id)
{
  Event_unit *_this = (Event_unit *)__this;
  int core_id = id >> 16;
  int event_id = id & 0xffff;
  _this->trace.msg("Received input event (core: %d, event: %d, active: %d)\n", core_id, event_id, active);
  Core_event_unit *eu = &_this->core_eu[core_id];
  eu->set_status(eu->status.get() | (1<<event_id));
  eu->check_state();
}

int Event_unit::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&Event_unit::req);
  new_slave_port("input", &in);

  core_eu = (Core_event_unit *)new Core_event_unit[nb_core];
  mutex = new Mutex_unit(this);
  semaphore = new Semaphore_unit(this);
  bitfield = new Bitfield_unit(this);
  dispatch = new Dispatch_unit(this);
  barrier_unit = new Barrier_unit(this);
  soc_event_unit = new Soc_event_unit(this);

  for (int i=0; i<nb_core; i++)
  {
    core_eu[i].build(this, i);
  }

  return 0;
}

void Event_unit::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Event_unit(config);
}



void Core_event_unit::irq_ack_sync(int irq, int core)
{
  clear_status(1<<irq);
  sync_irq = -1;

  check_state();
}



void Core_event_unit::set_status(uint32_t new_value)
{
  status.set(new_value);
}

void Core_event_unit::clear_status(uint32_t mask)
{
  status.set(status.get() & ~mask);
  top->soc_event_unit->check_state();
}


void Core_event_unit::check_pending_req()
{
    if (this->pending_req)
    {
        this->pending_req->get_resp_port()->resp(pending_req);
        this->pending_req = NULL;
    }
}

void Core_event_unit::cancel_pending_req()
{
  pending_req = NULL;
}



void Core_event_unit::check_wait_mask()
{
  if (clear_evt_mask)
  {
    clear_status(clear_evt_mask);
    top->trace.msg("Clear event after wake-up (evtMask: 0x%x, status: 0x%x)\n", clear_evt_mask, status);
    clear_evt_mask = 0;
  }
}

vp::io_req_status_e Core_event_unit::put_to_sleep(vp::io_req *req, Event_unit_core_state_e wait_state)
{
  state.set(wait_state);
  this->is_active.set(0);
  this->clock_itf.sync(0);
  pending_req = req;
  return vp::IO_REQ_PENDING;
}

vp::io_req_status_e Core_event_unit::wait_event(vp::io_req *req, Event_unit_core_state_e wait_state)
{
  top->trace.msg("Wait request (status: 0x%x, evt_mask: 0x%x)\n", status, evt_mask);

  // This takes 2 cycles for the event unit to clock-gate the core with replying
  // so this is seen as a latency of 2 cycles from the core point of view.
  // This will be added by the core after it is waken up.
  // Also if the event is already there it takes 2 cycles just to decide that we don't
  // go to sleep.
  req->inc_latency(EU_WAKEUP_REQ_LATENCY);

#if 1
  // Experimental model where the core always go to sleep even if the event is there
  // This gives much better tming results on barrier.
  // Check if it is also the case for other features and check in the RTL how it is
  // handled.
  if (evt_mask.get() & status.get())
  {
    // Case where the core ask for clock-gating but the event status prevent him from doing so
    // In this case, don't forget to clear the status in case of wait and clear.
    // Still apply the latency as to core will go to sleep before continuing.
    top->trace.msg("Activating clock (core: %d)\n", core_id);
    check_wait_mask();
    req->inc_latency(EU_WAKEUP_LATENCY);
    this->wakeup_handler();
    return vp::IO_REQ_OK;
  }
  else
  {
    return this->put_to_sleep(req, wait_state);
  }
#else
  if (evt_mask & status)
  {
    // Case where the core ask for clock-gating but the event status prevent him from doing so
    // In this case, don't forget to clear the status in case of wait and clear.
    // Still apply the latency as to core will go to sleep before continuing.
    req->inc_latency(EU_WAKEUP_LATENCY);
    check_wait_mask();
    return vp::IO_REQ_OK;
  }
  else
  {
    state = wait_state;
    pending_req = req;
    return vp::IO_REQ_PENDING;
  }
#endif
}

vp::io_req_status_e Core_event_unit::req(void *__this, vp::io_req *req)
{
  Event_unit *_this = (Event_unit *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Event_unit access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (offset == EU_CORE_BUFFER)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //*data = status;
    //checkCoreState();
  }
  else if (offset == EU_CORE_BUFFER_MASKED)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //*data = status & evtMask;
  }
  else if (offset == EU_CORE_BUFFER_CLEAR)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) return true;
    //setStatus(req->getSecure(), status & (~*data));
    //trace.msg("Clearing buffer status (mask: 0x%x, newValue: 0x%x)\n", *data, status);
    //checkCoreState();
  }
  else if (offset == EU_CORE_MASK)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) *data = evtMask;
    //else {
    //  evtMask = *data;
    //  trace.msg("Updating event mask (newValue: 0x%x)\n", *data);
    //  checkCoreState();
    //}
  }
  else if (offset == EU_CORE_MASK_AND)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) return true;
    //evtMask &= ~*data;
    //trace.msg("Clearing event mask (mask: 0x%x, newValue: 0x%x)\n", *data, evtMask);
    //checkCoreState();
  }
  else if (offset == EU_CORE_MASK_OR)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) return true;
    //evtMask |= *data;
    //trace.msg("Setting event mask (mask: 0x%x, newValue: 0x%x)\n", *data, evtMask);
    //checkCoreState();
  }
  else if (offset == EU_CORE_STATUS)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //*data = active;
  }
  else if (offset == EU_CORE_EVENT_WAIT)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //trace.msg("Wait\n");
    //*data = waitEvent(req);
    //return req->error != 0;
  }

  else if (offset == EU_CORE_EVENT_WAIT_CLEAR)
  {

    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //trace.msg("Wait and clear\n");
    //clearEvtMask = evtMask;
    //*data = waitEvent(req);
    //return req->error != 0;
  }
  else if (offset == EU_CORE_MASK_IRQ)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) *data = irqMask;
    //else {
    //  trace.msg("Updating irq mask (newValue: 0x%x)\n", *data);
    //  irqMask = *data;
    //  checkCoreState();
    //}
  }
  else if (offset == EU_CORE_BUFFER_IRQ_MASKED)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (!isRead) return true;
    //*data = status & irqMask;
  }
  else if (offset == EU_CORE_MASK_IRQ_AND)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) return true;
    //irqMask &= ~*data;
    //trace.msg("Clearing irq mask (mask: 0x%x, newValue: 0x%x)\n", *data, irqMask);
    //checkCoreState();
  }
  else if (offset == EU_CORE_MASK_IRQ_OR)
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
    //if (isRead) return true;
    //irqMask |= *data;
    //trace.msg("Setting irq mask (mask: 0x%x, newValue: 0x%x)\n", *data, irqMask);
    //checkCoreState();
  }
  else
  {
    _this->trace.warning("UNIMPLEMENTED at %s %d\n", __FILE__, __LINE__);
  }

  return vp::IO_REQ_INVALID;

  return vp::IO_REQ_OK;
}

void Core_event_unit::reset()
{
  status.set(0);
  evt_mask.set(0);
  irq_mask.set(0);
  clear_evt_mask = 0;
  sync_irq = -1;
  interrupted_elw = false;
  this->pending_req = NULL;
  state.set(CORE_STATE_NONE);
  this->clock_itf.sync(1);
}

void Core_event_unit::wakeup_handler()
{
  this->top->trace.msg("Replying to core after wakeup (core: %d)\n", this->core_id);
  this->is_active.set(1);
  this->clock_itf.sync(1);
  this->check_pending_req();
  this->check_state();
}

void Core_event_unit::irq_wakeup_handler()
{
  this->top->trace.msg("IRQ wakeup\n");
  this->is_active.set(1);
  this->clock_itf.sync(1);
  this->check_state();
}


void Core_event_unit::check_state()
{
  //top->trace.msg("Checking core state (coreId: %d, active: %d, waitingEvent: %d, status: 0x%llx, evtMask: 0x%llx, irqMask: 0x%llx)\n", coreId, active, waitingEvent, status, evtMask, irqMask);

  uint32_t status_irq_masked = status.get() & irq_mask.get();
  uint32_t status_evt_masked = status.get() & evt_mask.get();
  int irq = status_irq_masked ? 31 - __builtin_clz(status_irq_masked) : -1;

  top->trace.msg("Checking core state (coreId: %d, active: %d, status: 0x%llx, evtMask: 0x%llx, irqMask: 0x%llx)\n", core_id, this->is_active.get(), status.get(), evt_mask.get(), irq_mask.get());

  if (this->is_active.get())
  {
    if (irq != sync_irq) {
      top->trace.msg("Updating irq req (core: %d, irq: %d)\n", core_id, irq);
      sync_irq = irq;
      irq_req_itf.sync(irq);
    }
  }

  if (status_evt_masked)
  {
    // If a wakeup occurs while the core is handling an interrupt (e.g. a barrier is reached)
    // skip the next elw 
    if (this->interrupted_elw)
    {
      top->trace.msg("Wakeup during interrupted elw (core: %d)\n", core_id);
      this->set_state(CORE_STATE_SKIP_ELW);
      check_wait_mask();
      return;
    }
  }

  if (!this->is_active.get())
  {
    if (status_irq_masked && !status_evt_masked)
    {
      // There is an active IRQ but no event, the core must be be waken up
      // just for the duration of the IRQ handler. The elw instruction will
      // replay the access, so we must keep the state as it is to resume
      // the on-going synchronization.
      top->trace.msg("Activating clock for IRQ handling(core: %d)\n", core_id);

      // Remeber that we interrupted an ELW so that we can properly restore it after IRQ handling
      this->interrupted_elw = true;

      //if (!irq_wakeup_event->is_enqueued())
      {
        this->irq_wakeup_handler();
        //top->event_enqueue(irq_wakeup_event, EU_WAKEUP_LATENCY);
        sync_irq = -1;
      }
    }
    else
    {
      if (status_evt_masked)
      {
          switch (state.get())
          {
            case CORE_STATE_WAITING_EVENT:
            case CORE_STATE_WAITING_BARRIER:

              top->trace.msg("Activating clock (core: %d)\n", core_id);
              state.set(CORE_STATE_NONE);
              check_wait_mask();
              this->wakeup_handler();
              break;
          }
      }
    }
  }
}


/****************
 * SEMAPHORE UNIT
 ****************/

Semaphore_unit::Semaphore_unit(Event_unit *top)
: top(top)
{
    top->traces.new_trace("sem/trace", &trace, vp::DEBUG);
    nb_semaphores = top->get_config_int("**/properties/semaphores/nb_semaphores");
    this->width = top->get_config_int("**/properties/semaphores/width");
    semaphore_event = top->get_config_int("**/properties/events/semaphore");
    semaphores = new Semaphore[nb_semaphores];

    for (int i=0; i<nb_semaphores; i++)
    {
        Semaphore *sem = &this->semaphores[i];
        sem->width = width;
        this->top->traces.new_trace_event("sem" + std::to_string(i) + "/value", &sem->trace_value, 8);
        this->top->traces.new_trace_event("sem" + std::to_string(i) + "/waiting_cores", &sem->trace_waiting_cores, 32);
    }
}

vp::io_req_status_e Semaphore_unit::enqueue_sleep(Semaphore *mutex, vp::io_req *req, int core_id) {
  Core_event_unit *core_eu = &top->core_eu[core_id];

  // Enqueue the request so that the core can be unstalled when a value is pushed
  mutex->waiting_reqs[core_id] = req;
  mutex->waiting_mask |= 1<<core_id;
  mutex->trace_waiting_cores.event((uint8_t *)&mutex->waiting_mask);

  // Don't forget to remember to clear the event after wake-up by the dispatch event
  core_eu->clear_evt_mask = 1<<semaphore_event;

  return core_eu->wait_event(req);
}


void Semaphore_unit::reset()
{
  for (int i=0; i<nb_semaphores; i++)
  {
    semaphores[i].reset();
  }
}


void Semaphore::reset()
{
  value = 0;
  waiting_mask = 0;
  elected_core = 0;
  this->trace_value.event((uint8_t *)&value);
  this->trace_waiting_cores.event((uint8_t *)&waiting_mask);
}


void Semaphore::set_value(uint32_t value)
{
  this->value = value & ((1 << this->width) - 1);
}


vp::io_req_status_e Semaphore_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core)
{
  unsigned int id = EU_SEM_AREA_SEMID_GET(offset);
  offset = offset - (id << EU_SEM_SIZE_LOG2);

  if (id >= nb_semaphores) return vp::IO_REQ_INVALID;

  Semaphore *semaphore = &semaphores[id];
  Core_event_unit *evtUnit = &top->core_eu[core];
  this->trace.msg("Received semaphore IO access (offset: 0x%x, mutex: %d, is_write: %d)\n", offset, id, is_write);
  
  if (offset == EU_HW_SEM_VALUE)
  {
    if (!is_write)
    {
      *data = semaphore->value;
    }
    else
    {
      this->trace.msg("Setting semaphore value (semaphore: %d, value: 0x%x)\n", id, *data);
      semaphore->set_value(*data);
    }
  }
  else if (offset == EU_HW_SEM_COUNTER)
  {
    if (!is_write)
    {
      if (semaphore->value > 0)
      {
        semaphore->set_value(semaphore->value - 1);
        semaphore->trace_value.event((uint8_t *)&semaphore->value);
        this->trace.msg("Decrementing semaphore (semaphore: %d, coreId: %d, new_value: %d)\n", id, core);
      }
      else
      {
        this->trace.msg("Blocking semaphore, waiting (semaphore: %d, coreId: %d)\n", id, core);
        return enqueue_sleep(semaphore, req, core);
      }
    }
    else
    {
      semaphore->set_value(semaphore->value +  *(uint32_t *)req->get_data());
      semaphore->trace_value.event((uint8_t *)&semaphore->value);
      this->trace.msg("Incrementing semaphore (semaphore: %d, core: %d, inc: %d, value: %d)\n", id, core, *(uint32_t *)req->get_data(), semaphore->value);

      // The core is unlocking the semaphore, check if we have to wake-up someone
      while (semaphore->value > 0 && semaphore->waiting_mask)
      {
        if (semaphore->waiting_mask & (1<<semaphore->elected_core))
        {
          // Clear the mask and wake-up the core.
          this->trace.msg("Waking-up core waiting for semaphore (coreId: %d)\n", semaphore->elected_core);
          vp::io_req *waiting_req = semaphore->waiting_reqs[semaphore->elected_core];

          semaphore->waiting_mask &= ~(1<<semaphore->elected_core);
          semaphore->trace_waiting_cores.event((uint8_t *)&semaphore->waiting_mask);

          // Store the semaphore value into the pending request
          // Don't reply now to the initiator, this will be done by the wakeup event
          // to introduce some delays
          if (!this->top->core_eu[semaphore->elected_core].interrupted_elw)
          {
            *(uint32_t *)waiting_req->get_data() = semaphore->value;
          }
          else
          {
            this->top->core_eu[semaphore->elected_core].interrupt_elw_value = semaphore->value;
          }

          // And trigger the event to the core
          top->trigger_event(1<<semaphore_event, 1<<semaphore->elected_core); 

          semaphore->set_value(semaphore->value - 1);
          semaphore->trace_value.event((uint8_t *)&semaphore->value);
        }

        semaphore->elected_core++;
        if (semaphore->elected_core == this->top->nb_core)
        {
          semaphore->elected_core = 0;
        }
      }
    }
  }
  else if (offset == EU_HW_SEM_LOAD_INC)
  {
    if (!is_write)
    {
      *data = semaphore->value;
      semaphore->set_value(semaphore->value + 1);
      this->trace.msg("Incrementing semaphore (semaphore: %d, core: %d, value: %d)\n", id, core, semaphore->value);
      semaphore->trace_value.event((uint8_t *)&semaphore->value);
    }
  }

  return vp::IO_REQ_OK;
}




/****************
 * BITFIELD UNIT
 ****************/

Bitfield_unit::Bitfield_unit(Event_unit *top)
: top(top)
{
  top->traces.new_trace("bitfield/trace", &trace, vp::DEBUG);
  nb_bitfields = top->get_config_int("**/properties/bitfields/nb_bitfields");
  bitfields = new Bitfield[nb_bitfields];
}


void Bitfield_unit::reset()
{
  for (int i=0; i<nb_bitfields; i++)
  {
    bitfields[i].reset();
  }
}

void Bitfield::reset()
{
  value = 0;
}

vp::io_req_status_e Bitfield_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core)
{
  unsigned int id = EU_BITFIELD_AREA_BITFIELDID_GET(offset);
  offset = offset - (id << EU_BITFIELD_SIZE_LOG2);

  if (id >= nb_bitfields) return vp::IO_REQ_INVALID;

  Bitfield *bitfield = &bitfields[id];

  if (is_write)
  {
    if (offset == EU_HW_BITFIELD_VALUE)
    {
      bitfield->value = *data;
      this->trace.msg("Setting value (bitfield: %d, value: 0x%x)\n", id, bitfield->value);
    }
    else if (offset == EU_HW_BITFIELD_SET)
    {
      bitfield->value |= *data;
      this->trace.msg("Bitfield set (bitfield: %d, mask: 0x%x, value: 0x%x)\n", id, *data, bitfield->value);
    }
    else if (offset == EU_HW_BITFIELD_CLEAR)
    {
      bitfield->value &= ~(*data);
      this->trace.msg("Bitfield clear (bitfield: %d, mask: 0x%x, value: 0x%x)\n", id, *data, bitfield->value);
    }
  }
  else
  {
    if (offset == EU_HW_BITFIELD_VALUE)
    {
      *data = bitfield->value;
    }
    else if (offset == EU_HW_BITFIELD_ALLOC)
    {
      int bit = ffs(bitfield->value);
      if (bit == 0)
      {
        *data = 32;
        this->trace.msg("No more bit to allocate (bitfield: %d)\n", id);
      }
      else
      {
        bit = bit - 1;
        bitfield->value &= ~(1<<bit);
        *data = bit;
        this->trace.msg("Bitfield alloc (bitfield: %d, bit: 0x%d, value: %x)\n", id, bit, bitfield->value);
      }
    }
  }

  return vp::IO_REQ_OK;
}



/****************
 * MUTEX UNIT
 ****************/

Mutex_unit::Mutex_unit(Event_unit *top)
: top(top)
{
  top->traces.new_trace("mutex/trace", &trace, vp::DEBUG);
  nb_mutexes = top->get_config_int("**/properties/mutex/nb_mutexes");
  mutex_event = top->get_config_int("**/properties/events/mutex");
  mutexes = new Mutex[nb_mutexes];
}


vp::io_req_status_e Mutex_unit::enqueue_sleep(Mutex *mutex, vp::io_req *req, int core_id) {
  Core_event_unit *core_eu = &top->core_eu[core_id];

  // Enqueue the request so that the core can be unstalled when a value is pushed
  mutex->waiting_reqs[core_id] = req;
  mutex->waiting_mask |= 1<<core_id;

  // Don't forget to remember to clear the event after wake-up by the dispatch event
  core_eu->clear_evt_mask = 1<<mutex_event;

  return core_eu->wait_event(req);
}


void Mutex_unit::reset()
{
  for (int i=0; i<nb_mutexes; i++)
  {
    mutexes[i].reset();
  }
}

void Mutex::reset()
{
  locked = false;
  waiting_mask = 0;
}


#if 0
void Mutex::sleepCancel(int coreId)
{
  // The core is being interrupted while waiting on the mutex
  // Clear the waiting bit so that this core is ignored in case the owner of the lock unlocks it
  // and the core can redo the access to the mutex later on
  waitMask &= ~(1<<coreId);
  top->coreEvt[coreId].pendingSleep = false;
}
#endif

vp::io_req_status_e Mutex_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core)
{
  unsigned int id = EU_MUTEX_AREA_MUTEXID_GET(offset);
  offset = offset - (id << 2);
  if (id >= nb_mutexes) return vp::IO_REQ_INVALID;

  
  Mutex *mutex = &mutexes[id];
  Core_event_unit *evtUnit = &top->core_eu[core];
  top->trace.msg("Received mutex IO access (offset: 0x%x, mutex: %d, is_write: %d)\n", offset, id, is_write);
  
  if (!is_write)
  {
    if (!mutex->locked)
    {
      // The mutex is free, just lock it
      top->trace.msg("Locking mutex (mutex: %d, coreId: %d)\n", id, core);
      mutex->locked = 1;
    }
    else
    {
      // The mutex is locked, put the core to sleep
      top->trace.msg("Mutex already locked, waiting (mutex: %d, coreId: %d)\n", id, core);
      return enqueue_sleep(mutex, req, core);
    }
  }
  else
  {
    mutex->value = *(uint32_t *)req->get_data();

    // The core is unlocking the mutex, check if we have to wake-up someone
    unsigned int waiting_mask = mutex->waiting_mask;
    if (waiting_mask)
    {
      // We have to wake-up one core, take the first one
      for (unsigned int i=0; i<32; i++)
      {
        if (waiting_mask & (1<<i))
        {
          top->trace.msg("Transfering mutex lock (mutex: %d, fromCore: %d, toCore: %d)\n", id, core, i);
          // Clear the mask and wake-up the elected core. Don't unlock the mutex, as it is
          // taken by the new core
          top->trace.msg("Waking-up core waiting for dispatch value (coreId: %d)\n", i);
          vp::io_req *waiting_req = mutex->waiting_reqs[i];

          mutex->waiting_mask &= ~(1<<i);

          // Store the mutex value into the pending request
          // Don't reply now to the initiator, this will be done by the wakeup event
          // to introduce some delays
          if (!this->top->core_eu[i].interrupted_elw)
          {
            *(uint32_t *)waiting_req->get_data() = mutex->value;
          }
          else
          {
            this->top->core_eu[i].interrupt_elw_value = mutex->value;
          }

          // And trigger the event to the core
          top->trigger_event(1<<mutex_event, 1<<i); 

          break;
        }
      }
    } 
    else
    {
      // No one waiting, just unlock the mutex
      top->trace.msg("Unlocking mutex (mutex: %d, coreId: %d)\n", id, core);
      mutex->locked = 0;
    }
  }

  return vp::IO_REQ_OK;
}



/****************
 * DISPATCH UNIT
 ****************/


vp::io_req_status_e Dispatch_unit::enqueue_sleep(Dispatch *dispatch, vp::io_req *req, int core_id, bool is_caller) {
  Core_event_unit *core_eu = &top->core_eu[core_id];

  // Enqueue the request so that the core can be unstalled when a value is pushed
  dispatch->waiting_reqs[core_id] = req;
  dispatch->waiting_mask.set(dispatch->waiting_mask.get() | (1<<core_id));

  // Don't forget to remember to clear the event after wake-up by the dispatch event
  core_eu->clear_evt_mask = 1<<dispatch_event;

  if (is_caller)
    return core_eu->wait_event(req);
  else
    return core_eu->put_to_sleep(req);
}



Dispatch_unit::Dispatch_unit(Event_unit *top)
: top(top)
{
  dispatch_event = top->get_config_int("**/properties/events/dispatch");
  size = top->get_config_int("**/properties/dispatch/size");
  core = new Dispatch_core[top->nb_core];
  dispatches = new Dispatch[size];
}

  void Dispatch_unit::reset()
  {
    fifo_head = 0;
    config = 0;
    for (int i=0; i<top->nb_core; i++)
    {
      core[i].tail = 0;
    }
    for (int i=0; i<size; i++)
    {
      this->top->new_reg("dispatcher" + std::to_string(i) + "/value", &dispatches[i].value, 32);
      this->top->new_reg("dispatcher" + std::to_string(i) + "/status_mask", &dispatches[i].status_mask, 32);
      this->top->new_reg("dispatcher" + std::to_string(i) + "/config_mask", &dispatches[i].config_mask, 32);
      this->top->new_reg("dispatcher" + std::to_string(i) + "/waiting_mask", &dispatches[i].waiting_mask, 32);
      dispatches[i].value.set(0);
      dispatches[i].status_mask.set(0);
      dispatches[i].config_mask.set(0);
      dispatches[i].waiting_mask.set(0);
    }
  }

  vp::io_req_status_e Dispatch_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core_id)
  {
    if (offset == EU_DISPATCH_FIFO_ACCESS)
    {
      if (is_write)
      {
        unsigned int id = fifo_head++;
        if (fifo_head == size) fifo_head = 0;

        Dispatch *dispatch = &dispatches[id];

        // When pushing to the FIFO, the global config is pushed to the elected dispatcher
        dispatch->config_mask.set(config);     // Cores that will get a valid value

        top->trace.msg("Pushing dispatch value (dispatch: %d, value: 0x%x, coreMask: 0x%x)\n", id, *data, dispatch->config_mask);

        // Case where the master push a value
        dispatch->value.set(*data);
        // Reinitialize the status mask to notify a new value is ready
        dispatch->status_mask.set(-1);
        // Then wake-up the waiting cores
        unsigned int mask = dispatch->waiting_mask.get() & dispatch->status_mask.get();
        for (int i=0; i<32 && mask; i++)
        {
          if (mask & (1<<i))
          {
            // Only wake-up the core if he's actually involved in the team
            if (dispatch->config_mask.get() & (1<<i))
            {
              top->trace.msg("Waking-up core waiting for dispatch value (coreId: %d)\n", i);
              vp::io_req *waiting_req = dispatch->waiting_reqs[i];

              // Clear the status bit as the waking core takes the data
              dispatch->status_mask.set(dispatch->status_mask.get() & (~(1<<i)));
              dispatch->waiting_mask.set(dispatch->waiting_mask.get() & (~(1<<i)));

              // Store the dispatch value into the pending request
              // Don't reply now to the initiator, this will be done by the wakeup event
              // to introduce some delays
              if (!this->top->core_eu[i].interrupted_elw)
              {
              top->trace.msg("Storing to pending (coreId: %d)\n", i);
                *(uint32_t *)waiting_req->get_data() = dispatch->value.get();
              }
              else
              {
              top->trace.msg("Storing to interrupted_elw (coreId: %d)\n", i);
                this->top->core_eu[i].interrupt_elw_value = dispatch->value.get();
              }

              // Update the core fifo
              core[i].tail++;
              if (core[i].tail == size) core[i].tail = 0;

              // Clear the mask to stop iterating early
              mask &= ~(1<<i);

              // And trigger the event to the core
              top->trigger_event(1<<dispatch_event, 1<<i); 
            }
            // Otherwise keep him sleeping and increase its index so that he will bypass this entry when he wakes up
            else
            {
              // Cancel current dispatch sleep
              dispatch->status_mask.set(dispatch->status_mask.get() & (~(1<<i)));
              dispatch->waiting_mask.set(dispatch->waiting_mask.get() & (~(1<<i)));
              vp::io_req *pending_req = dispatch->waiting_reqs[i];

              // Bypass the current entry
              core[i].tail++;
              if (core[i].tail == size) core[i].tail = 0;

              // And reenqueue to the next entry
              id = core[i].tail;
              enqueue_sleep(&dispatches[id], pending_req, i, false);
              top->trace.msg("Incrementing core counter to bypass entry (coreId: %d, newIndex: %d)\n", i, id);
            }
          }
        }

        return vp::IO_REQ_OK;        
      }
      else
      {
        int id = core[core_id].tail;
        Dispatch *dispatch = &dispatches[id];

        top->trace.msg("Trying to get dispatch value (dispatch: %d)\n", id);

        // In case we found ready elements where this core is not involved, bypass them all
        while ((dispatch->status_mask.get() & (1<<core_id)) && !(dispatch->config_mask.get() & (1<<core_id))) {
          dispatch->status_mask.set(dispatch->status_mask.get() & (~(1<<core_id)));
          core[core_id].tail++;
          if (core[core_id].tail == size) core[core_id].tail = 0;
          id = core[core_id].tail;
          dispatch = &dispatches[id];
          top->trace.msg("Incrementing core counter to bypass entry (coreId: %d, newIndex: %d)\n", core_id, id);
        }

        // Case where a slave tries to get a value
        if (dispatch->status_mask.get() & (1<<core_id))
        {
          // A value is ready. Get it and clear the status bit to not read it again the next time
          // In case the core is not involved in this dispatch, returns 0
          if (dispatch->config_mask.get() & (1<<core_id)) *data = dispatch->value.get();
          else *data = 0;
          dispatch->status_mask.set(dispatch->status_mask.get() & (~(1<<core_id)));
          top->trace.msg("Getting ready dispatch value (dispatch: %d, value: %x, dispatchStatus: 0x%x)\n", id, dispatch->value.get(), dispatch->status_mask.get());
          core[core_id].tail++;
          if (core[core_id].tail == size) core[core_id].tail = 0;
        }
        else
        {
          // Nothing is ready, go to sleep
          top->trace.msg("No ready dispatch value, going to sleep (dispatch: %d, value: %x, dispatchStatus: 0x%x)\n", id, dispatch->value, dispatch->status_mask);
          return enqueue_sleep(dispatch, req, core_id);
        }

        return vp::IO_REQ_OK;       
      }

      return vp::IO_REQ_INVALID;
    }
    else if (offset == EU_DISPATCH_TEAM_CONFIG)
    {
      config = *data;
      return vp::IO_REQ_OK;
    }
    else
    {
      return vp::IO_REQ_INVALID;
    }
  }






/****************
 * BARRIER UNIT
 ****************/


Barrier_unit::Barrier_unit(Event_unit *top)
: top(top)
{
  top->traces.new_trace("barrier/trace", &trace, vp::DEBUG);
  nb_barriers = top->get_config_int("**/properties/barriers/nb_barriers");
  barrier_event = top->get_config_int("**/properties/events/barrier");
  barriers = new Barrier[nb_barriers];

  for (int i=0; i<nb_barriers; i++)
  {
      Barrier *barrier = &this->barriers[i];
      this->top->new_reg("barrier" + std::to_string(i) + "/status", &barrier->status, 32);
      this->top->traces.new_trace_event("barrier" + std::to_string(i) + "/core_mask", &barrier->trace_core_mask, 32);
      this->top->traces.new_trace_event("barrier" + std::to_string(i) + "/target_mask", &barrier->trace_target_mask, 32);
  }
}

void Barrier_unit::check_barrier(int barrier_id)
{
  Barrier *barrier = &barriers[barrier_id];

  if (barrier->status.get() == barrier->core_mask) 
  {
    trace.msg("Barrier reached, triggering event (barrier: %d, coreMask: 0x%x, targetMask: 0x%x)\n", barrier_id, barrier->core_mask, barrier->target_mask);
    barrier->status.set(0);

    top->trigger_event(1<<barrier_event, barrier->target_mask);
  }
}


vp::io_req_status_e Barrier_unit::req(vp::io_req *req, uint64_t offset, bool is_write, uint32_t *data, int core)
{
  unsigned int barrier_id = EU_BARRIER_AREA_BARRIERID_GET(offset);
  offset = offset - EU_BARRIER_AREA_OFFSET_GET(barrier_id);
  if (barrier_id >= nb_barriers) return vp::IO_REQ_INVALID;
  Barrier *barrier = &barriers[barrier_id];

  if (offset == EU_HW_BARR_TRIGGER_MASK)
  {
    if (!is_write) *data = barrier->core_mask;
    else {
      trace.msg("Setting barrier core mask (barrier: %d, mask: 0x%x)\n", barrier_id, *data);
      barrier->core_mask = *data;
      barrier->trace_core_mask.event((uint8_t *)&barrier->core_mask);
      check_barrier(barrier_id);
    }
  }

  else if (offset == EU_HW_BARR_TARGET_MASK)
  {
    if (!is_write) *data = barrier->target_mask;
    else {
      trace.msg("Setting barrier target mask (barrier: %d, mask: 0x%x)\n", barrier_id, *data);
      barrier->target_mask = *data;
      barrier->trace_target_mask.event((uint8_t *)&barrier->target_mask);
      check_barrier(barrier_id);
    }
  }
  else if (offset == EU_HW_BARR_STATUS)
  {
    if (!is_write) *data = barrier->status.get();
    else {
      trace.msg("Setting barrier status (barrier: %d, status: 0x%x)\n", barrier_id, *data);
      barrier->status.set(*data);
      check_barrier(barrier_id);
    }
  }
  else if (offset == EU_HW_BARR_TRIGGER)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    else {
      barrier->status.set(barrier->status.get() | (*data));
      trace.msg("Barrier mask trigger (barrier: %d, mask: 0x%x, newStatus: 0x%x)\n", barrier_id, *data, barrier->status.get());
    }

    check_barrier(barrier_id);
  }
  else if (offset == EU_HW_BARR_TRIGGER_SELF)
  {
    // The access is valid only through the demux
    if (core == -1) return vp::IO_REQ_INVALID;
    barrier->status.set(barrier->status.get() | (1 << core));

    trace.msg("Barrier trigger (barrier: %d, coreId: %d, newStatus: 0x%x)\n", barrier_id, core, barrier->status.get());

    check_barrier(barrier_id);
  }
  else if (offset == EU_HW_BARR_TRIGGER_WAIT)
  {
    // The access is valid only through the demux
    if (core == -1) return vp::IO_REQ_INVALID;

    Core_event_unit *core_eu = &top->core_eu[core];
    if (core_eu->get_state() == CORE_STATE_WAITING_BARRIER)
    {
      // The core was already waiting for the barrier which means it was interrupted
      // by an interrupt. Just resume the barrier by going to sleep
      trace.msg("Resuming barrier trigger and wait (barrier: %d, coreId: %d, newStatus: 0x%x)\n", barrier_id, core, barrier->status.get());
    }
    else
    {
      barrier->status.set(barrier->status.get() | (1 << core));
      trace.msg("Barrier trigger and wait (barrier: %d, coreId: %d, newStatus: 0x%x)\n", barrier_id, core, barrier->status.get());
    }

    check_barrier(barrier_id);

    return core_eu->wait_event(req, CORE_STATE_WAITING_BARRIER);
  }
  else if (offset == EU_HW_BARR_TRIGGER_WAIT_CLEAR)
  {
    // The access is valid only through the demux
    if (core == -1) return vp::IO_REQ_INVALID;

    Core_event_unit *core_eu = &top->core_eu[core];
    if (core_eu->get_state() == CORE_STATE_WAITING_BARRIER)
    {
      // The core was already waiting for the barrier which means it was interrupted
      // by an interrupt. Just resume the barrier by going to sleep
      trace.msg("Resuming barrier trigger and wait (barrier: %d, coreId: %d, mask: 0x%x, newStatus: 0x%x)\n", barrier_id, core, barrier->core_mask, barrier->status.get());
    }
    else
    {
      barrier->status.set(barrier->status.get() | (1 << core));
      trace.msg("Barrier trigger, wait and clear (barrier: %d, coreId: %d, newStatus: 0x%x)\n", barrier_id, core, barrier->status.get());
    }
    core_eu->clear_evt_mask = core_eu->evt_mask.get();

    check_barrier(barrier_id);

    return core_eu->wait_event(req, CORE_STATE_WAITING_BARRIER);
  }
  else if (offset == EU_HW_BARR_STATUS_SUMMARY)
  {
    if (is_write) return vp::IO_REQ_INVALID;
    uint32_t status = 0;
    for (unsigned int i=1; i<nb_barriers; i++) status |= barriers[i].status.get();
    *data = status;
  }
  else return vp::IO_REQ_INVALID;

  return vp::IO_REQ_OK;
}

void Barrier_unit::reset()
{
  for (int i=0; i<nb_barriers; i++)
  {
    Barrier *barrier = &barriers[i];
    barrier->core_mask = 0;
    barrier->status.set(0);
    barrier->target_mask = 0;
    barrier->trace_core_mask.event((uint8_t *)&barrier->core_mask);
    barrier->trace_target_mask.event((uint8_t *)&barrier->target_mask);
  }
}


Soc_event_unit::Soc_event_unit(Event_unit *top) : top(top)
{
  this->nb_fifo_events = top->get_config_int("**/nb_fifo_events");
  this->fifo_soc_event = top->get_config_int("**/fifo_event");

  top->traces.new_trace("soc_eu/trace", &trace, vp::DEBUG);

  this->fifo_event = new int[nb_fifo_events];

  this->soc_event_itf.set_sync_meth(&Soc_event_unit::sync);
  top->new_slave_port(this, "soc_event", &this->soc_event_itf);

  this->reset();
}

void Soc_event_unit::reset()
{
  this->nb_free_events = this->nb_fifo_events;
  this->fifo_event_head = 0;
  this->fifo_event_tail = 0;
}

void Soc_event_unit::check_state()
{
  if (this->fifo_soc_event != -1 && this->nb_free_events != this->nb_fifo_events) {
    this->trace.msg("Generating FIFO event (id: %d)\n", this->fifo_soc_event);
    this->top->trigger_event(1<<this->fifo_soc_event, -1);
  }
}

void Soc_event_unit::sync(void *__this, int event)
{
  Soc_event_unit *_this = (Soc_event_unit *)__this;
  _this->trace.msg("Received soc event (event: %d)\n", event);

  if (_this->nb_free_events == 0) {
    return;
  }

  _this->nb_free_events--;
  _this->fifo_event[_this->fifo_event_head] = event;
  _this->fifo_event_head++;
  if (_this->fifo_event_head == _this->nb_fifo_events) _this->fifo_event_head = 0;

  _this->check_state();
}

vp::io_req_status_e Soc_event_unit::ioReq(uint32_t offset, bool is_write, uint32_t *data)
{
  if (is_write) return vp::IO_REQ_INVALID;

  if (nb_free_events == nb_fifo_events) {
    trace.msg("Reading FIFO with no event\n");
    *data = 0;
  } else {
    trace.msg("Popping event from FIFO (id: %d)\n", fifo_event[fifo_event_tail]);
    *data = (1 << EU_SOC_EVENTS_VALID_BIT) | fifo_event[fifo_event_tail];
    fifo_event_tail++;
    if (fifo_event_tail == nb_fifo_events) fifo_event_tail = 0;
    nb_free_events++;
    if (nb_free_events != nb_fifo_events)
    {
      this->trace.msg("Generating FIFO soc event (id: %d)\n", this->fifo_soc_event);
      this->top->trigger_event(1<<this->fifo_soc_event, -1);
    }
  }

  return vp::IO_REQ_OK;
}
