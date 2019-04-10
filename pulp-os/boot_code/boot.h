/* 
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou (germain.haugou@gmail.com)
 */

#ifndef __BOOT_H__
#define __BOOT_H__

void wait_clock_ref(int nbTicks);

int WaitStableXtal(int AllowedDelta, int MinStablePeriod, int MaxPeriod);

#endif