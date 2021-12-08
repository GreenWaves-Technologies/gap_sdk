/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */


	.global __rt_thread_start
__rt_thread_start:
	mv 	  a0, s1
	mv    ra, s2
	jr    s0


	.global __rt_thread_switch
__rt_thread_switch:

    sw    ra,  0*4(a0)
    sw    s0,  1*4(a0)
    sw    s1,  2*4(a0)
    sw    s2,  3*4(a0)
    sw    s3,  4*4(a0)
    sw    s4,  5*4(a0)
    sw    s5,  6*4(a0)
    sw    s6,  7*4(a0)
    sw    s7,  8*4(a0)
    sw    s8,  9*4(a0)
    sw    s9, 10*4(a0)
    sw    s10,11*4(a0)
    sw    s11,12*4(a0)
    sw    sp, 13*4(a0)

    lw    ra,  0*4(a1)
    lw    s0,  1*4(a1)
    lw    s1,  2*4(a1)
    lw    s2,  3*4(a1)
    lw    s3,  4*4(a1)
    lw    s4,  5*4(a1)
    lw    s5,  6*4(a1)
    lw    s6,  7*4(a1)
    lw    s7,  8*4(a1)
    lw    s8,  9*4(a1)
    lw    s9, 10*4(a1)
    lw    s10,11*4(a1)
    lw    s11,12*4(a1)
    lw    sp, 13*4(a1)

    ret
