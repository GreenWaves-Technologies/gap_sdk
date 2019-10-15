/*
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
 * Authors: Andreas Traber
 */

#ifndef __CABLES_ADV_DBG_ITF_ADV_DBG_ITF_HPP__
#define __CABLES_ADV_DBG_ITF_ADV_DBG_ITF_HPP__

#include <vector>
#include <stdint.h>
#include <memory>

#include "cable.hpp"

class FTDILL;

struct jtag_device {
  uint32_t     id;
  unsigned int index;
  unsigned int ir_len;
  bool is_in_debug;
};

class Adv_dbg_itf : public Cable  {
  public:
    Adv_dbg_itf(js::config *system_config, const std::shared_ptr<Cable> &itf);
    ~Adv_dbg_itf();

    bool connect(js::config *config);
    void lock();
    void unlock();

    bool access(bool write, unsigned int addr, int size, char* buffer);

    void device_select(unsigned int i);

    bool chip_reset(bool active);
    bool jtag_reset(bool active);
    bool jtag_soft_reset();

    bool bit_inout(char* inbit, char outbit, bool last);
    bool stream_inout(char* instream, char* outstream, unsigned int n_bits, bool last);

    std::shared_ptr<Cable> get_device() { return m_dev; };

    int flush();


  private:
    enum ADBG_OPCODES {
      AXI_WRITE8  = 0x1,
      AXI_WRITE16 = 0x2,
      AXI_WRITE32 = 0x3,
      AXI_WRITE64 = 0x4,
      AXI_READ8   = 0x5,
      AXI_READ16  = 0x6,
      AXI_READ32  = 0x7,
      AXI_READ64  = 0x8
    };

    std::shared_ptr<Cable> m_dev;
    Log log;

    unsigned int debug_ir;
    int retry_count;
    int check_errors;
    int access_timeout;

    typedef std::vector<jtag_device> jtag_devices_t;
    typedef jtag_devices_t::size_type jtag_devices_size_t;

    jtag_devices_t m_jtag_devices;
    unsigned int             m_jtag_device_sel = 0;

    bool m_tms_on_last;

    bool jtag_reset_int(bool active);

    bool write(unsigned int addr, int size, char* buffer);
    bool write_internal(ADBG_OPCODES opcode, unsigned int addr, int size, char* buffer);

    bool read(unsigned int addr, int size, char* buffer);
    bool read_internal(ADBG_OPCODES opcode, unsigned int addr, int size, char* buffer);

    bool read_error_reg(uint32_t *addr, bool *error);
    bool clear_error_reg();

    bool jtag_pad_before();
    bool jtag_pad_after(bool tms);

    bool jtag_debug();

    int  ir_len_detect();
    int  dr_len_detect();

    int chain_len_detect();
    bool jtag_axi_select();
    bool jtag_auto_discovery();
    bool jtag_idle();
    bool jtag_set_selected_ir(char ir);


    uint32_t crc_compute(uint32_t crc, char* data_in, int length_bits);
};

#endif
