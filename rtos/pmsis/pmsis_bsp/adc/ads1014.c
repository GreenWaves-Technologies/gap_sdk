#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/adc/ads1014.h"

/**************/
/* Structures */
/**************/

enum ads1014_registers {
    ADS1014_REGISTER_VALUE          = 0x0,
    ADS1014_REGISTER_CONF           = 0x1,
    ADS1014_REGISTER_THRESHOLD_LOW  = 0x2,
    ADS1014_REGISTER_THRESHOLD_HIGH = 0x3,
};

typedef union __attribute__((__packed__)) {
    struct __attribute__((packed)) {
        uint8_t _unused :4;
        int16_t value :12;
    } reg;
    uint16_t value;
} ads1014_register_value_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        enum ads1014_comparator_status comparator_status :2;
        enum ads1014_comparator_latch comparator_latch :1;
        enum ads1014_comparator_polarity comparator_polarity :1;
        enum ads1014_comparator_mode comparator_mode :1;
        enum ads1014_data_rate data_rate :3;
        enum ads1014_operating_mode operating_mode :1;
        enum ads1014_pga pga :3;
        uint8_t _unused: 3;
        uint8_t converting :1;
    } reg;
    uint16_t value;
} ads1014_register_conf_t;

typedef struct {
    pi_device_t i2c_device;
    ads1014_register_conf_t adc_conf;
} ads1014_data_t;

/********************/
/* Static functions */
/********************/

static int
__ads1014_update_configuration(pi_device_t *device) {
    ads1014_data_t *data = (ads1014_data_t*) device->data;

    /* set the adc to trigger a conversion (useful in single shot mode) */
    data->adc_conf.reg.converting = 1;

    uint16_t packed_conf = data->adc_conf.value;
    uint8_t payload[3] = { ADS1014_REGISTER_CONF,
        (packed_conf >> 8) & 0xFF,
        packed_conf & 0xFF,
    };

    int status = pi_i2c_write(&data->i2c_device, payload, 3,
            PI_I2C_XFER_START | PI_I2C_XFER_STOP);
    return status;
};

/**
 *
 * Convert a float value (in mV) to fit in a comparator threshold register
 *
 * @param[in] pga ads1014 pga setting (to know the scale)
 * @param[in] f float to convert
 * @param[out] reg register that will contain the final 12 bit value
 *
 * @return PI_OK if operation was successful,
 *         an error otherwise
 */
static inline int
__convert_float_to_register_value(enum ads1014_pga pga, float f,
        ads1014_register_value_t *reg)
{
    if (NULL == reg) {
        return PI_ERR_INVALID_ARG;
    }

    /* Change the scale according to the ADC PGA */
    switch(pga) {
        case ADS1014_PGA_FSR_6V144:
            f /= 3;
            break;
        case ADS1014_PGA_FSR_4V096:
            f /= 2;
            break;
        case ADS1014_PGA_FSR_2V048:
            /* nothing, f *= 1*/
            break;
        case ADS1014_PGA_FSR_1V024:
            f *= 2;
            break;
        case ADS1014_PGA_FSR_0V512:
            f *= 4;
            break;
        case ADS1014_PGA_FSR_0V256: /* fallthrough */
        default:
            f *= 8;
            break;
    }

    /* fit into 12 bits */
    int32_t i = f;
    /* check if it fits in 12 bits */
    if (__builtin_pulp_clb(i) < 20) {
        return PI_ERR_INVALID_STATE;
    }
    reg->reg.value = f;

    return PI_OK;
}

/**
 * Helper to write a comparator register
 *
 * @param[in] device pointer to the ads1014 device (not checked)
 * @param[in] reg register to write
 * @param[in] value value to write in the register
 *
 * @return PI_OK if operation was succesful, an error code otherwise
 */
static inline int
__write_comparator_register(pi_device_t *device, enum ads1014_registers reg,
        ads1014_register_value_t value)
{
    ads1014_data_t *data = (ads1014_data_t*) device->data;

    uint16_t packed_conf = value.value;
    uint8_t payload[3] = { reg,
        (packed_conf >> 8) & 0xFF,
        packed_conf & 0xFF,
    };

    int status = pi_i2c_write(&data->i2c_device, payload, 3,
            PI_I2C_XFER_START | PI_I2C_XFER_STOP);
    return status;
}

/*****************/
/* API functions */
/*****************/

void pi_ads1014_conf_init(struct pi_ads1014_conf *conf) {
    if (NULL == conf) {
        return;
    }

    /* I2C related */
    conf->i2c_itf = 1; //TODO retrieve from BSP
    conf->i2c_addr = 0x90; //TODO retrieve from BSP

    /* ADC General settings */
    conf->operating_mode = ADS1014_OPERATING_MODE_SINGLE_SHOT;
    conf->pga = ADS1014_PGA_FSR_2V048;
    conf->data_rate = ADS1014_DATA_RATE_SPS_1600;

    /* ADC comparator settings */
    conf->comparator_status = ADS1014_COMPARATOR_STATUS_DISABLED;
    conf->comparator_mode = ADS1014_COMPARATOR_MODE_TRADITIONAL;
    conf->comparator_latch = ADS1014_COMPARATOR_LATCH_DISABLED;
    conf->comparator_polarity = ADS1014_COMPARATOR_POLARITY_ACTIVE_LOW;
}

