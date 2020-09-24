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
#include <string.h>
#include <vp/itf/jtag.hpp>
#include <vp/itf/wire.hpp>
#include <unordered_map>

#define BYPASS_INSTR    0x00
#define IDCODE_INSTR    0x01
#define DTM_CTRL_INSTR  0x10
#define DMI_INSTR      0x11
#define BYPASS2_INSTR   0x1f

#define IR_LEN 5

#define DMI_ADDR_LEN 7
#define DMI_DR_LEN   (2 + 32 + DMI_ADDR_LEN)



typedef union {
  struct {
    unsigned int regno:16;
    unsigned int write:1;
    unsigned int transfer:1;
    unsigned int postexec:1;
    unsigned int reserved:1;
    unsigned int aarsize:3;
    unsigned int reserved2:1;
    unsigned int cmdtype:8;
  } u;
  uint32_t raw;
} dm_control_reg_t;



typedef enum {
  JTAG_STATE_TEST_LOGIC_RESET,
  JTAG_STATE_RUN_TEST_IDLE,
  JTAG_STATE_SELECT_DR_SCAN,
  JTAG_STATE_CAPTURE_DR,
  JTAG_STATE_SHIFT_DR,
  JTAG_STATE_EXIT1_DR,
  JTAG_STATE_PAUSE_DR,
  JTAG_STATE_EXIT2_DR,
  JTAG_STATE_UPDATE_DR,
  JTAG_STATE_SELECT_IR_SCAN,
  JTAG_STATE_CAPTURE_IR,
  JTAG_STATE_SHIFT_IR,
  JTAG_STATE_EXIT1_IR,
  JTAG_STATE_PAUSE_IR,
  JTAG_STATE_EXIT2_IR,
  JTAG_STATE_UPDATE_IR
} JTAG_STATE_e;



static string tapStateName[] = {
  "Test-Logic-Reset", "Run-Test/Idle", "Select-DR-Scan", "Capture-DR", "Shift-DR", "Exit1-DR", "Pause-DR", "Exit2-DR", "Update-DR", "Select-IT-Scan", "Capture-IR", "Shift-IR", "Exit1-IR", "Pause-IR", "Exit2-IR", "Update-IR"
};



static string get_instr_name(uint32_t instr) {
  if (instr == IDCODE_INSTR) return "IDCODE";
  else if (instr == BYPASS_INSTR || instr == BYPASS2_INSTR) return "BYPASS";
  else if (instr == DMI_INSTR) return "DMI";
  else if (instr == DTM_CTRL_INSTR) return "DTM_CTRL";
  else return "RESERVED";
}


class Dtm_slave
{
public:
  Dtm_slave(int id) : id(id), halted(0), halt_requested(0) {}

  int id;
  int halted;
  int halt_requested;
  vp::wire_master<bool> itf;
};


class riscv_dtm : public vp::component
{

public:

  riscv_dtm(js::config *config);

  int build();
  void start();
  void reset(bool value);

private:

  static void sync(void *__this, int tck, int tdi, int tms, int trst);
  static void sync_cycle(void *__this, int tdi, int tms, int trst);
  static vp::io_req_status_e core_req(void *__this, vp::io_req *req);
  vp::io_req_status_e going_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e resume_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e halted_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e flags_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e program_buffer_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e data0_req(int reg_offset, int size, bool is_write, uint8_t *data);
  void tck_edge(int tck, int tdi, int tms, int trst);
  void tap_update(int tdi, int tms, int tclk);
  void tap_init();
  void tap_reset();
  void select_module(uint64_t command);
  void update_dr(int tdi, int tclk);
  void module_cmd(uint64_t dev_command);
  void burst_cmd(int word_bytes, int is_read, uint64_t command);
  void capture_dr(int tdi, int tclk);
  void shift_dr(int tdi, int tclk);
  void shift_ir(int tdi, int tclk);
  void update_ir(int tdi, int tclk);
  void handle_dm_control_access(uint32_t op, uint32_t data);
  void handle_command_access(uint32_t op, uint32_t data);
  void handle_dm_status_access(uint32_t op, uint32_t data);
  void handle_data0_access(uint32_t op, uint32_t data);
  void handle_dmi_access();

