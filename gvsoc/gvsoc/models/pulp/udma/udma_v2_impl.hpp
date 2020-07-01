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

#ifndef __PULP_UDMA_UDMA_V2_IMPL_HPP__
#define __PULP_UDMA_UDMA_V2_IMPL_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/wire.hpp>
#include <vp/itf/hyper.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/clk.hpp>
#include <vp/itf/i2s.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/udma/udma_v2.h"
#ifdef HAS_I2S
#include "archi/udma/i2s/udma_i2s_v1_new.h"
#endif
#ifdef HAS_TCDM
#include "archi/udma/memcpy/v1/udma_memcpy_v1.h"
#include "archi/udma/memcpy/v1/udma_memcpy_v1_gvsoc.h"
#endif

class udma;
class Udma_channel;


class Udma_transfer
{
public:
  uint32_t addr;
  uint32_t size;
  int transfer_size;
  int continuous_mode;
  uint32_t current_addr;
  int32_t remaining_size;
  uint64_t received_size;
  Udma_channel *channel;

  bool prepare_req(vp::io_req *req);
  void set_next(Udma_transfer *next) { this->next = next; }
  Udma_transfer *get_next() { return next; }
  Udma_transfer *next;
};



template<class T>
class Udma_queue
{
public:
  Udma_queue(int size) : size(size) { init(); }
  void init() { first=NULL; last=NULL; nb_cmd=0; }
  T *pop();
  inline void push(T *cmd);
  void push_from_latency(T *cmd);
  bool is_full() { return nb_cmd >= size; }
  bool is_empty() { return nb_cmd == 0; }
  T *get_first() { return first; }

private:
  T *first;    // First command of the queue, commands are popped from here
  T *last;     // Last command of the queue, commands are pushed here
  int        nb_cmd;
  int        size;
};

template<class T>
void Udma_queue<T>::push_from_latency(T *cmd)
{
  T *current = first, *prev = NULL;
  while (current && cmd->get_latency() > current->get_latency())
  {
    prev = current;
    current = current->get_next();
  }

  if (current == NULL)
    last = cmd;

  if (prev)
    prev->set_next(cmd);
  else
    first = cmd;
  cmd->set_next(current);
  nb_cmd++;
}


class Udma_channel
{
public:
  Udma_channel(udma *top, int id, std::string name);
  virtual vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
  virtual void reset(bool active);
  virtual bool is_tx() { return false; }
  void set_next(Udma_channel *next) { this->next = next; }
  Udma_channel *get_next() { return next; }
  void event_handler();
  bool prepare_req(vp::io_req *req);
  void push_ready_req(vp::io_req *req);
  void handle_ready_req_end(vp::io_req *req);
  virtual bool is_busy() { return false; }
  virtual void handle_ready() { }

  Udma_transfer *current_cmd;
  Udma_queue<vp::io_req> *ready_reqs;

  uint32_t saddr;
  uint32_t size;
  
protected:
  vp::trace     trace;
  udma *top;
  void handle_transfer_end();

private:
  virtual vp::io_req_status_e saddr_req(vp::io_req *req);
  virtual vp::io_req_status_e size_req(vp::io_req *req);
  virtual vp::io_req_status_e cfg_req(vp::io_req *req); 
  void enqueue_transfer();
  void check_state();
  virtual void handle_ready_req(vp::io_req *req);
  virtual void handle_ready_reqs();

  int transfer_size;
  bool continuous_mode;

  int id;
  string name;
  Udma_channel *next;

  vp::clock_event *event;

  Udma_queue<Udma_transfer> *free_reqs;
  Udma_queue<Udma_transfer> *pending_reqs;

  vp::trace     state_event;

  vp::wire_master<bool>    irq_itf;
};



class Udma_rx_channel : public Udma_channel
{
public:
  Udma_rx_channel(udma *top, int id, string name) : Udma_channel(top, id, name) {}
  bool is_tx() { return false; }
  void reset(bool active);
  bool push_data(uint8_t *data, int size);
  bool has_cmd() { return this->current_cmd != NULL; }

private:
  int pending_byte_index;
  uint32_t pending_word;
};



class Udma_tx_channel : public Udma_channel
{
public:
  Udma_tx_channel(udma *top, int id, string name) : Udma_channel(top, id, name) {}
  bool is_tx() { return true; }

  void handle_pending_word(void *__this, vp::clock_event *event);

};



