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

#ifndef __VP_ITF_IO_HPP__
#define __VP_ITF_IO_HPP__

#include "vp/vp.hpp"

namespace vp {

  class io_slave;
  class io_req;

  typedef enum
  {
    IO_REQ_OK,
    IO_REQ_INVALID,
    IO_REQ_DENIED,
    IO_REQ_PENDING
  } io_req_status_e;

  typedef enum
  {
    IO_REQ_FLAGS_DEBUG = (1<<0)
  } io_req_flags_e;

  #define IO_REQ_PAYLOAD_SIZE 64
  #define IO_REQ_NB_ARGS 16

  typedef io_req_status_e (io_req_meth_t)(void *, io_req *);
  typedef io_req_status_e (io_req_meth_muxed_t)(void *, io_req *, int id);

  typedef void (io_resp_meth_t)(void *, io_req *);
  typedef void (io_grant_meth_t)(void *, io_req *);

  class io_req
  {
    friend class io_master;
    friend class io_slave;

  public:
    io_req() {}

    io_req(uint64_t addr, uint8_t *data, uint64_t size, bool is_write)
    : addr(addr), data(data), size(size), is_write(is_write)
    {
      init();
    }

    io_slave *get_resp_port() { return resp_port;}
    void set_next(io_req *req) { next = req; }
    io_req *get_next() { return next; }

    uint64_t get_addr() { return addr; }
    void set_addr(uint64_t value) { addr = value; }

    uint64_t get_is_write() { return is_write; }
    void set_is_write(bool is_write) { this->is_write = is_write; }

    void set_size(uint64_t size) { this->size = size; }
    uint64_t get_size() { return size; }

    void set_actual_size(uint64_t actual_size) { this->actual_size = actual_size; }
    uint64_t get_actual_size() { return actual_size; }

    inline void set_latency(uint64_t latency) { this->latency = latency; }
    inline uint64_t get_latency() { return this->latency; }
    inline void inc_latency(uint64_t incr) { this->latency += incr; }

    inline void set_duration(uint64_t duration) { if (duration > this->duration) this->duration = duration; }
    inline uint64_t get_duration() { return this->duration; }

    inline uint64_t get_full_latency() { return latency + duration; }

    uint8_t *get_data() { return data; }
    void set_data(uint8_t *data) { this->data = data; }

    inline int get_payload_size() { return IO_REQ_PAYLOAD_SIZE; }
    inline uint8_t *get_payload() { return payload; }

    inline int get_nb_args() { return IO_REQ_NB_ARGS; }
    inline void **get_args() { return args; }

    inline void set_int(int index, int value) { *(int *)&get_args()[index] = value; }
    inline int get_int(int index) { return *(int *)&get_args()[index]; }

    inline bool is_debug() { return this->flags & IO_REQ_FLAGS_DEBUG; }
    inline void set_debug(bool debug)
    {
      if (debug)
        this->flags |= IO_REQ_FLAGS_DEBUG;
      else
        this->flags &= ~IO_REQ_FLAGS_DEBUG;
    }

    inline int arg_alloc() { return current_arg++; }
    inline void arg_free() { current_arg--; }

    inline void arg_push(void *arg) { this->args[this->current_arg++] = arg; }
    inline void *arg_pop() { return this->args[--this->current_arg];}

    inline void **arg_get() { return &args[current_arg-1]; }
    inline void **arg_get(int index) { return &args[index]; }
    inline void **arg_get_last() { return &args[current_arg]; }

    inline void prepare() { latency = 0; duration=0; flags=0; }
    inline void init() { prepare(); current_arg=0; }

    uint64_t flags;
    uint64_t addr;
    uint8_t *data;
    uint64_t size;
    uint64_t actual_size;
    bool is_write;
    io_req_status_e status;
    io_slave *resp_port;