  vp::trace     trace;
  vp::trace     debug;
  vp::jtag_slave  jtag_slave_itf;
  vp::jtag_master jtag_master_itf;
  std::unordered_map<int, Dtm_slave *> slaves_map;
  std::vector<Dtm_slave *> slaves;
  vp::io_slave  core_io_itf;

  JTAG_STATE_e state;
  JTAG_STATE_e prev_state;

  uint64_t tdi;
  int      tdo;
  int      dr_len;
  uint32_t idcode;
  int      instr;
  uint32_t ir_shift;

  uint32_t dmi_data;
  uint32_t dmi_addr;
  int dmi_op;

  int tclk;

  int nb_halt_reqs;
  int nb_halted;

  uint32_t abstract_cmd[32];

  vp::reg_32    dm_control_r;
  uint32_t data0;

  uint32_t flags;
};

riscv_dtm::riscv_dtm(js::config *config)
: vp::component(config)
{

}

void riscv_dtm::tap_reset()
{
  this->state = JTAG_STATE_TEST_LOGIC_RESET;
  this->tdi = 0x57575757;
  this->dr_len = 32;
  this->instr = IDCODE_INSTR;
  this->idcode = 0x12345678;
}

void riscv_dtm::reset(bool value)
{
  if (value)
  {
    this->tap_reset();
    this->nb_halt_reqs = 0;
    this->nb_halted = 0;
    this->flags = 0;
  }
}



void riscv_dtm::handle_dm_control_access(uint32_t op, uint32_t data)
{
  this->dm_control_r.access(0, 4, (uint8_t *)&data, op == 2);

  if (op == 2)
  {
    if (data >> 31)
    {
      int hart = (((data >> 16) & 0x3ff) << 0) | (((data >> 6) & 0x3ff) << 10);
      this->debug.msg("Halting core (hart: 0x%x)\n", hart);

      Dtm_slave *slave = slaves_map[hart];
      if (slave == NULL)
      {
        this->trace.force_warning("Trying to halt unknown core (hart id: %d)\n", hart);
      }
      else
      {
        this->nb_halt_reqs++;
        slave->halt_requested = 1;
        slave->itf.sync(true);
      }
    }
    else if (data >> 30)
    {
      int hart = (((data >> 16) & 0x3ff) << 0) | (((data >> 6) & 0x3ff) << 10);
      this->debug.msg("Resuming core (hart: 0x%x)\n", hart);

      this->flags |= 1<<1;
    }
  }
}


#define AUIPC(rd,imm) ( \
  ((((imm) >> 20) &   0x1) << 31) | \
  ((((imm) >>  1) & 0x3ff) << 21) | \
  ((((imm) >> 11) &   0x1) << 20) | \
  ((((imm) >> 12) &  0xff) << 12) | \
  ((((rd ) >>  0) &  0x1f) <<  7) | \
  0x17)

#define SRLI(rd,rs1,shamt) ( \
  ((((shamt) >> 0) & 0xfff) << 20) | \
  ((((rs1  ) >> 0) &  0x1f) << 15) | \
  ((((0x5  ) >> 0) &   0x7) << 12) | \
  ((((rd   ) >> 0) &  0x1f) << 7 ) | \
  ((((0x13 ) >> 0) &  0x7f) << 0 ))

#define SLLI(rd,rs1,shamt) ( \
  ((((shamt) >> 0) & 0xfff) << 20) | \
  ((((rs1  ) >> 0) &  0x1f) << 15) | \
  ((((0x1  ) >> 0) &   0x7) << 12) | \
  ((((rd   ) >> 0) &  0x1f) << 7 ) | \
  ((((0x13 ) >> 0) &  0x7f) << 0 ))

#define CSRW(csr,rs1) ( \
  ((((csr  ) >> 0) & 0xfff) << 20) | \
  ((((rs1  ) >> 0) &  0x1f) << 15) | \
  ((((0x1  ) >> 0) &   0x7) << 12) | \
  ((((0    ) >> 0) &  0x1f) << 7 ) | \
  ((((0x73 ) >> 0) &  0x7f) << 0 ))