class Udma_periph 
{
public:
  Udma_periph(udma *top, int id);
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
  virtual void reset(bool active);
  void clock_gate(bool is_on);

protected:
  Udma_channel *channel0 = NULL;
  Udma_channel *channel1 = NULL;
  Udma_channel *channel2 = NULL;
  udma *top;

private:
  virtual vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  bool is_on;
  int id;
};




/*
 * SPIM
 */

class Spim_periph_v2;


class Spim_rx_channel : public Udma_rx_channel
{
public:
  Spim_rx_channel(udma *top, Spim_periph_v2 *periph, int id, string name) : Udma_rx_channel(top, id, name), periph(periph) {}

  void reset(bool active);

  void handle_rx_bits(int data_0, int data_1, int data_2, int data_3, int mask);

private:
  Spim_periph_v2 *periph;
};


class Spim_tx_channel : public Udma_tx_channel
{
public:
  Spim_tx_channel(udma *top, Spim_periph_v2 *periph, int id, string name);
  void handle_ready_reqs();
  void check_state();

private:
  void reset(bool active);
  static void handle_pending_word(void *__this, vp::clock_event *event);
  static void handle_spi_pending_word(void *__this, vp::clock_event *event);
  void handle_eot();
  void handle_data(uint32_t data);

  vp::clock_event *pending_word_event;
  vp::clock_event *pending_spi_word_event;

  Spim_periph_v2 *periph;

  uint32_t spi_tx_pending_word;   // Word being flushed to spi pads
  int      spi_tx_pending_bits;   // Tell how many bits are ready to be sent to SPI pads
  bool spi_tx_quad;
  bool spi_tx_byte_align;

  bool     has_tx_pending_word;   // Tell if a TX pending word is present
  uint32_t tx_pending_word;       // Word received by last L2 req
  uint32_t rx_pending_word;
  int64_t next_bit_cycle;
  vp::io_req *pending_req;
  uint32_t command;
  int cs;
  bool gen_eot_with_evt;
};


class Spim_periph_v2 : public Udma_periph
{
  friend class Spim_tx_channel;
  friend class Spim_rx_channel;

public:
  Spim_periph_v2(udma *top, int id, int itf_id);
  static void slave_sync(void *_this, int data_0, int data_1, int data_2, int data_3, int mask);
  void reset(bool active);

protected:
  vp::qspim_master qspim_itf;
  int clkdiv;
  bool waiting_rx;
  bool waiting_tx;
  bool is_full_duplex;
  int byte_align;
  int qpi;
  int cmd_pending_bits;
  int nb_received_bits;
  uint32_t rx_pending_word;
  uint32_t tx_pending_word;
  int eot_event;
  int rx_count;
  int tx_count;
  uint32_t received_bits;
  
  int      spi_rx_pending_bits;   // Tell how many bits should be received from spi pads

};




/*
 * I2C
 */

class I2c_periph_v2;

class I2c_rx_channel : public Udma_rx_channel
{
public:
  I2c_rx_channel(udma *top, I2c_periph_v2 *periph, int id, string name);
  bool is_busy();
  void handle_rx_bit(int bit);

private:
  void reset(bool active);
  I2c_periph_v2 *periph;
  uint8_t  pending_rx_byte;
  int nb_received_bits;
};


class I2c_tx_channel : public Udma_tx_channel
{
public:
  I2c_tx_channel(udma *top, I2c_periph_v2 *periph, int id, string name);
  void handle_ready_reqs();
  bool is_busy();

private:
  void reset(bool active);
  void check_state();
  static void handle_pending_word(void *__this, vp::clock_event *event);

  I2c_periph_v2 *periph;

  vp::clock_event *pending_word_event;

  uint32_t pending_word;
  int pending_bits;
  vp::io_req *pending_req;
  int64_t next_bit_cycle;
};


typedef enum
{
  I2C_PERIPH_STATE_WAIT_CMD,
  I2C_PERIPH_STATE_WAIT_CFG,
  I2C_PERIPH_STATE_WR0,
  I2C_PERIPH_STATE_WR1,
  I2C_PERIPH_STATE_ACK0,
  I2C_PERIPH_STATE_ACK1,
  I2C_PERIPH_STATE_WAIT_RPT,
  I2C_PERIPH_STATE_WAIT_RPT_CMD,
  I2C_PERIPH_STATE_RD,
  I2C_PERIPH_STATE_START0,
  I2C_PERIPH_STATE_STOP0,
  I2C_PERIPH_STATE_STOP1
} i2c_periph_state_e;


