//***********************  ESP8266.h  ***********************
// Program written by:
// - Steven Prickett  steven.prickett@gmail.com
//
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
//*********************************************************
/* Modified by Jonathan Valvano
   Sept 19, 2015

*/

#ifndef ESP8266_H
#define ESP8266_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ESP8266_ENCRYPT_MODE_OPEN           0
#define ESP8266_ENCRYPT_MODE_WEP            1
#define ESP8266_ENCRYPT_MODE_WPA_PSK        2
#define ESP8266_ENCRYPT_MODE_WPA2_PSK       3
#define ESP8266_ENCRYPT_MODE_WPA_WPA2_PSK   4

#define ESP8266_WIFI_MODE_CLIENT            1
#define ESP8266_WIFI_MODE_AP                2
#define ESP8266_WIFI_MODE_AP_AND_CLIENT     3

//-------------------ESP8266_Init --------------
// initializes the module as a client
// Inputs: none
// Outputs: none
void ESP8266_Init(uint32_t baud, const char* ssid, const char* password);

//------------------- ESP8266_InitUART-------------------
// intializes uart and gpio needed to communicate with esp8266
// Configure UART1 for serial full duplex operation
// Inputs: baud rate (e.g., 115200 or 9600)
//         echo to UART0?
// Outputs: none
void ESP8266_InitUART(uint32_t baud, int echo);

//---------ESP8266_GetVersionNumber----------
// get status
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetVersionNumber(void);

//----------ESP8266_Reset------------
// resets the esp8266 module
// input:  none
// output: 1 if success, 0 if fail
int ESP8266_Reset(void);

//---------ESP8266_SetWifiMode----------
// configures the esp8266 to operate as a wifi client, access point, or both
// Input: mode accepts ESP8266_WIFI_MODE constants
// output: 1 if success, 0 if fail
int ESP8266_SetWifiMode(uint8_t mode);

//---------ESP8266_SetConnectionMux----------
// enables the esp8266 connection mux, required for starting tcp server
// Input: 0 (single) or 1 (multiple)
// output: 1 if success, 0 if fail
int ESP8266_SetConnectionMux(uint8_t enabled);

//---------ESP8266_CloseTCPConnection----------
// Close TCP connection
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_CloseTCPConnection(void);

//---------ESP8266_DisableServer----------
// disables tcp server
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_DisableServer(void);

//----------ESP8266_JoinAccessPoint------------
// joins a wifi access point using specified ssid and password
// input:  SSID and PASSWORD
// output: 1 if success, 0 if fail
int ESP8266_JoinAccessPoint(const char* ssid, const char* password);

//---------ESP8266_ListAccessPoints----------
// lists available wifi access points
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_ListAccessPoints(void);

//----------ESP8266_ConfigureAccessPoint------------
// configures esp8266 wifi access point settings
// input:  SSID, Password, channel, security
// output: 1 if success, 0 if fail
int ESP8266_ConfigureAccessPoint(const char* ssid, const char* password, uint8_t channel, uint8_t encryptMode);

//---------ESP8266_GetIPAddress----------
// Get local IP address
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetIPAddress(void);

//---------ESP8266_MakeTCPConnection----------
// Establish TCP connection
// Input: IP address or web page as a string
// output: 1 if success, 0 if fail
int ESP8266_MakeTCPConnection(char *IPaddress);

//---------ESP8266_SendTCP----------
// Send a TCP packet to server
// Input: TCP payload to send
// output: 1 if success, 0 if fail
int ESP8266_SendTCP(char* fetch);

//---------ESP8266_SetDataTransmissionMode----------
// set data transmission mode
// Input: 0 not data mode, 1 data mode; return "Link is builded"
// output: 1 if success, 0 if fail
int ESP8266_SetDataTransmissionMode(uint8_t mode);

//---------ESP8266_GetStatus----------
// get status
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetStatus(void);

//--------ESP8266_EnableRXInterrupt--------
// - enables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_EnableRXInterrupt(void);

//--------ESP8266_DisableRXInterrupt--------
// - disables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_DisableRXInterrupt(void);

//--------ESP8266_PrintChar--------
// prints a character to the esp8226 via uart
// Inputs: character to transmit
// Outputs: none
// busy-wait synchronization
void ESP8266_PrintChar(char input);

// ----------ESP8266_QuitAccessPoint-------------
// - disconnects from currently connected wifi access point
// Inputs: none
// Outputs: 1 if success, 0 if fail
int ESP8266_QuitAccessPoint(void);

//************the following are not tested********
void ESP8266_SetServerTimeout(uint16_t timeout);
void ESP8266_EnableServer(uint16_t port);

// serves a page via the ESP8266
void HTTP_ServePage(const char* body);
#endif