#define CSRR(csr,dest) ( \
  ((((csr  ) >> 0) & 0xfff) << 20) | \
  ((((0    ) >> 0) &  0x1f) << 15) | \
  ((((0x2  ) >> 0) &   0x7) << 12) | \
  ((((dest ) >> 0) &  0x1f) << 7 ) | \
  ((((0x73 ) >> 0) &  0x7f) << 0 ))

#define LOAD(size, dest, base, offset) ( \
  ((((offset) >> 0) & 0xfff) << 20) | \
  ((((base  ) >> 0) &  0x1f) << 15) | \
  ((((size  ) >> 0) &   0x7) << 12) | \
  ((((dest  ) >> 0) &  0x1f) << 7 ) | \
  ((((0x03  ) >> 0) &  0x7f) << 0 ))

#define EBREAK() 0x00100073

#define CSR_DSCRATCH0 0x7b2
#define CSR_DSCRATCH1 0x7b3
#define DATA_ADDR     0x380

void riscv_dtm::handle_command_access(uint32_t op, uint32_t data)
{
  dm_control_reg_t reg = { .raw=data };

  this->debug.msg("Abstract command (cmdtype: %d, aarsize: %d, postexec: %d, transfer: %d, write: %d, regno: 0x%x)\n", reg.u.cmdtype, reg.u.aarsize, reg.u.postexec, reg.u.transfer, reg.u.write, reg.u.regno);

  if (op == 1)
  {
#if 0
    // load debug module base address into a0, this is shared among all commands
    this->abstract_cmd[1] = AUIPC(10, 0);
    this->abstract_cmd[2] = SRLI(10, 10, 12); // clear lowest 12bit to get base offset of DM
    this->abstract_cmd[3] = SLLI(10, 10, 12);
    // CSR register to data
    // store s0 in dscratch
    this->abstract_cmd[4] = CSRW(CSR_DSCRATCH0, 8);
    // read value from CSR into s0
    this->abstract_cmd[5] = CSRR(dm::ac_ar.regno[11:0], 8);
                        // and store s0 into data section
                        abstract_cmd[3][31:0]  = dm::store(ac_ar.aarsize, 8, 10, dm::DataAddr);
                        // restore s0 again from dscratch
                        abstract_cmd[3][63:32] = dm::csrr(dm::CSR_DSCRATCH0, 8);

        abstract_cmd[4][31:0]  = dm::csrr(dm::CSR_DSCRATCH1, 10);
        abstract_cmd[4][63:32] = dm::ebreak();
#endif
  }
  else if (op == 2)
  {                  
    // store a0 in dscratch1
    this->abstract_cmd[0] = CSRW(CSR_DSCRATCH1, 10);
    // load debug module base address into a0, this is shared among all commands
    this->abstract_cmd[1] = AUIPC(10, 0);
    this->abstract_cmd[2] = SRLI(10, 10, 12); // clear lowest 12bit to get base offset of DM
    this->abstract_cmd[3] = SLLI(10, 10, 12);
    // data register to CSR
    // store s0 in dscratch
    this->abstract_cmd[4] = CSRW(CSR_DSCRATCH0, 8);
    // load from data register
    this->abstract_cmd[5] = LOAD(reg.u.aarsize, 8, 10, DATA_ADDR);
    // and store it in the corresponding CSR
    this->abstract_cmd[6] = CSRW(reg.u.regno, 8);
    // restore s0 again from dscratch
    this->abstract_cmd[7] = CSRR(CSR_DSCRATCH0, 8);
    this->abstract_cmd[8] = CSRR(CSR_DSCRATCH1, 10);
    this->abstract_cmd[9] = EBREAK();

    this->flags |= 1;
  }

}



void riscv_dtm::handle_dm_status_access(uint32_t op, uint32_t data)
{
  if (op == 1)
  {
    int allhalted = this->nb_halt_reqs == 0;
    this->dmi_data = (allhalted << 9);
  }

}