class I2c_periph_v2 : public Udma_periph
{
  friend class I2c_tx_channel;
  friend class I2c_rx_channel;

public:
  I2c_periph_v2(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

protected:
  vp::i2c_master i2c_itf;
  i2c_periph_state_e state;
  unsigned int pending_value;
  int pending_value_bits;
  int clkdiv;
  int repeat_count;
  bool waiting_rx;
  bool gen_ack;
  bool waiting_pending_bits;
  unsigned int repeat_command;
  int pending_rx_bit;
  int prev_sda;
  int prev_scl;

private:
  vp::io_req_status_e status_req(vp::io_req *req);
  vp::io_req_status_e setup_req(vp::io_req *req);
  static void rx_sync(void *, int data);

  vp::trace     trace;
};



/*
 * UART
 */

class Uart_periph_v1;

typedef enum
{
  UART_RX_STATE_WAIT_START,
  UART_RX_STATE_DATA,
  UART_RX_STATE_PARITY,
  UART_RX_STATE_WAIT_STOP
} uart_rx_state_e;

class Uart_rx_channel : public Udma_rx_channel
{
public:
  Uart_rx_channel(udma *top, Uart_periph_v1 *periph, int id, string name);
  bool is_busy();
  void handle_rx_bit(int bit);

private:
  void reset(bool active);
  Uart_periph_v1 *periph;
  uart_rx_state_e state;
  int parity;
  int stop_bits;
  uint8_t  pending_rx_byte;
  int nb_received_bits;
};


typedef enum
{
  UART_TX_STATE_START,
  UART_TX_STATE_DATA,
  UART_TX_STATE_PARITY,
  UART_TX_STATE_STOP
} uart_tx_state_e;

class Uart_tx_channel : public Udma_tx_channel
{
public:
  Uart_tx_channel(udma *top, Uart_periph_v1 *periph, int id, string name);
  void handle_ready_reqs();
  bool is_busy();

private:
  void reset(bool active);
  void check_state();
  static void handle_pending_word(void *__this, vp::clock_event *event);

  Uart_periph_v1 *periph;

  vp::clock_event *pending_word_event;

  uint32_t pending_word;
  int pending_bits;
  uart_tx_state_e state;
  vp::io_req *pending_req;
  int parity;
  int64_t next_bit_cycle;
  int stop_bits;
  int sent_bits;
};


class Uart_periph_v1 : public Udma_periph
{
  friend class Uart_tx_channel;
  friend class Uart_rx_channel;

public:
  Uart_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

  int parity;
  int bit_length;
  int stop_bits;
  int tx;
  int rx;
  int clkdiv;
  int rx_pe;

protected:
  vp::uart_master uart_itf;

private:
  vp::io_req_status_e status_req(vp::io_req *req);
  vp::io_req_status_e setup_req(vp::io_req *req);
  void set_setup_reg(uint32_t value);
  static void rx_sync(void *, int data);

  uint32_t setup_reg_value;

  vp::trace     trace;
};


/*
 * CPI
 */

class Cpi_periph_v1;

class Cpi_rx_channel : public Udma_rx_channel
{
public:
  Cpi_rx_channel(udma *top, Cpi_periph_v1 *periph, int id, string name);

private:
  void reset(bool active);
  Cpi_periph_v1 *periph;
};

class Cpi_periph_v1 : public Udma_periph
{
  friend class Cpi_rx_channel;

public:
  Cpi_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);
  void handle_sof();

protected:
  vp::cpi_slave cpi_itf;

private:
  static void sync(void *__this, int pclk, int href, int vsync, int data);
  static void sync_cycle(void *__this, int href, int vsync, int data);
  vp::io_req_status_e handle_global_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_l1_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_ur_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_size_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_filter_access(bool is_write, uint32_t *data);
  void push_pixel(uint32_t pixel);

  vp::trace     trace;

  int pending_byte;
  bool has_pending_byte;
  bool cmd_ready;

  uint32_t glob;
  uint32_t ll;
  uint32_t ur;
  uint32_t size;
  uint32_t filter;

  bool wroteGlob;
  bool wroteLl;
  bool wroteUr;
  bool wroteSize;
  bool wroteFilter;

  unsigned int enabled;
  unsigned int frameDrop;
  unsigned int nbFrameDrop;
  unsigned int frameSliceEn;
  unsigned int format;
  unsigned int shift;

  unsigned int frameSliceLlx;
  unsigned int frameSliceLly;
  unsigned int frameSliceUrx;
  unsigned int frameSliceUry;

  unsigned int rowLen;

  unsigned int bCoeff;
  unsigned int gCoeff;
  unsigned int rCoeff;

