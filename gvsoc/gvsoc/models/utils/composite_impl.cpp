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



class composite : public vp::component
{

public:

  composite(const char *config);

  int build();

private:

};

composite::composite(const char *config)
: vp::component(config)
{

}



int composite::build()
{
  return 0;
}



extern "C" void *vp_constructor(const char *config)
{
  return (void *)new composite(config);
}
