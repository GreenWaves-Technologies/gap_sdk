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

#ifndef __PI_BSP_NINA_B312_DEFINES_H__
#define __PI_BSP_NINA_B312_DEFINES_H__

/*!< Store current configuration. */
#define PI_AT_NINA_B312_CFG_SAVE           &W

/*!< Reboot chip. */
#define PI_AT_NINA_B312_DEVICE_REBOOT      +CPWROFF

/*!< Manufacturer ID. */
#define PI_AT_NINA_B312_MANUFACTURER_ID    +CGMI

/*<! Model ID. */
#define PI_AT_NINA_B312_MODEL_ID           +CGMM

/*<! Software version ID. */
#define PI_AT_NINA_B312_SW_VERSION_ID      +CGMR

/*<! Serial number. */
#define PI_AT_NINA_B312_SERIAL_NUMBER      +CGSN

/*!< Turn On/Off echo. */
#define PI_AT_NINA_B312_ECHO_OFF            E0
#define PI_AT_NINA_B312_ECHO_ON             E1

/*!< Factory reset. */
#define PI_AT_NINA_B312_FACTORY_RESET      +UFACTORY

/*!< Unbound devices. */
#define PI_AT_NINA_B312_DEVICE_UNBOND_SET  +UBTUB=

/*!< Bind devices. */
#define PI_AT_NINA_B312_DEVICE_BOND_SET    +UBTB=

/*!< Device role. */
#define PI_AT_NINA_B312_DEVICE_ROLE_GET    +UBTLE?
#define PI_AT_NINA_B312_DEVICE_ROLE_SET    +UBTLE=
#define PI_AT_NINA_B312_SERVER_ROLE        1
#define PI_AT_NINA_B312_CLIENT_ROLE        2
#define PI_AT_NINA_B312_BOTH_ROLE          3

/*!< Device name. */
#define PI_AT_NINA_B312_DEVICE_NAME_GET    +UBTLN?
#define PI_AT_NINA_B312_DEVICE_NAME_SET    +UBTLN=

/*!< Device mode. */
#define PI_AT_NINA_B312_O_CMD_MODE         O0 /*!< Command mode. */
#define PI_AT_NINA_B312_O_DATA_MODE        O1 /*!< Data mode. */
#define PI_AT_NINA_B312_O_EDM              O2 /*!< Extended data mode. */

/*!< Communication settings(uart settings). */
#define PI_AT_NINA_B312_UART_SETTINGS_GET  +UMRS?
#define PI_AT_NINA_B312_UART_SETTINGS_SET  +UMRS=

/*!< Connect to peer. */
#define PI_AT_NINA_B312_PEER_CONNECT       +UDCP=

/*!< Close a connection with peer. */
#define PI_AT_NINA_B312_PEER_DISCONNECT    +UDCPC=

#endif  /* __PI_BSP_NINA_B312_DEFINES_H__ */
