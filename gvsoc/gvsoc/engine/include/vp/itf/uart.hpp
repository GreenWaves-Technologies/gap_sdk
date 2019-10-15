/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Versuartn 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITuartNS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissuartns and
 * limitatuartns under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_ITF_UART_HPP__
#define __VP_ITF_UART_HPP__

#include "vp/vp.hpp"

namespace vp {



  class uart_slave;



  typedef void (uart_sync_meth_t)(void *, int data);
  typedef void (uart_sync_meth_muxed_t)(void *, int data, int id);



  class uart_master : public vp::master_port
  {
    friend class uart_slave;

  public:

    inline uart_master();

    inline void sync(int data)
    {
      return sync_meth(this->get_remote_context(), data);
    }

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_meth(uart_sync_meth_t *meth);
    inline void set_sync_meth_muxed(uart_sync_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_muxed_stub(uart_master *_this, int data);

    void (*slave_sync)(void *comp, int data);
    void (*slave_sync_mux)(void *comp, int data, int mux);

    void (*sync_meth)(void *, int data);
    void (*sync_meth_mux)(void *, int data, int mux);

    static inline void sync_default(void *, int data);

    vp::component *comp_mux;
    int sync_mux;
    uart_slave *slave_port = NULL;
    int mux_id;

  };



  class uart_slave : public vp::slave_port
  {

    friend class uart_master;

  public:

    inline uart_slave();

    inline void sync(int data)
    {
      slave_sync_meth(this->get_remote_context(), data);
    }

    inline void set_sync_meth(uart_sync_meth_t *meth);
    inline void set_sync_meth_muxed(uart_sync_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

  private:

    static inline void sync_muxed_stub(uart_slave *_this, int data);

    void (*slave_sync_meth)(void *, int data);
    void (*slave_sync_meth_mux)(void *, int data, int mux);

    void (*sync_meth)(void *comp, int data);
    void (*sync_mux_meth)(void *comp, int data, int mux);

    static inline void sync_default(uart_slave *, int data);

    vp::component *comp_mux;
    int sync_mux;
    int mux_id;

  };


  inline uart_master::uart_master() {
    slave_sync = &uart_master::sync_default;
    slave_sync_mux = NULL;
  }




  inline void uart_master::sync_muxed_stub(uart_master *_this, int data)
  {
    return _this->sync_meth_mux(_this->comp_mux, data, _this->sync_mux);
  }

  inline void uart_master::bind_to(vp::port *_port, vp::config *config)
  {
    uart_slave *port = (uart_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (uart_sync_meth_t *)&uart_master::sync_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void uart_master::set_sync_meth(uart_sync_meth_t *meth)
  {
    slave_sync = meth;
  }

  inline void uart_master::set_sync_meth_muxed(uart_sync_meth_muxed_t *meth, int id)
  {
    slave_sync_mux = meth;
    slave_sync = NULL;
    mux_id = id;
  }

  inline void uart_master::sync_default(void *, int data)
  {
  }

  inline void uart_slave::sync_muxed_stub(uart_slave *_this, int data)
  {
    return _this->slave_sync_meth_mux(_this->comp_mux, data, _this->sync_mux);
  }

  inline void uart_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    uart_master *port = (uart_master *)_port;
    port->slave_port = this;
    if (port->slave_sync_mux == NULL)
    {
      this->slave_sync_meth = port->slave_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      this->slave_sync_meth_mux = port->slave_sync_mux;
      this->slave_sync_meth = (uart_sync_meth_t *)&uart_slave::sync_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline uart_slave::uart_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (uart_sync_meth_t *)&uart_slave::sync_default;
  }

  inline void uart_slave::set_sync_meth(uart_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void uart_slave::set_sync_meth_muxed(uart_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void uart_slave::sync_default(uart_slave *, int data)
  {
  }



};

#endif
