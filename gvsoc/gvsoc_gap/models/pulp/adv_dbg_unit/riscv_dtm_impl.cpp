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
#include <archi/riscv_dbg/riscv_dbg_dm_regs.h>
#include <archi/riscv_dbg/riscv_dbg_dm_regfields.h>
#include <archi/riscv_dbg/riscv_dbg_dm_gvsoc.h>
#include <archi/riscv_dbg/riscv_dbg_dm_structs.h>

using namespace std::placeholders;


#define BYPASS_INSTR    0x00
#define IDCODE_INSTR    0x01
#define DTM_CTRL_INSTR  0x10
#define DMI_INSTR      0x11
#define BYPASS2_INSTR   0x1f

#define IR_LEN 5

#define DMI_ADDR_LEN 7
#define DMI_DR_LEN   (2 + 32 + DMI_ADDR_LEN)

#define DTM_CTRL_DR_LEN   (32)
#define DTM_CTRL_RESET_VAL (1 | (DMI_ADDR_LEN << 4))




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
  Dtm_slave(int id) : id(id), halted(0), pending_req(NULL), flags(0), resumeack(0), available(0) {}

  void set_flags(int flags);

  int id;
  int halted;
  vp::io_req *pending_req;
  vp::wire_master<bool> itf;
  uint32_t flags;
  int resumeack;
  int available;
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
  vp::io_req_status_e flags_req(int reg_offset, int size, bool is_write, uint8_t *data, vp::io_req *req);
  vp::io_req_status_e program_buffer_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e data0_req(int reg_offset, int size, bool is_write, uint8_t *data);
  static void hart_available_sync(void *__this, bool value, int id);
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
  void dm_abstractcs_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void dm_dmcontrol_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void dm_dmstatus_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void dm_sbdata0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void dm_command_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void dm_dmcs2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void handle_dmi_access();
  void update_dmstatus();


  int get_hartsel();

  vp::trace     trace;
  vp::trace     debug;
  vp::jtag_slave  jtag_slave_itf;
  vp::jtag_master jtag_master_itf;
  std::unordered_map<int, Dtm_slave *> slaves_map;
  std::vector<Dtm_slave *> slaves;
  vp::io_slave core_io_itf;
  vp::io_master io_itf;
  std::vector<vp::wire_slave<bool>> hart_available_itf;

  int nb_harts;

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

  uint32_t dtm_ctrl;

  vp::io_req req;
  uint32_t io_req_data;

  vp_regmap_riscv_dbg_dm dm_regmap;


  int tclk;

  uint32_t abstract_cmd[32];
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
  this->dtm_ctrl = DTM_CTRL_RESET_VAL;
  this->ir_shift = 1; // Instruction register should be 1 at reset according to JTAG specs
}

void riscv_dtm::reset(bool value)
{
  if (value)
  {
    this->tap_reset();
    // TODO should set flags to 0 in each hart and unblock pending requests
    //this->flags = 0;
  }
}


int riscv_dtm::get_hartsel()
{
    return (this->dm_regmap.dmcontrol.hartsello_get() << 0) | (this->dm_regmap.dmcontrol.hartselhi_get() << 10);
}


void riscv_dtm::update_dmstatus()
{
    int hart = this->get_hartsel();
    Dtm_slave *slave = slaves_map[hart];
    int halted = 0;
    int running = 0;
    int resumeack = 0;
    int available = 0;

    if (slave)
    {
        halted = slave->halted;
        running = !slave->halted;
        resumeack = slave->resumeack;
        available = slave->available;
    }

    this->dm_regmap.dmstatus.anyhalted_set(halted);
    this->dm_regmap.dmstatus.allhalted_set(halted);
    this->dm_regmap.dmstatus.anyrunning_set(running);
    this->dm_regmap.dmstatus.allrunning_set(running);
    this->dm_regmap.dmstatus.anyresumeack_set(resumeack);
    this->dm_regmap.dmstatus.allresumeack_set(resumeack);
    this->dm_regmap.dmstatus.anyunavail_set(!available);
    this->dm_regmap.dmstatus.allunavail_set(!available);
}


