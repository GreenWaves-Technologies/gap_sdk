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
#include <algorithm>
#include <vector>

#define PC_INFO_ARRAY_SIZE (64*1024)

#define MAX_DEBUG_INFO_WIDTH 24

class iss_pc_info {
public:
  unsigned int base;
  char *func;
  char *inline_func;
  char *file;
  int line;
  iss_pc_info *next;
};

static bool pc_infos_is_init = false;
static iss_pc_info *pc_infos[PC_INFO_ARRAY_SIZE];
static std::vector<std::string> binaries;

static void add_pc_info(unsigned int base, char *func, char *inline_func, char *file, int line)
{
  iss_pc_info *pc_info = new iss_pc_info();

  int index = base & (PC_INFO_ARRAY_SIZE - 1);
  pc_info->next = pc_infos[index];
  pc_infos[index] = pc_info;

  pc_info->base = base;
  pc_info->func = strdup(func);
  pc_info->inline_func = strdup(inline_func);
  pc_info->file = strdup(file);
  pc_info->line = line;
}

static iss_pc_info *get_pc_info(unsigned int base)
{
  int index = base & (PC_INFO_ARRAY_SIZE - 1);

  iss_pc_info *pc_info = pc_infos[index];

  while (pc_info && pc_info->base != base)
  {
    pc_info = pc_info->next;
  }

  return pc_info;
}

int iss_trace_pc_info(iss_addr_t addr, const char **func, const char **inline_func, const char **file, int *line)
{
  iss_pc_info *info = get_pc_info(addr);
  if (info == NULL)
    return -1;

  *func = info->func;
  *inline_func = info->inline_func;
  *file = info->file;
  *line = info->line;

  return 0;
}

void iss_register_debug_info(iss_t *iss, const char *binary)
{
  if (std::find(binaries.begin(), binaries.end(), std::string(binary)) != binaries.end())
    return;

  binaries.push_back(std::string(binary));

  FILE *file = fopen(binary, "r");
  if (file != NULL)
  {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1)
    {
      char *token = strtok(line, " ");
      char *tokens[5];
      int index = 0;
      while (token)
      {
        tokens[index++] = token;
        token = strtok(NULL, " ");
      }
      if (index == 5) add_pc_info(strtol(tokens[0], NULL, 16), tokens[1], tokens[2], tokens[3], atoi(tokens[4]));
    }
  }
}

static inline char iss_trace_get_mode(int mode) {
  switch (mode) {
    case 0: return 'U';
    case 1: return 'S';
    case 2: return 'H';
    case 3: return 'M';
  }
  return ' ';
}

static inline int iss_trace_dump_reg(iss_t *iss, iss_insn_t *insn, char *buff, unsigned int reg, bool is_long=true)
{
  //if (!cpu->conf->traceLikeRtl)
  {
    if (reg == 0) {
      return sprintf(buff, "0");
    } else if (reg == 1) {
      return sprintf(buff, "ra");
    } else if (reg == 2) {
      return sprintf(buff, "sp");
    } else if (reg >= 8 && reg <= 9) {
      return sprintf(buff, "s%d", reg-8);
    } else if (reg >= 18 && reg <= 27) {
      return sprintf(buff, "s%d", reg-16);
    } else if (reg == 4) {
      return sprintf(buff, "tp");
    } else if (reg >= 10 && reg <= 17) {
      return sprintf(buff, "a%d", reg-10);
    } else if (reg >= 5 && reg <= 7) {
      return sprintf(buff, "t%d", reg-5);
    } else if (reg >= 28 && reg <= 31) {
      return sprintf(buff, "t%d", reg-25);
    } else if (reg == 3) {
      return sprintf(buff, "gp");
    } else if (reg >= ISS_NB_REGS) {
      return sprintf(buff, "f%d", reg-ISS_NB_REGS);
    }
  }
   
  return sprintf(buff, "x%d", reg);
}

static char *iss_trace_dump_reg_value(iss_t *iss, iss_insn_t *insn, char *buff, bool is_out, int reg, uint64_t saved_value, iss_decoder_arg_t *arg, iss_decoder_arg_t **prev_arg, bool is_long)
{
  char regStr[16];
  iss_trace_dump_reg(iss, insn, regStr, reg);
  if (is_long) buff += sprintf(buff, "%3.3s", regStr);
  else buff += sprintf(buff, "%s", regStr);

  if (is_out) buff += sprintf(buff,  "=");
  else buff += sprintf(buff,  ":");
  if (arg->flags & ISS_DECODER_ARG_FLAG_REG64)
    buff += sprintf(buff,  "%" PRIxFULLREG64 " ", saved_value);
  else
    buff += sprintf(buff,  "%" PRIxFULLREG " ", (iss_reg_t)saved_value);
  return buff;
}

