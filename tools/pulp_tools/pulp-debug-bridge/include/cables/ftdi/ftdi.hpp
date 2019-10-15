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

#ifndef MEM_FTDI_LL_H
#define MEM_FTDI_LL_H

#include "adv_dbg_itf/adv_dbg_itf.hpp"

#include <stdint.h>
#include <map>
#include <list>

extern "C" {
#include <ftdi.h>
}

#define SETUP_DELAY		25000
#define FTDX_MAXSEND 4096
#define FTDX_MAXSEND_MPSSE (64 * 1024)
#define FTDI_MAXRECV   ( 4 * 64)

struct ftdi_param {
  uint32_t  send_buf_len;
  uint32_t  send_buffered;
  char  *send_buf;
  uint32_t  recv_buf_len;
  uint32_t  to_recv;
  uint32_t  recv_write_idx;
  uint32_t  recv_read_idx;
  char  *recv_buf;
};

class Log;

class Ftdi : public Cable {
  public:

    enum FTDIDeviceID {
      Olimex,
      Digilent,
    };

    Ftdi(js::config *config, FTDIDeviceID id, cable_cb_t cable_state_cb);
    ~Ftdi();

    bool connect(js::config *config);

    bool bit_inout(char* inbit, char outbit, bool last);

    bool stream_inout(char* instream, char* outstream, unsigned int n_bits, bool last);

    bool jtag_reset(bool active);

    bool chip_setVQPS(bool high);

    int flush();

    bool purge();

    bool chip_reset(bool active);

  private:
    struct device_desc {
      unsigned int vid;
      unsigned int pid;
      unsigned int index;

      device_desc(int vid, int pid, int index) : vid(vid), pid(pid), index(index) {}
      device_desc(int vid, int pid) : vid(vid), pid(pid), index(0) {}
    };

    void fatal_error(const char *str, ...);

    bool set_bit_value(int bit, int value);
    bool set_bit_direction(int bit, int isout);

    bool bit_out(char outbit, bool tms);

    bool stream_out(char* outstream, unsigned int n_bits, bool last);
    bool stream_in(char* instream, unsigned int n_bits, bool last);

    void state_trans_tms(int tms);

    bool stream_out_internal(char* outstream, unsigned int n_bits, bool postread, bool last);

    int ft2232_write_bytes(char *buf, int len, bool postread);
    int ft2232_write_bits(char *buf, int len, bool postread, bool with_tms);
    int ft2232_read_packed_bits(char *buf, int packet_len, int bits_per_packet, int offset);

    bool ft2232_common_open();
    bool ft2232_mpsse_open();
    int ft2232_seq_purge(int purge_rx, int purge_tx);
    int ft2232_seq_reset();
    int ft2232_read(char* buf, int len);
    int ft2232_write(char *buf, int len, int recv);
    bool dev_try_open(unsigned int vid, unsigned int pid, unsigned int index) const;

    std::map<FTDIDeviceID, std::list<struct device_desc>> m_descriptors;

    Log log;
    FTDIDeviceID m_id;
    struct ftdi_param m_params;
    struct ftdi_context m_ftdic;
    unsigned int bits_value;
    unsigned int bits_direction;
    cable_cb_t cable_state_cb;
    cable_state_t current_state = CABLE_DISCONNECTED;
};

#endif