  private:
    io_req *next;
    int64_t latency;
    int64_t duration;
    uint8_t payload[IO_REQ_PAYLOAD_SIZE];
    void *args[IO_REQ_NB_ARGS];
    int current_arg = 0;
  };


  /*
   * Class for IO master ports
   */
  class io_master : public vp::master_port
  {
    friend class io_slave;

  public:

    /*
     * Master binding methods
     */

    // Can be called to allocate an IO request.
    inline io_req *req_new(uint64_t addr, uint8_t *data, uint64_t size, bool is_write);

    // Can be called to deallocate an IO request.
    inline void req_del(io_req *req);

    // Return if this master port is bound.
    bool is_bound();

    // Can be called by master component to send an IO request.  
    inline io_req_status_e req(io_req *req);

    // Can be called by master component to forward an IO request.
    // Compared to the req method, this one will not redefined the response
    // port and thus responses sent back by the slave will be send to our
    // caller, not to us.
    inline io_req_status_e req_forward(io_req *req);

    // Can be called by master component to forward an IO request.
    // Compared to other req methods, with this one, the caller can define
    // on which port the response will be sent back by the slave.
    inline io_req_status_e req(io_req *req, io_slave *slave_port);



    /*
     * Master binding methods declaration
     */

    // Set the callback on master side called when the slave is sending back
    // an IO request grant. Before being set, a default empty callback is active.
    inline void set_grant_meth(io_grant_meth_t *meth);

    // Set the callback on master side called when the slave is sending back
    // an IO request response. Before being set, a default empty callback is active.
    inline void set_resp_meth(io_resp_meth_t *meth);



    /*
     * Reserved for framework
     */

    // Constructor
    inline io_master();

    // Called by the framework to bind the master port to a slave port.
    virtual inline void bind_to(vp::port *port, vp::config *config);

    // Called by the framework to finalize the binding, for example in order
    // to take into account cross-domains bindings
    void finalize();



  private:

    /*
     * Master callbacks
     */

    // Grant callback set by the user.
    // This gets called anytime the slave is granting an IO request.
    // This is set to an empty callback by default.
    void (*grant_meth)(void *context, io_req *req);

    // Default grant callback, just do nothing.
    static inline void grant_default(void *, io_req *);

    // Response callback set by the user.
    // This gets called anytime the slave is sending back a response.
    // This is set to an empty callback by default.
    void (*resp_meth)(void *context, io_req *req);

    // Default response callback, just do nothing.
    static inline void resp_default(void *, io_req *);


    /*
     * Slave callbacks
     */

    // Callback set by the user on slave port and retrieved during binding
    io_req_status_e (*req_meth)(void *, io_req *);

    // req_meth saved when the slave port is multiplexed as a stub is setup instead
    io_req_status_e (*req_meth_mux)(void *, io_req *, int mux);

    // req_meth when the binding is crossing frequency domains as a stub is 
    // setup instead
    io_req_status_e (*req_meth_freq_cross)(void *, io_req *);


    /*
     * Stubs
     */

    // This is a stub setup when the slave is multiplexing the port so that we
    // can capture the master call and transform it into the slave call with the
    // right mux ID.
    static inline io_req_status_e req_muxed_stub(io_master *_this, io_req *req);

    // This is a stub setup when the binding is crossing 2 different clock
    // domains so that we can capture the master call and resynchronize the slave
    // domain before we call it.
    static inline io_req_status_e req_freq_cross_stub(io_master *_this, io_req *req);


    /*
     * Internal data
     */

    // Slave context when slave port is multiplexed.
    // We keep here a copy of the slave context when the slave port is multiplexed
    // as the normal variable for this context is used to store ourself so that
    // the stub is working well.
    void *slave_context_for_mux = NULL;


    // Slave context when the binding is crossing frequency domains.
    // We keep here a copy of the slave context when the binding is crossing frequency
    // domains as the normal variable for this context is used to store ourself
    // so that the stub is working well.
    void *slave_context_for_freq_cross = NULL;

