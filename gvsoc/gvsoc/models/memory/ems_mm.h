/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2018 TU Kaiserslautern
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
 * Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
 */

#ifndef __EMS_MM_H__
#define __EMS_MM_H__

#include <vector>

#include <tlm.h>

namespace ems {

// Auto-extension
struct req_extension : tlm::tlm_extension<req_extension> {
  req_extension(vp::io_req *r, uint32_t id, bool last) :
    req(r),
    id(id),
    last(last)
  {
  }

  virtual tlm_extension_base *clone() const
  {
    return new req_extension(req, id, last);
  }

  virtual void copy_from(tlm_extension_base const &ext)
  {
    req = static_cast<req_extension const &>(ext).req;
    id = static_cast<req_extension const &>(ext).id;
    last = static_cast<req_extension const &>(ext).last;
  }

  vp::io_req *req; // Pointer to original request for integrity check
  uint32_t id;     // A request is split into multiple transactions
  bool last;       // Last transaction of a request
};

class mm : public tlm::tlm_mm_interface
{
public:
  mm()
  {
  }

  ~mm()
  {
    for (auto *p : pool) {
      // Default destructor ~tlm_generic_payload delete all extensions
      delete p;
    }
  }

  tlm::tlm_generic_payload *palloc()
  {
    tlm::tlm_generic_payload *p;
    if (pool.empty()) {
      // Recycle pool is empty, create a new generic payload object
      // and associate it to this memory manager
      p = new tlm::tlm_generic_payload();
      p->set_mm(this);
    } else {
      // Get the pointer from the recycling pool
      p = pool.back();
      // Remove the pointer from recycling pool
      pool.pop_back();
    }
    return p;
  }

  void free(tlm::tlm_generic_payload *p)
  {
    // Clear auto-extensions
    // Auto-extension object's free() will be called and the pointer is
    // set to NULL
    p->reset();
    // Add payload to recycle pool
    pool.push_back(p);
  }

private:
  std::vector<tlm::tlm_generic_payload *> pool;
};

} // namespace ems

#endif /* __EMS_MM_H__ */

