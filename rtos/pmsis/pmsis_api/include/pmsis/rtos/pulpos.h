/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS_RTOS_PULPOS_H__
#define __PMSIS_RTOS_PULPOS_H__

typedef enum
{
	PI_PULPOS_IO_DEV_BRIDGE = 0,
	PI_PULPOS_IO_DEV_UART = 1,
	PI_PULPOS_IO_DEV_HOST = 2,
} pi_pulpos_io_dev_e;


struct pi_pulpos_conf
{
	pi_pulpos_io_dev_e io_dev;
	union
	{
		struct 
		{
			int baudrate;
		} uart;
	};
};

void pi_pulpos_conf_init(struct pi_pulpos_conf *conf);

#endif