    // This data is the multiplex ID that we need to send to the slave when the slave port
    // is multiplexed.
    int slave_req_mux_id = -1;


    // Several IO master ports are often connected to the same slave port
    // while the slave will need to reply to the master.
    // For that, a slave port is associated to each master port and can
    // be used by the real slave port to reply to a specific master port.
    io_slave *slave_port = NULL;
  };



  /*
   * Class for IO slave ports
   */
  class io_slave : public vp::slave_port
  {

    friend class io_master;

  public:

    /*
     * Slave binding methods
     */

    // Can be called to grant an IO request.
    // Granting a request means that the request is accepted and owned by the slave
    // and that the master can consider the request gone and then proceeed with 
    // the rest.
    inline void grant(io_req *req) { 

      this->master_grant_meth(this->get_remote_context(), req); }

    // Can be called to reply to an IO request.
    // Replying means that the slave has finished handing the request and it is now
    // owned back by the master which can then proceed with the request.
    inline void resp(io_req *req) { this->master_resp_meth(this->get_remote_context(), req); }



    /*
     * Master binding methods declaration
     */

    // Set the callback on slave side called when the master is sending an IO
    // request.
    inline void set_req_meth(io_req_meth_t *meth);

    // Set the callback on slave side called when the master is sending an IO
    // request and associate a data to it. The data is providedas the last argument
    // when calling the callback, and can be used to multiplex a slave port
    inline void set_req_meth_muxed(io_req_meth_muxed_t *meth, int id);



    /*
     * Reserved for framework
     */

    // Constructor
    inline io_slave();

    // Called by the framework to bind the slave port to a master port.
    inline void bind_to(vp::port *_port, vp::config *config);

    // Called by the framework to finalize the binding, for example in order
    // to take into account cross-domains bindings
    void finalize();

  private:

    /*
     * Slave callbacks
     */

    // Request callback set by the user.
    // This gets called anytime the master is sending a request.
    // This is set to an empty callback by default.    
    io_req_status_e (*req_meth)(void *context, io_req *);

    // Default request callback, just do nothing.
    static inline io_req_status_e req_default(io_slave *, io_req *);

    // Multiplexed request callback set by the user.
    // Similar to the req callback but with an associated data.
    // This one gets called instead of the normal once in case it is not NULL
    io_req_status_e (*req_meth_mux)(void *context, io_req *, int mux);



    /*
     * Master callbacks
     */

    // Response callback set by the user on master port and retrived during binding
    void (*master_resp_meth)(void *, io_req *);

    // Grant callback set by the user on master port and retrived during binding
    void (*master_grant_meth)(void *, io_req *);

    // master_resp_meth when the binding is crossing frequency domains as a stub is 
    // setup instead
    void (*master_resp_meth_freq_cross)(void *, io_req *);

    // master_grant_meth when the binding is crossing frequency domains as a stub is 
    // setup instead
    void (*master_grant_meth_freq_cross)(void *, io_req *);


    /*
     * Stubs
     */

    // This is a stub setup when the binding is crossing 2 different clock
    // domains so that we can capture the slave call and resynchronize the master
    // domain before we call it.
    static inline void grant_freq_cross_stub(io_slave *_this, io_req *req);

    // This is a stub setup when the binding is crossing 2 different clock
    // domains so that we can capture the slave call and resynchronize the master
    // domain before we call it.
    static inline void resp_freq_cross_stub(io_slave *_this, io_req *req);

    // Setup stubs for cross frequency domain crossing
    inline void set_freq_stub();


    /*
     * Internal data
     */

    // Multiplexed ID set by the slave when port is multiplxed
    int req_mux_id;


    // Master context when the binding is crossing frequency domains.
    // We keep here a copy of the master context when the binding is crossing frequency
    // domains as the normal variable for this context is used to store ourself
    // so that the stub is working well.
    void *master_context_for_freq_cross;

  };



