/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2019 TU Kaiserslautern
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

#ifndef __EMS_ADDR_ADAPTER_H__
#define __EMS_ADDR_ADAPTER_H__

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "ems_common.h"

namespace ems {

class addr_adapter : public sc_module
{
public:
  tlm_utils::simple_initiator_socket<addr_adapter> isocket;
  tlm_utils::simple_target_socket<addr_adapter> tsocket;

  SC_HAS_PROCESS(addr_adapter);
  addr_adapter(sc_core::sc_module_name name, sc_dt::uint64 offset) :
    sc_core::sc_module(name),
    isocket("isocket"),
    tsocket("tsocket"),
    offset(offset)
  {
    tsocket.register_b_transport(this, &addr_adapter::b_transport);
    tsocket.register_nb_transport_fw(this, &addr_adapter::nb_transport_fw);
    tsocket.register_get_direct_mem_ptr(this, &addr_adapter::get_direct_mem_ptr);
    tsocket.register_transport_dbg(this, &addr_adapter::transport_dbg);

    isocket.register_nb_transport_bw(this, &addr_adapter::nb_transport_bw);
    isocket.register_invalidate_direct_mem_ptr(this, &addr_adapter::invalidate_direct_mem_ptr);

    debug(this->name() << " offset: 0x" << std::setfill('0') << std::setw(16) << std::hex << static_cast<uint64_t>(offset) << std::dec);
  }

  // Module interface - forward path
  void b_transport(tlm::tlm_generic_payload &p, sc_time &d)
  {
    p.set_address(p.get_address() - offset);
    isocket->b_transport(p, d);
  }

  tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &p, tlm::tlm_phase &ph, sc_time &d)
  {
    p.set_address(p.get_address() - offset);
    return isocket->nb_transport_fw(p, ph, d);
  }

  bool get_direct_mem_ptr(tlm::tlm_generic_payload &p, tlm::tlm_dmi &dmi_data)
  {
    // DMA request in the forward path
    // Modify the address attribute of the transaction
    p.set_address(conv_addr_ds(p.get_address()));
    bool s = isocket->get_direct_mem_ptr(p, dmi_data);
    // DMA descriptors overwritten in the return path
    dmi_data.set_start_address(conv_addr_up(dmi_data.get_start_address()));
    dmi_data.set_end_address(conv_addr_up(dmi_data.get_end_address()));
    return s;
  }

  unsigned int transport_dbg(tlm::tlm_generic_payload &p)
  {
    p.set_address(p.get_address() - offset);
    return isocket->transport_dbg(p);
  }

  // Module interface - backward path
  tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &p, tlm::tlm_phase &ph, sc_time &d)
  {
    return tsocket->nb_transport_bw(p, ph, d);
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
  {
    // Propagate to upstream component
    tsocket->invalidate_direct_mem_ptr(conv_addr_up(start_range), conv_addr_up(start_range));
  }

private:
  // Address conversion downstream
  sc_dt::uint64 conv_addr_ds(sc_dt::uint64 addr)
  {
    return addr - offset;
  }

  // Address conversion upstream
  sc_dt::uint64 conv_addr_up(sc_dt::uint64 addr)
  {
    return addr + offset;
  }

  sc_dt::uint64 offset;
};

} // namespace ems

#endif /* __EMS_ADDR_ADAPTER_H__ */

