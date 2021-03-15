/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "iss.hpp"


#if defined(ISS_HAS_PERF_COUNTERS)
void check_perf_config_change(iss_t *iss, unsigned int pcer, unsigned int pcmr);
#endif

/*
 *   USER CSRS
 */

static bool ustatus_read(iss_t *iss, iss_reg_t *value) {
  *value = 0;
  return false;
}

static bool ustatus_write(iss_t *iss, unsigned int value) {
  return false;
}



static bool uie_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: uie\n");
  return false;
}

static bool uie_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: uie\n");
  return false;
}




static bool utvec_read(iss_t *iss, iss_reg_t *value) {
  *value = 0;
  return false;
}

static bool utvec_write(iss_t *iss, unsigned int value) {
  return false;
}



static bool uscratch_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: uscratch\n");
  return false;
}

static bool uscratch_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: uscratch\n");
  return false;
}



static bool uepc_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: uepc\n");
  return false;
}

static bool uepc_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: uepc\n");
  return false;
}



static bool ucause_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: ucause\n");
  return false;
}

static bool ucause_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: ucause\n");
  return false;
}



static bool ubadaddr_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: ubadaddr\n");
  return false;
}

static bool ubadaddr_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: ubadaddr\n");
  return false;
}



static bool uip_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR READ: uip\n");
  return false;
}

static bool uip_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR WRITE: uip\n");
  return false;
}



static bool fflags_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.state.fcsr.fflags.raw;
  return false;
}

static bool fflags_write(iss_t *iss, unsigned int value) {
  iss->cpu.state.fcsr.fflags.raw = value;
  return false;
}



static bool frm_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.state.fcsr.frm;
  return false;
}

static bool frm_write(iss_t *iss, unsigned int value) {
  iss->cpu.state.fcsr.frm = value;
  return false;
}



static bool fcsr_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.state.fcsr.raw;
  return false;
}

static bool fcsr_write(iss_t *iss, unsigned int value) {
  iss->cpu.state.fcsr.raw = value & 0xff;
  return false;
}


static bool fprec_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.state.fprec;
  return false;
}

static bool fprec_write(iss_t *iss, unsigned int value) {
  iss->cpu.state.fprec = value & 0x1f;
  return false;
}



static bool cycle_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: cycle\n");
  return false;
}

static bool time_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: time\n");
  return false;
}

static bool instret_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: instret\n");
  return false;
}

static bool hpmcounter_read(iss_t *iss, int id, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hpmcounter\n");
  return false;
}

static bool cycleh_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: cycleh\n");
  return false;
}

static bool timeh_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: timeh\n");
  return false;
}

static bool instreth_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: instreth\n");
  return false;
}

static bool hpmcounterh_read(iss_t *iss, int id, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hpmcounterh\n");
  return false;
}







/*
 *   SUPERVISOR CSRS
 */

static bool sstatus_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->status & 0x133;
  return false;
}

static bool sstatus_write(iss_t *iss, unsigned int value) {
  //iss->status = (iss->status & ~0x133) | (value & 0x133);
  //checkInterrupts(iss, 1);
  return false;
}



static bool sedeleg_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: sedeleg\n");
  return false;
}

static bool sedeleg_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: sedeleg\n");
  return false;
}



static bool sideleg_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: sideleg\n");
  return false;
}

static bool sideleg_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: sideleg\n");
  return false;
}




static bool sie_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->ie[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool sie_write(iss_t *iss, unsigned int value) {
  //iss->ie[GVSIM_MODE_SUPERVISOR] = value;
  //checkInterrupts(iss, 1);
  return false;
}



static bool stvec_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->tvec[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool stvec_write(iss_t *iss, unsigned int value) {
  //iss->tvec[GVSIM_MODE_SUPERVISOR] = value;
  return false;
}



static bool sscratch_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->scratch[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool sscratch_write(iss_t *iss, unsigned int value) {
  //iss->scratch[GVSIM_MODE_SUPERVISOR] = value;
  return false;
}



static bool sepc_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->epc[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool sepc_write(iss_t *iss, unsigned int value) {
  //iss->epc[GVSIM_MODE_SUPERVISOR] = value;
  return false;
}



static bool scause_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->cause[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool scause_write(iss_t *iss, unsigned int value) {
  //iss->cause[GVSIM_MODE_SUPERVISOR] = value;
  return false;
}



static bool sbadaddr_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->badaddr[GVSIM_MODE_SUPERVISOR];
  return false;
}

static bool sbadaddr_write(iss_t *iss, unsigned int value) {
  //iss->badaddr[GVSIM_MODE_SUPERVISOR] = value;
  return false;
}



static bool sip_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: sip\n");
  return false;
}

static bool sip_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: sip\n");
  return false;
}



static bool sptbr_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->sptbr;
  return false;
}

static bool sptbr_write(iss_t *iss, unsigned int value) {
  //iss->sptbr = value;
  //sim_setPgtab(iss, value);
  return false;
}





/*
 *   HYPERVISOR CSRS
 */

static bool hstatus_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hstatus\n");
  return false;
}

