// ESP8266.c
/*
   Hardware connections
   Vcc is a separate regulated 3.3V supply with at least 215mA
   /------------------------------\
   |              chip      1   8 |
   | Ant                    2   7 |
   | enna       processor   3   6 |
   |                        4   5 |
   \------------------------------/
   ESP8266    TM4C123
   1 URxD    PB1   UART out of TM4C123, 115200 baud
   2 GPIO0         +3.3V for normal operation (ground to flash)
   3 GPIO2         +3.3V
   4 GND     Gnd   GND (70mA)
   5 UTxD    PB0   UART out of ESP8266, 115200 baud
   6 Ch_PD         chip select, 10k resistor to 3.3V
   7 Reset   PB5   TM4C123 can issue output low to cause hardware reset
   8 Vcc           regulated 3.3V supply with at least 70mA
*/
#include "ESP8266.h"
#include "AT_RING_BUFFER.h"

/*
  ===========================================================
  ==========          CONSTANTS                    ==========
  ===========================================================
*/
// Access point parameters
const char *SSID_NAME;
const char *PASSKEY;

#define MAXTRY 10
// prototypes for helper functions
void ESP8266SendCommand(const char* inputString);
int  ESP8266Read(const char* response);

/*
  =============================================================
  ==========            GLOBAL VARIABLES             ==========
  =============================================================
*/

#define  BUFFER_SIZE          256
char TXBuffer[BUFFER_SIZE];

#define  SERVER_RESPONSE_SIZE 1024
char     ServerResponseBuffer[SERVER_RESPONSE_SIZE]; // characters after +IPD,
uint32_t ServerResponseIndex = 0;

volatile bool ESP8266_ProcessBuffer = false;
volatile bool ESP8266_EchoResponse = false;
volatile bool ESP8266_ResponseComplete = false;
volatile bool ESP8266_APEnabled = false;
volatile bool ESP8266_ClientEnabled = false;
volatile bool ESP8266_ServerEnabled = false;
volatile bool ESP8266_InputProcessingEnabled = false;
volatile bool ESP8266_PageRequested = false;

/*
  =======================================================================
  ==========              search FUNCTIONS                     ==========
  =======================================================================
*/
char SearchString[32];
volatile bool SearchLooking = false;
volatile bool SearchFound = false;
volatile uint32_t SearchIndex = 0;

static char *strcpy(char *dst, char *src)
{
    char *start = dst;

    while(*src != '\0')
    {
        *dst = *src;
        dst++;
        src++;
    }

    *dst = '\0'; // add '\0' at the end
    return start;
}

static int strlen(const char *str)
{
    int result = 0;
    while (*str)
    {
        result++;
        str++;
    }
    return result;
}

char lc(char letter){
    if((letter>='A')&&(letter<='Z')) letter |= 0x20;
    return letter;
}
//-------------------SearchStart -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none
void SearchStart(char *pt){
    strcpy(SearchString,pt);
    SearchIndex = 0;
    SearchFound = false;
    SearchLooking = true;
}
//-------------------SearchCheck -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none
void SearchCheck(char letter){
    if(SearchLooking){
        if(SearchString[SearchIndex] == lc(letter)){ // match letter?
            SearchIndex++;
            if(SearchString[SearchIndex] == 0){ // match string?
                SearchFound = true;
                SearchLooking = false;
            }
        }else{
            SearchIndex = 0; // start over
        }
    }
}

char ServerResponseSearchString[16]="+ipd,";
volatile uint32_t ServerResponseSearchFinished = false;
volatile uint32_t ServerResponseSearchIndex = 0;
volatile uint32_t ServerResponseSearchLooking = 0;

//-------------------ServerResponseSearchStart -------------------
// - start looking for server response string in received data stream
// Inputs: none
// Outputs: none
void ServerResponseSearchStart(void){
    strcpy(ServerResponseSearchString,"+ipd,");
    ServerResponseSearchIndex = 0;
    ServerResponseSearchLooking = 1; // means look for "+IPD"
    ServerResponseSearchFinished = 0;
    ServerResponseIndex = 0;
}