  unsigned int frameDropCount;
  unsigned int currentLine;
  unsigned int currentRow;

};



#ifdef HAS_TCDM

/*
 * TCDM
 */

class Tcdm_periph_v1;


class Tcdm_tx_channel : public Udma_tx_channel
{
public:
  Tcdm_tx_channel(udma *top, Tcdm_periph_v1 *periph, int id, string name);
  void handle_ready_reqs();

private:
  void reset(bool active);
  static void handle_pending_word(void *__this, vp::clock_event *event);

  Tcdm_periph_v1 *periph;

  vp::clock_event *pending_word_event;
};


class Tcdm_rx_channel : public Udma_rx_channel
{
public:
  Tcdm_rx_channel(udma *top, Tcdm_periph_v1 *periph, int id, string name);
  void handle_ready();

private:

  Tcdm_periph_v1 *periph;
};


class Tcdm_periph_v1 : public Udma_periph
{
  friend class Tcdm_tx_channel;
  friend class Tcdm_rx_channel;

public:
  Tcdm_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

protected:
  vp::io_master io_itf;

private:

  void check_state();
  static void handle_reqs(void *__this, vp::clock_event *event);

  vp_udma_memcpy_dst_addr r_dst_addr;
  vp_udma_memcpy_src_addr r_src_addr;
  vp_udma_memcpy_mem_sel  r_mem_sel;

  Udma_queue<vp::io_req> *out_reqs;
  Udma_queue<vp::io_req> *out_waiting_reqs;

  vp::clock_event *pending_reqs_event;

  int read_size;

  vp::trace     trace;
};

#endif 


#ifdef HAS_I2S
/*
 * I2S
 */

class I2s_periph_v1;

class I2s_cic_filter {
public:
  I2s_cic_filter();

  bool handle_bit(int din, int pdm_decimation, int pdm_shift, uint32_t *dout);
  void reset();

  int     pdm_pending_bits;
  int64_t pdm_y1_old;
  int64_t pdm_y2_old;
  int64_t pdm_y3_old;
  int64_t pdm_y4_old;
  int64_t pdm_y5_old;
  int64_t pdm_z1_old;
  int64_t pdm_z2_old;
  int64_t pdm_z3_old;
  int64_t pdm_z4_old;
  int64_t pdm_z5_old;
  int64_t pdm_zin1_old;
  int64_t pdm_zin2_old;
  int64_t pdm_zin3_old;
  int64_t pdm_zin4_old;
  int64_t pdm_zin5_old;
};

class I2s_rx_channel : public Udma_rx_channel
{
  friend class I2s_periph_v1;
  
public:
  I2s_rx_channel(udma *top, I2s_periph_v1 *periph, int id, int event_id, string name);
  void handle_rx_bit(int sck, int ws, int bit);

protected:
  bool wait_ws_start = false;

private:
  void reset(bool active);
  I2s_periph_v1 *periph;

  I2s_cic_filter *filters[2];
  int id;
  uint32_t pending_samples[2];
  int pending_bits[2];
};

class I2s_periph_v1 : public Udma_periph
{
  friend class I2s_rx_channel;

public:
  I2s_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

protected:
  static void rx_sync(void *, int sck, int ws, int sd, int channel);

private:

  vp::io_req_status_e cfg_ext_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e cfg_clkgen0_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e cfg_clkgen1_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e chmode_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e filt_ch0_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e filt_ch1_req(int reg_offset, int size, bool is_write, uint8_t *data);

  static void clkgen_event_routine(void *__this, vp::clock_event *event);
  vp::io_req_status_e check_clkgen0();
  vp::io_req_status_e check_clkgen1();
  vp::io_req_status_e reset_clkgen0();
  vp::io_req_status_e reset_clkgen1();
  void handle_clkgen_tick(int clkgen);

  vp::trace     trace;
  vp::i2s_slave ch_itf[2];

  vp_udma_i2s_cfg_ext     r_cfg_ext;
  vp_udma_i2s_cfg_clkgen0 r_cfg_clkgen0;
  vp_udma_i2s_cfg_clkgen1 r_cfg_clkgen1;
  vp_udma_i2s_chmode      r_chmode;
  vp_udma_i2s_filt_ch0    r_filt_ch0;
  vp_udma_i2s_filt_ch1    r_filt_ch1;

  vp::clock_event *clkgen0_event;
  vp::clock_event *clkgen1_event;

  int sck[2];
  int current_ws[2];
  int current_ws_count[2];
};

#endif




/*
 * HYPER
 */

