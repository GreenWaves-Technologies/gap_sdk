/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
 */

#ifndef TRACE_DUMPER_PROFILING_HPP
#define TRACE_DUMPER_PROFILING_HPP

// used as indices, so don't play with the = 0 value
// stall_strings must correspond line by line, so don't shuffle the lines either
enum stall_e {
	JMP_STALL = 0,
	I_MISS,
	MISALIGNED,
	LD_STALL,
  N_STALL_REASONS
};
const char* const stall_strings[] = {
	"pcer_jmp_stall",
	"pcer_imiss",
	"misaligned",
	"pcer_ld_stall"
};
// depends on the number of parenthesis in the stall regex
#define STALL_NMATCH (N_STALL_REASONS + 6)

const char* const dma_c_regex = "^.*/dma/channel_[0-9]+$";
const char* const stall_cluster_regex = "^.*/cluster/pe([0-9]+)/";
const char* const stall_fc_regex = "^.*/chip/soc/fc/";

typedef struct {
  stall_e type;
  int core_id;
} stall_trace_t;

typedef struct {
  stall_trace_t trace;
  uint32_t pc;
  int cycle_penalty;
} stall_event_t;

#endif // TRACE_DUMPER_PROFILING_HPP