static bool hstatus_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hstatus\n");
  return false;
}



static bool hedeleg_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hedeleg\n");
  return false;
}

static bool hedeleg_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hedeleg\n");
  return false;
}



static bool hideleg_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hideleg\n");
  return false;
}

static bool hideleg_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hideleg\n");
  return false;
}



static bool hie_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hie\n");
  return false;
}

static bool hie_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hie\n");
  return false;
}



static bool htvec_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: htvec\n");
  return false;
}

static bool htvec_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: htvec\n");
  return false;
}



static bool hscratch_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hscratch\n");
  return false;
}

static bool hscratch_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hscratch\n");
  return false;
}



static bool hepc_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hepc\n");
  return false;
}

static bool hepc_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hepc\n");
  return false;
}



static bool hcause_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hcause\n");
  return false;
}

static bool hcause_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hcause\n");
  return false;
}



static bool hbadaddr_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: hbadaddr\n");
  return false;
}

static bool hbadaddr_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: hbadaddr\n");
  return false;
}



/*
 *   MACHINE CSRS
 */

static bool misa_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.config.misa;
  return false;
}

static bool misa_write(iss_t *iss, unsigned int value) {
  return false;
}

static bool mvendorid_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mvendorid\n");
  return false;
}

static bool marchid_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: marchid\n");
  return false;
}

static bool mimpid_read(iss_t *iss, iss_reg_t *value) {
  *value = 0;
  return false;
}

static bool mhartid_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.config.mhartid;
  return false;
}


static bool mstatus_read(iss_t *iss, iss_reg_t *value) {
  *value = (iss->cpu.csr.status & ~(1<<3)) | (iss->cpu.irq.irq_enable << 3) | (iss->cpu.irq.saved_irq_enable << 7);
  return false;
}

static bool mstatus_write(iss_t *iss, unsigned int value)
{
#if defined(SECURE)
  unsigned int mask = 0x21899;
  unsigned int or_mask = 0x0;
#else
  unsigned int mask = 0x88;
  unsigned int or_mask = 0x1800;
#endif
  iss->cpu.csr.status = (value & mask) | or_mask;
  iss_irq_enable(iss, (value >> 3) & 1);
  iss->cpu.irq.saved_irq_enable = (value >> 7) & 1;
  return false;
}



static bool medeleg_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->edeleg[GVSIM_MODE_MACHINE];
  return false;
}

static bool medeleg_write(iss_t *iss, unsigned int value) {
  //iss->edeleg[GVSIM_MODE_MACHINE] = value;
  return false;
}



static bool mideleg_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->ideleg[GVSIM_MODE_MACHINE];
  return false;
}

static bool mideleg_write(iss_t *iss, unsigned int value) {
  //iss->ideleg[GVSIM_MODE_MACHINE] = value;
  //checkInterrupts(iss, 1);
  return false;
}



static bool mie_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->ie[GVSIM_MODE_MACHINE];
  return false;
}

static bool mie_write(iss_t *iss, unsigned int value) {
  //iss->ie[GVSIM_MODE_MACHINE] = value;
  //checkInterrupts(iss, 1);
  return false;
}



static bool mtvec_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->tvec[GVSIM_MODE_MACHINE];
  *value = iss->cpu.csr.mtvec;
  return false;
}

static bool mtvec_write(iss_t *iss, unsigned int value) {
  iss->cpu.csr.mtvec = value;
  //iss->tvec[GVSIM_MODE_MACHINE] = value;
  iss_irq_set_vector_table(iss, value);
  return false;
}



