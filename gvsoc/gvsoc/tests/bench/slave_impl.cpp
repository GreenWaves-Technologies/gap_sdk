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
#include <stdio.h>
#include <time.h>


class slave : public vp::component
{

public:

  slave(js::config *config);

  int build();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::io_slave in;

};

vp::io_req_status_e slave::req(void *__this, vp::io_req *req)
{
  //resp_port->resp(req);
}

int slave::build()
{
  in.set_req_meth(&slave::req);

  new_slave_port("in", &in);

  return 0;
}

slave::slave(js::config *config)
: vp::component(config)
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new slave(config);
}
