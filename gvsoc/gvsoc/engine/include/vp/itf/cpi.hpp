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

#ifndef __VP_ITF_CPI_HPP__
#define __VP_ITF_CPI_HPP__

#include "vp/vp.hpp"

namespace vp {



  class cpi_slave;



  typedef void (cpi_sync_meth_t)(void *, int pclk, int href, int vsync, int data);
  typedef void (cpi_sync_meth_muxed_t)(void *, int pclk, int href, int vsync, int data, int id);

  typedef void (cpi_sync_cycle_meth_t)(void *, int href, int vsync, int data);
  typedef void (cpi_sync_cycle_meth_muxed_t)(void *, int href, int vsync, int data, int id);


  class cpi_master : public vp::master_port
  {
    friend class cpi_slave;

  public:

    inline cpi_master();

    inline void sync(int pclk, int href, int vsync, int data)
    {
      return sync_meth(this->get_remote_context(), pclk, href, vsync, data);
    }

    inline void sync_cycle(int href, int vsync, int data)
    {
      return sync_cycle_meth(this->get_remote_context(), href, vsync, data);
    }

    void bind_to(vp::port *port, vp::config *config);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_muxed_stub(cpi_master *_this, int pclk, int href, int vsync, int data);
    static inline void sync_cycle_muxed_stub(cpi_master *_this, int href, int vsync, int data);

    void (*sync_meth)(void *, int pclk, int href, int vsync, int data);
    void (*sync_meth_mux)(void *, int pclk, int href, int vsync, int data, int mux);

    void (*sync_cycle_meth)(void *, int href, int vsync, int data);
    void (*sync_cycle_meth_mux)(void *, int href, int vsync, int data, int mux);

    vp::component *comp_mux;
    int sync_mux;
    cpi_slave *slave_port = NULL;

  };



  class cpi_slave : public vp::slave_port
  {

    friend class cpi_master;

  public:

    inline cpi_slave();

    inline void set_sync_meth(cpi_sync_meth_t *meth);
    inline void set_sync_meth_muxed(cpi_sync_meth_muxed_t *meth, int id);

    inline void set_sync_cycle_meth(cpi_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(cpi_sync_cycle_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

  private:

    void (*sync_meth)(void *comp, int pclk, int href, int vsync, int data);
    void (*sync_mux_meth)(void *comp, int pclk, int href, int vsync, int data, int mux);

    void (*sync_cycle_meth)(void *comp, int href, int vsync, int data);
    void (*sync_cycle_mux_meth)(void *comp, int href, int vsync, int data, int mux);

    static inline void sync_default(cpi_slave *, int pclk, int href, int vsync, int data);
    static inline void sync_cycle_default(cpi_slave *, int href, int vsync, int data);

    int mux_id;

  };


  inline cpi_master::cpi_master() {
  }




  inline void cpi_master::sync_muxed_stub(cpi_master *_this, int pclk, int href, int vsync, int data)
  {
    return _this->sync_meth_mux(_this->comp_mux, pclk, href, vsync, data, _this->sync_mux);
  }

  inline void cpi_master::sync_cycle_muxed_stub(cpi_master *_this, int href, int vsync, int data)
  {
    return _this->sync_cycle_meth_mux(_this->comp_mux, href, vsync, data, _this->sync_mux);
  }

  inline void cpi_master::bind_to(vp::port *_port, vp::config *config)
  {
    cpi_slave *port = (cpi_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      sync_cycle_meth = port->sync_cycle_meth;
      set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (cpi_sync_meth_t *)&cpi_master::sync_muxed_stub;

      sync_cycle_meth_mux = port->sync_cycle_mux_meth;
      sync_cycle_meth = (cpi_sync_cycle_meth_t *)&cpi_master::sync_cycle_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void cpi_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
  }

  inline cpi_slave::cpi_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (cpi_sync_meth_t *)&cpi_slave::sync_default;
    sync_cycle_meth = (cpi_sync_cycle_meth_t *)&cpi_slave::sync_cycle_default;
  }

  inline void cpi_slave::set_sync_meth(cpi_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void cpi_slave::set_sync_meth_muxed(cpi_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void cpi_slave::set_sync_cycle_meth(cpi_sync_cycle_meth_t *meth)
  {
    sync_cycle_meth = meth;
    sync_cycle_mux_meth = NULL;
  }

  inline void cpi_slave::set_sync_cycle_meth_muxed(cpi_sync_cycle_meth_muxed_t *meth, int id)
  {
    sync_cycle_mux_meth = meth;
    sync_cycle_meth = NULL;
    mux_id = id;
  }

  inline void cpi_slave::sync_default(cpi_slave *, int pclk, int href, int vsync, int data)
  {
  }

  inline void cpi_slave::sync_cycle_default(cpi_slave *, int href, int vsync, int data)
  {
  }



};

#endif
