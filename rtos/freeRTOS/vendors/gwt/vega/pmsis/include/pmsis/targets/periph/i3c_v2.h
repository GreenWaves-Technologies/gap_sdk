
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __I3C_V2_H__
#define __I3C_V2_H__

#ifndef LANGUAGE_ASSEMBLY

#include <stdint.h>

#endif




//
// REGISTERS
//

// I3C Master control.
#define I3C_MST_CNTL_EN_REG_OFFSET               0x0

// Data length or Data byte value to be sent along with the CCC or HDR command.
#define I3C_CMD_TR_REQ_REG_1_OFFSET              0x4

// Command type, Transaction ID, CCC code and slave address. Writing to this register is considered as Doorbell for the master to initiate the transfer.
#define I3C_CMD_TR_REQ_REG_2_OFFSET              0x8

// Success or failure of the command, Transaction ID and the remaining data.
#define I3C_RESP_REG_OFFSET                      0x10

// In-Band interrupt status, timestamping information present, dynamic address of the slave or Hot Join request.
#define I3C_IBI_RESP_REG_OFFSET                  0x14

// Data received from the slave during the IBI process.
#define I3C_IBI_DATA_REG_OFFSET                  0x18

// Received data FIFO (Data_RX_FIFO).
#define I3C_DATA_RX_FIFO_REG_OFFSET              0x1c

// Sending data FIFO (Data_TX_FIFO).
#define I3C_DATA_TX_FIFO_REG_OFFSET              0x20

// Status of the event happened during the transfer.
#define I3C_IRQ_STATUS_REG_OFFSET                0x30

// Timing for Clock after Start condition (TCAS) is the time (in Number of Reference clocks) after the start condition after which the SCL pin can go low.
#define I3C_TCAS_TIMER_REG_OFFSET                0x40

// Low period of SCL pin in Open-Drain mode (in Number of Reference clock) after the SCL pin can go high.
#define I3C_TLOW_OD_TIMER_REG_OFFSET             0x44

// High period of SCL pin in Open-Drain mode (in Number of Reference clock) after the SCL pin can go low.
#define I3C_THIGH_OD_TIMER_REG_OFFSET            0x48

// Low period of SCL pin in Push-Pull mode (in Number of Reference clock) after the SCL pin can go high.
#define I3C_TLOW_PP_TIMER_REG_OFFSET             0x4c

// High period of SCL pin in Push-Pull mode (in Number of Reference clock) after the SCL pin can go low.
#define I3C_THIGH_PP_TIMER_REG_OFFSET            0x50

// SDA data setup time during both Open-Drain/Push-Pull mode (in Number of Reference clock).
#define I3C_TDS_TIMER_REG_OFFSET                 0x54

// SDA data hold time during the Push-Pull mode (in Number of Reference clock).
#define I3C_THD_PP_TIMER_REG_OFFSET              0x58

// Clock time before Stop condition.
#define I3C_TCBP_TIMER_REG_OFFSET                0x5c

// Clock time before Repeated start condition.
#define I3C_TCBSR_TIMER_REG_OFFSET               0x60

// SDA data hold time during the Push-Pull mode (in Number of Reference clock) in DDR Data rate.
#define I3C_THD_DDR_TIMER_REG_OFFSET             0x64

// Bus free time between the Stop condition and the next start condition (in Number of Reference clock).
#define I3C_BUS_FREE_TIMER_REG_OFFSET            0x68

// Time to keep the SDA and SCL pin to High (in Number of Reference clock).
#define I3C_BUS_AVAIL_TIMER_REG_OFFSET           0x6c

// Extended duration of the bus free condition after the Stop condition (in Number of Reference clock) to enable the device to drive the Hot Join request.
#define I3C_TIDLE_TIMER_REG_OFFSET               0x70

// Maximum time the slave needs to drive the bus during the ACK/read data after the clock change.
#define I3C_TSCO_TIMER_REG_OFFSET                0x74

// SDA data setup time during both Open-Drain (in Number of Reference clock) for a Repeated Start.
#define I3C_TSU_STA_TIMER_REG_OFFSET             0x90

// SDA data hold time during the Open Drain mode (in Number of Reference clock).
#define I3C_THD_STA_TIMER_REG_OFFSET             0x94

// Low period of SCL pin in Open Drain mode during Legacy I2c Mode (in Number of Reference clock) after the timer reached the SCL pin can go high.
#define I3C_TLOW_TIMER_REG_OFFSET                0x98

// High period of SCL pin in Open Drain Mode for Legacy I2C (in Number of Reference clock) after this timer count reached the SCL pin can go low.
#define I3C_THIGH_TIMER_REG_OFFSET               0x9c

// Data hold time in Open Drain Mode for Legacy I2C (in Number of Reference clock) after this timer count reached the SDA pin can change its value.
#define I3C_TVD_DATA_TIMER_REG_OFFSET            0xa0

// SDA data setup time during Open-Drain (in Number of Reference clock) for Stop condition.
#define I3C_TSU_STOP_TIMER_REG_OFFSET            0xa8

// Device type, IBI handling and Dynamic address to be set and Static address of the slave0.
#define I3C_DEVICE_ADDR_TABLE_REG0_OFFSET        0x100

// Device type, IBI handling and Dynamic address to be set and Static address of the slave1.
#define I3C_DEVICE_ADDR_TABLE_REG1_OFFSET        0x104

// Device type, IBI handling and Dynamic address to be set and Static address of the slave2.
#define I3C_DEVICE_ADDR_TABLE_REG2_OFFSET        0x108

// Device type, IBI handling and Dynamic address to be set and Static address of the slave3.
#define I3C_DEVICE_ADDR_TABLE_REG3_OFFSET        0x10c

// Device type, IBI handling and Dynamic address to be set and Static address of the slave4.
#define I3C_DEVICE_ADDR_TABLE_REG4_OFFSET        0x110

// Device type, IBI handling and Dynamic address to be set and Static address of the slave5.
#define I3C_DEVICE_ADDR_TABLE_REG5_OFFSET        0x114

// Device type, IBI handling and Dynamic address to be set and Static address of the slave6.
#define I3C_DEVICE_ADDR_TABLE_REG6_OFFSET        0x118

// Device type, IBI handling and Dynamic address to be set and Static address of the slave7.
#define I3C_DEVICE_ADDR_TABLE_REG7_OFFSET        0x11c

// Device type, IBI handling and Dynamic address to be set and Static address of the slave8.
#define I3C_DEVICE_ADDR_TABLE_REG8_OFFSET        0x120

// Device type, IBI handling and Dynamic address to be set and Static address of the slave9.
#define I3C_DEVICE_ADDR_TABLE_REG9_OFFSET        0x124

// Device type, IBI handling and Dynamic address to be set and Static address of the slave10.
#define I3C_DEVICE_ADDR_TABLE_REG10_OFFSET       0x128

// Device type, IBI handling and Dynamic address to be set and Static address of the slave11.
#define I3C_DEVICE_ADDR_TABLE_REG11_OFFSET       0x12c

// BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_0_OFFSET      0x200

// BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_0_OFFSET      0x204

// BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_0_OFFSET      0x208

// BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_0_OFFSET      0x20c

// BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_1_OFFSET      0x210

// BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_1_OFFSET      0x214

// BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_1_OFFSET      0x218

// BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_1_OFFSET      0x21c

// BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_2_OFFSET      0x220

// BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_2_OFFSET      0x224

// BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_2_OFFSET      0x228

// BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_2_OFFSET      0x22c

// BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_3_OFFSET      0x230

// BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_3_OFFSET      0x234

// BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_3_OFFSET      0x238

// BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_3_OFFSET      0x23c

// BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_4_OFFSET      0x240

// BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_4_OFFSET      0x244

// BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_4_OFFSET      0x248

// BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_4_OFFSET      0x24c

// BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_5_OFFSET      0x250

// BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_5_OFFSET      0x254

// BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_5_OFFSET      0x258

// BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_5_OFFSET      0x25c

// BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_6_OFFSET      0x260

// BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_6_OFFSET      0x264

// BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_6_OFFSET      0x268

// BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_6_OFFSET      0x26c

// BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_7_OFFSET      0x270

// BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_7_OFFSET      0x274

// BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_7_OFFSET      0x278

// BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_7_OFFSET      0x27c

// BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_8_OFFSET      0x280

// BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_8_OFFSET      0x284

// BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_8_OFFSET      0x288

// BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_8_OFFSET      0x28c

// BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_9_OFFSET      0x290

// BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_9_OFFSET      0x294

// BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_9_OFFSET      0x298

// BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_9_OFFSET      0x29c

// BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_10_OFFSET     0x2a0

// BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_10_OFFSET     0x2a4

// BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_10_OFFSET     0x2a8

// BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_10_OFFSET     0x2ac

// BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG0_11_OFFSET     0x2b0

// BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG1_11_OFFSET     0x2b4

// BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG2_11_OFFSET     0x2b8

// BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
#define I3C_DEVICE_CHAR_TABLE_REG3_11_OFFSET     0x2bc



//
// REGISTERS FIELDS
//

// Master Reset enable (Soft reset) : To enable the I3C Master controller to issue Soft reset during any transfers. Soft reset will be applied after the completion of ACK/NACK state. -1’b0 : I3C Master reset is disabled -1’b1 : I3C master reset is enabled (access: R/W)
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_BIT                        1
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_WIDTH                      1
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_MASK                       0x2