void riscv_dtm::handle_data0_access(uint32_t op, uint32_t data)
{
  if (op == 2)
  {
    this->data0 = data;
  }
}



void riscv_dtm::handle_dmi_access()
{
  switch (this->dmi_addr)
  {
    case 0x10:
      this->handle_dm_control_access(this->dmi_op, this->dmi_data);
      break;

    case 0x04:
      this->handle_data0_access(this->dmi_op, this->dmi_data);
      break;

    case 0x17:
      this->handle_command_access(this->dmi_op, this->dmi_data);
      break;

    case 0x11:
      this->handle_dm_status_access(this->dmi_op, this->dmi_data);
      break;
  }
}



void riscv_dtm::update_dr(int tdi, int tclk)
{
  if (tclk && this->instr == DMI_INSTR)
  {
    this->dmi_op   = (this->tdi >> 0) & 0x3;
    this->dmi_data = (this->tdi >> 2) & 0xffffffff;
    this->dmi_addr = (this->tdi >> 34) & 0x7f;

    this->debug.msg("Starting DMI operation (raw: %llx, op: %d, data: 0x%8.8x, addr: 0x%x\n", this->tdi, this->dmi_op, this->dmi_data, this->dmi_addr);

    this->handle_dmi_access();
  }
}



void riscv_dtm::capture_dr(int tdi, int tclk) 
{
  if (this->instr == IDCODE_INSTR)
  {
    this->tdi = this->idcode;
  }
  else if (tclk && this->instr == DMI_INSTR && this->dmi_op == 1)
  {
    this->tdi = this->dmi_data << 2;
  }
}



void riscv_dtm::shift_ir(int tdi, int tclk)
{
  if (tclk)
  {
    this->ir_shift = (this->ir_shift >> 1) | (tdi << (IR_LEN - 1));
  }
}


void riscv_dtm::update_ir(int tdi, int tclk)
{
  if (!tclk)
  {
    this->instr = this->ir_shift;

    this->debug.msg("Updated new instruction (ir: 0x%x / %s)\n", this->instr, get_instr_name(this->instr).c_str());

    switch (this->instr)
    {
      case BYPASS_INSTR:
      case BYPASS2_INSTR:
        this->dr_len = 1;
        break;

      case IDCODE_INSTR:
        this->dr_len = 32;
        break;

      case DMI_INSTR:
        this->dr_len = DMI_DR_LEN;
        break;
    }
  }
}



void riscv_dtm::shift_dr(int tdi, int tclk)
{
  if (!tclk)
  {
    this->tdo = this->tdi & 1;
  }

  if (tclk)
  {
    this->tdi = (this->tdi >> 1) | (((uint64_t)tdi) << (this->dr_len - 1));
  }
}

