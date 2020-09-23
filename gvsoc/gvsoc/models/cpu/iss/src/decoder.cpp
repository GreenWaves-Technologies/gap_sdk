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
#include <string.h>

extern iss_isa_set_t __iss_isa_set;
extern iss_isa_tag_t __iss_isa_tags[];

static int decode_item(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode, iss_decoder_item_t *item);

static uint64_t decode_ranges(iss_t *iss, iss_opcode_t opcode, iss_decoder_range_set_t *range_set, bool is_signed)
{
  int nb_ranges = range_set->nb_ranges;
  iss_decoder_range_t *ranges = range_set->ranges;;
  uint64_t result = 0;
  int bits = 0;
  for (int i=0; i<nb_ranges; i++)
  {
    iss_decoder_range_t *range = &ranges[i];
    result |= iss_get_field(opcode, range->bit, range->width) << range->shift;
    int last_bit = range->width + range->shift;
    if (last_bit > bits) bits = last_bit;
  }
  if (is_signed) result = iss_get_signed_value(result, bits);
  return result;
}


static int decode_info(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode, iss_decoder_arg_info_t *info, bool is_signed)
{
  if (info->type == ISS_DECODER_VALUE_TYPE_RANGE)
  {
    return decode_ranges(iss, opcode, &info->u.range_set, is_signed);
  }
  else if (info->type == ISS_DECODER_VALUE_TYPE_UIM)
  {
    return info->u.uim;
  }
  else if (info->type == ISS_DECODER_VALUE_TYPE_SIM)
  {
    return info->u.sim;
  }

  return 0;
}

static int decode_insn(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode, iss_decoder_item_t *item)
{
  if (!item->is_active) return -1;

  insn->latency = 0;
  insn->hwloop_handler = NULL;
  insn->fast_handler = item->u.insn.fast_handler;
  insn->handler = item->u.insn.handler;

  insn->decoder_item = item;
  insn->size = item->u.insn.size;
  insn->nb_out_reg = 0;
  insn->nb_in_reg = 0;
  insn->latency = item->u.insn.latency;

  for (int i=0; i<item->u.insn.nb_args; i++)
  {
    insn->out_regs[i] = -1;
    insn->in_regs[i] = -1;
  }

  for (int i=0; i<item->u.insn.nb_args; i++)
  {
    iss_decoder_arg_t *darg = &item->u.insn.args[i];
    iss_insn_arg_t *arg = &insn->args[i];
    arg->type = darg->type;
    arg->flags = darg->flags;

    switch (darg->type)
    {
      case ISS_DECODER_ARG_TYPE_IN_REG:
      case ISS_DECODER_ARG_TYPE_OUT_REG:
        arg->u.reg.index = decode_info(iss, insn, opcode, &darg->u.reg.info, false);
        
        if (darg->flags & ISS_DECODER_ARG_FLAG_COMPRESSED)
          arg->u.reg.index += 8;

#ifndef ISS_SINGLE_REGFILE
        if (darg->flags & ISS_DECODER_ARG_FLAG_FREG)
          arg->u.reg.index += ISS_NB_REGS;
#endif

        if (darg->type == ISS_DECODER_ARG_TYPE_IN_REG) {
          if (darg->u.reg.id >= insn->nb_in_reg)
            insn->nb_in_reg = darg->u.reg.id + 1;

          insn->in_regs[darg->u.reg.id] = arg->u.reg.index;
        }
        else {
          if (darg->u.reg.id >= insn->nb_out_reg)
            insn->nb_out_reg = darg->u.reg.id + 1;

          insn->out_regs[darg->u.reg.id] = arg->u.reg.index;
        }

        if (darg->type == ISS_DECODER_ARG_TYPE_OUT_REG && darg->u.reg.latency != 0)
        {
          iss_insn_t *next = insn_cache_get_decoded(iss, insn->addr + insn->size);

          // We can stall the next instruction either if latency is superior
          // to 2 (due to number of pipeline stages) or if there is a data
          // dependency

          // Go through the registers and set the handler to the stall handler
          // in case we find a register dependency so that we can properly
          // handle the stall
          bool set_pipe_latency = true;
          for (int j=0; j<next->nb_in_reg; j++)
          {
            if (next->in_regs[j] == arg->u.reg.index)
            {
              insn->latency += darg->u.reg.latency;
              set_pipe_latency = false;
              break;
            }
          }

          // If no dependency was found, apply the one for the pipeline stages
          if (set_pipe_latency && darg->u.reg.latency > PIPELINE_STAGES)
          {
            insn->latency += darg->u.reg.latency - PIPELINE_STAGES + 1;
          }
        }


        break;

      case ISS_DECODER_ARG_TYPE_UIMM:
        arg->u.uim.value = decode_ranges(iss, opcode, &darg->u.uimm.info.u.range_set, darg->u.uimm.is_signed);
        insn->uim[darg->u.uimm.id] = arg->u.uim.value;
        break;

      case ISS_DECODER_ARG_TYPE_SIMM:
        arg->u.sim.value = decode_ranges(iss, opcode, &darg->u.simm.info.u.range_set, darg->u.simm.is_signed);
        insn->sim[darg->u.simm.id] = arg->u.sim.value;
        break;

      case ISS_DECODER_ARG_TYPE_INDIRECT_IMM:
        arg->u.indirect_imm.reg_index = decode_info(iss, insn, opcode, &darg->u.indirect_imm.reg.info, false);
        if (darg->u.indirect_imm.reg.flags & ISS_DECODER_ARG_FLAG_COMPRESSED) arg->u.indirect_imm.reg_index += 8;
        insn->in_regs[darg->u.indirect_imm.reg.id] = arg->u.indirect_imm.reg_index;
        arg->u.indirect_imm.imm = decode_info(iss, insn, opcode, &darg->u.indirect_imm.imm.info, darg->u.indirect_imm.imm.is_signed);
        insn->sim[darg->u.indirect_imm.imm.id] = arg->u.indirect_imm.imm;
        break;

      case ISS_DECODER_ARG_TYPE_INDIRECT_REG:
        arg->u.indirect_reg.base_reg_index = decode_info(iss, insn, opcode, &darg->u.indirect_reg.base_reg.info, false);
        if (darg->u.indirect_reg.base_reg.flags & ISS_DECODER_ARG_FLAG_COMPRESSED) arg->u.indirect_reg.base_reg_index += 8;
        insn->in_regs[darg->u.indirect_reg.base_reg.id] = arg->u.indirect_reg.base_reg_index;

        arg->u.indirect_reg.offset_reg_index = decode_info(iss, insn, opcode, &darg->u.indirect_reg.offset_reg.info, false);
        if (darg->u.indirect_reg.offset_reg.flags & ISS_DECODER_ARG_FLAG_COMPRESSED) arg->u.indirect_reg.offset_reg_index += 8;
        insn->in_regs[darg->u.indirect_reg.offset_reg.id] = arg->u.indirect_reg.offset_reg_index;

        break;
    }
  }

  insn->next = insn_cache_get(iss, insn->addr + insn->size);

  if (item->u.insn.decode != NULL)
  {
    item->u.insn.decode(iss, insn);
  }

  if (insn->latency)
  {
    insn->stall_handler = insn->handler;
    insn->stall_fast_handler = insn->fast_handler;
    insn->handler = iss_exec_stalled_insn;
    insn->fast_handler = iss_exec_stalled_insn_fast;
  }

  return 0;
}