// Master control enable : To enable the I3C Master controller for any transfers. -1’b0 : I3C Master is disabled -1’b1 : I3C master is enabled (access: R/W)
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_BIT                    0
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_WIDTH                  1
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_MASK                   0x1

// Data length or Data byte value. This register contains Data length or Data byte value depending on the type of command being initiated. (access: W)
#define I3C_CMD_TR_REQ_REG_1_DATA_BIT                                0
#define I3C_CMD_TR_REQ_REG_1_DATA_WIDTH                              32
#define I3C_CMD_TR_REQ_REG_1_DATA_MASK                               0xffffffff

// Command request. This register contains information about the command type, Transaction ID, slave address and the type of CCC or HDR to be initiated. (access: W)
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_BIT                             0
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_WIDTH                           32
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_MASK                            0xffffffff

// Response register. Contains values of the success/failure of the command, transaction ID of the completed command & remaining data (for write transfer) or received data length (for read transfer) or number of remaining slaves (for dynamic address command). (access: R)
#define I3C_RESP_REG_RESP_REG_BIT                                    0
#define I3C_RESP_REG_RESP_REG_WIDTH                                  32
#define I3C_RESP_REG_RESP_REG_MASK                                   0xffffffff

// IBI status. Indicates how the IBI is handled : -1’b0 : Indicates IBI is handled with ACK -1’b1 : Indicates IBI is handled with NACK (access: R)
#define I3C_IBI_RESP_REG_IBI_STS_BIT                                 31
#define I3C_IBI_RESP_REG_IBI_STS_WIDTH                               1
#define I3C_IBI_RESP_REG_IBI_STS_MASK                                0x80000000

// IBI timestamp present for IBI : -1’b1 : IBI is timestamped -1’b0 : IBI is not timestamped (access: R)
#define I3C_IBI_RESP_REG_TS_PRESENT_BIT                              25
#define I3C_IBI_RESP_REG_TS_PRESENT_WIDTH                            1
#define I3C_IBI_RESP_REG_TS_PRESENT_MASK                             0x2000000

// IBI received ID. Contains slave address for IBI. Contains Hot Join ID for the Hot-Join IBI (access: R)
#define I3C_IBI_RESP_REG_IBI_ID_BIT                                  8
#define I3C_IBI_RESP_REG_IBI_ID_WIDTH                                8
#define I3C_IBI_RESP_REG_IBI_ID_MASK                                 0xff00

// IBI Data length. Number of bytes in the IBI requests received. (access: R)
#define I3C_IBI_RESP_REG_DATA_LEN_BIT                                0
#define I3C_IBI_RESP_REG_DATA_LEN_WIDTH                              8
#define I3C_IBI_RESP_REG_DATA_LEN_MASK                               0xff

// IBI Data. Data received during the IBI process is stored in FIFO and is sent out to the Software using this register. (access: R)
#define I3C_IBI_DATA_REG_IBI_DATA_BIT                                0
#define I3C_IBI_DATA_REG_IBI_DATA_WIDTH                              32
#define I3C_IBI_DATA_REG_IBI_DATA_MASK                               0xffffffff

// All the Data received from the slave is stored in FIFO. On request by the software, the FIFO read and the values are provided. (access: R)
#define I3C_DATA_RX_FIFO_REG_DATA_RX_BIT                             0
#define I3C_DATA_RX_FIFO_REG_DATA_RX_WIDTH                           32
#define I3C_DATA_RX_FIFO_REG_DATA_RX_MASK                            0xffffffff

// Write data to be sent from the Master to the slave. Software writes this register for sending the write data to the slave. (access: W)
#define I3C_DATA_TX_FIFO_REG_DATA_TX_BIT                             0
#define I3C_DATA_TX_FIFO_REG_DATA_TX_WIDTH                           32
#define I3C_DATA_TX_FIFO_REG_DATA_TX_MASK                            0xffffffff

// Indicates the Master controller issued soft reset. (access: R)
#define I3C_IRQ_STATUS_REG_RST_COMPLETION_BIT                        8
#define I3C_IRQ_STATUS_REG_RST_COMPLETION_WIDTH                      1
#define I3C_IRQ_STATUS_REG_RST_COMPLETION_MASK                       0x100

// Indicates the IBI payload is full. (access: R)
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_BIT                 7
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_WIDTH               1
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_MASK                0x80

// Indicates the IBI request is received from slave. (access: R)
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_BIT                     6
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_WIDTH                   1
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_MASK                    0x40

// Indicates the response completion FIFO is full. (access: R)
#define I3C_IRQ_STATUS_REG_RESP_COMPL_BIT                            5
#define I3C_IRQ_STATUS_REG_RESP_COMPL_WIDTH                          1
#define I3C_IRQ_STATUS_REG_RESP_COMPL_MASK                           0x20

// Indicates the Data Rx FIFO Full. (access: R)
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_BIT                     4
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_WIDTH                   1
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_MASK                    0x10

// Indicates the command transfer request FIFO Full. (access: R)
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_BIT                     3
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_WIDTH                   1
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_MASK                    0x8

// Indicates the Data Tx FIFO is empty. (access: R)
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_BIT                    2
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_WIDTH                  1
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_MASK                   0x4

// Indicates the Data Tx FIFO is full. (access: R)
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_BIT                     1
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_WIDTH                   1
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_MASK                    0x2

// Indicates the response completion is done. (access: R)
#define I3C_IRQ_STATUS_REG_RESP_DONE_BIT                             0
#define I3C_IRQ_STATUS_REG_RESP_DONE_WIDTH                           1
#define I3C_IRQ_STATUS_REG_RESP_DONE_MASK                            0x1

// Timing of the SCL pin to go low after the start condition (Number of ref_clk). As per specification, Min time value is 38.4ns. For 100MHz ref clock, total 4 clock periods are required. So, the value of the register is 0x3. (access: R/W)
#define I3C_TCAS_TIMER_REG_TCAS_TIMER_BIT                            0
#define I3C_TCAS_TIMER_REG_TCAS_TIMER_WIDTH                          32
#define I3C_TCAS_TIMER_REG_TCAS_TIMER_MASK                           0xffffffff

// Low period of the SCL clock pin (Number of ref_clk). As per specification, Min time value is 200ns + fall time of SDA signal. For 100MHz ref clock, total 24 clock periods are required. So, the value of the register is 0x23. (access: R/W)
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_BIT                      0
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_WIDTH                    5
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_MASK                     0x1f

// High period of the SCL clock pin (Number of ref_clk). As per specification, Max time value is 41ns. For 100MHz ref clock, Max. of total 4 clock periods is allowed. So, the value of the register is 0x2. (access: R/W)
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_BIT                    0
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_WIDTH                  5
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_MASK                   0x1f

// Low period of the SCL clock pin (Number of ref_clk) in Push pull mode. As per specification, Min time value is 24ns. For 100MHz ref clock, total 3 clock periods are required. So, the value of the register is 0x3. (access: R/W)
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_BIT                      0
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_WIDTH                    5
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_MASK                     0x1f

// High period of the SCL clock pin (Number of ref_clk). As per specification, Max time value is 41ns. For 100MHz ref clock, Max. of total 4 clock periods are required. So, the value of the register is 0x2. (access: R/W)
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_BIT                    0
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_WIDTH                  5
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_MASK                   0x1f

// SDA Set up time (Number of ref_clk). As per specification, Min time value is 3ns. For 100MHz ref clock, Max. of total 1 clock period is required. So, the value of the register is 0x1. (access: R/W)
#define I3C_TDS_TIMER_REG_TDS_TIMER_BIT                              0
#define I3C_TDS_TIMER_REG_TDS_TIMER_WIDTH                            3
#define I3C_TDS_TIMER_REG_TDS_TIMER_MASK                             0x7

// SDA Hold time (Number of ref_clk). As per specification, Min time value is 6ns. For 100MHz ref clock, Min. of total 1 clock period is required. So, the value of the register is 0x1. (access: R/W)
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_BIT                        0
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_WIDTH                      3
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_MASK                       0x7

// To signal STOP condition, the master should change the SDA pin after this time of SCL clock edge (Number of ref_clk). As per specification, Min time value is 19.2ns. For 100MHz ref clock, Max. of total 2 clock periods is required. So, the value of the register is 0x1. (access: R/W)
#define I3C_TCBP_TIMER_REG_TCBP_TIMER_BIT                            0
#define I3C_TCBP_TIMER_REG_TCBP_TIMER_WIDTH                          5
#define I3C_TCBP_TIMER_REG_TCBP_TIMER_MASK                           0x1f

// To signal Repeated start condition, the master should change the SDA pin after this time of SCL clock edge (Number of ref_clk). As per specification, Min time value is 19.2ns. For 100MHz ref clock, Max. of total 2 clock periods is required. So, the value of the register is 0x1. (access: R/W)
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_BIT                          0
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_WIDTH                        3
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_MASK                         0x7

// SDA Hold time (Number of ref_clk). As per specification, Min time value is 6ns. For 100MHz ref clock, Min. of total 1 clock period is required. So, the value of the register is 0x1. (access: R/W)
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_BIT                      0
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_WIDTH                    3
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_MASK                     0x7

