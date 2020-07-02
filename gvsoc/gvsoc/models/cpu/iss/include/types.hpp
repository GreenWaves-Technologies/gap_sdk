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

#ifndef __CPU_ISS_TYPES_HPP
#define __CPU_ISS_TYPES_HPP

#include "platform_types.hpp"
#include <stdint.h>
#define __STDC_FORMAT_MACROS    // This is needed for some old gcc versions
#include <inttypes.h>

#if defined(RISCY)
#define ISS_HAS_PERF_COUNTERS 1
#if defined(ISS_HAS_PERF_COUNTERS)
#ifdef PCER_VERSION_2
#include "archi/riscv/pcer_v2.h"
#else
#include "archi/riscv/pcer_v1.h"
#endif
#ifdef PRIV_1_10
#include "archi/riscv/priv_1_10.h"
#else
#include "archi/riscv/priv_1_9.h"
#endif
#endif
#else
#error Unknown core version
#endif

typedef uint64_t iss_reg64_t;

#define PRIxFULLREG64  "16.16" PRIx64

#if defined(ISS_WORD_64)

#define ISS_OPCODE_MAX_SIZE 8
#define ISS_REG_WIDTH 64

typedef uint64_t iss_reg_t;
typedef uint64_t iss_uim_t;
typedef int64_t iss_sim_t;
typedef uint64_t iss_addr_t;
typedef uint64_t iss_opcode_t;

#define PRIxREG  PRIx64
#define PRIxFULLREG  "16.16" PRIx64
#define PRIdREG  PRIx64

#else

#define ISS_OPCODE_MAX_SIZE 4
#define ISS_REG_WIDTH 32

typedef uint32_t iss_reg_t;
typedef uint32_t iss_uim_t;
typedef int32_t iss_sim_t;
typedef uint32_t iss_addr_t;
typedef uint32_t iss_opcode_t;

#define PRIxREG  PRIx32
#define PRIxFULLREG  "8.8" PRIx32
#define PRIdREG  PRId32

#endif

class iss;

#define ISS_NB_REGS  32
#define ISS_NB_FREGS 32
#define ISS_NB_TOTAL_REGS (ISS_NB_REGS + ISS_NB_FREGS)

#define ISS_PREFETCHER_SIZE (ISS_OPCODE_MAX_SIZE*4)

#define ISS_MAX_DECODE_RANGES 8
#define ISS_MAX_DECODE_ARGS 5
#define ISS_MAX_IMMEDIATES 4
#define ISS_MAX_NB_OUT_REGS 3
#define ISS_MAX_NB_IN_REGS 3

#define ISS_INSN_BLOCK_SIZE_LOG2 8
#define ISS_INSN_BLOCK_SIZE (1<<ISS_INSN_BLOCK_SIZE_LOG2)
#define ISS_INSN_PC_BITS 1
#define ISS_INSN_BLOCK_ID_BITS 12
#define ISS_INSN_NB_BLOCKS (1<<ISS_INSN_BLOCK_ID_BITS)

#define ISS_EXCEPT_RESET    0
#define ISS_EXCEPT_ILLEGAL  1
#define ISS_EXCEPT_ECALL    2
#define ISS_EXCEPT_DEBUG    3

typedef struct iss_cpu_s iss_cpu_t;
typedef struct iss_insn_s iss_insn_t;
typedef struct iss_insn_block_s iss_insn_block_t;
typedef struct iss_insn_cache_s iss_insn_cache_t;
typedef struct iss_decoder_item_s iss_decoder_item_t;

typedef enum {
  ISS_DECODER_ARG_TYPE_NONE,
  ISS_DECODER_ARG_TYPE_OUT_REG,
  ISS_DECODER_ARG_TYPE_IN_REG,
  ISS_DECODER_ARG_TYPE_UIMM,
  ISS_DECODER_ARG_TYPE_SIMM,
  ISS_DECODER_ARG_TYPE_INDIRECT_IMM,
  ISS_DECODER_ARG_TYPE_INDIRECT_REG,
  ISS_DECODER_ARG_TYPE_FLAG,
} iss_decoder_arg_type_e;

typedef enum {
  ISS_DECODER_ARG_FLAG_NONE = 0,
  ISS_DECODER_ARG_FLAG_POSTINC = 1,
  ISS_DECODER_ARG_FLAG_PREINC = 2,
  ISS_DECODER_ARG_FLAG_COMPRESSED = 4,
  ISS_DECODER_ARG_FLAG_FREG = 8,
  ISS_DECODER_ARG_FLAG_REG64 = 16,
} iss_decoder_arg_flag_e;

