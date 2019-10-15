/*
 *    Copyright (c) 2011-2014, http://www.proprojects.wordpress.com
 *    All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without modification,
 *    are permitted provided that the following conditions are met:
 *
 *    1.- Redistributions of source code must retain the above copyright notice,
 *          this list of conditions and the following disclaimer.
 *    2.- Redistributions in binary form must reproduce the above copyright notice,
 *          this list of conditions and the following disclaimer in the documentation
 *          and/or other materials provided with the distribution.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *    WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*********************************************************************
 * By:              Omar Gurrola
 * Company:         http://proprojects.wordpress.com
 * Processor:       PIC12
 * Compiler:        XC8 v1.32
 * File Name:       m24fc1025.h
 * Created On:      July 30, 2014, 4:29 PM
 * Description:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/30/14    Initial version
 *********************************************************************/

#ifndef __M24FC1025_H
#define    __M24FC1025_H

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

// MBED OS
#include "cmsis_os2.h"
#include "i2c_api.h"

/** INTERFACE CONFIGURATION ****************************************/
#define M24FC1025_STATIC_ADDRESS 0b1010 // Static address (4 MSB)

/** PUBLIC FUNCTIONS ***********************************************/
/**
 * Initialize the 24FC1025 I2C memory
 * @param addr_2b Physical 2b address
 */
void m24fc1025_init(uint8_t addr_2b);
/**
 * Set the 2b physical address of the device
 * @param addr_2b Physical 2b address
 */
void m24fc1025_set_slave_addr(uint8_t addr_2b);
/**
 * Write a byte to the specified address
 * @param addr_17b 17b Address
 * @param value Value to write
 */
void m24fc1025_write_byte(i2c_t *i2c, uint32_t addr_17b, uint8_t value);
/**
 * This function checks if memory is busy writing
 * @return 1 = true = memory is busy, 0 = false = memory is not busy
 */
bool m24fc1025_is_write_busy(i2c_t *i2c);
/**
 * Read a byte from memory
 * @param addr_17b 17b address to read from
 * @return Read value
 */
uint8_t m24fc1025_read_byte(i2c_t *i2c, uint32_t addr_17b);

#endif    /* __M24FC1025_H */