// Bus free time after the STOP condition and the Start condition. As per specification, Min. value of this period is 1us. (access: R/W)
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_BIT                    0
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_WIDTH                  32
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_MASK                   0xffffffff

// Bus aval time to drive SCL and SDA pin to high. As per specification, Min. value of this period is 1us. (access: R/W)
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_BIT                  0
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_WIDTH                32
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_MASK                 0xffffffff

// Bus aval time to drive SCL and SDA pin to high. As per specification, Min. value of this period is 1ms. (access: R/W)
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_BIT                          0
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_WIDTH                        32
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_MASK                         0xffffffff

// Maximum time for the slave to release bus after the clock change. As per specification, Max. value is 12ns. (access: R/W)
#define I3C_TSCO_TIMER_REG_TSCO_TIMER_BIT                            0
#define I3C_TSCO_TIMER_REG_TSCO_TIMER_WIDTH                          3
#define I3C_TSCO_TIMER_REG_TSCO_TIMER_MASK                           0x7

// SDA Set up time (Number of ref_clk) for a Repeated Start during Legacy I2C Mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E. (access: R/W)
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_BIT                      0
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_WIDTH                    9
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_MASK                     0x1ff

// SDA Hold time (Number of ref_clk) after Start/Repeated Start. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E. (access: R/W)
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_BIT                      0
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_WIDTH                    9
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_MASK                     0x1ff

// Low period of the SCL clock pin (Number of ref_clk) in Legacy I2C Mode. As per specification, Min time value is 1300ns for Legacy FM Mode. As per specification, Min time value is 500ns for Legacy FM+ Mode. For 100MHz ref clock, total 130 clock periods are required. For FM Mode, the value of the register is 0x8C. For FM+ Mode, the value of this register is 0x3c. (access: R/W)
#define I3C_TLOW_TIMER_REG_TLOW_TIMER_BIT                            0
#define I3C_TLOW_TIMER_REG_TLOW_TIMER_WIDTH                          16
#define I3C_TLOW_TIMER_REG_TLOW_TIMER_MASK                           0xffff

// High period of the SCL clock pin (Number of ref_clk) in Legacy I2C mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E. (access: R/W)
#define I3C_THIGH_TIMER_REG_THIGH_TIMER_BIT                          0
#define I3C_THIGH_TIMER_REG_THIGH_TIMER_WIDTH                        16
#define I3C_THIGH_TIMER_REG_THIGH_TIMER_MASK                         0xffff

// Data Hold time (Number of ref_clk) in Legacy I2C mode. As per specification, Min Data setup time value is 100ns for Legacy FM Mode. So, hold time will be min 600 ns, so that the Legacy system works correctly. As per specification, Min Data setup time value is 50ns for Legacy FM+ Mode. So, hold time will be min 260 ns, so that the Legacy system works correctly. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E. (access: R/W)
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_BIT                    0
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_WIDTH                  10
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_MASK                   0x3ff

// SDA Set up time (Number of ref_clk) for Stop condition during Legacy I2C Mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E. (access: R/W)
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_BIT                    0
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_WIDTH                  9
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_MASK                   0x1ff

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_BIT                   31
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_MASK                  0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_BIT                  16
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_WIDTH                8
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_MASK                 0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_BIT                     15
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_WIDTH                   1
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_MASK                    0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_BIT                   13
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_MASK                  0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_BIT                   12
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_WIDTH                 1
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_MASK                  0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_BIT                   0
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_WIDTH                 7
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_MASK                  0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_BIT                  31
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_MASK                 0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_BIT                 16
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_WIDTH               8
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_MASK                0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_BIT                    15
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_WIDTH                  1
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_MASK                   0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_BIT                  13
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_MASK                 0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_BIT                  12
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_MASK                 0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_BIT                  0
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_WIDTH                7
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_MASK                 0x7f

// Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_BIT                  31
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_MASK                 0x80000000

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_BIT                 16
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_WIDTH               8
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_MASK                0xff0000

// Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_BIT                    15
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_WIDTH                  1
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_MASK                   0x8000

// In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave. (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_BIT                  13
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_MASK                 0x2000

// IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_BIT                  12
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_WIDTH                1
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_MASK                 0x1000

// Device Static address. I3C/I2C static address (access: R/W)
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_BIT                  0
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_WIDTH                7
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_MASK                 0x7f

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_WIDTH                  32
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_MASK                   0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_BIT                     0
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_WIDTH                   16
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_MASK                    0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_BIT                         0
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_MASK                        0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_BIT                         8
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_WIDTH                       8
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_MASK                        0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_BIT                16
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_WIDTH              8
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_MASK               0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_BIT                   0
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_WIDTH                 32
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_MASK                  0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_WIDTH                  16
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_MASK                   0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_BIT                        0
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_WIDTH                      8
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_MASK                       0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_BIT                        8
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_WIDTH                      8
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_MASK                       0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_BIT               16
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_WIDTH             8
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_MASK              0xff0000

// Device provisional ID high : bits [48:16] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_BIT                   0
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_WIDTH                 32
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_MASK                  0xffffffff

// Device provisional ID low : bits [15:0] of Device’s I3C PID. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_BIT                    0
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_WIDTH                  16
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_MASK                   0xffff

// Device Characteristics Register(DCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_BIT                        0
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_WIDTH                      8
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_MASK                       0xff

// I3C Bus Characteristics Register(BCR) of the slave. (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_BIT                        8
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_WIDTH                      8
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_MASK                       0xff00

// Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave (access: R)
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_BIT               16
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_WIDTH             8
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_MASK              0xff0000



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

typedef union {
  struct {
    unsigned int padding0:1 ;
    unsigned int master_rst_en   :1 ; // Master Reset enable (Soft reset) : To enable the I3C Master controller to issue Soft reset during any transfers. Soft reset will be applied after the completion of ACK/NACK state. -1’b0 : I3C Master reset is disabled -1’b1 : I3C master reset is enabled
    unsigned int master_control_en:1 ; // Master control enable : To enable the I3C Master controller for any transfers. -1’b0 : I3C Master is disabled -1’b1 : I3C master is enabled
  };
  unsigned int raw;
} __attribute__((packed)) i3c_mst_cntl_en_reg_t;

