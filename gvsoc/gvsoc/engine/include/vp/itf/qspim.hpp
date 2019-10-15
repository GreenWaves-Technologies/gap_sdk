/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Versqspimn 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITqspimNS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissqspimns and
 * limitatqspimns under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_ITF_QSPIM_HPP__
#define __VP_ITF_QSPIM_HPP__

#include "vp/vp.hpp"

namespace vp {



  class qspim_slave;



  typedef void (qspim_sync_meth_t)(void *, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  typedef void (qspim_sync_cycle_meth_t)(void *, int data_0, int data_1, int data_2, int data_3, int mask);
  typedef void (qspim_cs_sync_meth_t)(void *, int cs, int active);

  typedef void (qspim_sync_meth_muxed_t)(void *, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  typedef void (qspim_sync_cycle_meth_muxed_t)(void *, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  typedef void (qspim_cs_sync_meth_muxed_t)(void *, int cs, int active, int id);

  typedef void (qspim_slave_sync_meth_t)(void *, int data_0, int data_1, int data_2, int data_3, int mask);
  typedef void (qspim_slave_sync_meth_muxed_t)(void *, int data_0, int data_1, int data_2, int data_3, int mask, int id);



  class qspim_master : public vp::master_port
  {
    friend class qspim_slave;

  public:

    inline qspim_master();

    inline void sync(int sck, int data_0, int data_1, int data_2, int data_3, int mask)
    {
      return sync_meth(this->get_remote_context(), sck, data_0, data_1, data_2, data_3, mask);
    }

    inline void sync_cycle(int data_0, int data_1, int data_2, int data_3, int mask)
    {
      return sync_cycle_meth(this->get_remote_context(), data_0, data_1, data_2, data_3, mask);
    }

    inline void cs_sync(int cs, int active)
    {
      return cs_sync_meth(this->get_remote_context(), cs, active);
    }

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_meth(qspim_slave_sync_meth_t *meth);

    inline void set_sync_meth_muxed(qspim_slave_sync_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_muxed_stub(qspim_master *_this, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
    static inline void sync_cycle_muxed_stub(qspim_master *_this, int data_0, int data_1, int data_2, int data_3, int mask);
    static inline void cs_sync_muxed_stub(qspim_master *_this, int cs, int active);

    void (*slave_sync)(void *comp, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*slave_sync_mux)(void *comp, int data_0, int data_1, int data_2, int data_3, int mask, int id);

    void (*sync_meth)(void *, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*sync_meth_mux)(void *, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int mux);
    void (*sync_cycle_meth)(void *, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*sync_cycle_meth_mux)(void *, int data_0, int data_1, int data_2, int data_3, int mask, int mux);
    void (*cs_sync_meth)(void *, int cs, int active);
    void (*cs_sync_meth_mux)(void *, int cs, int active, int mux);

    static inline void sync_default(void *, int data_0, int data_1, int data_2, int data_3, int mask);


    vp::component *comp_mux;
    int sync_mux;
    qspim_slave *slave_port = NULL;

    int mux_id;
  };



  class qspim_slave : public vp::slave_port
  {

    friend class qspim_master;

  public:

    inline qspim_slave();

    inline void sync(int data_0, int data_1, int data_2, int data_3, int mask)
    {
      slave_sync_meth(this->get_remote_context(), data_0, data_1, data_2, data_3, mask);
    }

    inline void set_sync_meth(qspim_sync_meth_t *meth);
    inline void set_sync_meth_muxed(qspim_sync_meth_muxed_t *meth, int id);

    inline void set_sync_cycle_meth(qspim_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(qspim_sync_cycle_meth_muxed_t *meth, int id);

    inline void set_cs_sync_meth(qspim_cs_sync_meth_t *meth);
    inline void set_cs_sync_meth_muxed(qspim_cs_sync_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

  private:

    static inline void sync_muxed_stub(qspim_slave *_this, int data_0, int data_1, int data_2, int data_3, int mask);


    void (*slave_sync_meth)(void *, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*slave_sync_meth_mux)(void *, int data_0, int data_1, int data_2, int data_3, int mask, int mux);

    void (*sync_meth)(void *comp, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*sync_mux_meth)(void *comp, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int mux);
    void (*sync_cycle_meth)(void *comp, int data_0, int data_1, int data_2, int data_3, int mask);
    void (*sync_cycle_mux_meth)(void *comp, int data_0, int data_1, int data_2, int data_3, int mask, int mux);
    void (*cs_sync)(void *comp, int cs, int active);
    void (*cs_sync_mux)(void *comp, int cs, int active, int mux);

    static inline void sync_default(qspim_slave *, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
    static inline void sync_cycle_default(qspim_slave *, int data_0, int data_1, int data_2, int data_3, int mask);
    static inline void cs_sync_default(qspim_slave *, int cs, int active);

    vp::component *comp_mux;
    int sync_mux;
    int mux_id;


  };


  inline qspim_master::qspim_master() {
    slave_sync = &qspim_master::sync_default;
    slave_sync_mux = NULL;
  }



  inline void qspim_master::sync_muxed_stub(qspim_master *_this, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
  {
    return _this->sync_meth_mux(_this->comp_mux, sck, data_0, data_1, data_2, data_3, mask, _this->sync_mux);
  }



  inline void qspim_master::sync_cycle_muxed_stub(qspim_master *_this, int data_0, int data_1, int data_2, int data_3, int mask)
  {
    return _this->sync_cycle_meth_mux(_this->comp_mux, data_0, data_1, data_2, data_3, mask, _this->sync_mux);
  }



  inline void qspim_master::cs_sync_muxed_stub(qspim_master *_this, int cs, int active)
  {
    return _this->cs_sync_meth_mux(_this->comp_mux, cs, active, _this->sync_mux);
  }



  inline void qspim_master::bind_to(vp::port *_port, vp::config *config)
  {
    qspim_slave *port = (qspim_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      sync_cycle_meth = port->sync_cycle_meth;
      cs_sync_meth = port->cs_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (qspim_sync_meth_t *)&qspim_master::sync_muxed_stub;

      sync_cycle_meth_mux = port->sync_cycle_mux_meth;
      sync_cycle_meth = (qspim_sync_cycle_meth_t *)&qspim_master::sync_cycle_muxed_stub;

      cs_sync_meth_mux = port->cs_sync_mux;
      cs_sync_meth = (qspim_cs_sync_meth_t *)&qspim_master::cs_sync_muxed_stub;

      this->set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void qspim_master::set_sync_meth(qspim_slave_sync_meth_t *meth)
  {
    slave_sync = meth;
  }

  inline void qspim_master::set_sync_meth_muxed(qspim_slave_sync_meth_muxed_t *meth, int id)
  {
    slave_sync_mux = meth;
    slave_sync = NULL;
    mux_id = id;
  }

  inline void qspim_master::sync_default(void *, int data_0, int data_1, int data_2, int data_3, int mask)
  {
  }

  inline void qspim_slave::sync_muxed_stub(qspim_slave *_this, int data_0, int data_1, int data_2, int data_3, int mask)
  {
    return _this->slave_sync_meth_mux(_this->comp_mux, data_0, data_1, data_2, data_3, mask, _this->sync_mux);
  }

  inline void qspim_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    qspim_master *port = (qspim_master *)_port;
    port->slave_port = this;
    if (port->slave_sync_mux == NULL)
    {
      this->slave_sync_meth = port->slave_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      this->slave_sync_meth_mux = port->slave_sync_mux;
      this->slave_sync_meth = (qspim_sync_cycle_meth_t *)&qspim_slave::sync_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline qspim_slave::qspim_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (qspim_sync_meth_t *)&qspim_slave::sync_default;
    sync_cycle_meth = (qspim_sync_cycle_meth_t *)&qspim_slave::sync_cycle_default;
    cs_sync = (qspim_cs_sync_meth_t *)&qspim_slave::cs_sync_default;
  }

  inline void qspim_slave::set_sync_meth(qspim_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void qspim_slave::set_sync_cycle_meth(qspim_sync_cycle_meth_t *meth)
  {
    sync_cycle_meth = meth;
    sync_cycle_mux_meth = NULL;
  }

  inline void qspim_slave::set_cs_sync_meth(qspim_cs_sync_meth_t *meth)
  {
    cs_sync = meth;
    cs_sync_mux = NULL;
  }

  inline void qspim_slave::set_sync_meth_muxed(qspim_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void qspim_slave::set_sync_cycle_meth_muxed(qspim_sync_cycle_meth_muxed_t *meth, int id)
  {
    sync_cycle_mux_meth = meth;
    sync_cycle_meth = NULL;
    mux_id = id;
  }

  inline void qspim_slave::set_cs_sync_meth_muxed(qspim_cs_sync_meth_muxed_t *meth, int id)
  {
    cs_sync_mux = meth;
    cs_sync = NULL;
    mux_id = id;
  }

  inline void qspim_slave::sync_default(qspim_slave *, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
  {
  }

  inline void qspim_slave::sync_cycle_default(qspim_slave *, int data_0, int data_1, int data_2, int data_3, int mask)
  {
  }


  inline void qspim_slave::cs_sync_default(qspim_slave *, int cs, int active)
  {
  }



};

#endif
