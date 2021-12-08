
#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "bsp/bsp.h"
#include "bsp/camera/epeas_cis001.h"
#include "epeas_cis001.h"
//#include <string.h>



PI_L2 uint8_t tx_buffer[4] __attribute__((aligned(32))); // alignment is a requirement for SPI 
PI_L2 uint8_t rx_buffer[4] __attribute__((aligned(32))); // alignment is a requirement for SPI 


typedef struct {
   uint8_t addr  ;
   uint8_t value ;
} spi_req_t;


typedef struct {
  struct pi_epeas_cis001_conf conf;

  struct pi_device cpi_device;
  struct pi_device spi_device;
  struct pi_device pwm_device;
  struct pi_device gpio_port;

  spi_req_t spi_req;
  uint8_t spi_read_value;

  int is_awake;
} epeas_cis001_t;



typedef struct {
    uint8_t addr;
    uint8_t data;
} epeas_cis001_reg_init_t;



static epeas_cis001_reg_init_t __epeas_cis001_reg_init[] =
{
  {EPEAS_CIS001_SOFT_RESET,1},
  {EPEAS_CIS001_MODE,1},
  {EPEAS_CIS001_PMODE,1},
  {EPEAS_CIS001_PCLK_MODE,1},
  {EPEAS_CIS001_EXPOSURE_B0,0},              
  {EPEAS_CIS001_EXPOSURE_B1,3},                  
  {EPEAS_CIS001_RESOLUTION,0},
  {EPEAS_CIS001_FLIP,3},                         
  {EPEAS_CIS001_ANALOG_GAIN,6},                           
  {EPEAS_CIS001_CCLK_CAL,22},
  {EPEAS_CIS001_ABB_EN,0}
};



static inline int is_spi_active()
{
#if (defined(__PLATFORM__) && (__PLATFORM__ == ARCHI_PLATFORM_RTL)) || defined(__PLATFORM_RTL__) || defined(__PLATFORM_GVSOC__)

  return 0;

#else

  return 1;

#endif
}



static void __epeas_cis001_reg_write(epeas_cis001_t *epeas_cis001, uint8_t addr, uint8_t value)
{
  if (is_spi_active())
  {
    tx_buffer[0] = addr;
    tx_buffer[1] = value;
    pi_spi_send(&epeas_cis001->spi_device, tx_buffer, 8*2 ,PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);

  }
}



static uint8_t __epeas_cis001_reg_read(epeas_cis001_t *epeas_cis001, uint8_t addr)
{
  if (is_spi_active())
  {
    tx_buffer[0] = addr + 0x80;
    pi_spi_transfer(&epeas_cis001->spi_device, tx_buffer, rx_buffer,8*2,PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);
    return rx_buffer[1];
  }
  else
  {
    return 0;
  }
}



static void __epeas_cis001_init_regs(epeas_cis001_t *epeas_cis001)
{
  int32_t i;
  for(i=0; i<(int32_t)(sizeof(__epeas_cis001_reg_init)/sizeof(epeas_cis001_reg_init_t)); i++)
  {
    __epeas_cis001_reg_write(epeas_cis001, __epeas_cis001_reg_init[i].addr, __epeas_cis001_reg_init[i].data);
  }
}

static void __pi_epeas_cis001_stby(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_PMODE, 0x01);
}

static void __pi_epeas_cis001_sleep(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_PMODE, 0x02);
}

static void __pi_epeas_cis001_start(epeas_cis001_t *epeas_cis001)
{
  pi_gpio_pin_write(&epeas_cis001->gpio_port, epeas_cis001->conf.gpio_reset, 1); 
  pi_pwm_timer_start(&epeas_cis001->pwm_device);
}

static void __pi_epeas_cis001_stop(epeas_cis001_t *epeas_cis001)
{
  //pi_pwm_timer_stop(&epeas_cis001->pwm_device);
  __pi_epeas_cis001_sleep(epeas_cis001);
}

static void __pi_epeas_cis001_launch_capture(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, (uint8_t) EPEAS_CIS001_CAPTURE, 0x01);
}