typedef union {
  struct {
    unsigned int data            :32; // Data length or Data byte value. This register contains Data length or Data byte value depending on the type of command being initiated.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_cmd_tr_req_reg_1_t;

typedef union {
  struct {
    unsigned int cmd_req         :32; // Command request. This register contains information about the command type, Transaction ID, slave address and the type of CCC or HDR to be initiated.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_cmd_tr_req_reg_2_t;

typedef union {
  struct {
    unsigned int resp_reg        :32; // Response register. Contains values of the success/failure of the command, transaction ID of the completed command & remaining data (for write transfer) or received data length (for read transfer) or number of remaining slaves (for dynamic address command).
  };
  unsigned int raw;
} __attribute__((packed)) i3c_resp_reg_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int ibi_sts         :1 ; // IBI status. Indicates how the IBI is handled : -1’b0 : Indicates IBI is handled with ACK -1’b1 : Indicates IBI is handled with NACK
    unsigned int ts_present      :1 ; // IBI timestamp present for IBI : -1’b1 : IBI is timestamped -1’b0 : IBI is not timestamped
    unsigned int ibi_id          :8 ; // IBI received ID. Contains slave address for IBI. Contains Hot Join ID for the Hot-Join IBI
    unsigned int data_len        :8 ; // IBI Data length. Number of bytes in the IBI requests received.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_ibi_resp_reg_t;

typedef union {
  struct {
    unsigned int ibi_data        :32; // IBI Data. Data received during the IBI process is stored in FIFO and is sent out to the Software using this register.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_ibi_data_reg_t;

typedef union {
  struct {
    unsigned int data_rx         :32; // All the Data received from the slave is stored in FIFO. On request by the software, the FIFO read and the values are provided.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_data_rx_fifo_reg_t;

typedef union {
  struct {
    unsigned int data_tx         :32; // Write data to be sent from the Master to the slave. Software writes this register for sending the write data to the slave.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_data_tx_fifo_reg_t;

typedef union {
  struct {
    unsigned int padding0:8 ;
    unsigned int rst_completion  :1 ; // Indicates the Master controller issued soft reset.
    unsigned int ibi_data_rx_fifo_full:1 ; // Indicates the IBI payload is full.
    unsigned int ibi_transfer_done:1 ; // Indicates the IBI request is received from slave.
    unsigned int resp_compl      :1 ; // Indicates the response completion FIFO is full.
    unsigned int data_rx_fifo_full:1 ; // Indicates the Data Rx FIFO Full.
    unsigned int cmd_req_fifo_full:1 ; // Indicates the command transfer request FIFO Full.
    unsigned int data_tx_fifo_empty:1 ; // Indicates the Data Tx FIFO is empty.
    unsigned int data_tx_fifo_full:1 ; // Indicates the Data Tx FIFO is full.
    unsigned int resp_done       :1 ; // Indicates the response completion is done.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_irq_status_reg_t;

typedef union {
  struct {
    unsigned int tcas_timer      :32; // Timing of the SCL pin to go low after the start condition (Number of ref_clk). As per specification, Min time value is 38.4ns. For 100MHz ref clock, total 4 clock periods are required. So, the value of the register is 0x3.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tcas_timer_reg_t;

typedef union {
  struct {
    unsigned int tlow_od_timer   :5 ; // Low period of the SCL clock pin (Number of ref_clk). As per specification, Min time value is 200ns + fall time of SDA signal. For 100MHz ref clock, total 24 clock periods are required. So, the value of the register is 0x23.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tlow_od_timer_reg_t;

typedef union {
  struct {
    unsigned int thigh_od_timer  :5 ; // High period of the SCL clock pin (Number of ref_clk). As per specification, Max time value is 41ns. For 100MHz ref clock, Max. of total 4 clock periods is allowed. So, the value of the register is 0x2.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thigh_od_timer_reg_t;

typedef union {
  struct {
    unsigned int tlow_pp_timer   :5 ; // Low period of the SCL clock pin (Number of ref_clk) in Push pull mode. As per specification, Min time value is 24ns. For 100MHz ref clock, total 3 clock periods are required. So, the value of the register is 0x3.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tlow_pp_timer_reg_t;

typedef union {
  struct {
    unsigned int thigh_pp_timer  :5 ; // High period of the SCL clock pin (Number of ref_clk). As per specification, Max time value is 41ns. For 100MHz ref clock, Max. of total 4 clock periods are required. So, the value of the register is 0x2.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thigh_pp_timer_reg_t;

typedef union {
  struct {
    unsigned int tds_timer       :3 ; // SDA Set up time (Number of ref_clk). As per specification, Min time value is 3ns. For 100MHz ref clock, Max. of total 1 clock period is required. So, the value of the register is 0x1.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tds_timer_reg_t;

typedef union {
  struct {
    unsigned int thd_pp_timer    :3 ; // SDA Hold time (Number of ref_clk). As per specification, Min time value is 6ns. For 100MHz ref clock, Min. of total 1 clock period is required. So, the value of the register is 0x1.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thd_pp_timer_reg_t;

typedef union {
  struct {
    unsigned int tcbp_timer      :5 ; // To signal STOP condition, the master should change the SDA pin after this time of SCL clock edge (Number of ref_clk). As per specification, Min time value is 19.2ns. For 100MHz ref clock, Max. of total 2 clock periods is required. So, the value of the register is 0x1.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tcbp_timer_reg_t;

typedef union {
  struct {
    unsigned int tcbsr_timer     :3 ; // To signal Repeated start condition, the master should change the SDA pin after this time of SCL clock edge (Number of ref_clk). As per specification, Min time value is 19.2ns. For 100MHz ref clock, Max. of total 2 clock periods is required. So, the value of the register is 0x1.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tcbsr_timer_reg_t;

typedef union {
  struct {
    unsigned int thd_ddr_timer   :3 ; // SDA Hold time (Number of ref_clk). As per specification, Min time value is 6ns. For 100MHz ref clock, Min. of total 1 clock period is required. So, the value of the register is 0x1.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thd_ddr_timer_reg_t;

typedef union {
  struct {
    unsigned int bus_free_timer  :32; // Bus free time after the STOP condition and the Start condition. As per specification, Min. value of this period is 1us.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_bus_free_timer_reg_t;

typedef union {
  struct {
    unsigned int bus_avail_timer :32; // Bus aval time to drive SCL and SDA pin to high. As per specification, Min. value of this period is 1us.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_bus_avail_timer_reg_t;

typedef union {
  struct {
    unsigned int tidle_timer     :32; // Bus aval time to drive SCL and SDA pin to high. As per specification, Min. value of this period is 1ms.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tidle_timer_reg_t;

typedef union {
  struct {
    unsigned int tsco_timer      :3 ; // Maximum time for the slave to release bus after the clock change. As per specification, Max. value is 12ns.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tsco_timer_reg_t;

typedef union {
  struct {
    unsigned int tsu_sta_timer   :9 ; // SDA Set up time (Number of ref_clk) for a Repeated Start during Legacy I2C Mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tsu_sta_timer_reg_t;

typedef union {
  struct {
    unsigned int thd_sta_timer   :9 ; // SDA Hold time (Number of ref_clk) after Start/Repeated Start. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thd_sta_timer_reg_t;

typedef union {
  struct {
    unsigned int tlow_timer      :16; // Low period of the SCL clock pin (Number of ref_clk) in Legacy I2C Mode. As per specification, Min time value is 1300ns for Legacy FM Mode. As per specification, Min time value is 500ns for Legacy FM+ Mode. For 100MHz ref clock, total 130 clock periods are required. For FM Mode, the value of the register is 0x8C. For FM+ Mode, the value of this register is 0x3c.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tlow_timer_reg_t;

typedef union {
  struct {
    unsigned int thigh_timer     :16; // High period of the SCL clock pin (Number of ref_clk) in Legacy I2C mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_thigh_timer_reg_t;

typedef union {
  struct {
    unsigned int tvd_data_timer  :10; // Data Hold time (Number of ref_clk) in Legacy I2C mode. As per specification, Min Data setup time value is 100ns for Legacy FM Mode. So, hold time will be min 600 ns, so that the Legacy system works correctly. As per specification, Min Data setup time value is 50ns for Legacy FM+ Mode. So, hold time will be min 260 ns, so that the Legacy system works correctly. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tvd_data_timer_reg_t;

typedef union {
  struct {
    unsigned int tsu_stop_timer  :9 ; // SDA Set up time (Number of ref_clk) for Stop condition during Legacy I2C Mode. As per specification, Min time value is 600ns for Legacy FM Mode. As per specification, Min time value is 260ns for Legacy FM+ Mode. For 100MHz ref clock, total 70 clock periods are required. For FM Mode, the value of the register is 0x46. For FM+ Mode, the value of this register is 0x1E.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_tsu_stop_timer_reg_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg0_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg1_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg2_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg3_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg4_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg5_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg6_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg7_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg8_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg9_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg10_t;

typedef union {
  struct {
    unsigned int padding0:31;
    unsigned int device_type     :1 ; // Device type : - 1’b0 : I3C Device - 1’b1 : I2C Device
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
    unsigned int timestamp       :1 ; // Device IBI timestamp. Enables or disables timestamping for a particular Device : - 1’b0 : Timestamp not required - 1’b1 : Timestamp enabled
    unsigned int in_band_req     :1 ; // In-band interrupt enable. Controls the master to ACK/NACK the IBI requests from the particular slave.
    unsigned int ibi_payload     :1 ; // IBI payload. This bit reflects the information from the BCR regarding the IBI data present to be received by the Master: - 1’b0 : No IBI data payload - 1’b1 : IBI data contains payload
    unsigned int static_addr     :7 ; // Device Static address. I3C/I2C static address
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_addr_table_reg11_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_0_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_0_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_0_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_1_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_1_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_1_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_2_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_2_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_2_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_3_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_3_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_3_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_4_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_4_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_4_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_4_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_5_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_5_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_5_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_5_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_6_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_6_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_6_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_6_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_7_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_7_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_7_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_7_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_8_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_8_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_8_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_8_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_9_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_9_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_9_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_9_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_10_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_10_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_10_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_10_t;

typedef union {
  struct {
    unsigned int pid_high        :32; // Device provisional ID high : bits [48:16] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg0_11_t;

typedef union {
  struct {
    unsigned int pid_low         :16; // Device provisional ID low : bits [15:0] of Device’s I3C PID.
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg1_11_t;

typedef union {
  struct {
    unsigned int dcr             :8 ; // Device Characteristics Register(DCR) of the slave.
    unsigned int bcr             :8 ; // I3C Bus Characteristics Register(BCR) of the slave.
    unsigned int dynamic_addr    :8 ; // Device I3C Dynamic address. 23 - Parity bit for the dynamic address calculated by software. 22:16 – Address of the slave
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg2_11_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i3c_device_char_table_reg3_11_t;

#endif




//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
  unsigned int mst_cntl_en_reg ; // I3C Master control.
  unsigned int cmd_tr_req_reg_1; // Data length or Data byte value to be sent along with the CCC or HDR command.
  unsigned int cmd_tr_req_reg_2; // Command type, Transaction ID, CCC code and slave address. Writing to this register is considered as Doorbell for the master to initiate the transfer.
  unsigned int resp_reg        ; // Success or failure of the command, Transaction ID and the remaining data.
  unsigned int ibi_resp_reg    ; // In-Band interrupt status, timestamping information present, dynamic address of the slave or Hot Join request.
  unsigned int ibi_data_reg    ; // Data received from the slave during the IBI process.
  unsigned int data_rx_fifo_reg; // Received data FIFO (Data_RX_FIFO).
  unsigned int data_tx_fifo_reg; // Sending data FIFO (Data_TX_FIFO).
  unsigned int irq_status_reg  ; // Status of the event happened during the transfer.
  unsigned int tcas_timer_reg  ; // Timing for Clock after Start condition (TCAS) is the time (in Number of Reference clocks) after the start condition after which the SCL pin can go low.
  unsigned int tlow_od_timer_reg; // Low period of SCL pin in Open-Drain mode (in Number of Reference clock) after the SCL pin can go high.
  unsigned int thigh_od_timer_reg; // High period of SCL pin in Open-Drain mode (in Number of Reference clock) after the SCL pin can go low.
  unsigned int tlow_pp_timer_reg; // Low period of SCL pin in Push-Pull mode (in Number of Reference clock) after the SCL pin can go high.
  unsigned int thigh_pp_timer_reg; // High period of SCL pin in Push-Pull mode (in Number of Reference clock) after the SCL pin can go low.
  unsigned int tds_timer_reg   ; // SDA data setup time during both Open-Drain/Push-Pull mode (in Number of Reference clock).
  unsigned int thd_pp_timer_reg; // SDA data hold time during the Push-Pull mode (in Number of Reference clock).
  unsigned int tcbp_timer_reg  ; // Clock time before Stop condition.
  unsigned int tcbsr_timer_reg ; // Clock time before Repeated start condition.
  unsigned int thd_ddr_timer_reg; // SDA data hold time during the Push-Pull mode (in Number of Reference clock) in DDR Data rate.
  unsigned int bus_free_timer_reg; // Bus free time between the Stop condition and the next start condition (in Number of Reference clock).
  unsigned int bus_avail_timer_reg; // Time to keep the SDA and SCL pin to High (in Number of Reference clock).
  unsigned int tidle_timer_reg ; // Extended duration of the bus free condition after the Stop condition (in Number of Reference clock) to enable the device to drive the Hot Join request.
  unsigned int tsco_timer_reg  ; // Maximum time the slave needs to drive the bus during the ACK/read data after the clock change.
  unsigned int tsu_sta_timer_reg; // SDA data setup time during both Open-Drain (in Number of Reference clock) for a Repeated Start.
  unsigned int thd_sta_timer_reg; // SDA data hold time during the Open Drain mode (in Number of Reference clock).
  unsigned int tlow_timer_reg  ; // Low period of SCL pin in Open Drain mode during Legacy I2c Mode (in Number of Reference clock) after the timer reached the SCL pin can go high.
  unsigned int thigh_timer_reg ; // High period of SCL pin in Open Drain Mode for Legacy I2C (in Number of Reference clock) after this timer count reached the SCL pin can go low.
  unsigned int tvd_data_timer_reg; // Data hold time in Open Drain Mode for Legacy I2C (in Number of Reference clock) after this timer count reached the SDA pin can change its value.
  unsigned int tsu_stop_timer_reg; // SDA data setup time during Open-Drain (in Number of Reference clock) for Stop condition.
  unsigned int device_addr_table_reg0; // Device type, IBI handling and Dynamic address to be set and Static address of the slave0.
  unsigned int device_addr_table_reg1; // Device type, IBI handling and Dynamic address to be set and Static address of the slave1.
  unsigned int device_addr_table_reg2; // Device type, IBI handling and Dynamic address to be set and Static address of the slave2.
  unsigned int device_addr_table_reg3; // Device type, IBI handling and Dynamic address to be set and Static address of the slave3.
  unsigned int device_addr_table_reg4; // Device type, IBI handling and Dynamic address to be set and Static address of the slave4.
  unsigned int device_addr_table_reg5; // Device type, IBI handling and Dynamic address to be set and Static address of the slave5.
  unsigned int device_addr_table_reg6; // Device type, IBI handling and Dynamic address to be set and Static address of the slave6.
  unsigned int device_addr_table_reg7; // Device type, IBI handling and Dynamic address to be set and Static address of the slave7.
  unsigned int device_addr_table_reg8; // Device type, IBI handling and Dynamic address to be set and Static address of the slave8.
  unsigned int device_addr_table_reg9; // Device type, IBI handling and Dynamic address to be set and Static address of the slave9.
  unsigned int device_addr_table_reg10; // Device type, IBI handling and Dynamic address to be set and Static address of the slave10.
  unsigned int device_addr_table_reg11; // Device type, IBI handling and Dynamic address to be set and Static address of the slave11.
  unsigned int device_char_table_reg0_0; // BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_0; // BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_0; // BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_0; // BCR, DCR & PID Values of the slave0 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_1; // BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_1; // BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_1; // BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_1; // BCR, DCR & PID Values of the slave1 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_2; // BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_2; // BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_2; // BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_2; // BCR, DCR & PID Values of the slave2 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_3; // BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_3; // BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_3; // BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_3; // BCR, DCR & PID Values of the slave3 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_4; // BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_4; // BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_4; // BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_4; // BCR, DCR & PID Values of the slave4 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_5; // BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_5; // BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_5; // BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_5; // BCR, DCR & PID Values of the slave5 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_6; // BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_6; // BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_6; // BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_6; // BCR, DCR & PID Values of the slave6 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_7; // BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_7; // BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_7; // BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_7; // BCR, DCR & PID Values of the slave7 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_8; // BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_8; // BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_8; // BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_8; // BCR, DCR & PID Values of the slave8 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_9; // BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_9; // BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_9; // BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_9; // BCR, DCR & PID Values of the slave9 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_10; // BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_10; // BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_10; // BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_10; // BCR, DCR & PID Values of the slave10 during the Dynamic address assignment command.
  unsigned int device_char_table_reg0_11; // BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
  unsigned int device_char_table_reg1_11; // BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
  unsigned int device_char_table_reg2_11; // BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
  unsigned int device_char_table_reg3_11; // BCR, DCR & PID Values of the slave11 during the Dynamic address assignment command.
} __attribute__((packed)) i3c_i3c_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

static inline uint32_t i3c_mst_cntl_en_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_MST_CNTL_EN_REG_OFFSET); }
static inline void i3c_mst_cntl_en_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_MST_CNTL_EN_REG_OFFSET, value); }

static inline uint32_t i3c_cmd_tr_req_reg_1_get(uint32_t base) { return ARCHI_READ(base, I3C_CMD_TR_REQ_REG_1_OFFSET); }
static inline void i3c_cmd_tr_req_reg_1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_CMD_TR_REQ_REG_1_OFFSET, value); }

static inline uint32_t i3c_cmd_tr_req_reg_2_get(uint32_t base) { return ARCHI_READ(base, I3C_CMD_TR_REQ_REG_2_OFFSET); }
static inline void i3c_cmd_tr_req_reg_2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_CMD_TR_REQ_REG_2_OFFSET, value); }

static inline uint32_t i3c_resp_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_RESP_REG_OFFSET); }
static inline void i3c_resp_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_RESP_REG_OFFSET, value); }

static inline uint32_t i3c_ibi_resp_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_IBI_RESP_REG_OFFSET); }
static inline void i3c_ibi_resp_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_IBI_RESP_REG_OFFSET, value); }

static inline uint32_t i3c_ibi_data_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_IBI_DATA_REG_OFFSET); }
static inline void i3c_ibi_data_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_IBI_DATA_REG_OFFSET, value); }

static inline uint32_t i3c_data_rx_fifo_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_DATA_RX_FIFO_REG_OFFSET); }
static inline void i3c_data_rx_fifo_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DATA_RX_FIFO_REG_OFFSET, value); }

static inline uint32_t i3c_data_tx_fifo_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_DATA_TX_FIFO_REG_OFFSET); }
static inline void i3c_data_tx_fifo_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DATA_TX_FIFO_REG_OFFSET, value); }

static inline uint32_t i3c_irq_status_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_IRQ_STATUS_REG_OFFSET); }
static inline void i3c_irq_status_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_IRQ_STATUS_REG_OFFSET, value); }

static inline uint32_t i3c_tcas_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TCAS_TIMER_REG_OFFSET); }
static inline void i3c_tcas_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TCAS_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tlow_od_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TLOW_OD_TIMER_REG_OFFSET); }
static inline void i3c_tlow_od_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TLOW_OD_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thigh_od_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THIGH_OD_TIMER_REG_OFFSET); }
static inline void i3c_thigh_od_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THIGH_OD_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tlow_pp_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TLOW_PP_TIMER_REG_OFFSET); }
static inline void i3c_tlow_pp_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TLOW_PP_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thigh_pp_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THIGH_PP_TIMER_REG_OFFSET); }
static inline void i3c_thigh_pp_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THIGH_PP_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tds_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TDS_TIMER_REG_OFFSET); }
static inline void i3c_tds_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TDS_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thd_pp_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THD_PP_TIMER_REG_OFFSET); }
static inline void i3c_thd_pp_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THD_PP_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tcbp_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TCBP_TIMER_REG_OFFSET); }
static inline void i3c_tcbp_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TCBP_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tcbsr_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TCBSR_TIMER_REG_OFFSET); }
static inline void i3c_tcbsr_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TCBSR_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thd_ddr_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THD_DDR_TIMER_REG_OFFSET); }
static inline void i3c_thd_ddr_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THD_DDR_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_bus_free_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_BUS_FREE_TIMER_REG_OFFSET); }
static inline void i3c_bus_free_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_BUS_FREE_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_bus_avail_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_BUS_AVAIL_TIMER_REG_OFFSET); }
static inline void i3c_bus_avail_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_BUS_AVAIL_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tidle_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TIDLE_TIMER_REG_OFFSET); }
static inline void i3c_tidle_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TIDLE_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tsco_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TSCO_TIMER_REG_OFFSET); }
static inline void i3c_tsco_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TSCO_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tsu_sta_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TSU_STA_TIMER_REG_OFFSET); }
static inline void i3c_tsu_sta_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TSU_STA_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thd_sta_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THD_STA_TIMER_REG_OFFSET); }
static inline void i3c_thd_sta_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THD_STA_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tlow_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TLOW_TIMER_REG_OFFSET); }
static inline void i3c_tlow_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TLOW_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_thigh_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_THIGH_TIMER_REG_OFFSET); }
static inline void i3c_thigh_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_THIGH_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tvd_data_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TVD_DATA_TIMER_REG_OFFSET); }
static inline void i3c_tvd_data_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TVD_DATA_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_tsu_stop_timer_reg_get(uint32_t base) { return ARCHI_READ(base, I3C_TSU_STOP_TIMER_REG_OFFSET); }
static inline void i3c_tsu_stop_timer_reg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_TSU_STOP_TIMER_REG_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg0_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG0_OFFSET); }
static inline void i3c_device_addr_table_reg0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG0_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg1_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG1_OFFSET); }
static inline void i3c_device_addr_table_reg1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG1_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg2_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG2_OFFSET); }
static inline void i3c_device_addr_table_reg2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG2_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg3_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG3_OFFSET); }
static inline void i3c_device_addr_table_reg3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG3_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg4_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG4_OFFSET); }
static inline void i3c_device_addr_table_reg4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG4_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg5_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG5_OFFSET); }
static inline void i3c_device_addr_table_reg5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG5_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg6_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG6_OFFSET); }
static inline void i3c_device_addr_table_reg6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG6_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg7_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG7_OFFSET); }
static inline void i3c_device_addr_table_reg7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG7_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg8_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG8_OFFSET); }
static inline void i3c_device_addr_table_reg8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG8_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg9_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG9_OFFSET); }
static inline void i3c_device_addr_table_reg9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG9_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg10_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG10_OFFSET); }
static inline void i3c_device_addr_table_reg10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG10_OFFSET, value); }