int pi_ads1014_open(pi_device_t *device) {
    if (NULL == device) {
        return PI_ERR_INVALID_ARG;
    }

    struct pi_ads1014_conf *conf = (struct pi_ads1014_conf *) device->config;

    /* allocate memory for runtime data */
    ads1014_data_t *ads1014 = (ads1014_data_t *) pi_l2_malloc(sizeof(ads1014_data_t));
    if (NULL == ads1014) {
        return PI_ERR_L2_NO_MEM;
    }
    device->data = (void*) ads1014;

    /* open bsp */
    //TODO

    /* initialize configuration register */
    {
        ads1014->adc_conf.reg.operating_mode = conf->operating_mode;
        ads1014->adc_conf.reg.pga = conf->pga;
        ads1014->adc_conf.reg.data_rate = conf->data_rate;
        ads1014->adc_conf.reg.comparator_status = conf->comparator_status;
        ads1014->adc_conf.reg.comparator_mode = conf->comparator_mode;
        ads1014->adc_conf.reg.comparator_latch = conf->comparator_latch;
        ads1014->adc_conf.reg.comparator_mode = conf->comparator_mode;
        ads1014->adc_conf.reg.comparator_polarity = conf->comparator_polarity;
        ads1014->adc_conf.reg.converting = 1; /* initial trigger */
    }

    /* initialize I2C device */
    {
        struct pi_i2c_conf i2c_conf;
        pi_i2c_conf_init(&i2c_conf);
        i2c_conf.itf = conf->i2c_itf;
        i2c_conf.max_baudrate = 100000;
        pi_i2c_conf_set_slave_addr(&i2c_conf, conf->i2c_addr, 0);

        pi_open_from_conf(&ads1014->i2c_device, &i2c_conf);

        if (PI_OK != pi_i2c_open(&ads1014->i2c_device)) {
            pi_l2_free(device->data, sizeof(ads1014_data_t));
            return PI_ERR_INVALID_STATE;
        }
    }

    /* ADC configuration */
    int status = __ads1014_update_configuration(device);
    if (status != PI_OK) {
        pi_i2c_close(&ads1014->i2c_device);
        pi_l2_free(device->data, sizeof(ads1014_data_t));
        return PI_ERR_INVALID_STATE;
    }

    return PI_OK;
}

void pi_ads1014_close(pi_device_t *device) {
    if (NULL == device || NULL == device->data) {
        return;
    }

    ads1014_data_t *data = (ads1014_data_t*) device->data;

    /* close devices */
    pi_i2c_close(&data->i2c_device);

    /* free memory */
    pi_l2_free(data, sizeof(ads1014_data_t));
    device->data = NULL;
}

int pi_ads1014_read(pi_device_t *device, float *value) {
    if (NULL == device || NULL == device->data || NULL == value) {
        return PI_ERR_INVALID_ARG;
    }

    ads1014_data_t *data = (ads1014_data_t*) device->data;

    int status = PI_OK;

    // if ADC is in continous mode, no need to trigger a conversion.
    // else we need to trigger for a single shot measurement
    if (data->adc_conf.reg.operating_mode == ADS1014_OPERATING_MODE_SINGLE_SHOT) {
        status = __ads1014_update_configuration(device);
        if (PI_OK != status) {
            return PI_ERR_INVALID_STATE;
        }

        //TODO wait for the end of the conversion ?
    }

    uint8_t write_payload = ADS1014_REGISTER_VALUE;
    ads1014_register_value_t result;

    status = pi_i2c_write(&data->i2c_device, &write_payload, 1,
            PI_I2C_XFER_START | PI_I2C_XFER_STOP);

    if (status != PI_OK)  {
        return PI_ERR_INVALID_STATE;
    }

    status = pi_i2c_read(&data->i2c_device, (uint8_t*) &result, 2,
            PI_I2C_XFER_START | PI_I2C_XFER_STOP);

    if (status != PI_OK) {
        return PI_ERR_INVALID_STATE;
    }

    result.value= (result.value << 8) | (result.value >> 8);

    /* conversion depending on the pga */
    switch(data->adc_conf.reg.pga) {
        case ADS1014_PGA_FSR_6V144:
            *value = result.reg.value * 3;
            break;
        case ADS1014_PGA_FSR_4V096:
            *value = result.reg.value * 2;
            break;
        case ADS1014_PGA_FSR_2V048:
            *value = result.reg.value * 1;
            break;
        case ADS1014_PGA_FSR_1V024:
            *value = result.reg.value * 0.5;
            break;
        case ADS1014_PGA_FSR_0V512:
            *value = result.reg.value * 0.25;
            break;
        case ADS1014_PGA_FSR_0V256: /* fallthrough */
        default:
            *value = result.reg.value * 0.125;
            break;
    }

    return PI_OK;
}

int pi_ads1014_set_comparator_thresholds(pi_device_t *device,
        float threshold_low, float threshold_high)
{
    if (NULL == device || NULL == device->data) {
        return PI_ERR_INVALID_ARG;
    }

    ads1014_data_t *data = (ads1014_data_t*) device->data;

    ads1014_register_value_t low_th_reg;
    ads1014_register_value_t high_th_reg;

    /* Convert threshold to 12bits */
    {
        /* return an error if they do not fit inside 12 bits */
        enum ads1014_pga pga = data->adc_conf.reg.pga;

        if (PI_OK != __convert_float_to_register_value(pga, threshold_low,
                    &low_th_reg)) {
            return PI_ERR_INVALID_STATE;
        }

        if (PI_OK != __convert_float_to_register_value(pga, threshold_high,
                    &high_th_reg)) {
            return PI_ERR_INVALID_STATE;
        }
    }

    /* write values to registers */
    int status = __write_comparator_register(device,
            ADS1014_REGISTER_THRESHOLD_LOW, low_th_reg);
    if (PI_OK != status) {
        return status;
    }

    status = __write_comparator_register(device,
            ADS1014_REGISTER_THRESHOLD_HIGH, high_th_reg);
    if (PI_OK != status) {
        return status;
    }

    return PI_OK;
}