static char *iss_trace_dump_arg_value(iss_t *iss, iss_insn_t *insn, char *buff, iss_insn_arg_t *insn_arg, iss_decoder_arg_t *arg, iss_insn_arg_t *saved_arg, iss_decoder_arg_t **prev_arg, int dump_out, bool is_long)
{
  if ((arg->type == ISS_DECODER_ARG_TYPE_OUT_REG || arg->type == ISS_DECODER_ARG_TYPE_IN_REG) && insn_arg->u.reg.index != 0)
  {
    if ((dump_out && arg->type == ISS_DECODER_ARG_TYPE_OUT_REG) || (!dump_out && arg->type == ISS_DECODER_ARG_TYPE_IN_REG))
    {
      buff = iss_trace_dump_reg_value(iss, insn, buff, arg->type == ISS_DECODER_ARG_TYPE_OUT_REG, insn_arg->u.reg.index, arg->flags & ISS_DECODER_ARG_FLAG_REG64 ? saved_arg->u.reg.value_64 : saved_arg->u.reg.value, arg, prev_arg, is_long);
    }
  }
  else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_IMM)
  {
    if (!dump_out) buff = iss_trace_dump_reg_value(iss, insn, buff, 0, insn_arg->u.indirect_imm.reg_index, saved_arg->u.indirect_imm.reg_value, arg, prev_arg, is_long);
    iss_addr_t addr;
    if (arg->flags & ISS_DECODER_ARG_FLAG_POSTINC)
    {
      addr = saved_arg->u.indirect_imm.reg_value;
      if (dump_out) buff = iss_trace_dump_reg_value(iss, insn, buff, 1, insn_arg->u.indirect_imm.reg_index, addr + insn_arg->u.indirect_imm.imm, arg, prev_arg, is_long);
    }
    else
    {
      addr = saved_arg->u.indirect_imm.reg_value + insn_arg->u.indirect_imm.imm;
    }
    if (!dump_out) buff += sprintf(buff, " PA:%" PRIxFULLREG " ", addr);
  }
  else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_REG)
  {
    if (!dump_out) buff = iss_trace_dump_reg_value(iss, insn, buff, 0, insn_arg->u.indirect_reg.offset_reg_index, saved_arg->u.indirect_reg.offset_reg_value, arg, prev_arg, is_long);
    if (!dump_out) buff = iss_trace_dump_reg_value(iss, insn, buff, 0, insn_arg->u.indirect_reg.base_reg_index, saved_arg->u.indirect_reg.base_reg_value, arg, prev_arg, is_long);
    iss_addr_t addr;
    if (arg->flags & ISS_DECODER_ARG_FLAG_POSTINC)
    {
      addr = saved_arg->u.indirect_reg.base_reg_value;
      if (dump_out) buff = iss_trace_dump_reg_value(iss, insn, buff, 1, insn_arg->u.indirect_reg.base_reg_index, addr + insn_arg->u.indirect_reg.offset_reg_value, arg, prev_arg, is_long);
    }
    else
    {
      addr = saved_arg->u.indirect_reg.base_reg_value + saved_arg->u.indirect_reg.offset_reg_value;      
    }
    if (!dump_out) buff += sprintf(buff, " PA:%" PRIxFULLREG " ", addr);
  }
  //else if (arg->type == TRACE_TYPE_FLAG)
  //  {
  //    buff += sprintf(buff, "  F:%d       ", *savedValue);
  //  }
  //else // if (*prevArg == NULL || (*prevArg)->type != TRACE_TYPE_INDIRECT_IMM)
  //  {
  //    //buff += sprintf(buff,  "             ");
  //  }
  *prev_arg = arg;
  return buff;
}