static inline uint32_t i3c_device_addr_table_reg11_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_ADDR_TABLE_REG11_OFFSET); }
static inline void i3c_device_addr_table_reg11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_ADDR_TABLE_REG11_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_0_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_0_OFFSET); }
static inline void i3c_device_char_table_reg0_0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_0_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_0_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_0_OFFSET); }
static inline void i3c_device_char_table_reg1_0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_0_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_0_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_0_OFFSET); }
static inline void i3c_device_char_table_reg2_0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_0_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_0_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_0_OFFSET); }
static inline void i3c_device_char_table_reg3_0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_0_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_1_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_1_OFFSET); }
static inline void i3c_device_char_table_reg0_1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_1_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_1_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_1_OFFSET); }
static inline void i3c_device_char_table_reg1_1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_1_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_1_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_1_OFFSET); }
static inline void i3c_device_char_table_reg2_1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_1_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_1_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_1_OFFSET); }
static inline void i3c_device_char_table_reg3_1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_1_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_2_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_2_OFFSET); }
static inline void i3c_device_char_table_reg0_2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_2_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_2_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_2_OFFSET); }
static inline void i3c_device_char_table_reg1_2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_2_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_2_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_2_OFFSET); }
static inline void i3c_device_char_table_reg2_2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_2_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_2_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_2_OFFSET); }
static inline void i3c_device_char_table_reg3_2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_2_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_3_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_3_OFFSET); }
static inline void i3c_device_char_table_reg0_3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_3_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_3_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_3_OFFSET); }
static inline void i3c_device_char_table_reg1_3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_3_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_3_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_3_OFFSET); }
static inline void i3c_device_char_table_reg2_3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_3_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_3_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_3_OFFSET); }
static inline void i3c_device_char_table_reg3_3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_3_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_4_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_4_OFFSET); }
static inline void i3c_device_char_table_reg0_4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_4_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_4_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_4_OFFSET); }
static inline void i3c_device_char_table_reg1_4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_4_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_4_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_4_OFFSET); }
static inline void i3c_device_char_table_reg2_4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_4_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_4_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_4_OFFSET); }
static inline void i3c_device_char_table_reg3_4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_4_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_5_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_5_OFFSET); }
static inline void i3c_device_char_table_reg0_5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_5_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_5_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_5_OFFSET); }
static inline void i3c_device_char_table_reg1_5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_5_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_5_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_5_OFFSET); }
static inline void i3c_device_char_table_reg2_5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_5_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_5_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_5_OFFSET); }
static inline void i3c_device_char_table_reg3_5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_5_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_6_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_6_OFFSET); }
static inline void i3c_device_char_table_reg0_6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_6_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_6_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_6_OFFSET); }
static inline void i3c_device_char_table_reg1_6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_6_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_6_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_6_OFFSET); }
static inline void i3c_device_char_table_reg2_6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_6_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_6_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_6_OFFSET); }
static inline void i3c_device_char_table_reg3_6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_6_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_7_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_7_OFFSET); }
static inline void i3c_device_char_table_reg0_7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_7_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_7_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_7_OFFSET); }
static inline void i3c_device_char_table_reg1_7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_7_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_7_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_7_OFFSET); }
static inline void i3c_device_char_table_reg2_7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_7_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_7_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_7_OFFSET); }
static inline void i3c_device_char_table_reg3_7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_7_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_8_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_8_OFFSET); }
static inline void i3c_device_char_table_reg0_8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_8_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_8_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_8_OFFSET); }
static inline void i3c_device_char_table_reg1_8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_8_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_8_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_8_OFFSET); }
static inline void i3c_device_char_table_reg2_8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_8_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_8_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_8_OFFSET); }
static inline void i3c_device_char_table_reg3_8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_8_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_9_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_9_OFFSET); }
static inline void i3c_device_char_table_reg0_9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_9_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_9_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_9_OFFSET); }
static inline void i3c_device_char_table_reg1_9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_9_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_9_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_9_OFFSET); }
static inline void i3c_device_char_table_reg2_9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_9_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_9_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_9_OFFSET); }
static inline void i3c_device_char_table_reg3_9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_9_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_10_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_10_OFFSET); }
static inline void i3c_device_char_table_reg0_10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_10_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_10_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_10_OFFSET); }
static inline void i3c_device_char_table_reg1_10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_10_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_10_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_10_OFFSET); }
static inline void i3c_device_char_table_reg2_10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_10_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_10_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_10_OFFSET); }
static inline void i3c_device_char_table_reg3_10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_10_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg0_11_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG0_11_OFFSET); }
static inline void i3c_device_char_table_reg0_11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG0_11_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg1_11_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG1_11_OFFSET); }
static inline void i3c_device_char_table_reg1_11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG1_11_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg2_11_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG2_11_OFFSET); }
static inline void i3c_device_char_table_reg2_11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG2_11_OFFSET, value); }

