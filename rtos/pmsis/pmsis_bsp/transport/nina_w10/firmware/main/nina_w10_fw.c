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

/*
 * Authors:  Esteban Gougeon, GreenWaves Technologies (esteban.gougeon@greenwaves-technologies.com)
 *           Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_event_loop.h"
#include "soc/rtc_cntl_reg.h"
//#include "esp32/rom/cache.h"
#include "driver/spi_slave.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/igmp.h"


//SPI code
//SPI pin on the EVK Board
#if 0
#define GPIO_HANDSHAKE 22
#define GPIO_MOSI 14
#define GPIO_MISO 5
#define GPIO_SCLK 12
#define GPIO_CS 13
#else
#define GPIO_HANDSHAKE 2
#define GPIO_NOTIF 32
#define GPIO_MOSI 19
#define GPIO_MISO 23
#define GPIO_SCLK 18
#define GPIO_CS 5
#endif


//#define GPIO_HANDSHAKE 32
//#define GPIO_MOSI 19
//#define GPIO_MISO 23
//#define GPIO_SCLK 18
//#define GPIO_CS 5


#define NINA_W10_CMD_SETUP        0x80
#define NINA_W10_CMD_SEND_PACKET  0x81
#define NINA_W10_CMD_TEST         0x82
#define NINA_W10_CMD_SET_VERBOSE  0x83


#define BUFFER_LENGTH 2048


#define GOT_IPV4_BIT  BIT(0)
#define CONNECTED_BITS  (GOT_IPV4_BIT)

static const char *TAG = "nina_fw";

/* FreeRTOS event group to signal when we are connected*/
static ip4_addr_t s_ip_addr;
static const char* s_connection_name;

/* The event group allows multiple bits for each event, but we only care about one event */
const int WIFI_CONNECTED_BIT = BIT0;

#define SPI_BUFFER_LEN SPI_MAX_DMA_LEN

static uint8_t *commandBuffer;
static uint8_t *responseBuffer;
static int sock = -1;
static int verbose = 1;

typedef struct
{
  uint32_t type;
  uint32_t size;
} __attribute__((packed)) nina_req_t;


//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
}

static EventGroupHandle_t s_connect_event_group;

/*static void on_got_ip(void* arg, esp_event_base_t event_base,
                      int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
    xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
}*/

/*static void on_wifi_disconnect(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    ESP_ERROR_CHECK( esp_wifi_connect() );
}*/

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        //esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        memcpy(&s_ip_addr, &event->event_info.got_ip.ip_info, sizeof(s_ip_addr));
        xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);

        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        {
            ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
            ESP_ERROR_CHECK( esp_wifi_connect() );
            
            break;
        }
    default:
        break;
    }
    return ESP_OK;
}

static void start(wifi_config_t *wifi_config)
{
  //uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    //ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_LOGI(TAG, "Connecting to %s...", wifi_config->sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
    //ESP_ERROR_CHECK(esp_base_mac_addr_set(mac));
    ESP_ERROR_CHECK(esp_wifi_start());
    //ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));
    ESP_ERROR_CHECK(esp_wifi_connect());
    s_connection_name = (const char *)wifi_config->sta.ssid;
}

#if 0
static void stop()
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}
#endif


static esp_err_t wifi_connect(wifi_config_t *config)
{
    if (s_connect_event_group != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    s_connect_event_group = xEventGroupCreate();
    start(config);
    xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected new to %s", s_connection_name);
    ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));

    return ESP_OK;
}

#if 0
static esp_err_t wifi_disconnect()
{
    if (s_connect_event_group == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    vEventGroupDelete(s_connect_event_group);
    s_connect_event_group = NULL;
    stop();
    ESP_LOGI(TAG, "Disconnected from %s", s_connection_name);
    s_connection_name = NULL;
    return ESP_OK;
}
#endif

void spi_slave_init()
{
    esp_err_t ret;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=GPIO_CS,
        .queue_size=3,
        .flags=0,
        .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };
    
    //Configuration for the handshake line
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_OUTPUT,
        .pin_bit_mask=(1<<GPIO_HANDSHAKE)
    };

    //Configure handshake line as output
    gpio_config(&io_conf);

    //Configuration for the handshake line
    gpio_config_t io_conf2={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_OUTPUT,
        .pin_bit_mask=(1ULL<<GPIO_NOTIF)
    };

    //Configure handshake line as output
    gpio_config(&io_conf2);

    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1ULL<<GPIO_NOTIF));


    //Initialize SPI slave interface
    ret=spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, 1);
    assert(ret==ESP_OK);

    commandBuffer = (uint8_t*)heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA);
    responseBuffer = (uint8_t*)heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA);

}

static int command_extract_string(uint8_t *buffer, const char **str, int max_len)
{
  *str = (const char *)buffer;
  return strnlen(*str, max_len) + 1;
}


static int command_extract_uint32(uint8_t *buffer, uint32_t *value, int max_len)
{
  if (max_len < 4)
    return 0;

  *value = *(uint32_t *)buffer;

  return 4;
}