typedef struct iss_insn_arg_s {
  iss_decoder_arg_type_e type;
  iss_decoder_arg_flag_e flags;

  union {
    struct {
      int index;
      iss_reg_t value;
      iss_reg64_t value_64;
    } reg;
    struct {
      iss_sim_t value;
    } sim;
    struct {
      iss_uim_t value;
    } uim;
    struct {
      int reg_index;
      iss_sim_t imm;
      iss_reg_t reg_value;
    } indirect_imm;
    struct {
      int base_reg_index;
      iss_reg_t base_reg_value;
      int offset_reg_index;
      iss_reg_t offset_reg_value;
    } indirect_reg;
  } u;
} iss_insn_arg_t;

typedef struct iss_decoder_range_s {
  int bit;
  int width;
  int shift;
} iss_decoder_range_t;

typedef struct iss_decoder_range_set_s {
  int nb_ranges;
  iss_decoder_range_t ranges[ISS_MAX_DECODE_RANGES];
} iss_decoder_range_set_t;

typedef enum {
  ISS_DECODER_VALUE_TYPE_UIM = 0,
  ISS_DECODER_VALUE_TYPE_SIM = 1,
  ISS_DECODER_VALUE_TYPE_RANGE = 2,
} iss_decoder_arg_info_type_e;

typedef struct iss_decoder_arg_value_s {
  iss_decoder_arg_info_type_e type;
  union {
    iss_uim_t uim;
    iss_sim_t sim;
    iss_decoder_range_set_t range_set;
  } u;
} iss_decoder_arg_info_t;

typedef struct iss_decoder_arg_s {
  iss_decoder_arg_type_e type;
  iss_decoder_arg_flag_e flags;
  union {
    struct {
      bool is_signed;
      int id;
      iss_decoder_arg_info_t info;
    } uimm;
    struct {
      bool is_signed;
      int id;
      iss_decoder_arg_info_t info;
    } simm;
    struct {
      int id;
      iss_decoder_arg_flag_e flags;
      bool dump_name;
      int latency;
      iss_decoder_arg_info_t info;
    } reg;
    struct {
      struct {
        int id;
        iss_decoder_arg_flag_e flags;
        bool dump_name;
      int latency;
        iss_decoder_arg_info_t info;
      } reg;
      struct {
        bool is_signed;
        int id;
        iss_decoder_arg_info_t info;
      } imm;
    } indirect_imm;
    struct {
      struct {
        int id;
        iss_decoder_arg_flag_e flags;
        bool dump_name;
        int latency;
        iss_decoder_arg_info_t info;
      } base_reg;
      struct {
        int id;
        iss_decoder_arg_flag_e flags;
        bool dump_name;
        int latency;
        iss_decoder_arg_info_t info;
      } offset_reg;
    } indirect_reg;
  } u;
} iss_decoder_arg_t;

typedef struct iss_decoder_item_s {

  bool is_insn;
  bool is_active;

  bool opcode_others;
  iss_opcode_t opcode;

  union {
    struct {
      iss_insn_t *(*handler)(iss_t *, iss_insn_t*);
      iss_insn_t *(*fast_handler)(iss_t *, iss_insn_t*);
      void (*decode)(iss_t *, iss_insn_t*);
      char *label;
      int size;
      int nb_args;
      int latency;
      iss_decoder_arg_t args[ISS_MAX_DECODE_ARGS];
    } insn;

    struct {
      int bit;
      int width;
      int nb_groups;
      iss_decoder_item_t **groups;
    } group;
  } u;

} iss_decoder_item_t;

typedef struct iss_isa_s
{
  char *name;
  iss_decoder_item_t *tree;
} iss_isa_t;

typedef struct iss_isa_set_s
{
  int nb_isa;
  iss_isa_t *isa_set;
} iss_isa_set_t;

typedef struct iss_isa_tag_s
{
  char *name;
  iss_decoder_item_t **insns;
} iss_isa_tag_t;

typedef struct {
  uint8_t data[ISS_PREFETCHER_SIZE];
  iss_addr_t addr;
} iss_prefetcher_t;

