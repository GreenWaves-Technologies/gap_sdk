#include "pmsis.h"
#include "pmsis/device.h"
#include "pmsis/rtos/rtos.h"

static inline pi_device_e get_pi_device_type(void *conf);

// extract pi_device_type
static inline pi_device_e get_pi_device_type(void *conf)
{
    pi_device_e *device_ptr = (pi_device_e*)conf;
    return *device_ptr;
}

void pi_open_from_conf(struct pi_device *device, void *conf)
{
    #if 0
    pi_device_e device_type = get_pi_device_type(conf);
    switch (device_type)
    {
    case PI_DEVICE_CLUSTER_TYPE:
        device->config = conf;
        pi_cluster_open(device);
        break;
    case PI_DEVICE_CPI_TYPE:
    case PI_DEVICE_HYPERBUS_TYPE:
    case PI_DEVICE_I2C_TYPE:
    case PI_DEVICE_SPI_TYPE:
    case PI_DEVICE_GPIO_TYPE:
        device->config = conf;
        break;
    default:
        device->config = conf;
        // TODO: add debug warning here
        break;
    }
    #else
    device->config = conf;
    #endif
}