void riscv_dtm::dm_abstractcs_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->dm_regmap.abstractcs.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        // Check if we have to clear cmderr
        riscv_dbg_dm_abstractcs_t reg = { .raw=0 };
        memcpy(((uint8_t *)&reg.raw) + reg_offset, value, size);
        this->dm_regmap.abstractcs.cmderr_set(this->dm_regmap.abstractcs.cmderr_get() & ~reg.cmderr);
    }
}


void Dtm_slave::set_flags(int value)
{
    this->flags = value;
    if (this->flags != 0 && this->pending_req)
    {
        vp::io_req *req = this->pending_req;
        this->pending_req = NULL;
        *(req->get_data()) = this->flags;
        req->get_resp_port()->resp(req);
    }

}


void riscv_dtm::dm_dmcontrol_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->dm_regmap.dmcontrol.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        int hart = this->get_hartsel();
        Dtm_slave *slave = slaves_map[hart];


        uint32_t data = this->dm_regmap.dmcontrol.get_32();
        if (data >> 31)
        {
          this->trace.msg(vp::trace::LEVEL_DEBUG, "Halting core (hart: 0x%x)\n", hart);

          if (slave == NULL)
          {
            this->trace.force_warning("Trying to halt unknown core (hart id: %d)\n", hart);
          }
          else
          {
            slave->itf.sync(true);
          }
        }
        else if (data >> 30)
        {
          this->trace.msg(vp::trace::LEVEL_DEBUG, "Resuming core (hart: 0x%x)\n", hart);

          slave->set_flags(slave->flags | (1<<1));
        }

        // Any write to resumereq should clear resumeack
        if (slave)
        {
            slave->resumeack = 0;
        }

        this->dm_regmap.dmcontrol.hartselhi_set(0);
        this->dm_regmap.dmcontrol.hartsello_set(this->dm_regmap.dmcontrol.hartsello_get() & (this->nb_harts - 1));
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

#define FLOAT_LOAD(size, dest, base, offset) ( \
  ((((offset) >> 0) & 0xfff) << 20) | \
  ((((base  ) >> 0) &  0x1f) << 15) | \
  ((((size  ) >> 0) &   0x7) << 12) | \
  ((((dest  ) >> 0) &  0x1f) << 7 ) | \
  ((((0x03  ) >> 0) &  0x7f) << 0 ))

#define STORE(size, src, base, offset) ( \
  ((((offset) >> 0) &  0x1f) <<  7) | \
  ((((offset) >> 5) &  0x7f) << 25) | \
  ((((base  ) >> 0) &  0x1f) << 15) | \
  ((((size  ) >> 0) &   0x7) << 12) | \
  ((((src   ) >> 0) &  0x1f) << 20 ) | \
  ((((0x23  ) >> 0) &  0x7f) << 0 ))

#define FLOAT_STORE(size, src, base, offset) ( \
  ((((offset) >> 0) &  0x1f) <<  7) | \
  ((((offset) >> 5) &  0x7f) << 25) | \
  ((((base  ) >> 0) &  0x1f) << 15) | \
  ((((size  ) >> 0) &   0x7) << 12) | \
  ((((src   ) >> 0) &  0x1f) << 20 ) | \
  ((((0x23  ) >> 0) &  0x7f) << 0 ))

#define NOP() 0x00000013

#define ILLEGAL() 0x00000000

#define EBREAK() 0x00100073

#define CSR_DSCRATCH0 0x7b2
#define CSR_DSCRATCH1 0x7b3
#define DATA_ADDR     0x380





typedef union {
  struct {
    unsigned int regno:16;
    unsigned int write:1;
    unsigned int transfer:1;
    unsigned int postexec:1;
    unsigned int aarpostincrement:1;
    unsigned int aarsize:3;
    unsigned int reserved2:1;
    unsigned int cmdtype:8;
  };
  uint32_t raw;
} dm_control_reg_t;


void riscv_dtm::dm_dmcs2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->dm_regmap.dmcs2.update(reg_offset, size, value, is_write);
}