static int decode_opcode_group(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode, iss_decoder_item_t *item)
{
  iss_opcode_t group_opcode = (opcode >> item->u.group.bit) & ((1ULL << item->u.group.width) - 1);
  iss_decoder_item_t *group_item_other = NULL;

  for (int i=0; i<item->u.group.nb_groups; i++)
  {
    iss_decoder_item_t *group_item = item->u.group.groups[i];
    if (group_opcode == group_item->opcode && !group_item->opcode_others) return decode_item(iss, insn, opcode, group_item);
    if (group_item->opcode_others) group_item_other = group_item;
  }

  if (group_item_other) return decode_item(iss, insn, opcode, group_item_other);

  return -1;
}

static int decode_item(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode, iss_decoder_item_t *item)
{
  if (item->is_insn) return decode_insn(iss, insn, opcode, item);
  else return decode_opcode_group(iss, insn, opcode, item);
}

static int decode_opcode(iss_t *iss, iss_insn_t *insn, iss_opcode_t opcode)
{
  for (int i=0; i<__iss_isa_set.nb_isa; i++)
  {
    iss_isa_t *isa = &__iss_isa_set.isa_set[i];
    if (decode_item(iss, insn, opcode, isa->tree) == 0) return 0;
  }

  iss_decoder_msg(iss, "Unknown instruction\n");

  return -1;
}


void iss_decode_activate_isa(iss_t *cpu, char *name)
{
  iss_isa_tag_t *isa = &__iss_isa_tags[0];
  while(isa->name)
  {
    if (strcmp(isa->name, name) == 0)
    {
      iss_decoder_item_t **insn_ptr = isa->insns;
      while(*insn_ptr)
      {
        iss_decoder_item_t *insn = *insn_ptr;
        insn->is_active = true;
        insn_ptr++;
      }
    }
    isa++;
  }
}


static iss_insn_t *iss_exec_insn_illegal(iss_t *iss, iss_insn_t *insn)
{
  iss_decoder_msg(iss, "Executing illegal instruction\n");
  return iss_except_raise(iss, ISS_EXCEPT_ILLEGAL);
}

iss_insn_t *iss_decode_pc_noexec(iss_t *iss, iss_insn_t *insn)
{
  iss_decoder_msg(iss, "Decoding instruction (pc: 0x%lx)\n", insn->addr);

  iss_opcode_t opcode = prefetcher_get_word(iss, insn->addr);

  iss_decoder_msg(iss, "Got opcode (opcode: 0x%lx)\n", opcode);

  if (decode_opcode(iss, insn, opcode) == -1)
  {
    insn->handler = iss_exec_insn_illegal;
    insn->fast_handler = iss_exec_insn_illegal;
    return insn;
  }

  insn->opcode = opcode;

  if (iss_insn_trace_active(iss) || iss_insn_event_active(iss))
  {
    insn->saved_handler = insn->handler;
    insn->handler = iss_exec_insn_with_trace;
    insn->fast_handler = iss_exec_insn_with_trace;
  }

  return insn;
}

iss_insn_t *iss_decode_pc(iss_t *iss, iss_insn_t *insn)
{
  return iss_exec_insn(iss, iss_decode_pc_noexec(iss, insn));
}