void riscv_dtm::tap_update(int tdi, int tms, int tclk)
{
  JTAG_STATE_e next_state = this->state;

  if (!this->tclk && tclk)
  {
    this->prev_state = this->state;

    switch (this->state)
    {
    case JTAG_STATE_TEST_LOGIC_RESET:
      this->tap_reset();
      if (!tms) next_state = JTAG_STATE_RUN_TEST_IDLE;
      break;

    case JTAG_STATE_RUN_TEST_IDLE:
      if (tms) next_state = JTAG_STATE_SELECT_DR_SCAN;
      break;

    case JTAG_STATE_SELECT_DR_SCAN:
      if (tms) next_state = JTAG_STATE_SELECT_IR_SCAN;
      else next_state = JTAG_STATE_CAPTURE_DR;
      break;

    case JTAG_STATE_CAPTURE_DR:
      if (tms) next_state = JTAG_STATE_EXIT1_DR;
      else next_state = JTAG_STATE_SHIFT_DR;
      break;

    case JTAG_STATE_SHIFT_DR:
      if (tms) next_state = JTAG_STATE_EXIT1_DR;
      break;

    case JTAG_STATE_EXIT1_DR:
      if (tms) next_state = JTAG_STATE_UPDATE_DR;
      else next_state = JTAG_STATE_PAUSE_DR;
      break;

    case JTAG_STATE_PAUSE_DR:
      if (tms) next_state = JTAG_STATE_EXIT2_DR;
      break;

    case JTAG_STATE_EXIT2_DR:
      if (tms) next_state = JTAG_STATE_UPDATE_DR;
      else next_state = JTAG_STATE_SHIFT_DR;
      break;

    case JTAG_STATE_UPDATE_DR:
      if (tms) next_state = JTAG_STATE_SELECT_DR_SCAN;
      else next_state = JTAG_STATE_RUN_TEST_IDLE;
      break;

    case JTAG_STATE_SELECT_IR_SCAN:
      if (tms) next_state = JTAG_STATE_TEST_LOGIC_RESET;
      else next_state = JTAG_STATE_CAPTURE_IR;
      break;

    case JTAG_STATE_CAPTURE_IR:
      if (tms) next_state = JTAG_STATE_EXIT1_IR;
      else next_state = JTAG_STATE_SHIFT_IR;
      break;

    case JTAG_STATE_SHIFT_IR:
      if (tms) next_state = JTAG_STATE_EXIT1_IR;
      break;

    case JTAG_STATE_EXIT1_IR:
      if (tms) next_state = JTAG_STATE_UPDATE_IR;
      else next_state = JTAG_STATE_PAUSE_IR;
      break;

    case JTAG_STATE_PAUSE_IR:
      if (tms) next_state = JTAG_STATE_EXIT2_IR;
      break;

    case JTAG_STATE_EXIT2_IR:
      if (tms) next_state = JTAG_STATE_UPDATE_IR;
      else next_state = JTAG_STATE_SHIFT_IR;
      break;

    case JTAG_STATE_UPDATE_IR:
      if (tms) next_state = JTAG_STATE_SELECT_DR_SCAN;
      else next_state = JTAG_STATE_RUN_TEST_IDLE;
      break;
    }
  }
  else if (this->tclk && !tclk)
  {
  }

  switch (this->state)
  {
    case JTAG_STATE_SHIFT_DR:
      this->shift_dr(tdi, tclk);
      break;

    case JTAG_STATE_SHIFT_IR:
      this->shift_ir(tdi, tclk);
      break;

    case JTAG_STATE_UPDATE_IR:
      this->update_ir(tdi, tclk);
      break;

    case JTAG_STATE_UPDATE_DR:
      this->update_dr(tdi, tclk);
      break;

    case JTAG_STATE_CAPTURE_DR:
      this->capture_dr(tdi, tclk);
      break;
  }

  if (next_state != this->state)
  {
    this->state = next_state;
    debug.msg("Changing TAP state (newState: %s)\n", tapStateName[this->state].c_str());
  }

  this->tclk = tclk;
}

void riscv_dtm::tck_edge(int tck, int tdi, int tms, int trst)
{
  this->trace.msg("Executing cycle (TMS_BIT: %1d, TDI_BIT: %1d, TRST_BIT: %1d, TCLK_BIT: %d)\n", tms, tdi, trst, tck);

  if (!this->tclk && tck)
  {
  }
  else if (this->tclk && !tck)
  {
  }

  this->jtag_master_itf.sync(tck, this->tdo, tms, trst);

  this->tap_update(tdi, tms, tck);
}

void riscv_dtm::sync(void *__this, int tck, int tdi, int tms, int trst)
{
  riscv_dtm *_this = (riscv_dtm *)__this;

  // First propagate all signals to the next device on the chain
  _this->tck_edge(tck, tdi, tms, trst);

  // And shift in tdi in case it is a raising clock edge
  //if (tck)
   // _this->tdi = (_this->tdi >> 1) | (tdi << (_this->dr_len - 1));
}


void riscv_dtm::sync_cycle(void *__this, int tdi, int tms, int trst)
{
  riscv_dtm *_this = (riscv_dtm *)__this;
  _this->tck_edge(1, tdi, tms, trst);
  _this->tck_edge(0, tdi, tms, trst);
}



