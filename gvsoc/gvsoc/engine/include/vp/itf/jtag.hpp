/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Versjtagn 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITjtagNS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissjtagns and
 * limitatjtagns under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_ITF_JTAG_HPP__
#define __VP_ITF_JTAG_HPP__

#include "vp/vp.hpp"

namespace vp {



  class jtag_slave;



  typedef void (jtag_sync_meth_t)(void *, int tck, int tdi, int tms, int trst);
  typedef void (jtag_sync_cycle_meth_t)(void *, int tdi, int tms, int trst);

  typedef void (jtag_sync_meth_muxed_t)(void *, int tck, int tdi, int tms, int trst, int id);
  typedef void (jtag_sync_cycle_meth_muxed_t)(void *, int tdi, int tms, int trst, int id);

  typedef void (jtag_slave_sync_meth_t)(void *, int tdo);
  typedef void (jtag_slave_sync_meth_muxed_t)(void *, int tdo, int id);



  class jtag_master : public vp::master_port
  {
    friend class jtag_slave;

  public:

    inline jtag_master();

    inline void sync(int tck, int tdi, int tms, int trst)
    {
      return sync_meth(this->get_remote_context(), tck, tdi, tms, trst);
    }

    inline void sync_cycle(int tdi, int tms, int trst)
    {
      return sync_cycle_meth(this->get_remote_context(), tdi, tms, trst);
    }

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_meth(jtag_slave_sync_meth_t *meth);
    inline void set_sync_meth_muxed(jtag_slave_sync_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

    int tdo;

  private:

    static inline void sync_muxed_stub(jtag_master *_this, int tck, int tdi, int tms, int trst);
    static inline void sync_cycle_muxed_stub(jtag_master *_this, int tdi, int tms, int trst);

    void (*slave_sync)(void *comp, int tdo);
    void (*slave_sync_muxed)(void *comp, int tdo, int id);

    void (*sync_meth)(void *, int tck, int tdi, int tms, int trst);
    void (*sync_meth_mux)(void *, int tck, int tdi, int tms, int trst, int mux);
    void (*sync_cycle_meth)(void *, int tdi, int tms, int trst);
    void (*sync_cycle_meth_mux)(void *, int tdi, int tms, int trst, int mux);

    static inline void sync_default(void *, int tdo);


    vp::component *comp_mux;
    int sync_mux;
    int mux_id;
    jtag_slave *slave_port = NULL;
  };



  class jtag_slave : public vp::slave_port
  {

    friend class jtag_master;

  public:

    inline jtag_slave();

    inline void sync(int tdo)
    {
      slave_sync_meth(this->get_remote_context(), tdo);
    }

    inline void set_sync_meth(jtag_sync_meth_t *meth);
    inline void set_sync_meth_muxed(jtag_sync_meth_muxed_t *meth, int id);

    inline void set_sync_cycle_meth(jtag_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(jtag_sync_cycle_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

  private:

    static inline void sync_muxed_stub(jtag_slave *_this, int tdo);
    

    void (*slave_sync_meth)(void *, int tdo);
    void (*slave_sync_mux_meth)(void *, int tdo, int id);

    void (*sync_meth)(void *comp, int tck, int tdi, int tms, int trst);
    void (*sync_mux_meth)(void *comp, int tck, int tdi, int tms, int trst, int mux);
    void (*sync_cycle_meth)(void *comp, int tdi, int tms, int trst);
    void (*sync_cycle_mux_meth)(void *comp, int tdi, int tms, int trst, int mux);

    static inline void sync_default(jtag_slave *, int tck, int tdi, int tms, int trst);
    static inline void sync_cycle_default(jtag_slave *, int tdi, int tms, int trst);

    int mux_id;
    vp::component *comp_mux;
    int sync_mux;


  };


  inline jtag_master::jtag_master() {
    slave_sync = &jtag_master::sync_default;
  }



  inline void jtag_master::sync_muxed_stub(jtag_master *_this, int tck, int tdi, int tms, int trst)
  {
    return _this->sync_meth_mux(_this->comp_mux, tck, tdi, tms, trst, _this->sync_mux);
  }



  inline void jtag_master::sync_cycle_muxed_stub(jtag_master *_this, int tdi, int tms, int trst)
  {
    return _this->sync_cycle_meth_mux(_this->comp_mux, tdi, tms, trst, _this->sync_mux);
  }




  inline void jtag_master::bind_to(vp::port *_port, vp::config *config)
  {
    jtag_slave *port = (jtag_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      sync_cycle_meth = port->sync_cycle_meth;
      this->set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (jtag_sync_meth_t *)&jtag_master::sync_muxed_stub;

      sync_cycle_meth_mux = port->sync_cycle_mux_meth;
      sync_cycle_meth = (jtag_sync_cycle_meth_t *)&jtag_master::sync_cycle_muxed_stub;

      this->set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void jtag_master::set_sync_meth(jtag_slave_sync_meth_t *meth)
  {
    slave_sync = meth;
    slave_sync_muxed = NULL;
  }

  inline void jtag_master::set_sync_meth_muxed(jtag_slave_sync_meth_muxed_t *meth, int id)
  {
    slave_sync = NULL;
    slave_sync_muxed = meth;
    mux_id = id;
  }

  inline void jtag_master::sync_default(void *, int tdo)
  {
  }

  inline void jtag_slave::sync_muxed_stub(jtag_slave *_this, int tdo)
  {
    return _this->slave_sync_mux_meth(_this->comp_mux, tdo, _this->sync_mux);
  }

  inline void jtag_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    jtag_master *port = (jtag_master *)_port;
    port->slave_port = this;
    if (port->slave_sync_muxed == NULL)
    {
      this->slave_sync_meth = port->slave_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      this->slave_sync_mux_meth = port->slave_sync_muxed;  
      this->slave_sync_meth = (jtag_slave_sync_meth_t *)&jtag_slave::sync_muxed_stub;
      this->set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline jtag_slave::jtag_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (jtag_sync_meth_t *)&jtag_slave::sync_default;
    sync_cycle_meth = (jtag_sync_cycle_meth_t *)&jtag_slave::sync_cycle_default;
  }

  inline void jtag_slave::set_sync_meth(jtag_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void jtag_slave::set_sync_cycle_meth(jtag_sync_cycle_meth_t *meth)
  {
    sync_cycle_meth = meth;
    sync_cycle_mux_meth = NULL;
  }

  inline void jtag_slave::set_sync_meth_muxed(jtag_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void jtag_slave::set_sync_cycle_meth_muxed(jtag_sync_cycle_meth_muxed_t *meth, int id)
  {
    sync_cycle_mux_meth = meth;
    sync_cycle_meth = NULL;
    mux_id = id;
  }

  inline void jtag_slave::sync_default(jtag_slave *, int tck, int tdi, int tms, int trst)
  {
  }

  inline void jtag_slave::sync_cycle_default(jtag_slave *, int tdi, int tms, int trst)
  {
  }




};

#endif