void riscv_dtm::dm_command_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->dm_regmap.command.update(reg_offset, size, value, is_write);

    int cmd = this->dm_regmap.command.cmdtype_get();
    bool unsupported_command = false;
 
    dm_control_reg_t reg = { .raw = this->dm_regmap.command.get_32() };

    if (cmd == 0)
    {
        int aarsize = reg.aarsize;
        int is_write = reg.write;
        int regno = reg.regno;
        int transfer = reg.transfer;
        int aarpostincrement = reg.aarpostincrement;
        int postexec = reg.postexec;
        int max_aarsize = 3;
        bool has_second_scratch = true;
        int dm_base_address = 1;
        int load_base_addr = (dm_base_address == 0) ? 0 : 10;

        // default memory
        // if transfer is not set then we can take a shortcut to the program buffer
        this->abstract_cmd[0]  = ILLEGAL();
        // load debug module base address into a0, this is shared among all commands
        this->abstract_cmd[1] = has_second_scratch ? AUIPC(10, 0) : NOP();
        this->abstract_cmd[2] = has_second_scratch ? SRLI(10, 10, 12) : NOP(); // clr lowest 12b -> DM base offset
        this->abstract_cmd[3] = has_second_scratch ? SLLI(10, 10, 12) : NOP();
        this->abstract_cmd[4] = NOP();
        this->abstract_cmd[5] = NOP();
        this->abstract_cmd[6] = NOP();
        this->abstract_cmd[7] = NOP();
        this->abstract_cmd[8] = has_second_scratch ? CSRW(CSR_DSCRATCH1, 10) : NOP();
        this->abstract_cmd[9] = EBREAK();

        this->dm_regmap.command.trace.msg(vp::trace::LEVEL_DEBUG, "Abstract command (cmdtype: %d, aarsize: %d, postexec: %d, transfer: %d, write: %d, regno: 0x%x)\n", reg.cmdtype, reg.aarsize, reg.postexec, reg.transfer, reg.write, reg.regno);

        if (aarsize < max_aarsize)
        {
            if (is_write && transfer)
            {
                // store a0 in dscratch1
                this->abstract_cmd[0] = has_second_scratch ? CSRW(CSR_DSCRATCH1, 10) : NOP();

                // this range is reserved
                if ((regno & (0x3 << 14)) != 0)
                {
                    this->abstract_cmd[0] = EBREAK(); // we leave asap
                    unsupported_command = 1;
                }
                // A0 access needs to be handled separately, as we use A0 to load
                // the DM address offset need to access DSCRATCH1 in this case
                else if (has_second_scratch && ((regno >> 12) & 1) && (((regno >> 5) & 1) == 0) && (((regno >> 0) & 0x1f) == 10))
                {
                    // store s0 in dscratch
                    this->abstract_cmd[4]  = CSRW(CSR_DSCRATCH0, 8);
                    // load from data register
                    this->abstract_cmd[5] = LOAD(aarsize, 8, load_base_addr, DATA_ADDR);
                    // and store it in the corresponding CSR
                    this->abstract_cmd[6] = CSRW(CSR_DSCRATCH1, 8);
                    // restore s0 again from dscratch
                    this->abstract_cmd[7] = CSRR(CSR_DSCRATCH0, 8);
                }
                // GPR/FPR access
                else if ((regno >> 12) & 1)
                {
                  // determine whether we want to access the floating point register or not
                  if ((regno >> 5) & 1)
                  {
                      this->abstract_cmd[4] = FLOAT_LOAD(aarsize, (regno >> 0) & 0x1f, load_base_addr, DATA_ADDR);
                  }
                  else
                  {
                      this->abstract_cmd[4] = LOAD(aarsize, (regno >> 0) & 0x1f, load_base_addr, DATA_ADDR);
                  }
                }
                // CSR access
                else
                {
                  // data register to CSR
                  // store s0 in dscratch
                  this->abstract_cmd[4]  = CSRW(CSR_DSCRATCH0, 8);
                  // load from data register
                  this->abstract_cmd[5] = LOAD(aarsize, 8, load_base_addr, DATA_ADDR);
                  // and store it in the corresponding CSR
                  this->abstract_cmd[6]  = CSRW((regno >> 0) & 0xfff, 8);
                  // restore s0 again from dscratch
                  this->abstract_cmd[7] = CSRR(CSR_DSCRATCH0, 8);
                }
            }
            else if (!is_write && transfer)
            {
                // store a0 in dscratch1
                this->abstract_cmd[0]  = has_second_scratch ? CSRW(CSR_DSCRATCH1, load_base_addr) : NOP();

                // this range is reserved
                if ((regno & (0x3 << 14)) != 0)
                {
                    this->abstract_cmd[0] = EBREAK(); // we leave asap
                    unsupported_command = 1;
                }
                // A0 access needs to be handled separately, as we use A0 to load
                // the DM address offset need to access DSCRATCH1 in this case
                else if (has_second_scratch && ((regno >> 12) & 1) && (((regno >> 5) & 1) == 0) && (((regno >> 0) & 0x1f) == 10))
                {
                    // store s0 in dscratch
                    this->abstract_cmd[4] = CSRW(CSR_DSCRATCH0, 8);
                    // read value from CSR into s0
                    this->abstract_cmd[5] = CSRR(CSR_DSCRATCH1, 8);
                    // and store s0 into data section
                    this->abstract_cmd[6] = STORE(aarsize, 8, load_base_addr, DATA_ADDR);
                    // restore s0 again from dscratch
                    this->abstract_cmd[7] = CSRR(CSR_DSCRATCH0, 8);
                }
                // GPR/FPR access
                // GPR/FPR access
                else if ((regno >> 12) & 1)
                {
                  // determine whether we want to access the floating point register or not
                  if ((regno >> 5) & 1)
                  {
                      this->abstract_cmd[4] = FLOAT_STORE(aarsize, (regno >> 0) & 0x1f, load_base_addr, DATA_ADDR);
                  }
                  else
                  {
                      this->abstract_cmd[4] = STORE(aarsize, (regno >> 0) & 0x1f, load_base_addr, DATA_ADDR);
                  }
                }
                // CSR access
                // CSR access
                else
                {
                    // CSR register to data
                    // store s0 in dscratch
                    this->abstract_cmd[4]  = CSRW(CSR_DSCRATCH0, 8);
                    // read value from CSR into s0
                    this->abstract_cmd[5] = CSRR((regno >> 0) & 0xfff, 8);
                    // and store s0 into data section
                    this->abstract_cmd[6] = STORE(aarsize, 8, load_base_addr, DATA_ADDR);
                    // restore s0 again from dscratch
                    this->abstract_cmd[7] = CSRR(CSR_DSCRATCH0, 8);
                }
            }
        }
        else if (aarsize >= max_aarsize || aarpostincrement)
        {
            // this should happend when e.g. aarsize >= max_aarsize
            // Openocd will try to do an access with aarsize=64 bits
            // first before falling back to 32 bits.
            this->abstract_cmd[0] = EBREAK(); // we leave asap
            unsupported_command = 1;
        }

        // Check whether we need to execute the program buffer. When we
        // get an unsupported command we really should abort instead of
        // still trying to execute the program buffer, makes it easier
        // for the debugger to recover
        if (postexec && !unsupported_command)
        {
            // issue a nop, we will automatically run into the program buffer
            abstract_cmd[8] = NOP();
        }
    }
    else
    {
        this->get_trace()->fatal("Unsupported command: %d\n", cmd);

        // not supported at the moment
        // dm::QuickAccess:;
        // dm::AccessMemory:;
        this->abstract_cmd[0] = EBREAK();
        unsupported_command = 1;
    }

    if (unsupported_command)
    {
        this->dm_regmap.abstractcs.cmderr_set(2);
    }

    int hart = this->get_hartsel();
    Dtm_slave *slave = slaves_map[hart];
    slave->set_flags(slave->flags | 1);
}