static bool mscratch_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.mscratch;
  return false;
}

static bool mscratch_write(iss_t *iss, unsigned int value) {
  iss->cpu.csr.mscratch = value;
  return false;
}


static bool mepc_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.epc;
  return false;
}

static bool mepc_write(iss_t *iss, unsigned int value) {
  iss_msg(iss, "Setting MEPC (value: 0x%x)\n", value);
  iss->cpu.csr.epc = value & ~1;
  return false;
}



static bool mcause_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.mcause;
  return false;
}

static bool mcause_write(iss_t *iss, unsigned int value) {
  iss->cpu.csr.mcause = value;
  return false;
}



static bool mbadaddr_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->badaddr[GVSIM_MODE_MACHINE];
  return false;
}

static bool mbadaddr_write(iss_t *iss, unsigned int value) {
 // iss->badaddr[GVSIM_MODE_MACHINE] = value;
  return false;
}



static bool mip_read(iss_t *iss, iss_reg_t *value) {
  //*value = iss->ip[GVSIM_MODE_MACHINE];
  return false;
}

static bool mip_write(iss_t *iss, unsigned int value) {
  //iss->ip[GVSIM_MODE_MACHINE] = value;
  //checkInterrupts(iss, 1);
  return false;
}



static bool mbase_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mbase\n");
  return false;
}

static bool mbase_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mbase\n");
  return false;
}



static bool mbound_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mbound\n");
  return false;
}

static bool mbound_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mbound\n");
  return false;
}



static bool mibase_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mibase\n");
  return false;
}

static bool mibase_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mibase\n");
  return false;
}



static bool mibound_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mibound\n");
  return false;
}

static bool mibound_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mibound\n");
  return false;
}



static bool mdbase_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mdbase\n");
  return false;
}

static bool mdbase_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mdbase\n");
  return false;
}



static bool mdbound_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mdbound\n");
  return false;
}

static bool mdbound_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mdbound\n");
  return false;
}



static bool mcycle_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mcycle\n");
  return false;
}

static bool mcycle_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mcycle\n");
  return false;
}

static bool minstret_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: minstret\n");
  return false;
}

static bool minstret_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: minstret\n");
  return false;
}

static bool mhpmcounter_read(iss_t *iss, int id, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmcounter\n");
  return false;
}

static bool mhpmcounter_write(iss_t *iss, int id, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmcounter\n");
  return false;
}

static bool mcycleh_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mcycleh\n");
  return false;
}

static bool mcycleh_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mcycleh\n");
  return false;
}

static bool minstreth_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: \n");
  return false;
}

static bool minstreth_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: \n");
  return false;
}

static bool mhpmcounterh_read(iss_t *iss, int id, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmcounterh\n");
  return false;
}

static bool mhpmcounterh_write(iss_t *iss, int id, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmcounterh\n");
  return false;
}



static bool mucounteren_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mucounteren\n");
  return false;
}

static bool mucounteren_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mucounteren\n");
  return false;
}



static bool mscounteren_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mscounteren\n");
  return false;
}

static bool mscounteren_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mscounteren\n");
  return false;
}



static bool mhcounteren_read(iss_t *iss, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mhcounteren\n");
  return false;
}

static bool mhcounteren_write(iss_t *iss, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mhcounteren\n");
  return false;
}



static bool mhpmevent_read(iss_t *iss, int id, iss_reg_t *value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmevent\n");
  return false;
}

static bool mhpmevent_write(iss_t *iss, int id, unsigned int value) {
  printf("WARNING UNIMPLEMENTED CSR: mhpmevent\n");
  return false;
}





/*
 *   PULP CSRS
 */

static bool stack_conf_write(iss_t *iss, unsigned int value)
{
  iss->cpu.csr.stack_conf = value;

  if (iss->cpu.csr.stack_conf)
    iss_csr_msg(iss, "Activating stack checking (start: 0x%x, end: 0x%x)\n", iss->cpu.csr.stack_start, iss->cpu.csr.stack_end);
  else
    iss_csr_msg(iss, "Deactivating stack checking\n");

  return false;
}

static bool stack_conf_read(iss_t *iss, unsigned int *value)
{
  *value = iss->cpu.csr.stack_conf;
  return false;
}

