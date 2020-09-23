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

#include "vp/vp.hpp"
#include "vp/itf/io.hpp"
#include "vp/itf/wire.hpp"
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

#include "archi/dma/mchan_v6.h"

// The number of writes to an input port queue register, that triggers an enqueue
#define MAX_CMD_WORDS 5

#define MCHAN_NB_COUNTERS 16

class mchan;
class Mchan_channel;

// The structure describing a DMA command
class Mchan_cmd {
public:
  Mchan_cmd(mchan *top) {}
  void init();

  // As the node is written through a sequence in the same register, this gives the step in the sequence
  // (from 0 to MAX_CMD_WORDS - 1). This triggers an enqueue when it reaches MAX_CMD_WORDS
  int step;

  // The raw content of the command (the sequence of writes)
  uint32_t content[MAX_CMD_WORDS];

  // Unpacked command fields
  int type;
  int incr;
  int is_2d;
  uint32_t received_size;
  uint32_t size_to_write;
  uint32_t size_to_read;
  uint32_t line_size_to_read;
  uint64_t source;
  uint64_t source_chunk;
  uint64_t dest;
  uint64_t dest_chunk;
  uint32_t stride;
  uint32_t length;
  uint32_t remLength;
  int loc2ext;         // 1 if transfer is from local port to external port
  int size;            // Size in bytes
  int size_left;
  int counter_id;       // Counter id for transfer termination
  int raise_irq;        // If not 0, raise an interrupt at end of transfer
  int raise_event;
  int broadcast;

  int id;
  
  Mchan_channel *channel;  // The channel port from which the command arrived

  void set_next(Mchan_cmd *next) { this->next = next; }
  Mchan_cmd *get_next() { return next; }

  //gv::event *event;

  //void endOfCmdEventHandle(Plp3_dma *dma);
  //void handleCounterZero(Plp3_dma *dma, int inputId);

private:
  Mchan_cmd *next;        // Used for linking command in linked lists
};

// The structure used for describing a DMA queue (global or private) as a linked list of commands
template<class T>
class Mchan_queue
{
public:
  Mchan_queue(int size) : size(size) {}
  void init() { first=NULL; last=NULL; nb_cmd=0; }
  T *pop();
  T *pop(bool loc2ext);
  void push(T *cmd);
  bool is_full() { return nb_cmd >= size; }
  bool is_empty() { return nb_cmd == 0; }
  T *get_first() { return first; }

private:
  T *first;    // First command of the queue, commands are popped from here
  T *last;     // Last command of the queue, commands are pushed here
  int        nb_cmd;
  int        size;
};


class Mchan_channel
{
  friend class mchan;

public:
  Mchan_channel(int id, mchan *top);
  vp::io_req_status_e req(vp::io_req *req);
  void reset();

protected:

  Mchan_cmd *pop_cmd(bool read_queue);
  void trigger_event(Mchan_cmd *cmd);

  int current_counter;

private:

  vp::io_req_status_e handle_queue_req(vp::io_req *req, bool is_write, uint32_t *value);
  vp::io_req_status_e handle_status_req(vp::io_req *req, bool is_write, uint32_t *value);
  vp::io_req_status_e handle_queue_write(vp::io_req *req, uint32_t *value);
  bool check_command(Mchan_cmd *cmd);
  int unpack_command(Mchan_cmd *cmd);
  void handle_req(vp::io_req *req, uint32_t *value);

  int id;
  mchan *top;
  vp::io_slave in;
  vp::io_req *pending_req;
  int pending_cmd;
  Mchan_cmd *current_cmd;
  Mchan_queue<Mchan_cmd> *pending_cmds;

  vp::wire_master<bool> event_itf;
  vp::wire_master<bool> irq_itf;
  vp::wire_master<bool> ext_irq_itf;

};

class mchan : public vp::component
{

  friend class Mchan_channel;

public:

  mchan(js::config *config);

  int build();
  void start();
  void reset(bool active);

protected:
  static vp::io_req_status_e req(void *__this, vp::io_req *req, int id);
  uint32_t get_status();
  void free_counters(uint32_t counter_mask);
  int alloc_counter(vp::io_req *req, Mchan_channel *channel);
  int do_alloc_counter(Mchan_channel *channel);
  Mchan_cmd *get_command();
  void free_command(Mchan_cmd *cmd);
  void check_queue();

  int pending_bytes[MCHAN_NB_COUNTERS];
  int nb_core_read_cmd;
  int nb_core_write_cmd;

private:

