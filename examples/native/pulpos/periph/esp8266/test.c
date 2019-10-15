#include "ESP8266.h"

#define SSID     "SSID"
#define PASSWORD "PASSWORD"

bool WIFI_Send(char *host, char *payload){
    ESP8266_GetStatus();

    bool success = false;

    if(ESP8266_MakeTCPConnection(host)){ // open socket in server
        success = ESP8266_SendTCP(payload);
    }

    ESP8266_CloseTCPConnection();

    return success;
}

int main()
{
    ESP8266_Init(115200, SSID, PASSWORD);

    printf("Initilization OK!\n");

    WIFI_Send("api.thingspeak.com", "GET /update?key=[YOUR_API_KEYS]&field1=55 \r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n");

    return 0;
}
