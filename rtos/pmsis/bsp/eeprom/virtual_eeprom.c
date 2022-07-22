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

#include <stdbool.h>

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/eeprom/virtual_eeprom.h"

// Buffer size in bytes
#define RX_BUFFER_SIZE 72
#define EEPROM_BUFFER_SIZE 64

typedef struct eeprom_slave
{
    struct eeprom_slave *next;
    struct pi_device i2c_slave_device;  // I2C slave device
    uint8_t itf_id; // i2c interface id
    uint8_t *rx_buffer; // buffer used for reception
    uint8_t *data_buffer; // eeprom memory
    uint8_t current_address; // current eeprom address, ranging from 0 to EEPROM_BUFFER_SIZE-1
} eeprom_slave_t;


static eeprom_slave_t *virtual_eeprom_list = NULL;

static eeprom_slave_t* __get_eeprom_slave_by_id(uint8_t id)
{
    eeprom_slave_t *iter = virtual_eeprom_list;
    while(iter != NULL)
    {
        if(iter->itf_id != id)
        {
            iter = iter->next;
        }
        else
        {
            return iter;
        }
    }
    return NULL;
}

static void __virtual_eeprom_cb_rx(pi_i2c_slave_args_t *arg)
{
    uint8_t *rx_buf = (uint8_t*) arg->l2_buffer;

    eeprom_slave_t *eeprom_slave =__get_eeprom_slave_by_id(arg->itf_id);
    /* first 1 byte of the request contains the slave address */
    /* next 2 bytes should contain the address (LSB MSB)*/
    /* next bytes are data to write */
    /* current address is increased for each byte of data */

    uint16_t slave_addr = arg->slave_addr;
    bool in_range = true; // indicate whether the write is in range

    if (arg->nb_bytes >= 2) {
        uint16_t memory_addr = rx_buf[0] | rx_buf[1] << 8;
        /* check that addr is in range */
        if (memory_addr > EEPROM_BUFFER_SIZE)
        {
            memory_addr = 0;
            in_range = false;
        }
        if (in_range)
        {
            eeprom_slave->current_address = memory_addr;
        }
    }

    if (arg->nb_bytes >= 3) {
        if (in_range)
        {
            /* write data in memory to eeprom_current_address */
            uint32_t eeprom_index = 0;
            for (uint32_t i = 2; i < arg->nb_bytes; i++)
            {
                eeprom_index = i - 2 + eeprom_slave->current_address;
                if (eeprom_index > EEPROM_BUFFER_SIZE)
                {
                    break;
                }
                eeprom_slave->data_buffer[eeprom_index] = rx_buf[i];
            }
        }
    }

    pi_i2c_slave_stop_rx(arg->handle);
    pi_i2c_slave_stop_tx(arg->handle);
    pi_i2c_slave_unlock(arg->handle, 1);
    pi_i2c_slave_set_tx(arg->handle, &eeprom_slave->data_buffer[eeprom_slave->current_address],
            EEPROM_BUFFER_SIZE - eeprom_slave->current_address+ 1);
    pi_i2c_slave_set_rx(arg->handle, eeprom_slave->rx_buffer, RX_BUFFER_SIZE);

    arg->ret = 0;

}

static void __virtual_eeprom_cb_tx(pi_i2c_slave_args_t *arg)
{
    // nothing to do for this emulation except unlocking and reloading buf
    eeprom_slave_t *eeprom_slave =__get_eeprom_slave_by_id(arg->itf_id);

    pi_i2c_slave_stop_tx(arg->handle);
    pi_i2c_slave_stop_rx(arg->handle);
    pi_i2c_slave_unlock(arg->handle, 0);
    pi_i2c_slave_set_tx(arg->handle, &eeprom_slave->data_buffer[eeprom_slave->current_address],
                        EEPROM_BUFFER_SIZE - eeprom_slave->current_address);
    pi_i2c_slave_set_rx(arg->handle, eeprom_slave->rx_buffer, RX_BUFFER_SIZE);
    arg->ret = 0;
}