static inline uint32_t i3c_device_char_table_reg3_11_get(uint32_t base) { return ARCHI_READ(base, I3C_DEVICE_CHAR_TABLE_REG3_11_OFFSET); }
static inline void i3c_device_char_table_reg3_11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, I3C_DEVICE_CHAR_TABLE_REG3_11_OFFSET, value); }

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_GET(value)       (ARCHI_BEXTRACTU((value),1,1))
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_GETS(value)      (ARCHI_BEXTRACT((value),1,1))
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,1))
#define I3C_MST_CNTL_EN_REG_MASTER_RST_EN(val)             ((val) << 1)

#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_GET(value)   (ARCHI_BEXTRACTU((value),1,0))
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_GETS(value)  (ARCHI_BEXTRACT((value),1,0))
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,0))
#define I3C_MST_CNTL_EN_REG_MASTER_CONTROL_EN(val)         ((val) << 0)

#define I3C_CMD_TR_REQ_REG_1_DATA_GET(value)               (ARCHI_BEXTRACTU((value),32,0))
#define I3C_CMD_TR_REQ_REG_1_DATA_GETS(value)              (ARCHI_BEXTRACT((value),32,0))
#define I3C_CMD_TR_REQ_REG_1_DATA_SET(value,field)         (ARCHI_BINSERT((value),(field),32,0))
#define I3C_CMD_TR_REQ_REG_1_DATA(val)                     ((val) << 0)

#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_GET(value)            (ARCHI_BEXTRACTU((value),32,0))
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_GETS(value)           (ARCHI_BEXTRACT((value),32,0))
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ_SET(value,field)      (ARCHI_BINSERT((value),(field),32,0))
#define I3C_CMD_TR_REQ_REG_2_CMD_REQ(val)                  ((val) << 0)

#define I3C_RESP_REG_RESP_REG_GET(value)                   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_RESP_REG_RESP_REG_GETS(value)                  (ARCHI_BEXTRACT((value),32,0))
#define I3C_RESP_REG_RESP_REG_SET(value,field)             (ARCHI_BINSERT((value),(field),32,0))
#define I3C_RESP_REG_RESP_REG(val)                         ((val) << 0)

#define I3C_IBI_RESP_REG_IBI_STS_GET(value)                (ARCHI_BEXTRACTU((value),1,31))
#define I3C_IBI_RESP_REG_IBI_STS_GETS(value)               (ARCHI_BEXTRACT((value),1,31))
#define I3C_IBI_RESP_REG_IBI_STS_SET(value,field)          (ARCHI_BINSERT((value),(field),1,31))
#define I3C_IBI_RESP_REG_IBI_STS(val)                      ((val) << 31)