class Hyper_periph_v1;

class Hyper_rx_channel : public Udma_rx_channel
{
public:
  Hyper_rx_channel(udma *top, Hyper_periph_v1 *periph, int id, string name);
  void handle_rx_data(int data);
  void handle_ready();

private:
  void reset(bool active);
  Hyper_periph_v1 *periph;
};


typedef enum
{
  HYPER_STATE_IDLE,
  HYPER_STATE_DELAY,
  HYPER_STATE_CS,
  HYPER_STATE_CA,
  HYPER_STATE_DATA,
  HYPER_STATE_CS_OFF,
} hyper_state_e;



class Hyper_tx_channel : public Udma_tx_channel
{
  friend class Hyper_periph_v1;

public:
  Hyper_tx_channel(udma *top, Hyper_periph_v1 *periph, int id, string name);

protected:
  void handle_ready_reqs();

private:
  void reset(bool active);

  Hyper_periph_v1 *periph;

};


class Hyper_periph_v1 : public Udma_periph
{
  friend class Hyper_tx_channel;
  friend class Hyper_rx_channel;

public:
  Hyper_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  static void rx_sync(void *__this, int data);
  void reset(bool active);
  static void handle_pending_word(void *__this, vp::clock_event *event);
  void check_state();
  void handle_ready_reqs();

protected:
  vp::hyper_master hyper_itf;
  unsigned int *regs; 
  int clkdiv;
  Hyper_tx_channel *tx_channel;
  Hyper_rx_channel *rx_channel;

private:
  vp::trace     trace;

  vector<Udma_transfer *> pending_transfers;

  int pending_bytes;
  vp::clock_event *pending_word_event;
  int64_t next_bit_cycle;
  vp::io_req *pending_req;
  uint32_t pending_word;
  int transfer_size;
  hyper_state_e state;
  int delay;
  int ca_count;
  bool pending_tx;
  bool pending_rx;
  Udma_transfer *current_cmd;
  union
  {
    struct {
      unsigned int low_addr:3;
      unsigned int reserved:13;
      unsigned int high_addr:29;
      unsigned int burst_type:1;
      unsigned int address_space:1;
      unsigned int read:1;
    } __attribute__((packed));
    uint8_t raw[6];
  } ca;
};



/*
 * UDMA
 */

template<class T>
inline void Udma_queue<T>::push(T *cmd)
{
  if (first)
    last->set_next(cmd);
  else
    first = cmd;
  cmd->set_next(NULL);
  last = cmd;
  nb_cmd++;
}



template<class T>
T *Udma_queue<T>::pop()
{
  if (!first) return NULL;
  T *cmd = first;
  first = cmd->get_next();
  nb_cmd--;

  return cmd;
}



class udma : public vp::component
{
  friend class Udma_periph;
  friend class Udma_rx_channel;

public:

  udma(js::config *config);

  int build();
  void start();
  void reset(bool active);

  void enqueue_ready(Udma_channel *channel);

  static void channel_handler(void *__this, vp::clock_event *event);
  void free_read_req(vp::io_req *req);

  void trigger_event(int event);

  vp::trace *get_trace() { return &this->trace; }
  vp::clock_engine *get_periph_clock() { return this->periph_clock; }

  vp::io_master l2_itf;

protected:
  void push_l2_write_req(vp::io_req *req);

private:

  void check_state();

  vp::io_req_status_e conf_req(vp::io_req *req, uint64_t offset);
  vp::io_req_status_e periph_req(vp::io_req *req, uint64_t offset);
  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  static void event_handler(void *__this, vp::clock_event *event);
  static void l2_grant(void *__this, vp::io_req *req);
  static void l2_response(void *__this, vp::io_req *req);
  static void set_periph_clock_frequency(void *_this, int64_t frequency);
  static void clk_reg(component *_this, component *clock);

  vp::trace        trace;
  vp::io_slave     in;
  vp::clk_slave    periph_clock_itf;
  vp::clock_engine *periph_clock;

  int nb_periphs;
  int l2_read_fifo_size;
  std::vector<Udma_periph *>periphs;
  Udma_queue<Udma_channel> *ready_rx_channels;
  Udma_queue<Udma_channel> *ready_tx_channels;
  uint32_t clock_gating;
  vp::clock_event *event;
  Udma_queue<vp::io_req> *l2_read_reqs;
  Udma_queue<vp::io_req> *l2_write_reqs;
  Udma_queue<vp::io_req> *l2_read_waiting_reqs;
  
  vp::wire_master<int>    event_itf;
};


#endif