typedef struct iss_insn_s {
  iss_addr_t addr;
  iss_reg_t opcode;
  iss_insn_t *(*fast_handler)(iss_t *, iss_insn_t*);
  iss_insn_t *(*handler)(iss_t *, iss_insn_t*);
  iss_insn_t *(*hwloop_handler)(iss_t *, iss_insn_t*);
  iss_insn_t *(*stall_handler)(iss_t *, iss_insn_t*);
  iss_insn_t *(*stall_fast_handler)(iss_t *, iss_insn_t*);
  int size;
  int nb_out_reg;
  int nb_in_reg;
  int out_regs[ISS_MAX_NB_OUT_REGS];
  int in_regs[ISS_MAX_NB_IN_REGS];
  iss_uim_t uim[ISS_MAX_IMMEDIATES];
  iss_sim_t sim[ISS_MAX_IMMEDIATES];
  iss_insn_arg_t args[ISS_MAX_DECODE_ARGS];
  iss_insn_t *next;
  iss_decoder_item_t *decoder_item;

  iss_insn_t *(*saved_handler)(iss_t *, iss_insn_t*);
  iss_insn_t *branch;

  int latency;

} iss_insn_t;

typedef struct iss_insn_block_s {
  iss_addr_t pc;
  iss_insn_t insns[ISS_INSN_BLOCK_SIZE];
  iss_insn_block_t *next;
  bool is_init;
} iss_insn_block_t;

typedef struct iss_insn_cache_s {
  iss_insn_block_t *blocks[ISS_INSN_NB_BLOCKS];
} iss_insn_cache_t;

typedef struct iss_regfile_s {
  iss_reg_t regs[ISS_NB_REGS + ISS_NB_FREGS];
} iss_regfile_t;

typedef struct
{
  union {
    struct {
      union {
        struct {
          unsigned int NX:1;
          unsigned int UF:1;
          unsigned int OF:1;
          unsigned int DZ:1;
          unsigned int NV:1;
        };
        unsigned int raw:5;
      } fflags;
      unsigned int frm:3;
    };
    iss_reg_t raw;
  };
} iss_fcsr_t;

typedef struct iss_cpu_state_s {
  iss_insn_t *hwloop_start_insn[2];

  iss_addr_t bootaddr;

  int insn_cycles;
  int saved_insn_cycles;
  int fetch_cycles;

  void (*stall_callback)(iss_t *iss);
  int stall_reg;
  int stall_size;

  iss_insn_arg_t saved_args[ISS_MAX_DECODE_ARGS];

  iss_reg_t vf0;
  iss_reg_t vf1;

  iss_insn_t *elw_insn;

  iss_fcsr_t fcsr;

  iss_reg_t fprec;

  bool debug_mode;

} iss_cpu_state_t;

typedef struct iss_config_s {
  iss_reg_t mhartid;
  const char *isa;
  iss_addr_t debug_handler;
} iss_config_t;

typedef struct iss_irq_s {
  iss_insn_t *vectors[35];
  int irq_enable;
  int saved_irq_enable;
  int debug_saved_irq_enable;
  int req_irq;
  bool req_debug;
  uint32_t vector_base;
  iss_insn_t *debug_handler;
} iss_irq_t;

typedef struct iss_csr_s
{
  iss_reg_t status;
  iss_reg_t epc;
  iss_reg_t depc;
  iss_reg_t mtvec;
  iss_reg_t mcause;
#if defined(ISS_HAS_PERF_COUNTERS)
  iss_reg_t pccr[CSR_PCER_NB_EVENTS];
  iss_reg_t pcer;
  iss_reg_t pcmr;
#endif
  iss_reg_t stack_conf;
  iss_reg_t stack_start;
  iss_reg_t stack_end;
  iss_reg_t scratch0;
  iss_reg_t scratch1;
} iss_csr_t;


#define PULPV2_HWLOOP_NB_REGS 7

typedef struct iss_pulpv2_s
{
  bool hwloop;
  iss_reg_t hwloop_regs[PULPV2_HWLOOP_NB_REGS];
} iss_pulpv2_t;


typedef struct iss_rnnext_s
{
  iss_insn_t *sdot_insn;
  iss_reg_t sdot_prefetch_0;
  iss_reg_t sdot_prefetch_1;
} iss_rnnext_t;


typedef struct iss_cpu_s {
  iss_prefetcher_t decode_prefetcher;
  iss_prefetcher_t prefetcher;
  iss_insn_cache_t insn_cache;
  iss_insn_t *current_insn;
  iss_insn_t *prev_insn;
  iss_insn_t *stall_insn;
  iss_regfile_t regfile;
  iss_cpu_state_t state;
  iss_config_t config;
  iss_irq_t irq;
  iss_csr_t csr;
  iss_pulpv2_t pulpv2;
  iss_rnnext_t rnnext;
} iss_cpu_t;

#endif
