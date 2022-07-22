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

#ifndef __PI_AT_COMMANDS_H__
#define __PI_AT_COMMANDS_H__


/* ***** PART 1 ***************** */
/* ***** VALUES ***************** */

#define PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH ( 2048 ) /*!< Default RESP array length for WiFi. */
#define PI_AT_BLE_DEFAULT_RESP_ARRAY_LENGTH  ( 64 )   /*!< Default RESP array length for BLE. */


/* ***** PART 2 ***************** */
/* ***** TYPES ****************** */

typedef enum
{
    PI_AT_RESP_NOT_STARTED,       /*!< Response not received yet from module.  */
    PI_AT_RESP_IN_PROGRESS,       /*!< Response transmission has started. */
    PI_AT_RESP_DONE               /*!< Response transmission received, with end of resp. */
} at_resp_state_e;

/**
 * @brief AT commands result codes enum.
 * Help managing responses.
 * 
 * @return int8_t Code corresponding to the response
 * \retval 0 OK response
 * \retval -1 ERROR response
 * \retval -2 UNSOLICITED response
 * \retval -3 NON AUTHORIZED response
 */
typedef enum
{
    CMD_RES_OK    =  0,
    CMD_RES_ERR   = -1,
    CMD_RES_UNSOL = -2,
    CMD_RES_NA    = -3
} cmd_res_e;


/* ***** PART 3 ***************** */
/* ***** COMMANDS *************** */

/* ***** General **************** */

/*!< Manufacturer ID. */
#define PI_AT_MANUFACTURER_ID    +CGMI

/*<! Model ID. */
#define PI_AT_MODEL_ID           +CGMM

/*<! Software version ID. */
#define PI_AT_SW_VERSION_ID      +CGMR

/*<! Serial number. */
#define PI_AT_SERIAL_NUMBER      +CGSN


/* ***** System ***************** */

/*!< Store current configuration. */
#define PI_AT_CFG_SAVE           &W

/*!< Factory reset. */
#define PI_AT_FACTORY_RESET      +UFACTORY

/*!< Reboot chip. */
#define PI_AT_REBOOT             +CPWROFF

/*!< Turn On/Off echo. */
#define PI_AT_ECHO_OFF           E0
#define PI_AT_ECHO_ON            E1

/* Factory default S3(CR) & S4(LF) response format character.  */
#define S3char                   '\r' /*!< CR char, ASCII 0x0D. */
#define S4char                   '\n' /*!< LF char, ASCII 0x0A. */
/* Same, but to use in string functions (strcat, strcmp, etc...). */
#define S3str                    "\r"  /*!< CR string.  */
#define S4str                    "\n"  /*!< LF string. */

/*!< Communication settings(uart settings). */
#ifdef CONFIG_WIFI_DA16200
#define PI_AT_UART_SETTINGS_SET  B=
#else
#define PI_AT_UART_SETTINGS_GET  +UMRS?
#define PI_AT_UART_SETTINGS_SET  +UMRS=
#endif


/* ***** Data mode ************** */

/*!< Device mode. */
#define PI_AT_O_CMD_MODE         O0 /*!< Command mode. */
#define PI_AT_O_DATA_MODE        O1 /*!< Data mode. */
#define PI_AT_O_EDM              O2 /*!< Extended data mode. */

/*!< Connect to peer. */
#define PI_AT_PEER_CONNECT       +UDCP=

/*!< Close a connection with peer. */
#define PI_AT_PEER_DISCONNECT    +UDCPC=


/* ***** Bluetooth ************** */

/*!< Unbound devices. */
#define PI_AT_DEVICE_UNBOND_SET  +UBTUB=

/*!< Bind devices. */
#define PI_AT_DEVICE_BOND_SET    +UBTB=

/*!< Device role. */
#define PI_AT_DEVICE_ROLE_GET    +UBTLE?
#define PI_AT_DEVICE_ROLE_SET    +UBTLE=
#define PI_AT_SERVER_ROLE        1
#define PI_AT_CLIENT_ROLE        2
#define PI_AT_BOTH_ROLE          3

/*!< Device name. */
#define PI_AT_DEVICE_NAME_GET    +UBTLN?
#define PI_AT_DEVICE_NAME_SET    +UBTLN=

/* ***** WiFi ******************* */

/*!< Scan for available networks */
#define PI_AT_WIFI_SCAN          +UWSCAN

/*!< Connect to AP */
#define PI_AT_CONNECT_TO_AP      +WFJAPA=

/*!< Disconnect from AP */
#define PI_AT_DISCONNECT_FROM_AP +WFQAP

/*!< Open a TCP server socket */
#define PI_AT_OPEN_TCP_SERVER    +TRTS=

/*!< Prefixs for received events */
#define PI_AT_PREFIX_MSG_RECEIVED        "+TRDTS"
#define PI_AT_PREFIX_CLIENT_DISCONNECTED "+TRXTS"
#define PI_AT_PREFIX_CLIENT_CONNECTED    "+TRCTS"
#define PI_AT_PREFIX_CONNECTED_TO_AP     "+WFJAP"


#endif  /* __PI_AT_COMMANDS_DEFINES_H__ */