#define I3C_IBI_RESP_REG_TS_PRESENT_GET(value)             (ARCHI_BEXTRACTU((value),1,25))
#define I3C_IBI_RESP_REG_TS_PRESENT_GETS(value)            (ARCHI_BEXTRACT((value),1,25))
#define I3C_IBI_RESP_REG_TS_PRESENT_SET(value,field)       (ARCHI_BINSERT((value),(field),1,25))
#define I3C_IBI_RESP_REG_TS_PRESENT(val)                   ((val) << 25)

#define I3C_IBI_RESP_REG_IBI_ID_GET(value)                 (ARCHI_BEXTRACTU((value),8,8))
#define I3C_IBI_RESP_REG_IBI_ID_GETS(value)                (ARCHI_BEXTRACT((value),8,8))
#define I3C_IBI_RESP_REG_IBI_ID_SET(value,field)           (ARCHI_BINSERT((value),(field),8,8))
#define I3C_IBI_RESP_REG_IBI_ID(val)                       ((val) << 8)

#define I3C_IBI_RESP_REG_DATA_LEN_GET(value)               (ARCHI_BEXTRACTU((value),8,0))
#define I3C_IBI_RESP_REG_DATA_LEN_GETS(value)              (ARCHI_BEXTRACT((value),8,0))
#define I3C_IBI_RESP_REG_DATA_LEN_SET(value,field)         (ARCHI_BINSERT((value),(field),8,0))
#define I3C_IBI_RESP_REG_DATA_LEN(val)                     ((val) << 0)

#define I3C_IBI_DATA_REG_IBI_DATA_GET(value)               (ARCHI_BEXTRACTU((value),32,0))
#define I3C_IBI_DATA_REG_IBI_DATA_GETS(value)              (ARCHI_BEXTRACT((value),32,0))
#define I3C_IBI_DATA_REG_IBI_DATA_SET(value,field)         (ARCHI_BINSERT((value),(field),32,0))
#define I3C_IBI_DATA_REG_IBI_DATA(val)                     ((val) << 0)

#define I3C_DATA_RX_FIFO_REG_DATA_RX_GET(value)            (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DATA_RX_FIFO_REG_DATA_RX_GETS(value)           (ARCHI_BEXTRACT((value),32,0))
#define I3C_DATA_RX_FIFO_REG_DATA_RX_SET(value,field)      (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DATA_RX_FIFO_REG_DATA_RX(val)                  ((val) << 0)

#define I3C_DATA_TX_FIFO_REG_DATA_TX_GET(value)            (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DATA_TX_FIFO_REG_DATA_TX_GETS(value)           (ARCHI_BEXTRACT((value),32,0))
#define I3C_DATA_TX_FIFO_REG_DATA_TX_SET(value,field)      (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DATA_TX_FIFO_REG_DATA_TX(val)                  ((val) << 0)

#define I3C_IRQ_STATUS_REG_RST_COMPLETION_GET(value)       (ARCHI_BEXTRACTU((value),1,8))
#define I3C_IRQ_STATUS_REG_RST_COMPLETION_GETS(value)      (ARCHI_BEXTRACT((value),1,8))
#define I3C_IRQ_STATUS_REG_RST_COMPLETION_SET(value,field) (ARCHI_BINSERT((value),(field),1,8))
#define I3C_IRQ_STATUS_REG_RST_COMPLETION(val)             ((val) << 8)

#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_GET(value) (ARCHI_BEXTRACTU((value),1,7))
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_GETS(value) (ARCHI_BEXTRACT((value),1,7))
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL_SET(value,field) (ARCHI_BINSERT((value),(field),1,7))
#define I3C_IRQ_STATUS_REG_IBI_DATA_RX_FIFO_FULL(val)      ((val) << 7)

#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_GET(value)    (ARCHI_BEXTRACTU((value),1,6))
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_GETS(value)   (ARCHI_BEXTRACT((value),1,6))
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE_SET(value,field) (ARCHI_BINSERT((value),(field),1,6))
#define I3C_IRQ_STATUS_REG_IBI_TRANSFER_DONE(val)          ((val) << 6)

#define I3C_IRQ_STATUS_REG_RESP_COMPL_GET(value)           (ARCHI_BEXTRACTU((value),1,5))
#define I3C_IRQ_STATUS_REG_RESP_COMPL_GETS(value)          (ARCHI_BEXTRACT((value),1,5))
#define I3C_IRQ_STATUS_REG_RESP_COMPL_SET(value,field)     (ARCHI_BINSERT((value),(field),1,5))
#define I3C_IRQ_STATUS_REG_RESP_COMPL(val)                 ((val) << 5)

#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_GET(value)    (ARCHI_BEXTRACTU((value),1,4))
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_GETS(value)   (ARCHI_BEXTRACT((value),1,4))
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL_SET(value,field) (ARCHI_BINSERT((value),(field),1,4))
#define I3C_IRQ_STATUS_REG_DATA_RX_FIFO_FULL(val)          ((val) << 4)

#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_GET(value)    (ARCHI_BEXTRACTU((value),1,3))
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_GETS(value)   (ARCHI_BEXTRACT((value),1,3))
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL_SET(value,field) (ARCHI_BINSERT((value),(field),1,3))
#define I3C_IRQ_STATUS_REG_CMD_REQ_FIFO_FULL(val)          ((val) << 3)

#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_GET(value)   (ARCHI_BEXTRACTU((value),1,2))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_GETS(value)  (ARCHI_BEXTRACT((value),1,2))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY_SET(value,field) (ARCHI_BINSERT((value),(field),1,2))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_EMPTY(val)         ((val) << 2)

#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_GET(value)    (ARCHI_BEXTRACTU((value),1,1))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_GETS(value)   (ARCHI_BEXTRACT((value),1,1))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL_SET(value,field) (ARCHI_BINSERT((value),(field),1,1))
#define I3C_IRQ_STATUS_REG_DATA_TX_FIFO_FULL(val)          ((val) << 1)

#define I3C_IRQ_STATUS_REG_RESP_DONE_GET(value)            (ARCHI_BEXTRACTU((value),1,0))
#define I3C_IRQ_STATUS_REG_RESP_DONE_GETS(value)           (ARCHI_BEXTRACT((value),1,0))
#define I3C_IRQ_STATUS_REG_RESP_DONE_SET(value,field)      (ARCHI_BINSERT((value),(field),1,0))
#define I3C_IRQ_STATUS_REG_RESP_DONE(val)                  ((val) << 0)

#define I3C_TCAS_TIMER_REG_TCAS_TIMER_GET(value)           (ARCHI_BEXTRACTU((value),32,0))
#define I3C_TCAS_TIMER_REG_TCAS_TIMER_GETS(value)          (ARCHI_BEXTRACT((value),32,0))
#define I3C_TCAS_TIMER_REG_TCAS_TIMER_SET(value,field)     (ARCHI_BINSERT((value),(field),32,0))
#define I3C_TCAS_TIMER_REG_TCAS_TIMER(val)                 ((val) << 0)

#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_GET(value)     (ARCHI_BEXTRACTU((value),5,0))
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_GETS(value)    (ARCHI_BEXTRACT((value),5,0))
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),5,0))
#define I3C_TLOW_OD_TIMER_REG_TLOW_OD_TIMER(val)           ((val) << 0)

#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_GET(value)   (ARCHI_BEXTRACTU((value),5,0))
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_GETS(value)  (ARCHI_BEXTRACT((value),5,0))
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),5,0))
#define I3C_THIGH_OD_TIMER_REG_THIGH_OD_TIMER(val)         ((val) << 0)

#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_GET(value)     (ARCHI_BEXTRACTU((value),5,0))
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_GETS(value)    (ARCHI_BEXTRACT((value),5,0))
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),5,0))
#define I3C_TLOW_PP_TIMER_REG_TLOW_PP_TIMER(val)           ((val) << 0)

#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_GET(value)   (ARCHI_BEXTRACTU((value),5,0))
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_GETS(value)  (ARCHI_BEXTRACT((value),5,0))
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),5,0))
#define I3C_THIGH_PP_TIMER_REG_THIGH_PP_TIMER(val)         ((val) << 0)

#define I3C_TDS_TIMER_REG_TDS_TIMER_GET(value)             (ARCHI_BEXTRACTU((value),3,0))
#define I3C_TDS_TIMER_REG_TDS_TIMER_GETS(value)            (ARCHI_BEXTRACT((value),3,0))
#define I3C_TDS_TIMER_REG_TDS_TIMER_SET(value,field)       (ARCHI_BINSERT((value),(field),3,0))
#define I3C_TDS_TIMER_REG_TDS_TIMER(val)                   ((val) << 0)

#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_GET(value)       (ARCHI_BEXTRACTU((value),3,0))
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_GETS(value)      (ARCHI_BEXTRACT((value),3,0))
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),3,0))
#define I3C_THD_PP_TIMER_REG_THD_PP_TIMER(val)             ((val) << 0)

#define I3C_TCBP_TIMER_REG_TCBP_TIMER_GET(value)           (ARCHI_BEXTRACTU((value),5,0))
#define I3C_TCBP_TIMER_REG_TCBP_TIMER_GETS(value)          (ARCHI_BEXTRACT((value),5,0))
#define I3C_TCBP_TIMER_REG_TCBP_TIMER_SET(value,field)     (ARCHI_BINSERT((value),(field),5,0))
#define I3C_TCBP_TIMER_REG_TCBP_TIMER(val)                 ((val) << 0)

