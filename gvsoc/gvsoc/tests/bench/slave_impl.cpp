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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <time.h>


class slave : public vp::component
{

public:

  slave(const char *config);

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

slave::slave(const char *config)
: vp::component(config)
{
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new slave(config);
}