void riscv_dtm::dm_dmstatus_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (!is_write)
      {
          this->update_dmstatus();
    }

    this->dm_regmap.dmstatus.update(reg_offset, size, value, is_write);
}


void riscv_dtm::dm_sbdata0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->dm_regmap.sbdata0.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "System bus write (address: 0x%x, data: 0x%x)\n", this->dm_regmap.sbaddress0.get_32(), this->dm_regmap.sbdata0.get_32());

        if (!this->dm_regmap.sbcs.sberror_get())
        {
            if (!this->dm_regmap.sbcs.sbbusy_get())
            {
                this->dm_regmap.sbcs.sbbusy_set(1);

                this->io_req_data = this->dm_regmap.sbdata0.get_32();

                this->req.init();
                this->req.set_data((uint8_t *)&this->io_req_data);
                this->req.set_addr(this->dm_regmap.sbaddress0.get_32());
                this->req.set_size(4);
                this->req.set_is_write(1);
                int err = this->io_itf.req(&this->req);
                if (err == vp::IO_REQ_INVALID)
                {
                    this->dm_regmap.sbcs.sbbusy_set(1);
                    this->dm_regmap.sbcs.sberror_set(1);
                }
                else
                {
                    if (this->dm_regmap.sbcs.sbautoincrement_get())
                    {
                        this->dm_regmap.sbaddress0.set_32(this->dm_regmap.sbaddress0.get_32() + 4);
                    }

                    if (err == vp::IO_REQ_OK)
                    {
                        if (this->req.get_full_latency())
                        {
                            this->get_trace()->fatal("Latency is not yet supported\n");
                        }
                        else
                        {
                            this->dm_regmap.sbcs.sbbusy_set(0);
                        }
                    }
                    else
                    {
                        this->get_trace()->fatal("Pending requests are not yet supported\n");
                    }
                }
            }
            else
            {
                this->dm_regmap.sbcs.sbbusyerror_set(1);
            }
        }
    }
    else
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "System bus read (address: 0x%x, data: 0x%x)\n", this->dm_regmap.sbaddress0.get_32(), this->dm_regmap.sbdata0.get_32());

        if (!this->dm_regmap.sbcs.sberror_get())
        {
            if (!this->dm_regmap.sbcs.sbbusy_get())
            {
                this->dm_regmap.sbcs.sbbusy_set(1);

                this->req.init();
                this->req.set_data((uint8_t *)&this->io_req_data);
                this->req.set_addr(this->dm_regmap.sbaddress0.get_32());
                this->req.set_size(4);
                this->req.set_is_write(0);
                int err = this->io_itf.req(&this->req);
                if (err == vp::IO_REQ_INVALID)
                {
                    this->dm_regmap.sbcs.sbbusy_set(1);
                    this->dm_regmap.sbcs.sberror_set(1);
                }
                else
                {
                    if (this->dm_regmap.sbcs.sbautoincrement_get())
                    {
                        this->dm_regmap.sbaddress0.set_32(this->dm_regmap.sbaddress0.get_32() + 4);
                    }

                    if (err == vp::IO_REQ_OK)
                    {
                        if (this->req.get_full_latency())
                        {
                            this->get_trace()->fatal("Latency is not yet supported\n");
                        }
                        else
                        {
                            //printf("READ %x at %x\n", this->io_req_data, this->dm_regmap.sbaddress0.get_32());
                            this->dm_regmap.sbdata0.set_32(this->io_req_data);
                            this->dm_regmap.sbcs.sbbusy_set(0);
                        }
                    }
                    else
                    {
                        this->get_trace()->fatal("Pending requests are not yet supported\n");
                    }
                }
            }
            else
            {
                this->dm_regmap.sbcs.sbbusyerror_set(1);
            }
        }
    }
}