vp::io_req_status_e riscv_dtm::going_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (size != 4)
    return vp::IO_REQ_INVALID;

  if (is_write)
    this->flags &= ~(1<<0);

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::resume_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (size != 4)
    return vp::IO_REQ_INVALID;

  if (is_write)
    this->flags &= ~(1<<1);

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::halted_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (size != 4)
    return vp::IO_REQ_INVALID;

  int hart_id = *(uint32_t *)data;

  this->debug.msg("Halted req (hart id: 0x%x)\n", hart_id);

  Dtm_slave *slave = this->slaves_map[hart_id];
  if (slave == NULL)
  {
    this->trace.force_warning("Received invalid hart id (hart id: %d)\n", hart_id);
    return vp::IO_REQ_INVALID;
  }

  slave->halted = 1;

  if (slave->halt_requested)
  {
    this->nb_halt_reqs--;
    this->nb_halted++;
    slave->halt_requested = 0;
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::data0_req(int offset, int size, bool is_write, uint8_t *data)
{
  if (size != 4)
    return vp::IO_REQ_INVALID;

  if (!is_write)
  {
    *(uint32_t *)data = this->data0;
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::program_buffer_req(int offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write)
  {
    if (data)
      memcpy(data, this->abstract_cmd + offset/4, size);
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::flags_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (size != 1)
    return vp::IO_REQ_INVALID;

  int hart_id = reg_offset;

  this->debug.msg("Flags req (hart id: 0x%x)\n", hart_id);

  Dtm_slave *slave = this->slaves_map[hart_id];
  if (slave == NULL)
  {
    this->trace.force_warning("Received invalid hart id (hart id: %d)\n", hart_id);
    return vp::IO_REQ_INVALID;
  }

  int active_hart = (((this->dm_control_r.get() >> 16) & 0x3ff) << 0) | (((this->dm_control_r.get() >> 6) & 0x3ff) << 10);

  if (active_hart == hart_id)
  {
    *data = this->flags;
  }
  else
  {
    *data = 0;
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::core_req(void *__this, vp::io_req *req)
{
  riscv_dtm *_this = (riscv_dtm *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();
  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  _this->debug.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (reg_id) {
    case 0x100/4:     err = _this->halted_req(reg_offset, size, is_write, data); break;
    case 0x104/4:     err = _this->going_req(reg_offset, size, is_write, data); break;
    case 0x108/4:     err = _this->resume_req(reg_offset, size, is_write, data); break;
    default:
      if (offset >= 0x300 && offset < 0x380)
      {
        err = _this->program_buffer_req(offset - 0x300, size, is_write, data);
        break;
      }
      else if (offset == 0x380)
      {
        err = _this->data0_req(offset - 0x380, size, is_write, data);
        break;
      }
      else if (offset >= 0x400)
      {
        err = _this->flags_req(offset - 0x400, size, is_write, data);
        break;
      }
  }



    // TODO we should be able to report an error in asynchronous requests

end:
  if (err != vp::IO_REQ_OK)
    _this->get_trace()->force_warning("RISCV DTM invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return err;
}


int riscv_dtm::build()
{
  traces.new_trace("trace", &trace, vp::TRACE);
  traces.new_trace("debug", &debug, vp::DEBUG);

  core_io_itf.set_req_meth(&riscv_dtm::core_req);
  new_slave_port("input", &core_io_itf);

  jtag_slave_itf.set_sync_meth(&riscv_dtm::sync);
  jtag_slave_itf.set_sync_cycle_meth(&riscv_dtm::sync_cycle);
  new_slave_port("jtag_in", &jtag_slave_itf);

  new_master_port("jtag_out", &jtag_master_itf);

  this->new_reg("dm_control", &this->dm_control_r, 0);

  this->tclk = 0;

  for (auto hart: this->get_js_config()->get("harts")->get_elems())
  {
    int hart_id = hart->get_elem(0)->get_int();
    string target = hart->get_elem(1)->get_str();

    Dtm_slave *slave = new Dtm_slave(hart_id);

    this->new_master_port(target, &slave->itf);

    this->slaves_map[hart_id] = slave;
    this->slaves.push_back(slave);
  }

  return 0;
}

void riscv_dtm::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new riscv_dtm(config);
}