static char *iss_trace_dump_arg(iss_t *iss, iss_insn_t *insn, char *buff, iss_insn_arg_t *insn_arg, iss_decoder_arg_t *arg, iss_decoder_arg_t **prev_arg, bool is_long)
{
    if (*prev_arg != NULL && (*prev_arg)->type != ISS_DECODER_ARG_TYPE_NONE && (*prev_arg)->type != ISS_DECODER_ARG_TYPE_FLAG && ((arg->type != ISS_DECODER_ARG_TYPE_IN_REG && arg->type != ISS_DECODER_ARG_TYPE_OUT_REG) || arg->u.reg.dump_name))
    {
     if (is_long) buff += sprintf(buff, ", ");
     else buff += sprintf(buff, ",");
    }


  if (arg->type != ISS_DECODER_ARG_TYPE_NONE)
  {
    if (arg->type == ISS_DECODER_ARG_TYPE_OUT_REG || arg->type == ISS_DECODER_ARG_TYPE_IN_REG)
    {
      if (arg->u.reg.dump_name) buff += iss_trace_dump_reg(iss, insn, buff, insn_arg->u.reg.index, is_long);
    }
    else if (arg->type == ISS_DECODER_ARG_TYPE_UIMM)
    {
      buff += sprintf(buff, "0x%" PRIxREG , insn_arg->u.uim.value);
    }
    else if (arg->type == ISS_DECODER_ARG_TYPE_SIMM)
    {
      buff += sprintf(buff, "%" PRIdREG, insn_arg->u.sim.value);
    }
    else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_IMM)
    {
      buff += sprintf(buff, "%" PRIdREG "(", insn_arg->u.indirect_imm.imm);
      if (arg->flags & ISS_DECODER_ARG_FLAG_PREINC) buff += sprintf(buff, "!");
      buff += iss_trace_dump_reg(iss, insn, buff, insn_arg->u.indirect_imm.reg_index, is_long);
      if (arg->flags & ISS_DECODER_ARG_FLAG_POSTINC) buff += sprintf(buff, "!");
      buff += sprintf(buff, ")");
    }
    else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_REG)
    {
      buff += iss_trace_dump_reg(iss, insn, buff, insn_arg->u.indirect_reg.offset_reg_index, is_long);
      buff += sprintf(buff, "(");
      if (arg->flags & ISS_DECODER_ARG_FLAG_PREINC) buff += sprintf(buff, "!");
      buff += iss_trace_dump_reg(iss, insn, buff, insn_arg->u.indirect_reg.base_reg_index, is_long);
      if (arg->flags & ISS_DECODER_ARG_FLAG_POSTINC) buff += sprintf(buff, "!");
      buff += sprintf(buff, ")");
    }
    *prev_arg = arg;
  }
  return buff;
}

static char *trace_dump_debug(iss_t *iss, iss_insn_t *insn, char *buff)
{
  char *name = (char *)"-";
  char *file = (char *)"-";
  uint32_t line = 0;
  char *inline_func = (char *)"-";
  iss_pc_info *pc_info = get_pc_info(insn->addr);
  if (pc_info)
  {
    name = pc_info->func;
    file = pc_info->file;
    line = pc_info->line;
    inline_func = pc_info->inline_func;
  }

  int line_len = sprintf(buff, ":%d", line);
  if (line_len > 5)
    line_len = 5;
  int max_name_len = MAX_DEBUG_INFO_WIDTH - line_len;

  int len = snprintf(buff, max_name_len+1, "%s", inline_func);
  if (len > max_name_len)
    len = max_name_len;

  len += sprintf(buff+len, ":%d", line);

  char *start_buff = buff;

  if (len > MAX_DEBUG_INFO_WIDTH)
    len = MAX_DEBUG_INFO_WIDTH;

  for (int i=len; i<MAX_DEBUG_INFO_WIDTH + 1; i++) {
    sprintf(buff + i, " ");
  }

  return buff + MAX_DEBUG_INFO_WIDTH + 1;
}

static void iss_trace_dump_insn(iss_t *iss, iss_insn_t *insn, char *buff, int buffer_size, iss_insn_arg_t *saved_args, bool is_long, int mode) {

  char *init_buff = buff;
  static int max_len = 20;
  static int max_arg_len = 17;
  int len;

  if (is_long) {
    if (binaries.size())
      buff = trace_dump_debug(iss, insn, buff);
  }

  if (is_long) {
    buff += sprintf(buff,  "%c %" PRIxFULLREG " ", iss_trace_get_mode(mode), insn->addr);
  }

  char *start_buff = buff;

  buff += sprintf(buff,  "%s ", insn->decoder_item->u.insn.label);

  if (is_long) {
    len = buff - start_buff;

    if (len > max_len) max_len = len;
    else 
    {
      memset(buff, ' ', max_len - len);
      buff += max_len - len;
    }
  }

  iss_decoder_arg_t *prev_arg = NULL;
  start_buff = buff;
  int nb_args = insn->decoder_item->u.insn.nb_args;
  for (int i=0; i<nb_args; i++) {
    buff = iss_trace_dump_arg(iss, insn, buff, &insn->args[i], &insn->decoder_item->u.insn.args[i], &prev_arg, is_long);
  }
  if (nb_args != 0) buff += sprintf(buff,  " ");

  if (is_long) {
    len = buff - start_buff;
  
    if (len > max_arg_len) max_arg_len = len;
    else
    {
      memset(buff, ' ', max_arg_len - len);
      buff += max_arg_len - len;
    }
  }

  if (is_long)
  {
    prev_arg = NULL;
    for (int i=0; i<nb_args; i++) {
      buff = iss_trace_dump_arg_value(iss, insn, buff, &insn->args[i], &insn->decoder_item->u.insn.args[i], &saved_args[i], &prev_arg, 1, is_long);
    }
    for (int i=0; i<nb_args; i++) {
      buff = iss_trace_dump_arg_value(iss, insn, buff, &insn->args[i], &insn->decoder_item->u.insn.args[i], &saved_args[i], &prev_arg, 0, is_long);
    }

    buff += sprintf(buff,  "\n");
  }

}

