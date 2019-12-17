/***************************************************************************
 *   Copyright (C) 2013 Franck Jullien                                     *
 *   elec4fun@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef OPENOCD_TARGET_GAP8_DU_H
#define OPENOCD_TARGET_GAP8_DU_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CPU_STALL	0
#define CPU_UNSTALL	1

#define CPU_RESET	0
#define CPU_NOT_RESET	1

int gap8_adv_dbg_unit_register(void);

/* Linear list over all available gap8 debug unit */
extern struct list_head gap8_dbg_unit_list;

struct gap8_dbg_unit {
	const char *name;
	struct list_head list;
	int options;

	int (*gap8_jtag_init)(struct gap8_jtag *jtag_info);

	int (*gap8_is_cpu_running)(struct gap8_jtag *jtag_info, int *running);

	int (*gap8_cpu_stall)(struct gap8_jtag *jtag_info, int action);

	int (*gap8_cpu_reset)(struct gap8_jtag *jtag_info, int action);

	int (*gap8_jtag_read_cpu)(struct gap8_jtag *jtag_info,
				  uint32_t addr, int count, uint32_t *value);

	int (*gap8_jtag_write_cpu)(struct gap8_jtag *jtag_info,
				   uint32_t addr, int count, const uint32_t *value);

	int (*gap8_jtag_read_memory)(struct gap8_jtag *jtag_info, uint32_t addr, uint32_t size,
					int count, uint8_t *buffer);

	int (*gap8_jtag_write_memory)(struct gap8_jtag *jtag_info, uint32_t addr, uint32_t size,
					 int count, const uint8_t *buffer);
};

static inline struct gap8_dbg_unit *gap8_jtag_to_du(struct gap8_jtag *jtag_info)
{
	return (struct gap8_dbg_unit *)jtag_info->du_core;
}

static inline struct gap8_dbg_unit *gap8_to_du(struct gap8_info *gap8)
{
	struct gap8_jtag *jtag = &gap8->jtag;
	return (struct gap8_dbg_unit *)jtag->du_core;
}
#endif /* OPENOCD_TARGET_GAP8_DU_H */
