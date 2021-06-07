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