//-------------------ServerResponseSearchCheck -------------------
// - start looking for string in received data stream
// Inputs: none
// Outputs: none
void ServerResponseSearchCheck(char letter){
    if(ServerResponseSearchLooking==1){
        if(ServerResponseSearchString[ServerResponseSearchIndex] == lc(letter)){ // match letter?
            ServerResponseSearchIndex++;
            if(ServerResponseSearchString[ServerResponseSearchIndex] == 0){ // match string?
                ServerResponseSearchLooking = 2;
                strcpy(ServerResponseSearchString,"\r\nok\r\n");
                ServerResponseSearchIndex = 0;
            }
        }else{
            ServerResponseSearchIndex = 0; // start over
        }
    }else if(ServerResponseSearchLooking==2){
        if(ServerResponseIndex < SERVER_RESPONSE_SIZE){
            ServerResponseBuffer[ServerResponseIndex] = lc(letter); // copy data from "+IPD," to "OK"
            ServerResponseIndex++;
        }
        if(ServerResponseSearchString[ServerResponseSearchIndex] == lc(letter)){ // match letter?
            ServerResponseSearchIndex++;
            if(ServerResponseSearchString[ServerResponseSearchIndex] == 0){   // match OK string?
                ServerResponseSearchFinished = 1;
                ServerResponseSearchLooking = 0;
            }
        }else{
            ServerResponseSearchIndex = 0; // start over
        }
    }
}

/*
  =======================================================================
  ==========         UART1 and private FUNCTIONS               ==========
  =======================================================================
*/

//---------ESP8266SendCommand-----
// - sends a string to the esp8266 module
// uses busy-wait
// however, hardware has 16 character hardware FIFO
// Inputs: string to send (null-terminated)
// Outputs: none
void ESP8266SendCommand(const char* command){
    int index = 0;
    while(command[index] != 0){
        AT_COMMAND_Write((char *)&command[index++], 1);
    }
}

//---------ESP8266Read-----
// - sends a string to the esp8266 module
// uses busy-wait
// however, hardware has 16 character hardware FIFO
// Inputs: string to send (null-terminated)
// output: 1 if success, 0 if fail
int ESP8266Read(const char* response){
    SearchStart((char *)response);

    while (!SearchFound) {
        char c;
        // Read from ring buffer
        AT_COMMAND_Read(&c, 1);

        SearchCheck(c);               // check for end of command
        ServerResponseSearchCheck(c); // check for server response
    }

    return 1;
}

/*
  =======================================================================
  ==========          ESP8266 PUBLIC FUNCTIONS                 ==========
  =======================================================================
*/
//-------------------ESP8266_Init --------------
// initializes the module as a client
// Inputs: baud rate: tested with 9600 and 115200
// Outputs: none
void ESP8266_Init(uint32_t baud, const char* ssid, const char* password){

    SSID_NAME = ssid;
    PASSKEY   = password;

    // Init uart ring buffer
    AT_COMMAND_Init(baud);

    // step 1: AT+RST reset module
    if(ESP8266_Reset()==0){
        printf("Reset failure, could not reset\n\r"); while(1){};
    }

    // step 2: AT+CWMODE=1 set wifi mode to client (not an access point)
    if(ESP8266_SetWifiMode(ESP8266_WIFI_MODE_CLIENT)==0){
        printf("SetWifiMode, could not set mode\n\r"); while(1){};
    }

    if(ESP8266_QuitAccessPoint()==0){
        printf("QuitAccessPoint error, could not quit AP\n\r"); while(1){};
    }

    // step 3: AT+CWJAP="ValvanoAP","12345678"  connect to access point
    if(ESP8266_JoinAccessPoint(SSID_NAME,PASSKEY)==0){
        printf("JoinAccessPoint error, could not join AP\n\r"); while(1){};
    }

    // optional step: AT+CIFSR check to see our IP address
    if(ESP8266_GetIPAddress()==0){ // data streamed to UART0, OK
        printf("GetIPAddress error, could not get IP address\n\r"); while(1){};
    }
    //// optional step: AT+CIPMUX==0 set mode to single socket
    //  if(ESP8266_SetConnectionMux(0)==0){ // single socket
    //    printf("SetConnectionMux error, could not set connection mux\n\r"); while(1){};
    //  }

    // optional step: AT+CWLAP check to see other AP in area
    if(ESP8266_ListAccessPoints()==0){
        printf("ListAccessPoints, could not list access points\n\r"); while(1){};
    }

    // step 4: AT+CIPMODE=0 set mode to not data mode
    if(ESP8266_SetDataTransmissionMode(0)==0){
        printf("SetDataTransmissionMode, could not make connection\n\r"); while(1){};
    }
    ESP8266_InputProcessingEnabled = false; // not a server
}