static bool stack_start_write(iss_t *iss, unsigned int value)
{
  iss->cpu.csr.stack_start = value;
  return false;
}

static bool stack_start_read(iss_t *iss, unsigned int *value)
{
  *value = iss->cpu.csr.stack_start;
  return false;
}

static bool stack_end_write(iss_t *iss, unsigned int value)
{
  iss->cpu.csr.stack_end = value;
  return false;
}

static bool stack_end_read(iss_t *iss, unsigned int *value)
{
  *value = iss->cpu.csr.stack_end;
  return false;
}

static bool umode_read(iss_t *iss, iss_reg_t *value) {
  *value = 3;
  //*value = iss->state.mode;
  return false;
}

static bool dcsr_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.dcsr;
  return false;
}

static bool dcsr_write(iss_t *iss, iss_reg_t value) {
  iss->cpu.csr.dcsr = value;
  iss->step_mode.set((value >> 2) & 1);
  return false;
}

static bool depc_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.depc;
  return false;
}

static bool depc_write(iss_t *iss, iss_reg_t value) {
  iss->cpu.csr.depc = value;
  return false;
}

static bool scratch0_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.scratch0;
  return false;
}

static bool scratch0_write(iss_t *iss, iss_reg_t value) {
  iss->cpu.csr.scratch0 = value;
  return false;
}

static bool scratch1_read(iss_t *iss, iss_reg_t *value) {
  *value = iss->cpu.csr.scratch1;
  return false;
}

static bool scratch1_write(iss_t *iss, iss_reg_t value) {
  iss->cpu.csr.scratch1 = value;
  return false;
}


static bool pcer_write(iss_t *iss, unsigned int prev_val, unsigned int value) {
  iss->cpu.csr.pcer = value;
  check_perf_config_change(iss, prev_val, iss->cpu.csr.pcmr);
  return false;
}

static bool pcmr_write(iss_t *iss, unsigned int prev_val, unsigned int value) {
  iss->cpu.csr.pcmr = value;

  check_perf_config_change(iss, iss->cpu.csr.pcer, prev_val);
  return false;
}

static bool hwloop_read(iss_t *iss, int reg, iss_reg_t *value) {
  *value = iss->cpu.pulpv2.hwloop_regs[reg];
  return false;
}

static bool hwloop_write(iss_t *iss, int reg, unsigned int value) {
  iss->cpu.pulpv2.hwloop_regs[reg] = value;
  return false;
}

#if defined(ISS_HAS_PERF_COUNTERS)

void update_external_pccr(iss_t *iss, int id, unsigned int pcer, unsigned int pcmr) {
  unsigned int incr = 0;
  // Only update if the counter is active as the external signal may report 
  // a different value whereas the counter must remain the same
  if (((pcer & CSR_PCER_EVENT_MASK(id)) && (pcmr & CSR_PCMR_ACTIVE)) || iss_pccr_trace_active(iss, id))
  {
    iss_csr_ext_counter_get(iss, id, &incr);
    iss->cpu.csr.pccr[id] += incr;
    iss_pccr_incr(iss, id, incr);
  }
  else {
    // Nothing to do if the counter is inactive, it will get reset so that
    // we get read events from now if it becomes active
  }

  // Reset the counter
  if (iss_csr_ext_counter_is_bound(iss, id))
    iss_csr_ext_counter_set(iss, id, 0);

  //if (cpu->traceEvent) sim_trace_event_incr(cpu, id, incr);
}

void check_perf_config_change(iss_t *iss, unsigned int pcer, unsigned int pcmr)
{
  // In case PCER or PCMR is modified, there is a special care about external signals as they
  // are still counting whatever the event active flag is. Reset them to start again from a
  // clean state
  {
    int i;
    // Check every external signal separatly
    for (i=CSR_PCER_NB_INTERNAL_EVENTS; i<CSR_PCER_NB_EVENTS; i++) {
      // This will update our internal counter in case it is active or just reset it
      update_external_pccr(iss, i, pcer, pcmr);
    }
  }
}

void flushExternalCounters(iss_t *iss)
{
  int i;
  for (i=CSR_PCER_NB_INTERNAL_EVENTS; i<CSR_PCER_NB_EVENTS; i++)
  {
    update_external_pccr(iss, i, iss->cpu.csr.pcer, iss->cpu.csr.pcmr);
  }
}

