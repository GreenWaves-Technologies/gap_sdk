/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @file
 * @brief Common defines and macros for firmware developed by Nordic Semiconductor.
 */

#ifndef NORDIC_COMMON_H__
#define NORDIC_COMMON_H__

/** The upper 8 bits of a 32 bit value */
//lint -emacro(572,MSB) // Suppress warning 572 "Excessive shift value"
#define MSB(a) (((a) & 0xFF000000) >> 24)
/** The lower 8 bits (of a 32 bit value) */
#define LSB(a) ((a) & 0x000000FF)

/** The upper 8 bits of a 16 bit value */
//lint -emacro(572,MSB_16) // Suppress warning 572 "Excessive shift value"
#define MSB_16(a) (((a) & 0xFF00) >> 8)
/** The lower 8 bits (of a 16 bit value) */
#define LSB_16(a) ((a) & 0x00FF)

/** Leaves the minimum of the two 32-bit arguments */
/*lint -emacro(506, MIN) */ /* Suppress "Constant value Boolean */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/** Leaves the maximum of the two 32-bit arguments */
/*lint -emacro(506, MAX) */ /* Suppress "Constant value Boolean */
#define MAX(a, b) ((a) < (b) ? (b) : (a))

/** Concatenates two parameters. Useful as a second level of indirection,
 *  when a parameter can be macro itself. */
#define CONCAT_2(p1, p2)      p1##p2
/** Concatenates three parameters. Useful as a second level of indirection,
 *  when a parameter can be macro itself. */
#define CONCAT_3(p1, p2, p3)  p1##p2##p3

/**@brief Set a bit in the uint32 word.
 *
 * @param[in] W  Word whose bit is being set.
 * @param[in] B  Bit number in the word to be set.
 */
#define SET_BIT(W,B)  ((W) |= (uint32_t)(1U << (B)))


/**@brief Clears a bit in the uint32 word.
 *
 * @param[in] W   Word whose bit is to be cleared.
 * @param[in] B   Bit number in the word to be cleared.
 */
#define CLR_BIT(W, B) ((W) &= (~((uint32_t)1 << (B))))


/**@brief Checks if a bit is set.
 *
 * @param[in] W   Word whose bit is to be checked.
 * @param[in] B   Bit number in the word to be checked.
 *
 * @retval 1 if bit is set.
 * @retval 0 if bit is not set.
 */
#define IS_SET(W,B) (((W) >> (B)) & 1)

#define BIT_0 0x01 /**< The value of bit 0 */
#define BIT_1 0x02 /**< The value of bit 1 */
#define BIT_2 0x04 /**< The value of bit 2 */
#define BIT_3 0x08 /**< The value of bit 3 */
#define BIT_4 0x10 /**< The value of bit 4 */
#define BIT_5 0x20 /**< The value of bit 5 */
#define BIT_6 0x40 /**< The value of bit 6 */
#define BIT_7 0x80 /**< The value of bit 7 */
#define BIT_8 0x0100 /**< The value of bit 8 */
#define BIT_9 0x0200 /**< The value of bit 9 */
#define BIT_10 0x0400 /**< The value of bit 10 */
#define BIT_11 0x0800 /**< The value of bit 11 */
#define BIT_12 0x1000 /**< The value of bit 12 */
#define BIT_13 0x2000 /**< The value of bit 13 */
#define BIT_14 0x4000 /**< The value of bit 14 */
#define BIT_15 0x8000 /**< The value of bit 15 */
#define BIT_16 0x00010000 /**< The value of bit 16 */
#define BIT_17 0x00020000 /**< The value of bit 17 */
#define BIT_18 0x00040000 /**< The value of bit 18 */
#define BIT_19 0x00080000 /**< The value of bit 19 */
#define BIT_20 0x00100000 /**< The value of bit 20 */
#define BIT_21 0x00200000 /**< The value of bit 21 */
#define BIT_22 0x00400000 /**< The value of bit 22 */
#define BIT_23 0x00800000 /**< The value of bit 23 */
#define BIT_24 0x01000000 /**< The value of bit 24 */
#define BIT_25 0x02000000 /**< The value of bit 25 */
#define BIT_26 0x04000000 /**< The value of bit 26 */
#define BIT_27 0x08000000 /**< The value of bit 27 */
#define BIT_28 0x10000000 /**< The value of bit 28 */
#define BIT_29 0x20000000 /**< The value of bit 29 */
#define BIT_30 0x40000000 /**< The value of bit 30 */
#define BIT_31 0x80000000 /**< The value of bit 31 */

#define UNUSED_VARIABLE(X)  ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)

#endif // NORDIC_COMMON_H__