static void __epeas_cis001_reset(epeas_cis001_t *epeas_cis001)
{
  pi_gpio_pin_write(&epeas_cis001->gpio_port, epeas_cis001->conf.gpio_reset, 0);  
  pi_time_wait_us(1000); 
  pi_gpio_pin_write(&epeas_cis001->gpio_port, epeas_cis001->conf.gpio_reset, 1);
  pi_time_wait_us(1000); 

}

static void __epeas_cis001_set_vga(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_RESOLUTION, 0x00);
}


static void __epeas_cis001_set_qvga(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_RESOLUTION, 0x01);
}


static void __epeas_cis001_set_qqvga(epeas_cis001_t *epeas_cis001)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_RESOLUTION, 0x02);
}


static void __epeas_cis001_set_mode(epeas_cis001_t *epeas_cis001, uint8_t mode)
{
  __epeas_cis001_reg_write(epeas_cis001, EPEAS_CIS001_MODE, mode);
}


int32_t __epeas_cis001_open(struct pi_device *device)
{
  struct pi_epeas_cis001_conf *conf = (struct pi_epeas_cis001_conf *)device->config;

  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)pmsis_l2_malloc(sizeof(epeas_cis001_t));
  if (epeas_cis001 == NULL) return -1;
  memcpy(&epeas_cis001->conf, conf, sizeof(*conf));

  device->data = (void *)epeas_cis001;

  if (bsp_epeas_cis001_open(conf))
    goto error1;

  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&epeas_cis001->cpi_device, &cpi_conf);
  
  if (pi_cpi_open(&epeas_cis001->cpi_device))
    goto error1;
  
  pi_cpi_set_format(&epeas_cis001->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);

  struct pi_spi_conf spi_conf;
  pi_spi_conf_init(&spi_conf);
  spi_conf.itf = conf->spi_itf; 
  spi_conf.cs = conf->spi_cs;
  spi_conf.wordsize = PI_SPI_WORDSIZE_8;
  spi_conf.big_endian = 0;
  spi_conf.max_baudrate = 1000000;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;

  pi_open_from_conf(&epeas_cis001->spi_device, &spi_conf);
  if (pi_spi_open(&epeas_cis001->spi_device))
    goto error2;

  struct pi_pwm_conf pwm_conf;
  pi_pwm_conf_init(&pwm_conf);
  pwm_conf.pwm_id = conf->pwm_id;
  pwm_conf.ch_id = conf->pwm_channel;      //gapA13
  pwm_conf.timer_conf &= ~PI_PWM_CLKSEL_REFCLK_32K;
  pwm_conf.timer_conf |= PI_PWM_CLKSEL_FLL;
  pi_open_from_conf(&epeas_cis001->pwm_device, &pwm_conf);
  
  if (pi_pwm_open(&epeas_cis001->pwm_device))
    goto error3;
  
  pi_pwm_duty_cycle_set(&epeas_cis001->pwm_device, 12000000, 50);
  pi_pwm_timer_start(&epeas_cis001->pwm_device);

  
  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);
  pi_open_from_conf(&epeas_cis001->gpio_port, &gpio_conf);
  if (pi_gpio_open(&epeas_cis001->gpio_port))
    goto error4;

  /*RSTB pin*/  
  pi_pad_set_function(PI_PAD_16_A44_RF_PACTRL4,PI_PAD_FUNC1);
  pi_gpio_pin_configure(&epeas_cis001->gpio_port, conf->gpio_reset, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(&epeas_cis001->gpio_port,conf->gpio_reset,1);


  /*Capture pin */
  pi_pad_set_function(PI_PAD_31_B11_TIMER0_CH0,PI_PAD_FUNC1);
  pi_gpio_pin_configure(&epeas_cis001->gpio_port, conf->gpio_capture, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(&epeas_cis001->gpio_port,conf->gpio_capture,0);



#ifdef __GAP9__
  // The rowlen, ur_x and ll_x need to be div by 2 when pixel size is 1 byte.
  uint8_t div = 1;
#else
  // The rowlen, ur_x and ll_x need to be div by 2 and -1 when pixel size is 1 byte.
  // The div and -1 has been integrated in gap8 cpi driver.
  uint8_t div = 0;
#endif
  uint16_t rowlen=0;

  // If it's not the predefined format
  if(epeas_cis001->conf.format>PI_CAMERA_QQVGA){
      rowlen = epeas_cis001->conf.format;
  }else if(epeas_cis001->conf.format==PI_CAMERA_QQVGA){
      rowlen = 160+2;
  }else{
      rowlen = 320+4;
  }

  pi_cpi_set_rowlen(&epeas_cis001->cpi_device, rowlen>>div);

  if (epeas_cis001->conf.roi.slice_en != 0)
    pi_cpi_set_slice(&epeas_cis001->cpi_device, epeas_cis001->conf.roi.x>>div,
            epeas_cis001->conf.roi.y,
            epeas_cis001->conf.roi.w>>div,
            epeas_cis001->conf.roi.h);

#ifdef __GAP9__
  // Reverse the vsync, since the vsync of epeas_cis001 is always 1 during one frame.
#if (defined(__PLATFORM__) && (__PLATFORM__ != ARCHI_PLATFORM_RTL)) || !defined(__PLATFORM_RTL__)
  pi_cpi_set_sync_polarity(&epeas_cis001->cpi_device, 1, 0);
#endif
#endif

  epeas_cis001->is_awake = 0;

  __epeas_cis001_reset(epeas_cis001);

  __epeas_cis001_init_regs(epeas_cis001);

  if(epeas_cis001->conf.format==PI_CAMERA_QVGA){
    __epeas_cis001_set_qvga(epeas_cis001);
  }
  else if(epeas_cis001->conf.format==PI_CAMERA_QQVGA){
    __epeas_cis001_set_qqvga(epeas_cis001);
  }

  return 0;

error4:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pi_spi_close(&epeas_cis001->spi_device);
  pi_pwm_close(&epeas_cis001->pwm_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -4;
error3:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pi_spi_close(&epeas_cis001->spi_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -3;
error2:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -2;
error1:
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -1;
}


//This is a copy of open with an additional argument, not very nice but effective
//Otherwise we would need to add an argument to open anche change all user code

int32_t __epeas_cis001_reopen(struct pi_device *device, pi_camera_opts_e opts)
{
  struct pi_epeas_cis001_conf *conf = (struct pi_epeas_cis001_conf *)device->config;

  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)pmsis_l2_malloc(sizeof(epeas_cis001_t));
  if (epeas_cis001 == NULL) return -1;
  memcpy(&epeas_cis001->conf, conf, sizeof(*conf));

  device->data = (void *)epeas_cis001;

  if (bsp_epeas_cis001_open(conf))
    goto error1;

  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&epeas_cis001->cpi_device, &cpi_conf);

  if (pi_cpi_open(&epeas_cis001->cpi_device))
    goto error1;
  
  pi_cpi_set_format(&epeas_cis001->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);


  struct pi_spi_conf spi_conf;
  pi_spi_conf_init(&spi_conf);
  spi_conf.itf = conf->spi_itf;
  spi_conf.cs = conf->spi_cs;
  spi_conf.wordsize = PI_SPI_WORDSIZE_8;
  spi_conf.big_endian = 0;
  spi_conf.max_baudrate = 1000000;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;

  pi_open_from_conf(&epeas_cis001->spi_device, &spi_conf);
  if (pi_spi_open(&epeas_cis001->spi_device))
    goto error2;
  
  struct pi_pwm_conf pwm_conf;
  pi_pwm_conf_init(&pwm_conf);
  pwm_conf.pwm_id = conf->pwm_id;
  pwm_conf.ch_id = conf->pwm_channel;      //gapA13
  pwm_conf.timer_conf &= ~PI_PWM_CLKSEL_REFCLK_32K;
  pwm_conf.timer_conf |= PI_PWM_CLKSEL_FLL;
  pi_open_from_conf(&epeas_cis001->pwm_device, &pwm_conf);
  
  if (pi_pwm_open(&epeas_cis001->pwm_device))
    goto error3;
  
  pi_pwm_duty_cycle_set(&epeas_cis001->pwm_device, 12000000, 50);
  pi_pwm_timer_start(&epeas_cis001->pwm_device);
 
  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);
  pi_open_from_conf(&epeas_cis001->gpio_port, &gpio_conf);
  if (pi_gpio_open(&epeas_cis001->gpio_port))
    goto error4;

  /*RSTB pin*/  
  pi_pad_set_function(PI_PAD_16_A44_RF_PACTRL4,PI_PAD_FUNC1);
  pi_gpio_pin_configure(&epeas_cis001->gpio_port, conf->gpio_reset, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(&epeas_cis001->gpio_port,conf->gpio_reset,1);


   /*GPIO B11 for capture pin */
  pi_pad_set_function(PI_PAD_31_B11_TIMER0_CH0,PI_PAD_FUNC1);
  pi_gpio_pin_configure(&epeas_cis001->gpio_port, conf->gpio_capture, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(&epeas_cis001->gpio_port,conf->gpio_capture,0);
  

  epeas_cis001->is_awake = 0;

  if(opts != PI_CAMERA_OPT_NO_REG_INIT){
    __epeas_cis001_reset(epeas_cis001);
    __epeas_cis001_init_regs(epeas_cis001);
  }

  if(epeas_cis001->conf.format==PI_CAMERA_QVGA){
    __epeas_cis001_set_qvga(epeas_cis001);
  }
  else if(epeas_cis001->conf.format==PI_CAMERA_QQVGA){
    __epeas_cis001_set_qqvga(epeas_cis001);
  }

  return 0;

error4:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pi_spi_close(&epeas_cis001->spi_device);
  pi_pwm_close(&epeas_cis001->pwm_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -4;
error3:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pi_spi_close(&epeas_cis001->spi_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -3;
error2:
  pi_cpi_close(&epeas_cis001->cpi_device);
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -2;
error1:
  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
  return -1;
}



static void __epeas_cis001_close(struct pi_device *device)
{
  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)device->data;

  pi_cpi_close(&epeas_cis001->cpi_device);
  pi_spi_close(&epeas_cis001->spi_device);
  pi_pwm_close(&epeas_cis001->pwm_device);

  pmsis_l2_malloc_free(epeas_cis001, sizeof(epeas_cis001_t));
}


static int32_t __epeas_cis001_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
  int irq = disable_irq();
  pi_camera_opts_e open_opt = (pi_camera_opts_e) arg;

  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)device->data;

  switch (cmd)
  {

    case PI_CAMERA_CMD_START:
      __pi_epeas_cis001_start(epeas_cis001);
      pi_cpi_control_start(&epeas_cis001->cpi_device);
      __pi_epeas_cis001_launch_capture(epeas_cis001);
      break;

    case PI_CAMERA_CMD_STOP:
      pi_cpi_control_stop(&epeas_cis001->cpi_device);
      __pi_epeas_cis001_stop(epeas_cis001);
      break;


    default:
      break;
  }

  restore_irq(irq);

  return 0;
}



void __epeas_cis001_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)device->data;

  pi_cpi_capture_async(&epeas_cis001->cpi_device, buffer, bufferlen, task);
}



int32_t __epeas_cis001_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)device->data;
  __epeas_cis001_reg_write(epeas_cis001, addr, *value);
  return 0;
}



int32_t __epeas_cis001_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  epeas_cis001_t *epeas_cis001 = (epeas_cis001_t *)device->data;
  *value = __epeas_cis001_reg_read(epeas_cis001, addr);
  return 0;
}



static pi_camera_api_t epeas_cis001_api =
{
  .open           = &__epeas_cis001_open,
  .close          = &__epeas_cis001_close,
  .control        = &__epeas_cis001_control,
  .capture_async  = &__epeas_cis001_capture_async,
  .reg_set        = &__epeas_cis001_reg_set, 
  .reg_get        = &__epeas_cis001_reg_get
};

void pi_epeas_cis001_conf_init(struct pi_epeas_cis001_conf *conf)
{
  conf->camera.api = &epeas_cis001_api;
  conf->skip_pads_config = 0;
  conf->format = PI_CAMERA_VGA; //By Default VGA
  conf->roi.slice_en = 0; //Disable the ROI by default.
  bsp_epeas_cis001_conf_init(conf);
  __camera_conf_init(&conf->camera);
}