static bool perfCounters_read(iss_t *iss, int reg, iss_reg_t *value) {
  // In case of counters connected to external signals, we need to synchronize first
  if (reg >= CSR_PCCR(CSR_PCER_NB_INTERNAL_EVENTS) && reg < CSR_PCCR(CSR_NB_PCCR))
  {
    update_external_pccr(iss, reg - CSR_PCCR(0), iss->cpu.csr.pcer, iss->cpu.csr.pcmr);
    *value = iss->cpu.csr.pccr[reg - CSR_PCCR(0)];
    iss_perf_counter_msg(iss, "Reading PCCR (index: %d, value: 0x%x)\n", reg - CSR_PCCR(0), *value);
  }
  else if (reg == CSR_PCER)
  {
    *value = iss->cpu.csr.pcer;
  }
  else if (reg == CSR_PCMR)
  {
    *value = iss->cpu.csr.pcmr;
  }
  else
  {
    *value = iss->cpu.csr.pccr[reg - CSR_PCCR(0)];
    iss_perf_counter_msg(iss, "Reading PCCR (index: %d, value: 0x%x)\n", reg - CSR_PCCR(0), *value);
  }


  return false;
}

static bool perfCounters_write(iss_t *iss, int reg, unsigned int value)
{
  if (reg == CSR_PCER)
  {
    iss_perf_counter_msg(iss, "Setting PCER (value: 0x%x)\n", value);
    return pcer_write(iss, iss->cpu.csr.pcer, value);
  }
  else if (reg == CSR_PCMR)
  {
    iss_perf_counter_msg(iss, "Setting PCMR (value: 0x%x)\n", value);
    return pcmr_write(iss, iss->cpu.csr.pcmr, value);
  }
  // In case of counters connected to external signals, we need to synchronize the external one
  // with our
  else if (reg >= CSR_PCCR(CSR_PCER_NB_INTERNAL_EVENTS) && reg < CSR_PCCR(CSR_NB_PCCR))
  {
      // This will update out counter, which will be overwritten afterwards by the new value and
      // also set the external counter to 0 which makes sure they are synchroninez
    update_external_pccr(iss, reg - CSR_PCCR(0), iss->cpu.csr.pcer, iss->cpu.csr.pcmr);
  }
  else if (reg == CSR_PCCR(CSR_NB_PCCR)) 
  {
    iss_perf_counter_msg(iss, "Setting value to all PCCR (value: 0x%x)\n", value);

    int i;
    for (i=0; i<CSR_PCER_NB_EVENTS; i++)
    {
      iss->cpu.csr.pccr[i] = value;
      if (i >= CSR_PCER_NB_INTERNAL_EVENTS)
      {
        update_external_pccr(iss, i, 0, 0);
      }
    }  
  }
  else
  {
    iss_perf_counter_msg(iss, "Setting PCCR value (pccr: %d, value: 0x%x)\n", reg - CSR_PCCR(0), value);
    iss->cpu.csr.pccr[reg - CSR_PCCR(0)] = value;
  }
  return false;
}
#endif



static bool checkCsrAccess(iss_t *iss, int reg, bool isWrite) {
  //bool isRw = (reg >> 10) & 0x3;
  //bool priv = (reg >> 8) & 0x3;
  //if ((isWrite && !isRw) || (priv > iss->state.mode)) {
  //  triggerException_cause(iss, iss->currentPc, EXCEPTION_ILLEGAL_INSTR, ECAUSE_ILL_INSTR);
  //  return true;
  //}
  return false;
}