void riscv_dtm::handle_dmi_access()
{
    switch (this->dmi_addr)
    {
        default:

            if (this->dmi_op == 2 || this->dmi_op == 1)
            {
                this->dm_regmap.access(this->dmi_addr, 4, (uint8_t *)&this->dmi_data, this->dmi_op == 2);
            }
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

        this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting DMI operation (raw: %llx, op: %d, data: 0x%8.8x, addr: 0x%x)\n", this->tdi, this->dmi_op, this->dmi_data, this->dmi_addr);

        this->handle_dmi_access();
    }
    else if (tclk && this->instr == DTM_CTRL_INSTR)
    {
        this->trace.msg(vp::trace::LEVEL_DEBUG, "Writing DTM Control and Status (raw: %llx)\n", this->tdi);

    }
}



void riscv_dtm::capture_dr(int tdi, int tclk) 
{
  if (this->instr == IDCODE_INSTR)
  {
    this->tdi = this->idcode;
  }
  else if (tclk && this->instr == DMI_INSTR)
  {
    this->tdi = this->dmi_data << 2;
  }
  else if (tclk && this->instr == DTM_CTRL_INSTR)
  {
    this->tdi = this->dtm_ctrl;
  }
}



void riscv_dtm::shift_ir(int tdi, int tclk)
{
  if (!tclk)
  {
    this->tdo = this->ir_shift & 1;
    //printf("WRITE TDO %d from ir %x\n", this->tdo, this->ir_shift);
  }

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

    this->trace.msg(vp::trace::LEVEL_DEBUG, "Updated new instruction (ir: 0x%x / %s)\n", this->instr, get_instr_name(this->instr).c_str());

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

      case DTM_CTRL_INSTR:
        this->dr_len = DTM_CTRL_DR_LEN;
        break;
    }
  }
}