  static void check_queue_handler(void *_this, vp::clock_event *event);
  static void check_ext_read_handler(void *_this, vp::clock_event *event);
  static void check_ext_write_handler(void *_this, vp::clock_event *event);
  static void check_loc_transfer_handler(void *_this, vp::clock_event *event);
  void move_to_global_queue(bool read_queue);
  void push_req_to_loc(vp::io_req *req);
  void send_req();
  void send_loc_read_req();
  static void ext_grant(void *_this, vp::io_req *req);
  static void ext_response(void *_this, vp::io_req *req);
  static void loc_grant(void *_this, vp::io_req *req);
  static void loc_response(void *_this, vp::io_req *req);
  void handle_cmd_termination(Mchan_cmd *cmd);
  void account_transfered_bytes(Mchan_cmd *cmd, int bytes);
  void send_req_to_ext(Mchan_cmd *cmd, vp::io_req *req);
  void handle_ext_write_req_end(Mchan_cmd *cmd, vp::io_req *req);

  vp::trace     trace;

  int nb_channels;
  int core_queue_depth;
  int global_queue_depth;
  bool is_64;
  int max_nb_ext_read_req;
  int max_nb_ext_write_req;
  int max_burst_length;
  int nb_loc_ports;
  int tcdm_addr_width;

  int nb_pending_ext_read_req;
  int nb_pending_ext_write_req;
  uint32_t free_counter_mask;
  vector<Mchan_channel *> channels;
  vp::io_req *first_alloc_pending_req;
  vp::io_req *last_alloc_pending_req;
  vp::clock_event *check_queue_event;
  vp::clock_event *check_ext_read_event;
  vp::clock_event *check_ext_write_event;
  vp::clock_event *check_loc_transfer_event;
  int sched_core_queue;
  Mchan_queue<Mchan_cmd> *pending_read_cmds;
  Mchan_queue<Mchan_cmd> *pending_write_cmds;
  Mchan_queue<vp::io_req> *pending_write_reqs;
  Mchan_cmd *current_ext_read_cmd;
  Mchan_cmd *current_ext_write_cmd;
  Mchan_cmd *current_loc_cmd;
  vp::io_req *first_ext_read_req = NULL;
  vp::io_req *first_ext_write_req = NULL;
  vp::io_req *loc_req;
  vp::io_req *pending_loc_read_req;

  Mchan_cmd *first_command = NULL;

  vp::io_master ext_itf;
  vp::io_master *loc_itf;

  int64_t *loc_port_ready_cycle;

  bool ext_is_stalled;

  vp::trace     cmd_events[MCHAN_NB_COUNTERS];
};

void Mchan_channel::reset()
{
  pending_req = NULL;
  pending_cmd = 0;
  current_cmd = NULL;
  pending_cmds->init();
}

/* Check if a raw command is ready and unpack it to make it easier to parse */
int Mchan_channel::unpack_command(Mchan_cmd *cmd)
{
  if (cmd->step == 1)
  {
    cmd->size = PLP_DMA_SIZE_GET(cmd->content[0]);
    cmd->size_to_read = cmd->size;
    cmd->size_to_write = cmd->size;
    cmd->received_size = 0;
    cmd->loc2ext = !PLP_DMA_TYPE_GET(cmd->content[0]);
    cmd->incr = PLP_DMA_INCR_GET(cmd->content[0]);
    cmd->is_2d = PLP_DMA_2D_GET(cmd->content[0]);
    cmd->raise_irq = PLP_DMA_ILE_GET(cmd->content[0]);
    cmd->raise_event = PLP_DMA_ELE_GET(cmd->content[0]);
    cmd->broadcast = PLP_DMA_BLE_GET(cmd->content[0]);
    cmd->counter_id = current_counter;
  }
  else if ((cmd->step == 3 && !top->is_64) || (cmd->step == 4 && top->is_64))
  {
    if (cmd->loc2ext) {
      if (top->is_64) {
        cmd->dest = (uint64_t)cmd->content[2] | ((uint64_t)cmd->content[3] << 32);
      } else {
        cmd->dest = cmd->content[2];
      }
      cmd->source = cmd->content[1];
    } else {
      cmd->dest = cmd->content[1];
      if (top->is_64) {
        cmd->source = (uint64_t)cmd->content[2] | ((uint64_t)cmd->content[3] << 32);
      } else {
        cmd->source = cmd->content[2];
      }
    }

    if (!cmd->is_2d) 
    {
     top->trace.msg("New 1D command ready (input: %d, source: 0x%lx, dest: 0x%lx, size: 0x%x, loc2ext: %d, counter: %d, raise_evt: %d, raise_irq: %d)\n", id, cmd->source, cmd->dest, cmd->size, cmd->loc2ext, cmd->counter_id, cmd->raise_event, cmd->raise_irq);
     goto unpackDone;
   }
 }
 else if ((cmd->step == 4 && !top->is_64) || (cmd->step == 5 && top->is_64))
 {
    int twd_index = top->is_64 ? 4 : 3;
    cmd->stride = PLP_DMA_2D_STRIDE_GET(cmd->content[twd_index]);
    cmd->length = PLP_DMA_2D_LEN_GET(cmd->content[twd_index]);
    cmd->line_size_to_read = cmd->length;
    cmd->source_chunk = cmd->source;
    cmd->dest_chunk = cmd->dest;

    top->trace.msg("New 2D command ready (input: %d, source: 0x%lx, dest: 0x%lx, size: 0x%x, loc2ext: %d, stride: 0x%x, len: 0x%x)\n", id, cmd->source, cmd->dest, cmd->size, cmd->loc2ext, cmd->stride, cmd->length);
    goto unpackDone;
 }
 return 0;

 unpackDone:
 pending_cmd++;
 cmd->step = 0;
 return 1;
}

