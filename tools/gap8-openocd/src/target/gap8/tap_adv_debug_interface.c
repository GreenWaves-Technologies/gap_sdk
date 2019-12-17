/***************************************************************************
 *   Copyright (C) 2013 by Franck Jullien                                  *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gap8_tap.h"
#include "gap8.h"

#include <jtag/jtag.h>

#define GAP8_TAP_INST_DEBUG	0x8

static int gap8_tap_adv_dbg_itf_init(struct gap8_jtag *jtag_info)
{
	LOG_DEBUG("Initialising GAP8 JTAG TAP");
	printf("Initialising GAP8 JTAG TAP\n");

	/* Put TAP into state where it can talk to the debug interface
	 * by shifting in correct value to IR.
	 */

	/* Ensure TAP is reset - maybe not necessary*/
	jtag_add_reset(1,0);
	jtag_add_sleep(100000);
	jtag_add_reset(0,0);
	jtag_add_sleep(100000);
	jtag_execute_queue();
#if 1
	// Just notify openocd to put tap in idle state
	// make a read on 0x2 reg (tap id)
	struct jtag_tap *tap = jtag_info->tap;
	struct scan_field field;
	uint8_t ir_value = 0x2;

	field.num_bits = tap->ir_length;
	field.out_value = &ir_value;
	field.in_value = NULL;

	jtag_add_ir_scan(tap, &field, TAP_IDLE);

	return jtag_execute_queue();
#else
	return ERROR_OK;
#endif
}

static struct gap8_tap_ip adv_dbg_itf_tap = {
	.name = "gap8_adv_debug_itf",
	.init = gap8_tap_adv_dbg_itf_init,
};

int gap8_tap_adv_dbg_itf_register(void)
{
	list_add_tail(&adv_dbg_itf_tap.list, &gap8_tap_list);
	return 0;
}