#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_GET(value)         (ARCHI_BEXTRACTU((value),3,0))
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_GETS(value)        (ARCHI_BEXTRACT((value),3,0))
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER_SET(value,field)   (ARCHI_BINSERT((value),(field),3,0))
#define I3C_TCBSR_TIMER_REG_TCBSR_TIMER(val)               ((val) << 0)

#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_GET(value)     (ARCHI_BEXTRACTU((value),3,0))
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_GETS(value)    (ARCHI_BEXTRACT((value),3,0))
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),3,0))
#define I3C_THD_DDR_TIMER_REG_THD_DDR_TIMER(val)           ((val) << 0)

#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_BUS_FREE_TIMER_REG_BUS_FREE_TIMER(val)         ((val) << 0)

#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_GET(value) (ARCHI_BEXTRACTU((value),32,0))
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_GETS(value) (ARCHI_BEXTRACT((value),32,0))
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_BUS_AVAIL_TIMER_REG_BUS_AVAIL_TIMER(val)       ((val) << 0)

#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_GET(value)         (ARCHI_BEXTRACTU((value),32,0))
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_GETS(value)        (ARCHI_BEXTRACT((value),32,0))
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER_SET(value,field)   (ARCHI_BINSERT((value),(field),32,0))
#define I3C_TIDLE_TIMER_REG_TIDLE_TIMER(val)               ((val) << 0)

#define I3C_TSCO_TIMER_REG_TSCO_TIMER_GET(value)           (ARCHI_BEXTRACTU((value),3,0))
#define I3C_TSCO_TIMER_REG_TSCO_TIMER_GETS(value)          (ARCHI_BEXTRACT((value),3,0))
#define I3C_TSCO_TIMER_REG_TSCO_TIMER_SET(value,field)     (ARCHI_BINSERT((value),(field),3,0))
#define I3C_TSCO_TIMER_REG_TSCO_TIMER(val)                 ((val) << 0)

#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_GET(value)     (ARCHI_BEXTRACTU((value),9,0))
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_GETS(value)    (ARCHI_BEXTRACT((value),9,0))
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),9,0))
#define I3C_TSU_STA_TIMER_REG_TSU_STA_TIMER(val)           ((val) << 0)

#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_GET(value)     (ARCHI_BEXTRACTU((value),9,0))
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_GETS(value)    (ARCHI_BEXTRACT((value),9,0))
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),9,0))
#define I3C_THD_STA_TIMER_REG_THD_STA_TIMER(val)           ((val) << 0)

#define I3C_TLOW_TIMER_REG_TLOW_TIMER_GET(value)           (ARCHI_BEXTRACTU((value),16,0))
#define I3C_TLOW_TIMER_REG_TLOW_TIMER_GETS(value)          (ARCHI_BEXTRACT((value),16,0))
#define I3C_TLOW_TIMER_REG_TLOW_TIMER_SET(value,field)     (ARCHI_BINSERT((value),(field),16,0))
#define I3C_TLOW_TIMER_REG_TLOW_TIMER(val)                 ((val) << 0)

#define I3C_THIGH_TIMER_REG_THIGH_TIMER_GET(value)         (ARCHI_BEXTRACTU((value),16,0))
#define I3C_THIGH_TIMER_REG_THIGH_TIMER_GETS(value)        (ARCHI_BEXTRACT((value),16,0))
#define I3C_THIGH_TIMER_REG_THIGH_TIMER_SET(value,field)   (ARCHI_BINSERT((value),(field),16,0))
#define I3C_THIGH_TIMER_REG_THIGH_TIMER(val)               ((val) << 0)

#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_GET(value)   (ARCHI_BEXTRACTU((value),10,0))
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_GETS(value)  (ARCHI_BEXTRACT((value),10,0))
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),10,0))
#define I3C_TVD_DATA_TIMER_REG_TVD_DATA_TIMER(val)         ((val) << 0)

#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_GET(value)   (ARCHI_BEXTRACTU((value),9,0))
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_GETS(value)  (ARCHI_BEXTRACT((value),9,0))
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER_SET(value,field) (ARCHI_BINSERT((value),(field),9,0))
#define I3C_TSU_STOP_TIMER_REG_TSU_STOP_TIMER(val)         ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG0_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG0_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG0_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG0_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG0_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG0_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG1_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG1_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG1_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG1_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG1_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG1_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG2_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG2_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG2_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG2_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG2_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG2_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG3_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG3_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG3_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG3_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG3_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG3_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG4_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG4_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG4_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG4_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG4_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG4_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG5_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG5_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG5_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG5_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG5_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG5_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG6_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG6_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG6_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG6_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG6_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG6_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG7_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG7_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG7_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG7_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG7_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG7_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG8_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG8_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG8_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG8_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG8_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG8_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_GET(value)  (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG9_DEVICE_TYPE(val)        ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG9_DYNAMIC_ADDR(val)       ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG9_TIMESTAMP(val)          ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_GET(value)  (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG9_IN_BAND_REQ(val)        ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_GET(value)  (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG9_IBI_PAYLOAD(val)        ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_GET(value)  (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG9_STATIC_ADDR(val)        ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_GET(value) (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG10_DEVICE_TYPE(val)       ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG10_DYNAMIC_ADDR(val)      ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_GET(value)   (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_GETS(value)  (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG10_TIMESTAMP(val)         ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_GET(value) (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG10_IN_BAND_REQ(val)       ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_GET(value) (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG10_IBI_PAYLOAD(val)       ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG10_STATIC_ADDR(val)       ((val) << 0)

#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_GET(value) (ARCHI_BEXTRACTU((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),1,31))
#define I3C_DEVICE_ADDR_TABLE_REG11_DEVICE_TYPE(val)       ((val) << 31)

#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_ADDR_TABLE_REG11_DYNAMIC_ADDR(val)      ((val) << 16)

#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_GET(value)   (ARCHI_BEXTRACTU((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_GETS(value)  (ARCHI_BEXTRACT((value),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define I3C_DEVICE_ADDR_TABLE_REG11_TIMESTAMP(val)         ((val) << 15)

#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_GET(value) (ARCHI_BEXTRACTU((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_GETS(value) (ARCHI_BEXTRACT((value),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define I3C_DEVICE_ADDR_TABLE_REG11_IN_BAND_REQ(val)       ((val) << 13)

#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_GET(value) (ARCHI_BEXTRACTU((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_GETS(value) (ARCHI_BEXTRACT((value),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD_SET(value,field) (ARCHI_BINSERT((value),(field),1,12))
#define I3C_DEVICE_ADDR_TABLE_REG11_IBI_PAYLOAD(val)       ((val) << 12)

#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),7,0))
#define I3C_DEVICE_ADDR_TABLE_REG11_STATIC_ADDR(val)       ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_0_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_0_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_0_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_1_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_1_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_1_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_2_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_2_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_2_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_3_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_3_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_3_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_4_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_4_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_4_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_5_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_5_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_5_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_6_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_6_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_6_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_7_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_7_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_7_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_8_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_8_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_8_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_GET(value)   (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_GETS(value)  (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_9_PID_HIGH(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_GET(value)    (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_GETS(value)   (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_9_PID_LOW(val)          ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_GET(value)        (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_GETS(value)       (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DCR(val)              ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_GET(value)        (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_GETS(value)       (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR_SET(value,field)  (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_BCR(val)              ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_9_DYNAMIC_ADDR(val)     ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_GET(value)  (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_GETS(value) (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_10_PID_HIGH(val)        ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_GET(value)   (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_GETS(value)  (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_10_PID_LOW(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_GET(value)       (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_GETS(value)      (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR_SET(value,field) (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DCR(val)             ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_GET(value)       (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_GETS(value)      (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR_SET(value,field) (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_BCR(val)             ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_10_DYNAMIC_ADDR(val)    ((val) << 16)

#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_GET(value)  (ARCHI_BEXTRACTU((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_GETS(value) (ARCHI_BEXTRACT((value),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH_SET(value,field) (ARCHI_BINSERT((value),(field),32,0))
#define I3C_DEVICE_CHAR_TABLE_REG0_11_PID_HIGH(val)        ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_GET(value)   (ARCHI_BEXTRACTU((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_GETS(value)  (ARCHI_BEXTRACT((value),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW_SET(value,field) (ARCHI_BINSERT((value),(field),16,0))
#define I3C_DEVICE_CHAR_TABLE_REG1_11_PID_LOW(val)         ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_GET(value)       (ARCHI_BEXTRACTU((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_GETS(value)      (ARCHI_BEXTRACT((value),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR_SET(value,field) (ARCHI_BINSERT((value),(field),8,0))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DCR(val)             ((val) << 0)

#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_GET(value)       (ARCHI_BEXTRACTU((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_GETS(value)      (ARCHI_BEXTRACT((value),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR_SET(value,field) (ARCHI_BINSERT((value),(field),8,8))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_BCR(val)             ((val) << 8)

#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_GET(value) (ARCHI_BEXTRACTU((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_GETS(value) (ARCHI_BEXTRACT((value),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR_SET(value,field) (ARCHI_BINSERT((value),(field),8,16))
#define I3C_DEVICE_CHAR_TABLE_REG2_11_DYNAMIC_ADDR(val)    ((val) << 16)

#endif

#endif