static int handle_setup_command(uint8_t *command_buffer, uint8_t *response_buffer)
{
  const char *ssid;
  const char *passwd;
  const char *ip_addr;
  uint32_t port;

  uint8_t *buffer = &command_buffer[8];

  buffer += command_extract_string(buffer, &ssid, SPI_BUFFER_LEN - (buffer - command_buffer));
  buffer += command_extract_string(buffer, &passwd, SPI_BUFFER_LEN - (buffer - command_buffer));
  buffer += command_extract_string(buffer, &ip_addr, SPI_BUFFER_LEN - (buffer - command_buffer));
  buffer += command_extract_uint32(buffer, &port, SPI_BUFFER_LEN - (buffer - command_buffer));

  printf("Received command setup (ssid: %s, passwd: %s, ip_addr: %s, port: %d)\n", ssid, passwd, ip_addr, port);

  wifi_config_t wifi_config;

  memset((void *)&wifi_config, 0, sizeof(wifi_config_t));

  strncpy((char *)wifi_config.sta.ssid, ssid, 32);
  strncpy((char *)wifi_config.sta.password, passwd, 64);

  /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.*/
  ESP_ERROR_CHECK(wifi_connect(&wifi_config));

  int addr_family;
  int ip_protocol;

  //Network dependent structure

  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = inet_addr(ip_addr);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;

  // Sock creation and connection
  sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
  if (sock < 0) {
      ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
      return -1;
  }
  ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip_addr, port);

  int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err != 0) {
      ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
      return -1;
  }
  ESP_LOGI(TAG, "Successfully connected");

  return 0;
}


static int handle_test_command(uint8_t *command_buffer, uint8_t *response_buffer)
{
  nina_req_t *req = (nina_req_t *)command_buffer;

  if (verbose)
    printf("Executing test command (size: 0x%x)\n", req->size);

  spi_slave_transaction_t t;
  memset(&t, 0, sizeof(t));

  t.length = req->size*8;
  t.tx_buffer = NULL;
  t.rx_buffer = response_buffer;
  t.trans_len = 0;

  if (spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY))
      return -1;

  for (int i=0; i<req->size; i++)
    printf("%d: %x\n", i, response_buffer[i]);

  printf("Raising IRQ\n");
  WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1ULL<<GPIO_NOTIF));

  memset(&t, 0, sizeof(t));

  t.length = req->size*8;
  t.tx_buffer = response_buffer;
  t.rx_buffer = NULL;
  t.trans_len = 0;

  if (spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY))
      return -1;

  return 0;
}



static int handle_set_verbose_command(uint8_t *command_buffer, uint8_t *response_buffer)
{
  verbose = 1;
  return 0;
}



static int handle_send_packet_command(uint8_t *command_buffer, uint8_t *response_buffer)
{
  nina_req_t *req = (nina_req_t *)command_buffer;

  if (sock != -1)
  {
    int size = req->size;

    while(size > 0)
    {
      int iter_size = 1024;
      if (iter_size > size)
        iter_size = size;

      spi_slave_transaction_t t;
      memset(&t, 0, sizeof(t));

      //Set up a transaction of 128 bytes to send/receive
      t.length = iter_size*8;
      t.tx_buffer = NULL;
      t.rx_buffer = commandBuffer;
      t.trans_len = 0;

      if (spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY))
          return -1;

      if (t.trans_len == 0)
          return 0;

      int err = send(sock,(const char*)command_buffer, iter_size, 0);
      if (err < 0) {
          ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
          sock = -1;
          return -1;
      }

      size -= iter_size;
    }
  }
  return 0;
}




static int handle_command(uint8_t *command_buffer, uint8_t *response_buffer)
{
  nina_req_t *req = (nina_req_t *)command_buffer;
  //printf("Handling command %x\n", req->type);

  switch (req->type)
  {
    case NINA_W10_CMD_SETUP:
      handle_setup_command(command_buffer, response_buffer);
      break;

    case NINA_W10_CMD_SEND_PACKET:
      handle_send_packet_command(command_buffer, response_buffer);
      break;

    case NINA_W10_CMD_TEST:
      handle_test_command(command_buffer, response_buffer);
      break;

    case NINA_W10_CMD_SET_VERBOSE:
      handle_set_verbose_command(command_buffer, response_buffer);
      break;
  }

  return 0;

}



static int get_command(uint8_t *buffer)
{
  spi_slave_transaction_t t;
  memset(&t, 0, sizeof(t));

  t.length = BUFFER_LENGTH*8;
  t.tx_buffer = NULL;
  t.rx_buffer = buffer;
  t.trans_len = 0;

  if (spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY))
    return -1;

  if (t.trans_len == 0)
      return 0;

  return t.trans_len;

}


static void tcp_client_task(void *pvParameters)
{
  spi_slave_init();

  while (1)
  {
    while(1)
    {
      if (get_command(commandBuffer) <= 0)
        continue;

      handle_command(commandBuffer, responseBuffer);
    }

    if (sock != -1)
    {
      ESP_LOGE(TAG, "Shutting down socket and restarting...");
      shutdown(sock, 0);
      close(sock);
    }
  }
  vTaskDelete(NULL);
}

#if 0
void init_gpio(int gpio, int value)
{
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_INPUT,
        .pin_bit_mask=(1<<gpio)
    };

}

void set_gpio(int gpio, int value)
{
    if (value)
      WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<gpio));
    else
      WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<gpio));
}
#endif

#define BLINK_GPIO 4
void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);

  //Set the LED GPIO for blinking
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
	//printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
	//printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
