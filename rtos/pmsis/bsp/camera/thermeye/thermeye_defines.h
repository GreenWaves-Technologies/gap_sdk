/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __BSP_THERMEYE_DEFINES_H__
#define __BSP_THERMEYE_DEFINES_H__

/* I2C chip select address. */
#define I2CAD       ( 0x12 )

/* Registers address, to be addressed using I2C. */

/* Serial number of the sensor is written in those registers. */
#define SERIAL_NB_A ( 0x0000 )
#define SERIAL_NB_B ( 0x0001 )
#define SERIAL_NB_C ( 0x0002 )
#define SERIAL_NB_D ( 0x0003 )

/*
 * Gain values
 *
 * Gain | CINT | Reg_val
 * 0.75 | 8pF  | 0x7X
 * 0.86 | 7pF  | 0x6X
 * 1.00 | 6pF  | 0x5X     <--- Default config.
 * 1.20 | 5pF  | 0x4X
 * 1.50 | 4pF  | 0x3X
 * 2.00 | 3pF  | 0x2X
 * 3.00 | 2pF  | 0x1X
 * 6.00 | 1pF  | 0x0X
 *
 * Image flip
 *
 * Use GAIN_IMAGE's bit 0 & 1(UPROW & UPCOL) to flip image.
 * SCANNING DIRECTION                                 | UPROW UPCOL
 * left → right / down → up (default configuration) |   1     1
 * left → right / up → down                         |   0     1
 * right → left / down → up                         |   1     0
 * right → left / up → down                         |   0     0
 *
 * Default value : 0x53.
 */
#define GAIN_IMAGE  ( 0x0040 )

/*
 * VTEMP_ADC = VTEMP_SEL|MUX_ADC_2x8|ADC_ENABLE|X|SEQ_TRIGGER|X|X|X|
 *
 * ADC Operating mode | MUX-ADC 2x8 | ADC_Enable
 *          0         |      1     |       0
 *          1         |      0     |       1
 *          1         |      1     |       1
 *
 * VTEMP_SEL = 0 -> -4.8 mv/C.  -> FPA temp = -0.0305 * VTEMP + 413 *C.
 * VTEMP_SEL = 1 -> +14.8 mv/C. -> FPA temp = 33*C, VTEMP = 1.85V.
 * Default value : 0x40
 */
#define VTEMP_ADC   ( 0x0041 )

#define YMIN        ( 0x0044 )
#define YMAX_B      ( 0x0046 )
#define XMIN_B      ( 0x0048 )
#define XMAX_B      ( 0x004A )

/*
 * GFID internal = 0.0102V DACGFID value + 0.7775V
 * GFID min = 0V, GFID max = 2.86V
 */
#define DACGFID     ( 0x004B )

/*
 * GSK Internal (V) = 0.0026 DACGSK value + 0.9697 V
 * GSK min = 1.65 V and GSK max = 3.6 V
 */
#define DACGSK_A    ( 0x004C )
#define DACGSK_B    ( 0x004D )
/* Generated internally, default value is 0x0. VBUS = 2.3V. */
#define DACVBUS     ( 0x004E )

/*
 * TINT = val(INTEGRATION) * PSYNC.
 * Max TINT = 300us.
 * if ( TINT > (nb_col - 13) )
 *    Additional interline   = ( TINT - (nb_col - 13) * PSYNC )
 *    Additional interframwe = ( TINT - (nb_col - 13) * PSYNC )
 */
#define INTEGRATION ( 0x0050 )  /* Default value : 0x50. */
#define ADC_BIAS    ( 0x005A )

/*
 * EXT_POLAR = X|X|X|X|GOC_EXT|VBUS_EXT|GFID_EXT|GSK_EXT
 *
 * Default value : 0x0B.
 * GOC_EXT  = 1
 * VBUS_EXT = 0 (must be internal)
 * GFID_EXT = 1
 * GSK_EXT  = 1
 */
#define EXT_POLAR   ( 0x005B )

/*
 * CONFIG = X|X|X|X|X|START_SEQ|WINDOW|I2CDIFF.
 */
#define CONFIG      ( 0x005C )

/*
 * Value of inter-line duration while HSYNC = 0.
 * Preset factory value : 17.
 * Min val = 17, Max val = 17 + 255.
 */
#define INTERLINE   ( 0x0062 )

/*
 * Check window resizing errors.
 * FEEDBACK[7] = BAD_XY_PROG : 0 OK, 1 KO Fault : incorrect minimum & maximum values.
 * FEEDBACK[6] = STOP_SEQ : 0 Sequencer running, 1 Sequencer stopped.
 * FEEDBACK[4] = AD_SIZE_PROG : 0 OK, 1 KO Fault : unauthorized format.
 * FEEDBACK[3] = CL_INT_DONE : 0 Sequencer not ready, 1 Sequencer ready.
 */
#define FEEDBACK    ( 0x0063 )

#define EXT_CAPA_LD ( 0x0082 )
#define INT_CAPA_LD ( 0x008A )

/*
 * Value of inter-frame duration while VSYNC = 0.
 * INTERFRAME_A/B = 0  -> Interframe_real = 16 * PSYNC.
 * INTERFRAME_A/B != 0 -> Interframe_real = (nb_col * INTERFRAME_A/B + 18 + INTERFRAME_A/B) * PSYNC.
 */
#define INTERFRAME_A ( 0x008E )
#define INTERFRAME_B ( 0x008F )

#define READ_ONLY_A  ( 0x00F7 ) /* Read-only, factory value = 0x55. */
#define READ_ONLY_B  ( 0x00F8 ) /* Read-only, factory value = 0xC6. */
#define READ_ONLY_C  ( 0x00F9 ) /* Read-only, factory value = 0xC5. */

#endif  /* __BSP_THERMEYE_DEFINES_H__ */

/*
 * Nominal config:
 * Master clock = 780kHz, framerate = 50Hz.
 * SCL = 52kHz.
 * ADC & DAC on.
 * ADC & DAC voltage : internal.
 * GFID internally generated, default value of DACGFID.
 * GSK internally generated, default value of DACGSK.
 * VBUS internally generated.
 * CINT = 6pF.
 * TINT = 29 period of PSYNC -> Tint = 75us.
 * INTERLINE = 0.
 * INTERFRAME = 0.
 */
/*
 * Starting config given(from User Guide) :
 *
 */