static void iss_trace_save_arg(iss_t *iss, iss_insn_t *insn, iss_insn_arg_t *insn_arg, iss_decoder_arg_t *arg, iss_insn_arg_t *saved_arg, bool save_out)
{
  if (arg->type == ISS_DECODER_ARG_TYPE_OUT_REG || arg->type == ISS_DECODER_ARG_TYPE_IN_REG)
  {
    if (save_out && arg->type == ISS_DECODER_ARG_TYPE_OUT_REG ||
      !save_out && arg->type == ISS_DECODER_ARG_TYPE_IN_REG)
    {
      if (arg->flags & ISS_DECODER_ARG_FLAG_REG64)
      {
        saved_arg->u.reg.value_64 = iss_get_reg64_untimed(iss, insn_arg->u.reg.index);
      }
      else
        saved_arg->u.reg.value = iss_get_reg_untimed(iss, insn_arg->u.reg.index);
    }

  }
  else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_IMM)
  {
    if (save_out) return;
    saved_arg->u.indirect_imm.reg_value = iss_get_reg_untimed(iss, insn_arg->u.indirect_imm.reg_index);
  }
  //else if (arg->type == TRACE_TYPE_FLAG)
  //  {
  //    *savedValue = cpu->flag;
  //  }
  else if (arg->type == ISS_DECODER_ARG_TYPE_INDIRECT_REG)
  {
    if (save_out) return;
    saved_arg->u.indirect_reg.base_reg_value = iss_get_reg_untimed(iss, insn_arg->u.indirect_reg.base_reg_index);
    saved_arg->u.indirect_reg.offset_reg_value = iss_get_reg_untimed(iss, insn_arg->u.indirect_reg.offset_reg_index);
  }
  //else
  //  {
  //    *savedValue = arg->val;
  //  }
}

static void iss_trace_save_args(iss_t *iss, iss_insn_t *insn, iss_insn_arg_t saved_args[], bool save_out)
{
  for (int i=0; i<insn->decoder_item->u.insn.nb_args; i++) {
    iss_decoder_arg_t *arg = &insn->decoder_item->u.insn.args[i];
    iss_trace_save_arg(iss, insn, &insn->args[i], arg, &saved_args[i], save_out);
  }
}

void iss_trace_dump(iss_t *iss, iss_insn_t *insn)
{
  char buffer[1024];

  iss_trace_save_args(iss, insn, iss->cpu.state.saved_args, true);
  
  iss_trace_dump_insn(iss, insn, buffer, 1024, iss->cpu.state.saved_args, true, 3);

  iss_insn_msg(iss, buffer);
}

void iss_event_dump(iss_t *iss, iss_insn_t *insn)
{
  char buffer[1024];

  iss_trace_dump_insn(iss, insn, buffer, 1024, iss->cpu.state.saved_args, false, 3);

  char *current = buffer;
  while (*current)
  {
    if (*current == ' ')
      *current = '_';

    current++;
  }

  iss_insn_event_dump(iss, buffer);
}

iss_insn_t *iss_exec_insn_with_trace(iss_t *iss, iss_insn_t *insn)
{
  iss_insn_t *next_insn;

  if (iss_insn_event_active(iss))
  {
    iss_event_dump(iss, insn);
  }

  if (iss_insn_trace_active(iss))
  {
    iss_trace_save_args(iss, insn, iss->cpu.state.saved_args, false);
    
    next_insn = iss_exec_insn_handler(iss, insn, insn->saved_handler);

    if (!iss_exec_is_stalled(iss))
      iss_trace_dump(iss, insn);
  }
  else
  {
    next_insn = iss_exec_insn_handler(iss, insn, insn->saved_handler);
  }


  return next_insn;
}

void iss_trace_init(iss_t *iss)
{
  if (!pc_infos_is_init)
  {
    pc_infos_is_init = true;
    memset(pc_infos, 0, sizeof(pc_infos));
  }
}