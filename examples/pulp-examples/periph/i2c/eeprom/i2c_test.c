#include "rt/rt_api.h"

// For test:
#define addr_reg_test         0x0060
//#define addr_reg_test2         0x0101

#define TEST_VALUE            0x43

static RT_L2_DATA unsigned char addr_buff[2];
static RT_L2_DATA unsigned char tx_data[40];
static RT_L2_DATA unsigned char rx_data[40];

static void data_init(int nb){
    unsigned int addr = addr_reg_test;
    tx_data[0] = addr >> 8;
    tx_data[1] = addr & 0xff;
    for (int i=2; i<nb+2; i++)
        tx_data[i] = (i+1) | TEST_VALUE;

}

static void test_i2c0(int *err){
    printf("I2C0 test start\n");
    rt_i2c_conf_t i2c_conf;
    rt_i2c_conf_init(&i2c_conf);
    i2c_conf.cs = 0xA0;
    i2c_conf.id = 0;
    i2c_conf.max_baudrate = 200000;

    rt_i2c_t *i2c0 = rt_i2c_open(NULL, &i2c_conf, NULL);
    if (i2c0 == NULL) printf ("Filed to open I2C0\n");

    unsigned int addr = addr_reg_test;
    addr_buff[0] = addr >> 8;
    addr_buff[1] = addr & 0xff;
    data_init(1);
    rt_event_t *call_event;
    rt_i2c_write(i2c0, tx_data, 3, 0, NULL);
    for (volatile int x=0; x<300000; x++);

    rt_i2c_write(i2c0, addr_buff, 2, 1, NULL);
    rt_i2c_read(i2c0, rx_data, 1, 0, NULL);

    if (tx_data[2] != rx_data[0]) *err += 1;
    printf("tx: %x, rx: %x\n", tx_data[2], rx_data[0]);

    data_init(20);
    rt_i2c_write(i2c0, tx_data, 20+2, 0, NULL);
    for (volatile int x=0; x<300000; x++);

    rt_i2c_write(i2c0, addr_buff, 2, 1, NULL);
    rt_i2c_read(i2c0, rx_data, 20, 0, NULL);

    for (int i=2; i<20+2; i++){
        if(tx_data[i] != rx_data[i-2]) *err += 1;
        printf("%d tx: %x, rx: %x\n", i, tx_data[i], rx_data[i-2]);
    }
}

int main() {
    printf("Entering main controller\n");
    int err = 0;
    rt_event_alloc(NULL, 8);
    test_i2c0(&err);
    return err;
}
