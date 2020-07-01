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



// This model replicate a master which is sending IO requests through its
// output port.
// It can handle several requests at the same time and will stop sending
// request when its maximum is reached until a request is finished.
// This master immediatly stop when a request is denied and continues only
// when this request is granted.
// It supports all modes, synchronous and asynchronous, with or without latency.

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <time.h>



class master : public vp::component
{

public:

  master(js::config *config);

  int build();

  void start();

  // This function can be called to check if the master can send another request
  // in which case he will post an event to send it.
  void check_send_req();

  // Called when a response is fully handled.
  // The slave can reply with a latency and thus this method is executed as an
  // event callback with this latency to process the real timestamp of the reply.
  static void handle_resp(void *__this, vp::clock_event *event);

  // Called as an event callback to send a request.
  static void send_req(void *_this, vp::clock_event *event);

  // Called by the slave to grant a previously denied access.
  static void grant(void *_this, vp::io_req *req);

  // Called by the slave to notify a request response.
  static void resp(void *_this, vp::io_req *req);

private:

  // Coponents properties.
  // They can be set from the JSON file.
  int nb_requests      = 16;  // Total number of requests that the master
                              // will send
  int max_pending_reqs = 4;   // Maximum number of pending requests that the
                              // master can send
  int size             = 4;   // Size in bytes of the requests
  int delay            = 4;   // Delay between 2 requests


  vp::io_master out;  // Output port
  vp::trace     trace;
  vp::clock_event *send_req_event;  // Event used to send requests

  int nb_pending_reqs  = 0;      // Current number of pending requests
  bool stalled         = false;  // If stalled, the master cannot send requests
                                 // until he is granted the stalled request

};



void master::check_send_req()
{
  // Don't continue if we've already sent all requests
  if (nb_requests == 0) return;

  // Schedule the event for sending a requests if it is not already enqueued
  // and we haven't already reached the maximum number of pending requests.
  if (!send_req_event->is_enqueued() && nb_pending_reqs < max_pending_reqs)
  {
    // Events sending requests must always been enqueued the cycle after
    // so that the requests are sent before any contention resolution is started.
    event_enqueue(send_req_event, delay + 1);
  }  
}

void master::send_req(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  _this->nb_requests--;

  _this->trace.msg("Master sending req (pendings: %d, max pending: %d)\n",
    _this->nb_pending_reqs, _this->max_pending_reqs);

  vp::io_req *req = _this->out.req_new(0, NULL, _this->size, 1);

  // Send the request and get the error code which will tell us if the request
  // has been handled synchronously or asynchronously.
  int err = _this->out.req(req);

  _this->trace.msg("Master got synchronous response (retval: %d, latency: %ld)\n", err, req->get_latency());
  
  if (err)
  {

    // This should never happen in this test, just in case
    if (err == vp::IO_REQ_INVALID)
    {
      _this->out.req_del(req);
      return;
    }

    // If there is an error, the request has either not been granted or it is
    // pending. In both cases, this counts as a pending request as the request
    // will become pending when we are notified about the grant.
    _this->nb_pending_reqs += 1;

    if (err == vp::IO_REQ_DENIED)
    {
      // In case it is not granted, stall completely this master.
      // We have to remember it to make sure we don't try to send another
      // request when a response is received before we get the grant.
      _this->stalled = true;
      return;
    }
    else if (err == vp::IO_REQ_PENDING)
    {
      // In case it is pending, continue to send requests unless we reached the
      // maximum
    }
  }

  if (!err)
  {
    // If the request was synchronous, check if there was a latency and in
    // this case enqueue an event so that we eumlate the latency.
    int64_t latency = req->get_latency();
    if (latency)
    {
      _this->nb_pending_reqs += 1;
      _this->event_enqueue(_this->event_new(master::handle_resp), latency);
    }

    // If the call was synchronous, free now the request as we won't have 
    // any callback with it.
    if (!err) _this->out.req_del(req);
  }

  // Finally check if we can continue sending requests.
  _this->check_send_req();
}



void master::grant(void *__this, vp::io_req *req)
{
  master *_this = (master *)__this;

  // The slave called us to grant a stalled request, just unstall the master
  // and continue sending

  _this->trace.msg("Master got grant (pendings: %d, max pending: %d)\n",
    _this->nb_pending_reqs,
    _this->max_pending_reqs);

  // Make sure the master can now continue sending requests
  _this->stalled = false;

  _this->check_send_req();
}



void master::handle_resp(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  _this->event_del(event);

  _this->trace.msg("Master handling response (pendings: %d, max pending: %d)\n",
    _this->nb_pending_reqs,
    _this->max_pending_reqs);

  _this->nb_pending_reqs--;

  // We got a reply, continue to send requests unless we are stalled waiting
  // for a grant.
  // Be careful to enqueue the event only if has not already been done.
  if (!_this->stalled)
  {
    _this->check_send_req();
  }
}

void master::resp(void *__this, vp::io_req *req)
{
  master *_this = (master *)__this;

  int64_t latency = req->get_latency();

  _this->trace.msg("Master got response (pendings: %d, max pending: %d, latency: %ld)\n",
    _this->nb_pending_reqs,
    _this->max_pending_reqs, req->get_latency());

  // Either the response is received with a latency, in which case we post
  // an event to handle it later on, or we directly handle the repsone now.
  if (latency)
  {
    _this->event_enqueue(_this->event_new(master::handle_resp), latency);
  }
  else
  {
    handle_resp(__this, NULL);
  }

  _this->out.req_del(req);
}



int master::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  out.set_resp_meth(&master::resp);
  out.set_grant_meth(&master::grant);

  new_master_port("out", &out);

  send_req_event = event_new(master::send_req);

  nb_pending_reqs = 0;
  stalled = false;

  size = get_config_int("size");
  delay = get_config_int("delay");
  max_pending_reqs = get_config_int("max_pending_reqs");
  nb_requests = get_config_int("nb_requests");

  return 0;
}



void master::start()
{
  event_enqueue(send_req_event, 0);
}



master::master(js::config *config)
: vp::component(config)
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new master(config);
}