void riscv_dtm::shift_dr(int tdi, int tclk)
{
  if (!tclk)
  {
    this->tdo = this->tdi & 1;
    //printf("WRITE TDO %d from %lx\n", this->tdo, this->tdi);
  }

  if (tclk)
  {
    this->tdi = (this->tdi >> 1) | (((uint64_t)tdi) << (this->dr_len - 1));
    //printf("SHIFT TDI\n");
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
    this->trace.msg(vp::trace::LEVEL_TRACE, "Changing TAP state (newState: %s)\n", tapStateName[this->state].c_str());
  }

  this->tclk = tclk;
}

void riscv_dtm::tck_edge(int tck, int tdi, int tms, int trst)
{
  this->trace.msg(vp::trace::LEVEL_TRACE, "TCK edge (tms: %1d, tdi: %1d, trst: %1d, tck: %d)\n", tms, tdi, trst, tck);

  if (!this->tclk && tck)
  {
  }
  else if (this->tclk && !tck)
  {
  }

  this->tap_update(tdi, tms, tck);

  //printf("MASTER SYNC tdo %d\n", this->tdo);
  this->jtag_master_itf.sync(tck, this->tdo, tms, trst);

}

void riscv_dtm::hart_available_sync(void *__this, bool value, int id)
{
    riscv_dtm *_this = (riscv_dtm *)__this;

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Changing hart status (hartid: %d, available: %d)\n", id, value);

    Dtm_slave *hart = _this->slaves_map[id];
    if (hart)
    {
        hart->available = value;
    }
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
  {
    int hart = this->get_hartsel();
    Dtm_slave *slave = slaves_map[hart];
    slave->set_flags(slave->flags & ~(1<<0));
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::resume_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
    if (size != 4)
        return vp::IO_REQ_INVALID;

    if (is_write)
    {
        int hart_id = *(uint32_t *)data;
        Dtm_slave *slave = slaves_map[hart_id];

        slave->set_flags(slave->flags & ~(1<<1));

        if (slave == NULL)
        {
          this->trace.force_warning("Received invalid hart id (hart id: %d)\n", hart_id);
          return vp::IO_REQ_INVALID;
        }

        slave->halted = 0;
        slave->resumeack = 1;
    }

    return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::halted_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (size != 4)
    return vp::IO_REQ_INVALID;

  int hart_id = *(uint32_t *)data;

  this->trace.msg(vp::trace::LEVEL_DEBUG, "Halted req (hart id: 0x%x)\n", hart_id);

  Dtm_slave *slave = this->slaves_map[hart_id];
  if (slave == NULL)
  {
    this->trace.force_warning("Received invalid hart id (hart id: %d)\n", hart_id);
    return vp::IO_REQ_INVALID;
  }


  slave->halted = 1;

  return vp::IO_REQ_OK;
}



vp::io_req_status_e riscv_dtm::data0_req(int offset, int size, bool is_write, uint8_t *data)
{
    if (size != 4)
        return vp::IO_REQ_INVALID;

    if (!is_write)
    {
        *(uint32_t *)data = this->dm_regmap.data0.data_get();
    }
    else
    {
        this->dm_regmap.data0.data_set(*(uint32_t *)data);
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



vp::io_req_status_e riscv_dtm::flags_req(int reg_offset, int size, bool is_write, uint8_t *data, vp::io_req *req)
{
  if (size != 1)
    return vp::IO_REQ_INVALID;

  int hart_id = reg_offset;

  this->trace.msg(vp::trace::LEVEL_DEBUG, "Flags req (hart id: 0x%x)\n", hart_id);

  Dtm_slave *slave = this->slaves_map[hart_id];
  if (slave == NULL)
  {
    this->trace.force_warning("Received invalid hart id (hart id: %d)\n", hart_id);
    return vp::IO_REQ_INVALID;
  }

  int active_hart = this->dm_regmap.dmcontrol.hartsello_get() | ( this->dm_regmap.dmcontrol.hartselhi_get() << 10);

  if (active_hart == hart_id)
  {
    *data = slave->flags;
  }
  else
  {
    *data = 0;
  }

  if (*data == 0)
  {
    slave->pending_req = req;
    return vp::IO_REQ_PENDING;
  }
  else
  {
    return vp::IO_REQ_OK;
  }
}



vp::io_req_status_e riscv_dtm::core_req(void *__this, vp::io_req *req)
{
  riscv_dtm *_this = (riscv_dtm *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();
  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  _this->trace.msg(vp::trace::LEVEL_DEBUG, "IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

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
        err = _this->flags_req(offset - 0x400, size, is_write, data, req);
        break;
      }
  }



    // TODO we should be able to report an error in asynchronous requests

end:
  if (err == vp::IO_REQ_INVALID)
    _this->get_trace()->force_warning("RISCV DTM invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return err;
}


int riscv_dtm::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    core_io_itf.set_req_meth(&riscv_dtm::core_req);
    new_slave_port("input", &core_io_itf);

    new_master_port("io", &io_itf);

    jtag_slave_itf.set_sync_meth(&riscv_dtm::sync);
    jtag_slave_itf.set_sync_cycle_meth(&riscv_dtm::sync_cycle);
    new_slave_port("jtag_in", &jtag_slave_itf);

    new_master_port("jtag_out", &jtag_master_itf);

    this->dm_regmap.build(this, &this->trace, "dm");
    this->dm_regmap.abstractcs.register_callback(std::bind(&riscv_dtm::dm_abstractcs_req, this, _1, _2, _3, _4));
    this->dm_regmap.dmcontrol.register_callback(std::bind(&riscv_dtm::dm_dmcontrol_req, this, _1, _2, _3, _4));
    this->dm_regmap.dmstatus.register_callback(std::bind(&riscv_dtm::dm_dmstatus_req, this, _1, _2, _3, _4));
    this->dm_regmap.sbdata0.register_callback(std::bind(&riscv_dtm::dm_sbdata0_req, this, _1, _2, _3, _4));
    this->dm_regmap.command.register_callback(std::bind(&riscv_dtm::dm_command_req, this, _1, _2, _3, _4));
    this->dm_regmap.dmcs2.register_callback(std::bind(&riscv_dtm::dm_dmcs2_req, this, _1, _2, _3, _4));

    this->nb_harts = this->get_js_config()->get_int("nb_harts");
    this->idcode = this->get_js_config()->get_int("idcode");

    this->tclk = 0;

    this->hart_available_itf.resize(this->nb_harts);
    for (int i=0; i<this->nb_harts; i++)
    {
        this->hart_available_itf[i].set_sync_meth_muxed(&riscv_dtm::hart_available_sync, i);
        this->new_slave_port("hart_available_" + std::to_string(i), &this->hart_available_itf[i]);
    }

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