static void __virtual_eeprom_data_init(eeprom_slave_t *eeprom_slave)
{
    eeprom_slave->data_buffer = (uint8_t*) pi_l2_malloc(sizeof(uint8_t) * EEPROM_BUFFER_SIZE);
    eeprom_slave->rx_buffer = (uint8_t*) pi_l2_malloc(sizeof(uint8_t) * RX_BUFFER_SIZE);
    //memset(eeprom_slave->data_buffer, 0, EEPROM_BUFFER_SIZE);
    for (int i = 0; i < EEPROM_BUFFER_SIZE; i++)
    {
        eeprom_slave->data_buffer[i] = i;
    }
    memset(eeprom_slave->rx_buffer, 0x77, RX_BUFFER_SIZE);
}

static void __virtual_eeprom_data_finalize(eeprom_slave_t *eeprom_slave)
{
    pi_l2_free(eeprom_slave->data_buffer, sizeof(uint8_t) * EEPROM_BUFFER_SIZE);
}

int pi_virtual_eeprom_open(struct pi_device *device)
{
    struct pi_virtual_eeprom_conf *conf = (struct pi_virtual_eeprom_conf *)device->config;
    struct pi_i2c_slave_conf i2c_conf;

    eeprom_slave_t *eeprom_slave = pi_l2_malloc(sizeof(eeprom_slave_t));

    device->data = (void*) eeprom_slave;
    device->api = (struct pi_device_api*) conf->eeprom_slave.api;

    eeprom_slave->itf_id = conf->i2c_itf;

    /* Initialize EEPROM */
    __virtual_eeprom_data_init(eeprom_slave);

    /* Initialize slave configuration */
    pi_i2c_slave_conf_init(&i2c_conf);
    i2c_conf.itf = conf->i2c_itf;
    pi_i2c_slave_conf_set_addr0(&i2c_conf, conf->i2c_addr, 0x1F, 0, 1, 0);

    /* setup RX and TX callbacks */
    i2c_conf.rx_callback = __virtual_eeprom_cb_rx;
    i2c_conf.tx_callback = __virtual_eeprom_cb_tx;

    pi_open_from_conf(&eeprom_slave->i2c_slave_device, &i2c_conf);
    int ret = pi_i2c_slave_open(&eeprom_slave->i2c_slave_device);
    if(!ret)
    {
        pi_i2c_slave_set_rx(eeprom_slave->i2c_slave_device.data, eeprom_slave->rx_buffer, RX_BUFFER_SIZE);
        pi_i2c_slave_set_tx(eeprom_slave->i2c_slave_device.data, eeprom_slave->data_buffer, EEPROM_BUFFER_SIZE);

        int irq = disable_irq();
        /* Add to EEPROM list */
        eeprom_slave->next = virtual_eeprom_list;
        virtual_eeprom_list = eeprom_slave;
        restore_irq(irq);
    }
    else
    {
        __virtual_eeprom_data_finalize(eeprom_slave);
    }

    return ret;
}


static void pi_virtual_eeprom_close(struct pi_device *device)
{
    eeprom_slave_t *eeprom_slave = (eeprom_slave_t*) device->data;

    /* finalize EEPROM */
    __virtual_eeprom_data_finalize(eeprom_slave);

    /* close I2C slave */
    pi_i2c_slave_close(&eeprom_slave->i2c_slave_device);
    pi_l2_free(&eeprom_slave->i2c_slave_device, sizeof(pi_device_t));

    /* remove from EEPROM list */
    int irq = disable_irq();
    eeprom_slave_t *iter = virtual_eeprom_list;
    eeprom_slave_t *pre_iter = virtual_eeprom_list;

    while (iter != NULL)
    {
        if (iter != eeprom_slave)
        {
            pre_iter = iter;
            iter = iter->next;
        }
        else
        {
            if (iter == virtual_eeprom_list)
            {
                virtual_eeprom_list = iter->next;
            }
            else
            {
                pre_iter->next = iter->next;
            }
            break;
        }
    }
    restore_irq(irq);

    return;
}

static pi_eeprom_slave_api_t pi_virtual_eeprom_api =
{
  .open                 = &pi_virtual_eeprom_open,
  .close                = &pi_virtual_eeprom_close
};

void pi_virtual_eeprom_conf_init(struct pi_virtual_eeprom_conf *conf)
{
  conf->eeprom_slave.api = &pi_virtual_eeprom_api;
  bsp_virtual_eeprom_conf_init(conf);
}