  inline io_master::io_master() {
    // Set default callbacks in case the user does not set them
    this->resp_meth = &io_master::resp_default;
    this->grant_meth = &io_master::grant_default;
  }



  inline io_req_status_e io_master::req(io_req *req)
  {
    // We need to store our response port in the request
    // as the slave port is serving several master ports and need
    // to reply to us.
    req->resp_port = slave_port;
    return this->req_meth(this->get_remote_context(), req);
  }



  inline io_req_status_e io_master::req_forward(io_req *req)
  {
    // We don't redefine the slave port, as the request must be forwarded,
    // this way the slave will reply directly to the previous initiator
    return this->req_meth(this->get_remote_context(), req);
  }



  inline io_req_status_e io_master::req(io_req *req, io_slave *port)
  {
    // Case where the response port is given by the called
    req->resp_port = port;
    return port->req_meth((void *)port->get_remote_context(), req);
  }




  inline io_req *io_master::req_new(uint64_t addr, uint8_t *data, uint64_t size, bool is_write)
  {
    // For now we allocate new requests but this would be better to manage a pool of requests
    io_req *req = new io_req(addr, data, size, is_write);

    return req;
  }



  inline void io_master::req_del(io_req *req)
  {
    delete req;
  }



  inline void io_master::set_resp_meth(io_resp_meth_t *meth)
  {
    resp_meth = meth;
  }



  inline void io_master::set_grant_meth(io_grant_meth_t *meth)
  {
    grant_meth = meth;
  }



  inline void io_master::resp_default(void *, io_req *)
  {
  }



  inline void io_master::grant_default(void *, io_req *)
  {
  }



  inline void io_master::bind_to(vp::port *_port, vp::config *config)
  {
    io_slave *port = (io_slave *)_port;
    this->remote_port = port;

    vp_assert(port != NULL, this->get_owner()->get_trace(),
      "Binding to NULL slave port\n");

    if (port->req_meth_mux == NULL)
    {
      // Normal binding, just register the method and context into the master
      // port for fast access
      this->req_meth = port->req_meth;
      this->set_remote_context(port->get_context());
    }
    else
    {
      // Multiplexed binding, tweak the normal callback so that we enter
      // the stub to insert the multiplex ID.
      this->req_meth_mux = port->req_meth_mux;
      this->req_meth = (io_req_meth_t *)&io_master::req_muxed_stub;
      this->set_remote_context(this);
      this->slave_context_for_mux = port->get_context();
      this->slave_req_mux_id = port->req_mux_id;
    }
  }


  inline bool io_master::is_bound()
  {
    return this->slave_port != NULL;
  }