template<class T>
T *Mchan_queue<T>::pop()
{
  if (!first) return NULL;
  T *cmd = first;
  first = cmd->get_next();
  nb_cmd--;

  return cmd;
}

template<class T>
T *Mchan_queue<T>::pop(bool loc2ext)
{
  T *current = first, *prev=NULL;
  while (current && current->loc2ext != loc2ext)
  {
    prev = current;
    current = current->get_next();
  }

  if (!current) return NULL;

  if (prev)
    prev->set_next(current->get_next());
  else
    first = current->get_next();

  if (last == current)
  {
    last = prev;
  }

  nb_cmd--;

  return current;
}

template<class T>
void Mchan_queue<T>::push(T *cmd)
{
  if (first)
    last->set_next(cmd);
  else
    first = cmd;
  cmd->set_next(NULL);
  last = cmd;
  nb_cmd++;
}

Mchan_cmd *Mchan_channel::pop_cmd(bool read_queue)
{
  Mchan_cmd *cmd = pending_cmds->pop(!read_queue);
  if (cmd == NULL) return NULL;
  pending_cmd--;

  if (cmd->loc2ext)
    top->nb_core_write_cmd--;
  else
    top->nb_core_read_cmd--;

  if (pending_req)
  {
    vp::io_req *req = pending_req;
    pending_req = NULL;
    handle_req(req, (uint32_t *)req->get_data());
    req->get_resp_port()->resp(req);
  }

  return cmd;
}

/* This checks if a command is ready after the queue has been written and if it is the case, enqueue it
 * to the proper queue */
bool Mchan_channel::check_command(Mchan_cmd *cmd)
{
  cmd->channel = this;

  if (!unpack_command(cmd)) return false;

  top->pending_bytes[current_counter] += cmd->size;

  // Enqueue the command to the core queue
  uint8_t one = 1;
  this->top->cmd_events[cmd->counter_id].event(&one);

  pending_cmds->push(cmd);

  if (cmd->loc2ext)
    top->nb_core_write_cmd++;
  else
    top->nb_core_read_cmd++;
  top->check_queue();

  return true;
}

void Mchan_channel::handle_req(vp::io_req *req, uint32_t *value)
{

  if (current_cmd == NULL) {
    // No on-going command, allocate a new one
    current_cmd = top->get_command();
    top->trace.msg("Starting new command\n");
  }

  current_cmd->content[current_cmd->step++] = *value;

  if (check_command(current_cmd)) {
    current_cmd = NULL;
  }

}


vp::io_req_status_e Mchan_channel::handle_queue_write(vp::io_req *req, uint32_t *value)
{
  top->trace.msg("Pushing word to queue (value: 0x%x, pending_cmd: %d)\n", *value, pending_cmd);

  // In case the core command queue is full, stall the calling core
  if (pending_cmd == top->core_queue_depth)
  {
    top->trace.msg("Core queue is full, stalling calling core\n");
    pending_req = req;
    return vp::IO_REQ_PENDING;
  }

  handle_req(req, value);

  return vp::IO_REQ_OK;
}

vp::io_req_status_e Mchan_channel::handle_queue_req(vp::io_req *req, bool is_write, uint32_t *value)
{
  if (is_write)
  {
    return handle_queue_write(req, value);
  }
  else
  {
    *value = top->alloc_counter(req, this);
    if (*value == -1)
      return vp::IO_REQ_PENDING;
    else
      return vp::IO_REQ_OK;
  }
}