bool iss_csr_read(iss_t *iss, iss_reg_t reg, iss_reg_t *value)
{
  bool status;

  iss_csr_msg(iss, "Reading CSR (reg: 0x%x)\n", reg);

  #if 0
  // First check permissions
  if (checkCsrAccess(iss, reg, 0)) return true;


  if (!getOption(iss, __priv_pulp)) {
    if (reg >= 0xC03 && reg <= 0xC1F) return hpmcounter_read(iss, reg - 0xC03, value);
    if (reg >= 0xC83 && reg <= 0xC9F) return hpmcounterh_read(iss, reg - 0xC83, value);

    if (reg >= 0xB03 && reg <= 0xB1F) return mhpmcounter_read(iss, reg - 0xB03, value);
    if (reg >= 0xB83 && reg <= 0xB9F) return mhpmcounterh_read(iss, reg - 0xB83, value);

    if (reg >= 0x323 && reg <= 0x33F) return mhpmevent_read(iss, reg - 0x323, value);
  }
  #endif

  // And dispatch
  switch (reg) {

    // User trap setup
    case 0x000: status = ustatus_read   (iss, value); break;
    case 0x004: status = uie_read       (iss, value); break;
    case 0x005: status = utvec_read     (iss, value); break;

    // User trap handling
    case 0x040: status = uscratch_read  (iss, value); break;
    case 0x041: status = uepc_read      (iss, value); break;
    case 0x042: status = ucause_read    (iss, value); break;
    case 0x043: status = ubadaddr_read  (iss, value); break;
    case 0x044: status = uip_read       (iss, value); break;

    // User floating-point CSRs
    case 0x001: status = fflags_read    (iss, value); break;
    case 0x002: status = frm_read       (iss, value); break;
    case 0x003: status = fcsr_read      (iss, value); break;
    case 0x006: status = fprec_read     (iss, value); break;

    // User counter / timers
    case 0xC00: status = cycle_read     (iss, value); break;
    case 0xC01: status = time_read      (iss, value); break;
    case 0xC02: status = instret_read   (iss, value); break;
    case 0xC80: status = cycleh_read    (iss, value); break;
    case 0xC81: status = timeh_read     (iss, value); break;
    case 0xC82: status = instreth_read  (iss, value); break;




    // Supervisor trap setup
    case 0x100: status = sstatus_read   (iss, value); break;
    case 0x102: status = sedeleg_read   (iss, value); break;
    case 0x103: status = sideleg_read   (iss, value); break;
    case 0x104: status = sie_read       (iss, value); break;
    case 0x105: status = stvec_read     (iss, value); break;

    // Supervisor trap handling
    case 0x140: status = sscratch_read  (iss, value); break;
    case 0x141: status = sepc_read      (iss, value); break;
    case 0x142: status = scause_read    (iss, value); break;
    case 0x143: status = sbadaddr_read  (iss, value); break;
    case 0x144: status = sip_read       (iss, value); break;

    // Supervisor protection and translation
    case 0x180: status = sptbr_read     (iss, value); break;




    // Hypervisor trap setup
    case 0x200: status = hstatus_read   (iss, value); break;
    case 0x202: status = hedeleg_read   (iss, value); break;
    case 0x203: status = hideleg_read   (iss, value); break;
    case 0x204: status = hie_read       (iss, value); break;
    case 0x205: status = htvec_read     (iss, value); break;

    // Hypervisor trap handling
    case 0x240: status = hscratch_read  (iss, value); break;
    case 0x241: status = hepc_read      (iss, value); break;
    case 0x242: status = hcause_read    (iss, value); break;
    case 0x243: status = hbadaddr_read  (iss, value); break;

    // Hypervisor protection and translation




    // Machine information registers
    case 0xF11: status = mvendorid_read  (iss, value); break;
    case 0xF12: status = marchid_read    (iss, value); break;
    case 0xF13: status = mimpid_read     (iss, value); break;
    case 0xF14: status = mhartid_read    (iss, value); break;

    // Machine trap setup
    case 0x300: status = mstatus_read    (iss, value); break;
    case 0x301: status = misa_read       (iss, value); break;
    case 0x302: status = medeleg_read    (iss, value); break;
    case 0x303: status = mideleg_read    (iss, value); break;
    case 0x304: status = mie_read        (iss, value); break;
    case 0x305: status = mtvec_read      (iss, value); break;

    // Machine trap handling
    case 0x340: status = mscratch_read   (iss, value); break;
    case 0x341: status = mepc_read       (iss, value); break;
    case 0x342: status = mcause_read     (iss, value); break;
    case 0x343: status = mbadaddr_read   (iss, value); break;
    case 0x344: status = mip_read        (iss, value); break;

    // Machine protection and translation
    case 0x380: status = mbase_read      (iss, value); break;
    case 0x381: status = mbound_read     (iss, value); break;
    case 0x382: status = mibase_read     (iss, value); break;
    case 0x383: status = mibound_read    (iss, value); break;
    case 0x384: status = mdbase_read     (iss, value); break;
    case 0x385: status = mdbound_read    (iss, value); break;

    // Machine timers and counters
    case 0xB00: status = mcycle_read     (iss, value); break;
    case 0xB02: status = minstret_read   (iss, value); break;
    case 0xB80: status = mcycleh_read    (iss, value); break;
    case 0xB82: status = minstreth_read  (iss, value); break;

    // Machine counter setup
    case 0x320: status = mucounteren_read(iss, value); break;
    case 0x321: status = mscounteren_read(iss, value); break;
    case 0x322: status = mhcounteren_read(iss, value); break;

    // PULP extensions
    case 0xC10: status = umode_read(iss, value); break;
    case 0x014: status = mhartid_read(iss, value); break;

#if CSR_HWLOOP0_START != 0x7b0
    case 0x7b0: status = dcsr_read    (iss, value); break;
    case 0x7b1: status = depc_read    (iss, value); break;
    case 0x7b2: status = scratch0_read(iss, value); break;
    case 0x7b3: status = scratch1_read(iss, value); break;
#endif

#ifdef CSR_STACK_CONF
    case CSR_STACK_CONF:  status = stack_conf_read(iss, value); break;
    case CSR_STACK_START: status = stack_start_read(iss, value); break;
    case CSR_STACK_END:   status = stack_end_read(iss, value); break;
#endif

    default:

#if defined(ISS_HAS_PERF_COUNTERS)
    if ((reg >= CSR_PCCR(0) && reg <= CSR_PCCR(CSR_NB_PCCR)) || reg == CSR_PCER || reg == CSR_PCMR)
    {
      status = perfCounters_read(iss, reg, value);
    }
#endif

    else if (iss->cpu.pulpv2.hwloop)
    {
      if (reg >= CSR_HWLOOP0_START && reg <= CSR_HWLOOP1_COUNTER)
      {
        status = hwloop_read(iss, reg - CSR_HWLOOP0_START, value);
      }
    }

    else
    {
#if 0
      triggerException_cause(iss, iss->currentPc, EXCEPTION_ILLEGAL_INSTR, ECAUSE_ILL_INSTR);
#endif
      return true;
    }
  }

  iss_csr_msg(iss, "Read CSR (reg: 0x%x, value: 0x%x)\n", reg, value);

  return status;
}

