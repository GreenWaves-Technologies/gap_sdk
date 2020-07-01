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

#include "iss.hpp"
#include <string.h>

static int iss_parse_isa(iss_t *iss)
{
  const char *current = iss->cpu.config.isa;
  int len = strlen(current);

  bool arch_rv32 = false;
  bool arch_rv64 = false;

  if (strncmp(current, "rv32", 4) == 0)
  {
    current += 4;
    len -= 4;
    arch_rv32 = true;
  }
  else
  {
    iss_warning(iss, "Unsupported ISA: %s\n", current);
    return -1;
  }

  iss_decode_activate_isa(iss, (char *)"priv");
  iss_decode_activate_isa(iss, (char *)"priv_pulp_v2");

  bool has_f = false;
  bool has_d = false;
  bool has_c = false;
  bool has_f16 = false;
  bool has_f16alt = false;
  bool has_f8 = false;
  bool has_fvec = false;
  bool has_faux = false;

  while (len > 0)
  {
    switch (*current)
    {
      case 'd':
        has_d = true; // D needs F
      case 'f':
        has_f = true;
      case 'i':
      case 'm': {
        char name[2];
        name[0] = *current;
        name[1] = 0;
        iss_decode_activate_isa(iss, name);
        current++;
        len--;
        break;
      }
      case 'c': {
        iss_decode_activate_isa(iss, (char *)"c");
        current++;
        len--;
        has_c = true;
        break;
      }
      case 'X': {
        char *token = strtok(strdup(current), "X");

        while(token)
        {

          iss_decode_activate_isa(iss, token);

          if (strcmp(token, "pulpv2") == 0)
          {
            iss_isa_pulpv2_activate(iss);
          }
          else if (strcmp(token, "gap8") == 0)
          {
            iss_isa_pulpv2_activate(iss);
            iss_decode_activate_isa(iss, (char *)"pulpv2");
          }
          else if (strcmp(token, "f16") == 0)
          {
            has_f16 = true;
          }
          else if (strcmp(token, "f16alt") == 0)
          {
            has_f16alt = true;
          }
          else if (strcmp(token, "f8") == 0)
          {
            has_f8 = true;
          }
          else if (strcmp(token, "fvec") == 0)
          {
            has_fvec = true;
          }
          else if (strcmp(token, "faux") == 0)
          {
            has_faux = true;
          }

          token =  strtok(NULL, "X");
        }

        len = 0;


        break;
      }
      default:
        iss_warning(iss, "Unknwon ISA descriptor: %c\n", *current);
        return -1;
    }
  }

  //
  // Activate inter-dependent ISA extension subsets
  //

  // Compressed floating-point instructions
  if (has_c)
  {
    if (has_f)
      iss_decode_activate_isa(iss, (char *)"cf");
    if (has_d)
      iss_decode_activate_isa(iss, (char *)"cd");
  }

  // For F Extension
  if (has_f) {
    if (arch_rv64)
      iss_decode_activate_isa(iss, (char *)"rv64f");
    // Vectors
    if (has_fvec && has_d) { // make sure FLEN >= 64
      iss_decode_activate_isa(iss, (char *)"f32vec");
      if (!(arch_rv32 && has_d))
        iss_decode_activate_isa(iss, (char *)"f32vecno32d");
    }
    // Auxiliary Ops
    if (has_faux) {
      // nothing for scalars as expansions are to fp32
      if (has_fvec)
        iss_decode_activate_isa(iss, (char *)"f32auxvec");
    }
  }

 // For Xf16 Extension
  if (has_f16) {
    if (arch_rv64)
      iss_decode_activate_isa(iss, (char *)"rv64f16");
    if (has_f)
      iss_decode_activate_isa(iss, (char *)"f16f");
    if (has_d)
      iss_decode_activate_isa(iss, (char *)"f16d");
    // Vectors
    if (has_fvec && has_f) { // make sure FLEN >= 32
      iss_decode_activate_isa(iss, (char *)"f16vec");
      if (!(arch_rv32 && has_d))
        iss_decode_activate_isa(iss, (char *)"f16vecno32d");
      if (has_d)
        iss_decode_activate_isa(iss, (char *)"f16vecd");
    }
    // Auxiliary Ops
    if (has_faux) {
      iss_decode_activate_isa(iss, (char *)"f16aux");
      if (has_fvec)
        iss_decode_activate_isa(iss, (char *)"f16auxvec");
    }
  }

 // For Xf16alt Extension
  if (has_f16alt) {
    if (arch_rv64)
      iss_decode_activate_isa(iss, (char *)"rv64f16alt");
    if (has_f)
      iss_decode_activate_isa(iss, (char *)"f16altf");
    if (has_d)
      iss_decode_activate_isa(iss, (char *)"f16altd");
    if (has_f16)
      iss_decode_activate_isa(iss, (char *)"f16altf16");
    // Vectors
    if (has_fvec && has_f) { // make sure FLEN >= 32
      iss_decode_activate_isa(iss, (char *)"f16altvec");
      if (!(arch_rv32 && has_d))
        iss_decode_activate_isa(iss, (char *)"f16altvecno32d");
      if (has_d)
        iss_decode_activate_isa(iss, (char *)"f16altvecd");
      if (has_f16)
        iss_decode_activate_isa(iss, (char *)"f16altvecf16");
    }
    // Auxiliary Ops
    if (has_faux) {
      iss_decode_activate_isa(iss, (char *)"f16altaux");
      if (has_fvec)
        iss_decode_activate_isa(iss, (char *)"f16altauxvec");
    }
  }

 // For Xf8 Extension
  if (has_f8) {
    if (arch_rv64)
      iss_decode_activate_isa(iss, (char *)"rv64f8");
    if (has_f)
      iss_decode_activate_isa(iss, (char *)"f8f");
    if (has_d)
      iss_decode_activate_isa(iss, (char *)"f8d");
    if (has_f16)
      iss_decode_activate_isa(iss, (char *)"f8f16");
    if (has_f16alt)
      iss_decode_activate_isa(iss, (char *)"f8f16alt");
    // Vectors
    if (has_fvec && (has_f16 || has_f16alt || has_f)) { // make sure FLEN >= 16
      iss_decode_activate_isa(iss, (char *)"f8vec");
      if (!(arch_rv32 && has_d))
        iss_decode_activate_isa(iss, (char *)"f8vecno32d");
      if (has_f)
        iss_decode_activate_isa(iss, (char *)"f8vecf");
      if (has_d)
        iss_decode_activate_isa(iss, (char *)"f8vecd");
      if (has_f16)
        iss_decode_activate_isa(iss, (char *)"f8vecf16");
      if (has_f16alt)
        iss_decode_activate_isa(iss, (char *)"f8vecf16alt");
    }
    // Auxiliary Ops
    if (has_faux) {
      iss_decode_activate_isa(iss, (char *)"f8aux");
      if (has_fvec)
        iss_decode_activate_isa(iss, (char *)"f8auxvec");
    }
  }

  return 0;
}



void iss_reset(iss_t *iss, int active)
{
  if (active)
  {
    for (int i=0; i<ISS_NB_TOTAL_REGS; i++)
    {
      iss->cpu.regfile.regs[i] = 0;
    }

    iss_cache_flush(iss);
    
    iss->cpu.prev_insn = NULL;
    iss->cpu.state.elw_insn = NULL;

    iss_irq_init(iss);
  }

  iss_csr_init(iss, active);
}

int iss_open(iss_t *iss)
{
  iss_isa_pulpv2_init(iss);

  if (iss_parse_isa(iss)) return -1;

  insn_cache_init(iss);
  prefetcher_init(iss);

  iss->cpu.regfile.regs[0] = 0;
  iss->cpu.current_insn = NULL;
  iss->cpu.state.fetch_cycles = 0;

  iss_irq_build(iss);

  iss_trace_init(iss);

  iss_init(iss);

  return 0;
}



void iss_start(iss_t *iss)
{
}



void iss_pc_set(iss_t *iss, iss_addr_t value)
{
  iss->cpu.current_insn = insn_cache_get(iss, value);
}