vp::io_req_status_e Mchan_channel::handle_status_req(vp::io_req *req, bool is_write, uint32_t *value)
{
  if (is_write)
  {
    top->trace.msg("Freeing counters (mask: 0x%x, free_counters: 0%x)\n", *value, top->free_counter_mask);
    top->free_counters(*value);
  }
  else
  {
    *value = top->get_status();
    top->trace.msg("Getting status (status: 0x%x)\n", *value);
  }
  return vp::IO_REQ_OK;
}

Mchan_channel::Mchan_channel(int id, mchan *top)
: id(id), top(top)
{
  in.set_req_meth_muxed(&mchan::req, id);
  top->new_slave_port("in_" + std::to_string(id), &in);
  pending_cmds = new Mchan_queue<Mchan_cmd>(top->core_queue_depth);

  top->new_master_port("event_itf_" + std::to_string(id), &event_itf);
  top->new_master_port("irq_itf_" + std::to_string(id), &irq_itf);
  top->new_master_port("ext_irq_itf", &ext_irq_itf);
}

vp::io_req_status_e Mchan_channel::req(vp::io_req *req)
{
  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  top->trace.msg("mchan access (channel: %d, offset: 0x%x, size: 0x%x, is_write: %d)\n", id, offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  if      (offset == PLP_DMA_QUEUE_OFFSET)  return handle_queue_req (req, is_write, (uint32_t *)data);
  else if (offset == PLP_DMA_STATUS_OFFSET) return handle_status_req(req, is_write, (uint32_t *)data);

  return vp::IO_REQ_INVALID;
}

void Mchan_channel::trigger_event(Mchan_cmd *cmd)
{
  if (id == top->nb_channels - 1)
  {
    top->trace.msg("Raising external irq line (channel: %d)\n", id);
    ext_irq_itf.sync(true);
  }
  else
  {
    if (cmd->raise_irq) {
      top->trace.msg("Raising irq line (channel: %d)\n", id);
      irq_itf.sync(true);
    }

    if (cmd->raise_event) {
      top->trace.msg("Raising event line (channel: %d)\n", id);
      event_itf.sync(true);
    }
  }
}


void mchan::ext_grant(void *__this, vp::io_req *req)
{
  mchan *_this = (mchan *)__this;
  _this->trace.msg("Received grant (req: %p\n", req);
  _this->ext_is_stalled = false;
  _this->check_queue();
}

void mchan::ext_response(void *__this, vp::io_req *req)
{
  mchan *_this = (mchan *)__this;
  _this->trace.msg("Received response (req: %p\n", req);
  if (req->get_is_write())
  {
    Mchan_cmd *cmd = (Mchan_cmd *)*req->arg_get(0);
    _this->handle_ext_write_req_end(cmd, req);
  }
  else
  {
    _this->push_req_to_loc(req);
  }
  _this->check_queue();
}

void mchan::loc_grant(void *_this, vp::io_req *req)
{
}

void mchan::loc_response(void *_this, vp::io_req *req)
{
}


mchan::mchan(js::config *config)
: vp::component(config)
{
  nb_channels = get_config_int("nb_channels");
  core_queue_depth = get_config_int("core_queue_depth");
  global_queue_depth = get_config_int("global_queue_depth");
  is_64 = get_config_bool("is_64");
  max_nb_ext_read_req = get_config_int("max_nb_ext_read_req");
  max_nb_ext_write_req = get_config_int("max_nb_ext_write_req");
  max_burst_length = get_config_int("max_burst_length");
  nb_loc_ports = get_config_int("nb_loc_ports");
  tcdm_addr_width = get_config_int("tcdm_addr_width");

  check_queue_event = event_new(mchan::check_queue_handler);
  check_ext_read_event = event_new(mchan::check_ext_read_handler);
  check_ext_write_event = event_new(mchan::check_ext_write_handler);
  check_loc_transfer_event = event_new(mchan::check_loc_transfer_handler);

  pending_read_cmds = new Mchan_queue<Mchan_cmd>(global_queue_depth);
  pending_write_cmds = new Mchan_queue<Mchan_cmd>(global_queue_depth);
  pending_write_reqs = new Mchan_queue<vp::io_req>(global_queue_depth);

  loc_req = new vp::io_req[nb_loc_ports];
  loc_itf = new vp::io_master[nb_loc_ports];
  loc_port_ready_cycle = new int64_t[nb_loc_ports];

  for (int i=0; i<max_nb_ext_read_req; i++)
  {
    vp::io_req *req = new vp::io_req();
    // Allocate 3 arguments to store local port address, command and size done
    req->arg_alloc();
    req->arg_alloc();
    req->arg_alloc();
    req->set_data(new uint8_t[max_burst_length]);
    req->set_is_write(false);
    req->set_next(first_ext_read_req);
    first_ext_read_req = req;
  }

  for (int i=0; i<max_nb_ext_write_req; i++)
  {
    vp::io_req *req = new vp::io_req();
    // Allocate 3 arguments to store local port address, command and size done
    req->arg_alloc();
    req->arg_alloc();
    req->arg_alloc();
    req->set_data(new uint8_t[max_burst_length]);
    req->set_is_write(true);
    req->set_next(first_ext_write_req);
    first_ext_write_req = req;
  }

  ext_itf.set_resp_meth(&mchan::ext_response);
  ext_itf.set_grant_meth(&mchan::ext_grant);
  new_master_port("ext_itf", &ext_itf);

  for (int i=0; i<nb_loc_ports; i++)
  {
    loc_itf[i].set_resp_meth(&mchan::loc_response);
    loc_itf[i].set_grant_meth(&mchan::loc_grant);
    new_master_port("loc_itf_" + std::to_string(i), &loc_itf[i]);
  }

}

vp::io_req_status_e mchan::req(void *__this, vp::io_req *req, int id)
{
  mchan *_this = (mchan *)__this;
  return _this->channels[id]->req(req);
}

uint32_t mchan::get_status()
{
  uint32_t status = 0;
  for (unsigned int i=0; i<MCHAN_NB_COUNTERS; i++) {
    status |= (pending_bytes[i] != 0) << i;
  }
  status |= ((~free_counter_mask) & ((1 << MCHAN_NB_COUNTERS) - 1)) << 16;
  return status;
}

void mchan::free_counters(uint32_t counter_mask)
{
  if ((counter_mask & this->free_counter_mask) != 0)
    trace.force_warning("Freeing non-allocated counters (free_counters: 0x%x, freeing counters: 0x%x)\n", this->free_counter_mask, counter_mask);

  free_counter_mask |= counter_mask & ((1<<MCHAN_NB_COUNTERS)-1);

  // Now that we freed a counter, check if a core is waiting for it
  if (first_alloc_pending_req)
  {
    // In this case, remove the first from the queue
    vp::io_req *req = first_alloc_pending_req;
    first_alloc_pending_req = req->get_next();

    // Get the counter and unstall the core
    *(uint32_t *)req->get_data() = do_alloc_counter((Mchan_channel *)*req->arg_get_last());
    req->get_resp_port()->resp(req);
  }
}

int mchan::do_alloc_counter(Mchan_channel *channel)
{
  int i;
  for (i=0; i<MCHAN_NB_COUNTERS; i++) {
    if (free_counter_mask & (1 << i)) {
      free_counter_mask &= ~(1 << i);
      trace.msg("Allocated counter (counter: %d)\n", i);
      channel->current_counter = i;
      return i;
    }
  }
  return -1;
}

int mchan::alloc_counter(vp::io_req *req, Mchan_channel *channel)
{
  if (free_counter_mask) {
    return do_alloc_counter(channel);
  } else {
    trace.force_warning("No more counter, stalling core\n");

    // In case no counter is available, put the request in the queue, 
    // this will stall the calling core
    if (first_alloc_pending_req == NULL)
      first_alloc_pending_req = req;
    else
      last_alloc_pending_req->set_next(req);

    req->set_next(NULL);
    *req->arg_get_last() = channel;
    last_alloc_pending_req = req;

    return -1;
  }
}

Mchan_cmd *mchan::get_command()
{
  Mchan_cmd *cmd;

  if (first_command == NULL) {
    // No more commands in the pool, add one.
    cmd = new Mchan_cmd(this);
  }
  else
  {
    cmd = first_command;
    first_command = cmd->get_next();
  }
  cmd->init();
  return cmd;
}

void mchan::free_command(Mchan_cmd *cmd)
{
  cmd->set_next(first_command);
  first_command = cmd;
}

void mchan::move_to_global_queue(bool read_queue)
{
  Mchan_queue<Mchan_cmd> *queue = read_queue ? pending_read_cmds : pending_write_cmds;

  for (int i=0, j=sched_core_queue; i<nb_channels; i++)
  {
    Mchan_cmd *cmd = channels[j]->pop_cmd(read_queue);

    if (cmd)
    {
      trace.msg("Moving command from core queue to global queue (channel: %d)\n", j);

      queue->push(cmd);

      sched_core_queue++;
      if (sched_core_queue == nb_channels)
        sched_core_queue = 0;

      break;
    }

    j++;
    if (j == nb_channels)
      j = 0;
  }

}

void mchan::push_req_to_loc(vp::io_req *req)
{
  pending_write_reqs->push(req);
}

void mchan::handle_ext_write_req_end(Mchan_cmd *cmd, vp::io_req *req)
{
  int64_t size = req->get_size();

  cmd->size_to_write -= size;

  trace.msg("Updating command (size_to_write: %d)\n", cmd->size_to_write);

  req->set_next(first_ext_write_req);
  first_ext_write_req = req;
  nb_pending_ext_write_req--;

  account_transfered_bytes(cmd, size);
  if (cmd->size_to_write == 0)
  {
    handle_cmd_termination(cmd);
  }
}

void mchan::send_req_to_ext(Mchan_cmd *cmd, vp::io_req *req)
{
  vp::io_req_status_e err = ext_itf.req(req);
  if (err == vp::IO_REQ_OK)
  {
    handle_ext_write_req_end(cmd, req);
  }
}

void mchan::send_loc_read_req()
{
  Mchan_cmd *cmd = current_ext_write_cmd;

  int size = cmd->is_2d ? cmd->line_size_to_read : cmd->size_to_read;
  if (size > max_burst_length)
    size = max_burst_length;

  nb_pending_ext_write_req++;

  vp::io_req *req = first_ext_write_req;
  first_ext_write_req = req->get_next();

  trace.msg("Preparing write request to external interface (req: %p, addr: 0x%x, size: 0x%x)\n",
    req, cmd->dest, size);

  req->set_addr(cmd->dest);
  req->set_size(size);

  *req->arg_get(0) = (void *)cmd;
  *(uint32_t *)req->arg_get(1) = cmd->source & ((1<<tcdm_addr_width) - 1);
  *(uint32_t *)req->arg_get(2) = 0;

  cmd->dest += size;
  cmd->source += size;
  cmd->size_to_read -= size;

  if (cmd->is_2d)
  {
    cmd->line_size_to_read -= size;
    if (cmd->line_size_to_read == 0)
    {
      cmd->line_size_to_read = cmd->length;
      cmd->dest = cmd->dest_chunk + cmd->stride;
      cmd->dest_chunk = cmd->dest;
    }
  }

  pending_loc_read_req = req;

  if (cmd->size_to_read == 0)
  {
    current_ext_write_cmd = NULL;
  }
}

void mchan::send_req()
{
  Mchan_cmd *cmd = current_ext_read_cmd;

  int size = cmd->is_2d ? cmd->line_size_to_read : cmd->size_to_read;
  if (size > max_burst_length)
    size = max_burst_length;

  nb_pending_ext_read_req++;

  vp::io_req *req = first_ext_read_req;
  first_ext_read_req = req->get_next();

  if (cmd->is_2d)
    trace.msg("Sending read request to external interface (req: %p, addr: 0x%lx, size: 0x%x, remaining line size: 0x%x)\n",
    req, cmd->source, size, cmd->line_size_to_read);
  else
    trace.msg("Sending read request to external interface (req: %p, addr: 0x%lx, size: 0x%x, remaining size: 0x%x)\n",
    req, cmd->source, size, cmd->size_to_read);

  req->set_addr(cmd->source);
  req->set_size(size);

  *req->arg_get(0) = (void *)cmd;
  *(uint32_t *)req->arg_get(1) = cmd->dest & ((1<<tcdm_addr_width) - 1);
  *(uint32_t *)req->arg_get(2) = 0;

  cmd->dest += size;
  cmd->source += size;
  cmd->size_to_read -= size;
  if (cmd->is_2d)
  {
    cmd->line_size_to_read -= size;
    if (cmd->line_size_to_read == 0)
    {
      cmd->line_size_to_read = cmd->length;
      cmd->source = cmd->source_chunk + cmd->stride;
      cmd->source_chunk = cmd->source;
    }
  }

  if (cmd->size_to_read == 0)
  {
    current_ext_read_cmd = NULL;
  }

  vp::io_req_status_e err = ext_itf.req(req);
  if (err == vp::IO_REQ_OK)
  {
    cmd->received_size += size;
    push_req_to_loc(req);
  }
  else if (err == vp::IO_REQ_DENIED)
  {
    ext_is_stalled = true;
  }
}

void mchan::check_ext_read_handler(void *__this, vp::clock_event *event)
{
  mchan *_this = (mchan *)__this;

  if (_this->current_ext_read_cmd == NULL)
    _this->current_ext_read_cmd = _this->pending_read_cmds->pop();


  if (_this->current_ext_read_cmd != NULL)
  {
    if (_this->nb_pending_ext_read_req < _this->max_nb_ext_read_req)
    {
      _this->send_req();
    }
  }


  _this->check_queue();
}

void mchan::check_ext_write_handler(void *__this, vp::clock_event *event)
{
  mchan *_this = (mchan *)__this;

  if (_this->current_ext_write_cmd == NULL)
    _this->current_ext_write_cmd = _this->pending_write_cmds->pop();

  if (_this->current_ext_write_cmd != NULL)
  {
    if (_this->nb_pending_ext_write_req < _this->max_nb_ext_write_req && 
      _this->pending_loc_read_req == NULL)
    {
      _this->send_loc_read_req();
    }
  }


  _this->check_queue();
}

void mchan::handle_cmd_termination(Mchan_cmd *cmd)
{
  this->cmd_events[cmd->counter_id].event(NULL);
  free_command(cmd);
}

void mchan::account_transfered_bytes(Mchan_cmd *cmd, int bytes)
{
  pending_bytes[cmd->counter_id] -= bytes;

  if (pending_bytes[cmd->counter_id] == 0)
  {
    trace.msg("Counter reached zero, raising event\n");
    
    if (cmd->broadcast)
    {
      for (unsigned int i=0; i<nb_channels; i++)
        channels[i]->trigger_event(cmd);
    }
    else
    {
      cmd->channel->trigger_event(cmd);
    }
  }
}

void mchan::check_loc_transfer_handler(void *__this, vp::clock_event *event)
{
  mchan *_this = (mchan *)__this;

  // Go through the local ports to see if we can send a request from
  // pending commands
  int64_t min_ready_cycle = -1;
  int64_t cycles = _this->get_cycles();

  for (int i=0; i<_this->nb_loc_ports; i++)
  {
    // Bypass this port if it is still busy with a previous request
    if (_this->loc_port_ready_cycle[i] > cycles)
    {
      _this->trace.msg("Bypassing port (port: %d, ready_cycle: %ld)\n",
        i, cycles + _this->loc_port_ready_cycle[i]);
      continue;
    }

    // Get the current write request coming from external port
    // and stop is nothing is there
    vp::io_req *ext_req;
    bool is_write;

    if (i < 2)
    {
      ext_req = _this->pending_write_reqs->get_first();
      if (ext_req == NULL)
        continue;
      is_write = true;
    }
    else
    {
      ext_req = _this->pending_loc_read_req;
      if (ext_req == NULL)
        continue;
      is_write = false;
    }

    // Extract access information from external request

    uint32_t done_size = *(uint32_t *)ext_req->arg_get(2);
    int32_t ext_size = ext_req->get_size() - done_size;
    uint32_t addr = *(uint32_t *)ext_req->arg_get(1) + done_size;
    uint8_t *data = ext_req->get_data() + done_size;
    int32_t size = 4;
    if (addr & 0x3) size -= addr & 0x3;
    if (size > ext_size) size = ext_size;

    // Create request to local port
    vp::io_req *req = &_this->loc_req[i];
    _this->trace.msg("Sending %s request to local port (req: %p, port: %d, addr: 0x%x, size: 0x%x)\n",
      is_write ? "write" : "read", req, i, addr, size);
    req->init();
    req->set_addr(addr);
    req->set_size(size);
    req->set_is_write(is_write);
    req->set_data(data);

    // Send the request to the local port
    // TODO for now we assume this is synchronous
    vp::io_req_status_e err = _this->loc_itf[i].req(req);

    if (err) 
    {

    }
    else
    {
      _this->loc_port_ready_cycle[i] = cycles + req->get_latency() + 1;
    }

    if (is_write)
    {
      // Update the command and check if it can be released
      Mchan_cmd *cmd = (Mchan_cmd *)*ext_req->arg_get(0);
      cmd->size_to_write -= size;
      _this->trace.msg("Updating command (size_to_write: %d)\n", cmd->size_to_write);
      _this->account_transfered_bytes(cmd, size);
      if (cmd->size_to_write == 0)
      {
        _this->handle_cmd_termination(cmd);
      }

      // Removed the request if it is finished
      if (ext_size - size == 0)
      {
        _this->pending_write_reqs->pop();
        ext_req->set_next(_this->first_ext_read_req);
        _this->first_ext_read_req = ext_req;
        _this->nb_pending_ext_read_req--;
      }
      else
      {
        *(uint32_t *)ext_req->arg_get(2) = done_size + size;
      }
    }
    else
    {
      _this->trace.msg("Updating request (size_to_read: %d)\n", ext_size - size);

      // Removed the request if it is finished
      if (ext_size - size == 0)
      {
        _this->trace.msg("Finished request\n");
        Mchan_cmd *cmd = (Mchan_cmd *)*ext_req->arg_get(0);
        _this->pending_loc_read_req = NULL;
        _this->send_req_to_ext(cmd, ext_req);
      }
      else
      {
        *(uint32_t *)ext_req->arg_get(2) = done_size + size;
      }

    }

  }

  _this->check_queue();
}


void mchan::check_queue_handler(void *__this, vp::clock_event *event)
{
  mchan *_this = (mchan *)__this;

  if (_this->nb_core_read_cmd && !_this->pending_read_cmds->is_full())
  {
    _this->move_to_global_queue(true);
  }

  else if (_this->nb_core_write_cmd && !_this->pending_write_cmds->is_full())
  {
    _this->move_to_global_queue(false);
  }

  _this->check_queue();
}

void mchan::check_queue()
{
  if (nb_core_read_cmd && !pending_read_cmds->is_full() ||
    nb_core_write_cmd && !pending_write_cmds->is_full())
  {
    if (!check_queue_event->is_enqueued())
      event_enqueue(check_queue_event, 1);
  }

  if (!pending_read_cmds->is_empty() && current_ext_read_cmd == NULL ||
    current_ext_read_cmd != NULL && nb_pending_ext_read_req < max_nb_ext_read_req)
  {
    if (!ext_is_stalled)
    {
      if (!check_ext_read_event->is_enqueued())
        event_enqueue(check_ext_read_event, 1);
    }
  }

  if (!pending_write_cmds->is_empty() && current_ext_write_cmd == NULL ||
    current_ext_write_cmd != NULL && nb_pending_ext_write_req < max_nb_ext_write_req &&
    pending_loc_read_req == NULL)
  {
    if (!ext_is_stalled)
    {
      if (!check_ext_write_event->is_enqueued())
        event_enqueue(check_ext_write_event, 1);
    }
  }

  if (!pending_write_reqs->is_empty() || pending_loc_read_req != NULL)
  {
    if (!check_loc_transfer_event->is_enqueued())
    {
      // Go through the port availabilities to see when we can enqueue the event
      int64_t min_ready_cycle = -1;
      int64_t cycles = get_cycles();

      for (int i=0; i<nb_loc_ports; i++)
      {
        if ((min_ready_cycle == -1 || loc_port_ready_cycle[i] < min_ready_cycle))
        {
          min_ready_cycle = loc_port_ready_cycle[i];
        }
      }
      if (min_ready_cycle <= cycles){
        event_enqueue(check_loc_transfer_event, 1);
      }
      else
      {
        event_enqueue(check_loc_transfer_event, min_ready_cycle - cycles);
      }
    }
  }
}

int mchan::build()
{
  traces.new_trace("trace", &this->trace, vp::DEBUG);

  for (int i=0; i<nb_channels; i++)
  {
    channels.push_back(new Mchan_channel(i, this));
  }

  for (int i=0; i<MCHAN_NB_COUNTERS; i++)
  {
    traces.new_trace_event("channel_" + std::to_string(i), &this->cmd_events[i], 8);
  }

  return 0;
}

void mchan::start()
{
}

void mchan::reset(bool active)
{
  if (active)
  {
    free_counter_mask = (1 << MCHAN_NB_COUNTERS) - 1;

    for (int i=0; i<MCHAN_NB_COUNTERS; i++)
    {
      pending_bytes[i] = 0;
    }

    for (int i=0; i<nb_channels; i++)
    {
      channels[i]->reset();
    }

    for (int i=0; i<nb_loc_ports; i++)
    {
      loc_port_ready_cycle[i] = 0;
    }

    first_alloc_pending_req = NULL;
    last_alloc_pending_req = NULL;
    nb_core_read_cmd = 0;
    nb_core_write_cmd = 0;
    sched_core_queue = 0;
    nb_pending_ext_read_req = 0;
    nb_pending_ext_write_req = 0;
    pending_read_cmds->init();
    pending_write_cmds->init();
    pending_write_reqs->init();
    current_ext_read_cmd = NULL;
    current_ext_write_cmd = NULL;
    current_loc_cmd = NULL;
    pending_loc_read_req = NULL;
    ext_is_stalled = false;
    for (int i=0; i<MCHAN_NB_COUNTERS; i++)
    {
      this->cmd_events[i].event(NULL);
    }
  }
  else
  {
  }
}

void Mchan_cmd::init()
{
  step = 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new mchan(config);
}