bool iss_csr_write(iss_t *iss, iss_reg_t reg, iss_reg_t value)
{
  iss_csr_msg(iss, "Writing CSR (reg: 0x%x, value: 0x%x)\n", reg, value);

  // If there is any write to a CSR, switch to full check instruction handler
  // in case something special happened (like HW counting become active)
  iss_trigger_check_all(iss);

#if 0
  // First check permissions
  if (checkCsrAccess(iss, reg, 0)) return true;

  if (!getOption(iss, __priv_pulp)) {
    if (reg >= 0xB03 && reg <= 0xB1F) return mhpmcounter_write(iss, reg - 0xB03, value);
    if (reg >= 0xB83 && reg <= 0xB9F) return mhpmcounterh_write(iss, reg - 0xB83, value);

    if (reg >= 0x323 && reg <= 0x33F) return mhpmevent_write(iss, reg - 0x323, value);
  }
#endif

  // And dispatch
  switch (reg) {

    // User trap setup
    case 0x000: return ustatus_write   (iss, value);
    case 0x004: return uie_write       (iss, value);
    case 0x005: return utvec_write     (iss, value);

    // User trap handling
    case 0x040: return uscratch_write  (iss, value);
    case 0x041: return uepc_write      (iss, value);
    case 0x042: return ucause_write    (iss, value);
    case 0x043: return ubadaddr_write  (iss, value);
    case 0x044: return uip_write       (iss, value);

    // User floating-point CSRs
    case 0x001: return fflags_write    (iss, value);
    case 0x002: return frm_write       (iss, value);
    case 0x003: return fcsr_write      (iss, value);
    case 0x006: return fprec_write     (iss, value);




    // Supervisor trap setup
    case 0x100: return sstatus_write   (iss, value);
    case 0x102: return sedeleg_write   (iss, value);
    case 0x103: return sideleg_write   (iss, value);
    case 0x104: return sie_write       (iss, value);
    case 0x105: return stvec_write     (iss, value);

    // Supervisor trap handling
    case 0x140: return sscratch_write  (iss, value);
    case 0x141: return sepc_write      (iss, value);
    case 0x142: return scause_write    (iss, value);
    case 0x143: return sbadaddr_write  (iss, value);
    case 0x144: return sip_write       (iss, value);

    // Supervisor protection and translation
    case 0x180: return sptbr_write     (iss, value);




    // Hypervisor trap setup
    case 0x200: return hstatus_write   (iss, value);
    case 0x202: return hedeleg_write   (iss, value);
    case 0x203: return hideleg_write   (iss, value);
    case 0x204: return hie_write       (iss, value);
    case 0x205: return htvec_write     (iss, value);

    // Hypervisor trap handling
    case 0x240: return hscratch_write  (iss, value);
    case 0x241: return hepc_write      (iss, value);
    case 0x242: return hcause_write    (iss, value);
    case 0x243: return hbadaddr_write  (iss, value);




    // Machine trap setup
    case 0x300: return mstatus_write    (iss, value);
    case 0x301: return misa_write       (iss, value);
    case 0x302: return medeleg_write    (iss, value);
    case 0x303: return mideleg_write    (iss, value);
    case 0x304: return mie_write        (iss, value);
    case 0x305: return mtvec_write      (iss, value);

    // Machine trap handling
    case 0x340: return mscratch_write   (iss, value);
    case 0x341: return mepc_write       (iss, value);
    case 0x342: return mcause_write     (iss, value);
    case 0x343: return mbadaddr_write   (iss, value);
    case 0x344: return mip_write        (iss, value);

    // Machine protection and translation
    case 0x380: return mbase_write      (iss, value);
    case 0x381: return mbound_write     (iss, value);
    case 0x382: return mibase_write     (iss, value);
    case 0x383: return mibound_write    (iss, value);
    case 0x384: return mdbase_write     (iss, value);
    case 0x385: return mdbound_write    (iss, value);

    // Machine timers and counters
    case 0xB00: return mcycle_write     (iss, value);
    case 0xB02: return minstret_write   (iss, value);
    case 0xB80: return mcycleh_write    (iss, value);
    case 0xB82: return minstreth_write  (iss, value);

    // Machine counter setup
    case 0x310: return mucounteren_write(iss, value);
    case 0x311: return mscounteren_write(iss, value);
    case 0x312: return mhcounteren_write(iss, value);

#if CSR_HWLOOP0_START != 0x7b0
    case 0x7b0: return dcsr_write    (iss, value);
    case 0x7b1: return depc_write    (iss, value);
    case 0x7b2: return scratch0_write(iss, value);
    case 0x7b3: return scratch1_write(iss, value);
#endif

#ifdef CSR_STACK_CONF
    case CSR_STACK_CONF:  return stack_conf_write(iss, value); break;
    case CSR_STACK_START: return stack_start_write(iss, value); break;
    case CSR_STACK_END:   return stack_end_write(iss, value); break;
#endif

  }

#if defined(ISS_HAS_PERF_COUNTERS)
  if ((reg >= CSR_PCCR(0) && reg <= CSR_PCCR(CSR_NB_PCCR)) || reg == CSR_PCER || reg == CSR_PCMR) return perfCounters_write(iss, reg, value);
#endif

  if (iss->cpu.pulpv2.hwloop)
  {
    if (reg >= CSR_HWLOOP0_START && reg <= CSR_HWLOOP1_COUNTER) return hwloop_write(iss, reg - CSR_HWLOOP0_START, value);
  }

#if 0
  triggerException_cause(iss, iss->currentPc, EXCEPTION_ILLEGAL_INSTR, ECAUSE_ILL_INSTR);
#endif

  return true;
}

void iss_csr_init(iss_t *iss, int reset)
{
  iss->cpu.csr.status = 0x3 << 11;
  iss->cpu.csr.mcause = 0;
#if defined(ISS_HAS_PERF_COUNTERS)
  iss->cpu.csr.pcmr = 3;
  iss->cpu.csr.pcer = 3;
#endif
  iss->cpu.csr.stack_conf = 0;
  iss->cpu.csr.dcsr = 4 << 28;
}
