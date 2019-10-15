/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Vershypern 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDIThyperNS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permisshyperns and
 * limitathyperns under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_ITF_HYPER_HPP__
#define __VP_ITF_HYPER_HPP__

#include "vp/vp.hpp"

namespace vp {



  class hyper_slave;



  typedef void (hyper_sync_cycle_meth_t)(void *, int data);
  typedef void (hyper_cs_sync_meth_t)(void *, int cs, int active);

  typedef void (hyper_sync_cycle_meth_muxed_t)(void *, int data, int id);
  typedef void (hyper_cs_sync_meth_muxed_t)(void *, int cs, int active, int id);


  class hyper_master : public vp::master_port
  {
    friend class hyper_slave;

  public:

    inline hyper_master();

    inline void sync_cycle(int data)
    {
      return sync_cycle_meth(this->get_remote_context(), data);
    }

    inline void cs_sync(int cs, int active)
    {
      return cs_sync_meth(this->get_remote_context(), cs, active);
    }

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_cycle_meth(hyper_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(hyper_sync_cycle_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_cycle_muxed_stub(hyper_master *_this, int data);
    static inline void cs_sync_muxed_stub(hyper_master *_this, int cs, int active);

    void (*slave_sync_cycle)(void *comp, int data);
    void (*slave_sync_cycle_mux)(void *comp, int data, int mux);

    void (*sync_cycle_meth)(void *, int data);
    void (*sync_cycle_meth_mux)(void *, int data, int mux);
    void (*cs_sync_meth)(void *, int cs, int active);
    void (*cs_sync_meth_mux)(void *, int cs, int active, int mux);

    static inline void sync_cycle_default(void *, int data);


    vp::component *comp_mux;
    int sync_mux;
    hyper_slave *slave_port = NULL;
    int mux_id;
  };



  class hyper_slave : public vp::slave_port
  {

    friend class hyper_master;

  public:

    inline hyper_slave();

    inline void sync_cycle(int data)
    {
      slave_sync_cycle_meth(this->get_remote_context(), data);
    }

    inline void set_sync_cycle_meth(hyper_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(hyper_sync_cycle_meth_muxed_t *meth, int id);

    inline void set_cs_sync_meth(hyper_cs_sync_meth_t *meth);
    inline void set_cs_sync_meth_muxed(hyper_cs_sync_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

    static inline void sync_cycle_muxed_stub(hyper_slave *_this, int data);

  private:


    void (*slave_sync_cycle_meth)(void *, int data);
    void (*slave_sync_cycle_meth_mux)(void *, int data, int mux);

    void (*sync_cycle_meth)(void *comp, int data);
    void (*sync_cycle_mux_meth)(void *comp, int data, int mux);
    void (*cs_sync)(void *comp, int cs, int active);
    void (*cs_sync_mux)(void *comp, int cs, int active, int mux);

    static inline void sync_cycle_default(hyper_slave *, int data);
    static inline void cs_sync_default(hyper_slave *, int cs, int active);

    vp::component *comp_mux;
    int sync_mux;
    int mux_id;


  };


  inline hyper_master::hyper_master() {
    slave_sync_cycle = &hyper_master::sync_cycle_default;
    slave_sync_cycle_mux = NULL;
  }


  inline void hyper_master::sync_cycle_default(void *, int data)
  {
  }


  inline void hyper_master::set_sync_cycle_meth(hyper_sync_cycle_meth_t *meth)
  {
    slave_sync_cycle = meth;
  }

  inline void hyper_master::set_sync_cycle_meth_muxed(hyper_sync_cycle_meth_muxed_t *meth, int id)
  {
    slave_sync_cycle_mux = meth;
    slave_sync_cycle = NULL;
    mux_id = id;
  }



  inline void hyper_master::sync_cycle_muxed_stub(hyper_master *_this, int data)
  {
    return _this->sync_cycle_meth_mux(_this->comp_mux, data, _this->sync_mux);
  }



  inline void hyper_master::cs_sync_muxed_stub(hyper_master *_this, int cs, int active)
  {
    return _this->cs_sync_meth_mux(_this->comp_mux, cs, active, _this->sync_mux);
  }



  inline void hyper_master::bind_to(vp::port *_port, vp::config *config)
  {
    hyper_slave *port = (hyper_slave *)_port;
    if (port->sync_cycle_mux_meth == NULL)
    {
      sync_cycle_meth = port->sync_cycle_meth;
      cs_sync_meth = port->cs_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      sync_cycle_meth_mux = port->sync_cycle_mux_meth;
      sync_cycle_meth = (hyper_sync_cycle_meth_t *)&hyper_master::sync_cycle_muxed_stub;

      cs_sync_meth_mux = port->cs_sync_mux;
      cs_sync_meth = (hyper_cs_sync_meth_t *)&hyper_master::cs_sync_muxed_stub;

      this->set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void hyper_slave::sync_cycle_muxed_stub(hyper_slave *_this, int data)
  {
    return _this->slave_sync_cycle_meth_mux(_this->comp_mux, data, _this->sync_mux);
  }

  inline void hyper_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    hyper_master *port = (hyper_master *)_port;
    port->slave_port = this;
    if (port->slave_sync_cycle_mux == NULL)
    {
      this->slave_sync_cycle_meth = port->slave_sync_cycle;
      this->set_remote_context(port->get_context());
    }
    else
    {
      this->slave_sync_cycle_meth_mux = port->slave_sync_cycle_mux;
      this->slave_sync_cycle_meth = (hyper_sync_cycle_meth_t *)&hyper_slave::sync_cycle_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline hyper_slave::hyper_slave() : sync_cycle_meth(NULL), sync_cycle_mux_meth(NULL) {
    sync_cycle_meth = (hyper_sync_cycle_meth_t *)&hyper_slave::sync_cycle_default;
    cs_sync = (hyper_cs_sync_meth_t *)&hyper_slave::cs_sync_default;
  }

  inline void hyper_slave::set_sync_cycle_meth(hyper_sync_cycle_meth_t *meth)
  {
    sync_cycle_meth = meth;
    sync_cycle_mux_meth = NULL;
  }

  inline void hyper_slave::set_cs_sync_meth(hyper_cs_sync_meth_t *meth)
  {
    cs_sync = meth;
    cs_sync_mux = NULL;
  }

  inline void hyper_slave::set_sync_cycle_meth_muxed(hyper_sync_cycle_meth_muxed_t *meth, int id)
  {
    sync_cycle_mux_meth = meth;
    sync_cycle_meth = NULL;
    mux_id = id;
  }

  inline void hyper_slave::set_cs_sync_meth_muxed(hyper_cs_sync_meth_muxed_t *meth, int id)
  {
    cs_sync_mux = meth;
    cs_sync = NULL;
    mux_id = id;
  }

  inline void hyper_slave::sync_cycle_default(hyper_slave *, int data)
  {
  }


  inline void hyper_slave::cs_sync_default(hyper_slave *, int cs, int active)
  {
  }



};

#endif