//----------ESP8266_Reset------------
// resets the esp8266 module
// input:  none
// output: 1 if success, 0 if fail
int ESP8266_Reset() {
    ESP8266SendCommand("AT+RST\r\n");

    if(ESP8266Read("ready")) return 1; // success

    return 0; // fail
}

//---------ESP8266_SetWifiMode----------
// configures the esp8266 to operate as a wifi client, access point, or both
// since it searches for "ok" it will execute twice when changing modes
// Input: mode accepts ESP8266_WIFI_MODE constants
// output: 1 if success, 0 if fail
int ESP8266_SetWifiMode(uint8_t mode){
    if(mode > ESP8266_WIFI_MODE_AP_AND_CLIENT)return 0; // fail

    sprintf((char*)TXBuffer, "AT+CWMODE=%d\r\n", mode);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_SetConnectionMux----------
// enables the esp8266 connection mux, required for starting tcp server
// Input: 0 (single) or 1 (multiple)
// output: 1 if success, 0 if fail
int ESP8266_SetConnectionMux(uint8_t enabled){
    sprintf((char*)TXBuffer, "AT+CIPMUX=%d\r\n", enabled);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//----------ESP8266_JoinAccessPoint------------
// joins a wifi access point using specified ssid and password
// input:  SSID and PASSWORD
// output: 1 if success, 0 if fail
int ESP8266_JoinAccessPoint(const char* ssid, const char* password){
    sprintf((char*)TXBuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_ListAccessPoints----------
// lists available wifi access points
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_ListAccessPoints(void){
    ESP8266SendCommand("AT+CWLAP\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

// ----------ESP8266_QuitAccessPoint-------------
// - disconnects from currently connected wifi access point
// Inputs: none
// Outputs: 1 if success, 0 if fail
int ESP8266_QuitAccessPoint(void){
    ESP8266SendCommand("AT+CWQAP\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//----------ESP8266_ConfigureAccessPoint------------
// configures esp8266 wifi access point settings
// use this function only when in AP mode (and not in client mode)
// input:  SSID, Password, channel, security
// output: 1 if success, 0 if fail
int ESP8266_ConfigureAccessPoint(const char* ssid, const char* password, uint8_t channel, uint8_t encryptMode){
    sprintf((char*)TXBuffer, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ssid, password, channel, encryptMode);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_GetIPAddress----------
// Get local IP address
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetIPAddress(void){
    ESP8266SendCommand("AT+CIFSR\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_MakeTCPConnection----------
// Establish TCP connection
// Input: IP address or web page as a string
// output: 1 if success, 0 if fail
int ESP8266_MakeTCPConnection(char *IPaddress){
    sprintf((char*)TXBuffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", IPaddress);
    ESP8266SendCommand(TXBuffer);   // open and connect to a socket

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_SendTCP----------
// Send a TCP packet to server
// Input: TCP payload to send
// output: 1 if success, 0 if fail
int ESP8266_SendTCP(char* fetch){
    volatile uint32_t time,n;
    sprintf((char*)TXBuffer, "AT+CIPSEND=%d\r\n", strlen(fetch));
    ESP8266SendCommand(TXBuffer);

    rt_time_wait_us(50000);

    ESP8266SendCommand(fetch);
    ServerResponseSearchStart();
    n = 8000;
    while(n&&(ServerResponseSearchFinished==0)){
        time = (75825*8)/91;  // 1msec, tuned at 80 MHz
        while(time){
            time--;
        }
        n--;
    }
    if(ServerResponseSearchFinished==0) return 0; // no response
    return 1; // success
}

//---------ESP8266_CloseTCPConnection----------
// Close TCP connection
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_CloseTCPConnection(void){
    ESP8266SendCommand("AT+CIPCLOSE\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}
//---------ESP8266_SetDataTransmissionMode----------
// set data transmission mode
// Input: 0 not data mode, 1 data mode; return "Link is builded"
// output: 1 if success, 0 if fail
int ESP8266_SetDataTransmissionMode(uint8_t mode){
    sprintf((char*)TXBuffer, "AT+CIPMODE=%d\r\n", mode);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_GetStatus----------
// get status
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetStatus(void){
    ESP8266SendCommand("AT+CIPSTATUS\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

//---------ESP8266_GetVersionNumber----------
// get status
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_GetVersionNumber(void){
    ESP8266SendCommand("AT+GMR\r\n");

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}
// Program written by:
// - Steven Prickett  steven.prickett@gmail.com
//
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi access
//   point with a http server (IP = ...) serving a webpage
//   that allows the user to enter a message to send to the
//   microcontroller. The message will then be relayed back
//   over the USB UART.
//
//*********************************************************
/*
  This work is copyright Steven Prickett (steven.prickett@gmail.com) and
  licensed under a Creative Commons Attribution 3.0 Unported License,
  available at:

  https://creativecommons.org/licenses/by/3.0/

  THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
  OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
  VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
  OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*/
uint16_t ESP8266_ServerPort = 80;
uint16_t ESP8266_ServerTimeout = 28800;
// defines the form served by the http server

// ----------ESP8266_SetServerTimeout--------------
// - sets connection timeout for tcp server, 0-28800 seconds
// ***Prickett code for server, not used or tested in this client project
// Inputs: timeout parameter
// Outputs: none
void ESP8266_SetServerTimeout(uint16_t timeout){
    ESP8266_ServerTimeout = timeout;
    sprintf((char*)TXBuffer, "AT+CIPSTO=%d\r\n", ESP8266_ServerTimeout);
    ESP8266SendCommand((const char*)TXBuffer);
}

// --------ESP8266_EnableServer------------------
//  - enables tcp server on specified port
// ***Prickett code for server, not used or tested in this client project
// Inputs: port number
// Outputs: none
void ESP8266_EnableServer(uint16_t port){
    ESP8266_ServerPort = port;
    sprintf((char*)TXBuffer, "AT+CIPSERVER=1,%d\r\n", ESP8266_ServerPort);
    ESP8266SendCommand((const char*)TXBuffer);
}


//---------ESP8266_DisableServer----------
// disables tcp server
// ***Prickett code for server, not used or tested in this client project
// Input: none
// output: 1 if success, 0 if fail
int ESP8266_DisableServer(void){
    sprintf((char*)TXBuffer, "AT+CIPSERVER=0,%d\r\n", ESP8266_ServerPort);
    ESP8266SendCommand((const char*)TXBuffer);

    if(ESP8266Read("ok")) return 1; // success

    return 0; // fail
}

/*
  ========================================================================================================================
  ==========                                           HTTP FUNCTIONS                                           ==========
  ========================================================================================================================
*/

/*
  ===================================================================================================
  HTTP :: HTTP_ServePage

  - constructs and sends a web page via the ESP8266 server

  - NOTE: this seems to work for sending pages to Firefox (and maybe other PC-based browsers),
  but does not seem to load properly on iPhone based Safari. May need to add some more
  data to the header.
  ===================================================================================================
*/
void HTTP_ServePage(const char* body){
    char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: ";

    char contentLength[16];
    sprintf(contentLength, "%d\r\n\r\n", strlen(body));

    sprintf((char*)TXBuffer, "AT+CIPSEND=%d,%d\r\n", 0, strlen(body) + strlen(contentLength) + strlen(header));
    ESP8266SendCommand((const char*)TXBuffer);

    rt_time_wait_us(100000);

    ESP8266SendCommand(header);
    ESP8266SendCommand(contentLength);
    ESP8266SendCommand(body);
}