  inline io_req_status_e io_master::req_muxed_stub(io_master *_this, io_req *req)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. Now generate the normal call with the mux ID using the saved handler
    return _this->req_meth_mux((component *)_this->slave_context_for_mux, req, _this->slave_req_mux_id);
  }



  inline io_req_status_e io_master::req_freq_cross_stub(io_master *_this, io_req *req)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. 
    // First synchronize the target engine in case it was left behind,
    // and then generate the normal call with the mux ID using the saved handler
    _this->remote_port->get_owner()->get_clock()->sync();
    return _this->req_meth_freq_cross((component *)_this->slave_context_for_freq_cross, req);
  }



  inline void io_master::finalize()
  {
    vp_assert(this->get_owner() != NULL, NULL,
      "No master port owner found when finalizing master binding\n");
    vp_assert(this->get_owner()->get_clock() != NULL, NULL,
      "No master port owner clock found when finalizing master binding\n");
    vp_assert(this->remote_port != NULL, this->get_owner()->get_trace(),
      "No remote port found when finalizing master binding\n");
    vp_assert(this->remote_port != NULL, this->get_owner()->get_trace(),
      "No remote port found when finalizing master binding\n");
    vp_assert(this->remote_port->get_owner() != NULL, this->get_comp()->get_trace(),
      "No remote port owner found when finalizing master binding\n");
    vp_assert(this->remote_port->get_owner()->get_clock() != NULL, this->get_comp()->get_trace(),
      "No remote port owner clock found when finalizing master binding\n");

    // We have to instantiate a stub in case the binding is crossing different
    // frequency domains in order to resynchronize the target engine.
    if (this->get_owner()->get_clock() != this->remote_port->get_owner()->get_clock())
    {
      // Just save the normal handler and tweak it to enter the stub when the
      // master is pushing the request.
      this->req_meth_freq_cross = this->req_meth;
      this->req_meth = (io_req_meth_t *)&io_master::req_freq_cross_stub;
      this->slave_context_for_freq_cross = this->get_remote_context();
      this->set_remote_context(this);
    }
  }



  inline io_slave::io_slave() : req_meth(NULL), req_meth_mux(NULL) {
    req_meth = (io_req_meth_t *)&io_slave::req_default;
  }


  inline void io_slave::bind_to(vp::port *_port, vp::config *config)
  {
    // Instantiate a new slave port which is just used as a reference to reply
    // to the correct master port
    slave_port::bind_to(_port, config);
    io_master *port = (io_master *)_port;
    port->slave_port = new io_slave();
    port->slave_port->remote_port = port;
    port->slave_port->set_owner(this->get_owner());
    port->slave_port->master_resp_meth = port->resp_meth;
    port->slave_port->master_grant_meth = port->grant_meth;
    port->slave_port->set_remote_context(port->get_context());
  }


  inline void io_slave::set_req_meth(io_req_meth_t *meth)
  {
    this->req_meth = meth;
    this->req_meth_mux = NULL;
  }



  inline void io_slave::set_req_meth_muxed(io_req_meth_muxed_t *meth, int id)
  {
    this->req_meth_mux = meth;
    this->req_meth = NULL;
    this->req_mux_id = id;
  }



  inline io_req_status_e io_slave::req_default(io_slave *, io_req *)
  {
    return IO_REQ_OK;
  }



  inline void io_slave::grant_freq_cross_stub(io_slave *_this, io_req *req)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. 
    // First synchronize the target engine in case it was left behind,
    // and then generate the normal call with the mux ID using the saved handler
    _this->remote_port->get_owner()->get_clock()->sync();
    _this->master_grant_meth_freq_cross((component *)_this->master_context_for_freq_cross, req);
  }



  inline void io_slave::resp_freq_cross_stub(io_slave *_this, io_req *req)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. 
    // First synchronize the target engine in case it was left behind,
    // and then generate the normal call with the mux ID using the saved handler
    _this->remote_port->get_owner()->get_clock()->sync();
    _this->master_resp_meth_freq_cross((component *)_this->master_context_for_freq_cross, req);
  }



  inline void io_slave::set_freq_stub()
  {
      // Just save the normal handler and tweak it to enter the stub when the
      // master is pushing the request.
      this->master_grant_meth_freq_cross = this->master_grant_meth;
      this->master_grant_meth = (void (*)(void *, io_req *))&io_slave::grant_freq_cross_stub;

      this->master_resp_meth_freq_cross = this->master_resp_meth;
      this->master_resp_meth = (void (*)(void *, io_req *))&io_slave::resp_freq_cross_stub;
      
      this->master_context_for_freq_cross = this->get_remote_context();
      this->set_remote_context(this);
  }


  inline void io_slave::finalize()
  {
    // We have to instantiate a stub in case the binding is crossing different
    // frequency domains in order to resynchronize the target engine.
    if (this->remote_port && this->get_owner()->get_clock() != this->remote_port->get_owner()->get_clock())
    {
      this->set_freq_stub();

      if (((io_master *)this->remote_port)->slave_port != NULL)
      {
        ((io_master *)this->remote_port)->slave_port->set_freq_stub();
      }
    }
  }

};

#endif
