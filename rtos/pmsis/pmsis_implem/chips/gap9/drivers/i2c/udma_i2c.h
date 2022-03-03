/*
 * Copyright (C) 2022 GreenWaves Technologies
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

#pragma once

typedef enum i2c_mode {
    PI_I2C_STD_MODE,
    PI_I2C_FAST_MODE,
    PI_I2C_FAST_MODE_PLUS,
    PI_I2C_NB_MODES
} pi_i2c_mode_e;

// TEMPORARY, until can be generated
#define I2C_STATUS_FOLL_EOF_RCV_EVENT 2
#define I2C_STATUS_FOLL_EOF_SND_EVENT 3
#define I2C_STATUS_FOLL_ERROR_ARLO_EVENT 4
#define I2C_STATUS_FOLL_ERROR_FRAMING_EVENT 5
#define I2C_FLAG_CMD_EVENT_I 19
#define I2C_FLAG_FOLL_UNLOCK_EVENT_O 14
#define I2C_FLAG_FOLL_PURGE_EVENT_O 15
#define I2C_STATUS_ERROR_NACK_EVENT 16
#define I2C_STATUS_ERROR_ARLO_EVENT 17
#define I2C_STATUS_ERROR_FRAMING_EVENT 18
#define I2C_FLAG_UNLOCK_EVENT_O 22
#define I2C_FLAG_PURGE_EVENT_O 23
#define I2C_FLAG_SOFT_RESET_EVENT 24
#define I2C_FLAG_PRESC_DIV10_EVENT_O 25

#define I2C_CMD_TIMING(T) ((0x10 << 24) | (T))
#define I2C_CMD_EVENT(T) ((0x41 << 24))


#define CMD_FOLL_ADDR(match_id,push_en,addr_10_bit,slave_addr,mask,sof,eof) \
    ((0x20 << 24) | (match_id << 22) | (push_en << 21) | (eof << 20) \
    | (sof << 19) | (eof << 18) | (sof << 17) | (addr_10_bit << 16) \
    | ((!(addr_10_bit)) << 15) | (mask << 10) \
    | ((addr_10_bit) ? (slave_addr) : ((slave_addr)>>1) << 0))

/* enable automatic sending of stop when receiving a nack error */
#define __I2C_NACK_STOP ((1 << 23))


#define I2C_CMD_LEAD_START(T)  ((0x30 << 24) | __I2C_NACK_STOP)
#define I2C_CMD_MISC_WAIT(T)   ((0x3<<24))
#define I2C_CMD_NOP(T)         ((0x0<<24))
// add -1, well, hw guys saving on bits...
#define I2C_CMD_RPT(T) ((0x02 << 24) | (((T)&0xFFFF)))
#define I2C_CMD_LEAD_SEND_IMM(T) ((0x32 << 24) | __I2C_NACK_STOP | ((T)&0xFF))
#define I2C_CMD_LEAD_SEND_IMM_ADDR(IS_10BITS, T) ((0x37 << 24) | __I2C_NACK_STOP | (IS_10BITS << 15) | ((T)&0xFFFF))
#define I2C_CMD_MISC_RECEIVE(T) ((0x33 << 24) | __I2C_NACK_STOP)
#define I2C_CMD_MISC_RECEIVE_LAST(T) ((0x34 << 24) | __I2C_NACK_STOP)
#define I2C_CMD_MISC_SEND(T) ((0x31 << 24) | __I2C_NACK_STOP)
#define I2C_CMD_STOP(T) ((0x36 << 24) | __I2C_NACK_STOP)
#define I2C_CMD_UDMA_TX_CHAN_CFG(T) (( 0x50 << 24 ) | (T))
#define I2C_CMD_UDMA_RX_CHAN_CFG(T) (( 0x51 << 24 ) | (T))

#define I2C_CHAN_ADDR_REG 0x0
#define I2C_CHAN_SIZE_REG 0x2
#define I2C_CHAN_CFG_REG 0x7



/** get_current value of event in i2c event register. Defined as a macro to work with O0  **/
#define __pi_i2c_get_event_status(base, event_id) __BITEXTRACT(udma_i2c_status_reg_idx_get(base), 1, event_id)
