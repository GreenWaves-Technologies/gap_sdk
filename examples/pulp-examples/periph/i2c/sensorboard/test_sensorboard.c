/*
 * This test check read/write values in registers of different device
 * 1)read ID chip of the following sensor then read/write/read a configuration register
 *   BMP280
 *   HTS221
 *   LISMDL
 *   LSM6DSL
 * 2) TMP007 read only chip ID as device registers only 8 times writeable =>
 * 3) VEML6030 read/write/read a configuration register (No default register value in datasheet)
 * 4) VL53L0X read reference registers as only information available in the datasheet
 *
 * In order to check fails, in the Makefile FORCE_TEST_FAIL could be define by a line uncomment.
*/

#include "rt/rt_api.h"
#include "stdio.h"

#define SIZE_BUF                        (16)
#define FAIL                            (-1)
#define SUCCESS                          (0)

//BMP280
#ifdef FORCE_TEST_FAIL
#define BMP280_ADDRESS                (0xAA)
#else
#define BMP280_ADDRESS                (0xEC) //(0x76<<1) SDO pin at ground
#endif
#define BMP280_CHIPID                 (0x58)
#define BMP280_CTRL_CFG               (0x55)
#define BMP280_CTRL_CFG2              (0x54)

#define BMP280_VERSION_REG            (0xD0) //contain BMP280_CHIPID
#define BMP280_CONFIG_REG             (0xF5)
#define BMP280_CTRL_MEAS              (0xF4)

//HTS221 section
#ifdef FORCE_TEST_FAIL
#define HTS221_ADDRESS                 (0xAA)
#else
#define HTS221_ADDRESS                 (0x5F << 1) //0xBE
#endif
#define HTS221_CHIPID                  (0xBC)
#define HTS221_VAL_CTRL_REG1           (0x87)
#define HTS221_VAL2_CTRL_REG1          (0x86)

#define HTS221_WHO_AM_I_REG            (0x0F)
#define HTS221_CTRL_REG1               (0x20)

//LISMDL
#ifdef FORCE_TEST_FAIL
#define LIS3MDL_ADDRESS                (0xAA)
#else
#define LIS3MDL_ADDRESS                (0x38) //pin SA1 at ground
#endif
#define LIS3MDL_CHIPID                 (0x3D)
#define LIS3MDL_VAL_CTRL_REG1          (0x87)
#define LIS3MDL_VAL2_CTRL_REG1         (0x86)

#define LIS3MDL_WHO_AM_I_REG           (0x0F)
#define LIS3MDL_CTRL_REG1              (0x20)

//LSM6DSL
#ifdef FORCE_TEST_FAIL
#define LSM6DSL_ADDRESS                 (0xAA)
#else
#define LSM6DSL_ADDRESS                 (0xD4)
#endif
#define LSM6DSL_CHIPID                  (0x6A)
#define LSM6DSL_VAL_CTRL1_REG           (0xFE)
#define LSM6DSL_VAL2_CTRL1_REG          (0xFC)

#define LSM6DSL_WHO_AM_I_REG            (0x0F)
#define LSM6DSL_CTRL1_XL_REG            (0x10)

//VEML6030
#ifdef FORCE_TEST_FAIL
#define VEML6030_ADDRESS                (0xAA)
#else
#define VEML6030_ADDRESS                (0x20) //addr connected to ground
#endif
#define VEML6030_CONFIGURATION_VAL      (0xB30)
#define VEML6030_ALS_WH_LW              (0x0D)
#define VEML6030_ALS_WH_LW2             (0x09)
#define VEML6030_ALS_WH_HI              (0x03)

#define VEML6030_CONFIGURATION_REG        (0)
#define VEML6030_ALS_REG                  (4)//contains ambient light on 16bits
#define VEML6030_ALS_WH_REG               (1)//ALS high threshold window setting on 16bits

//VL53L0X Time-of-Flight ranging and gesture detection sensor
#ifdef FORCE_TEST_FAIL
#define VL53L0X_ADDRESS                   (0xAA)
#else
#define VL53L0X_ADDRESS                   (0x52)
#endif
#define VL53L0X_REF_DEFAULT_VAL_REG1      (0xEE)
#define VL53L0X_REF_DEFAULT_VAL_REG2      (0xAA)
#define VL53L0X_REF_DEFAULT_VAL_REG3      (0x10)
#define VL53L0X_REF_DEFAULT_VAL_REG4      (0x0099)
#define VL53L0X_REF_DEFAULT_VAL_REG5      (0x0000)

#define VL53L0X_REF_REG1                  (0xC0)
#define VL53L0X_REF_REG2                  (0xC1)
#define VL53L0X_REF_REG3                  (0xC2)
#define VL53L0X_REF_REG4                  (0x51)
#define VL53L0X_REF_REG5                  (0x61)

void iniBuf(unsigned char *buf, unsigned int size)
{
  for(unsigned int i=0; i<size; i++)
    buf[i] = 0xaa;
}

RT_L2_DATA unsigned char valueReg = 0;
RT_L2_DATA rt_i2c_conf_t conf;
int testBMP280, testHTS221, testLIS3MDL, testLSM6DSL, testTMP007, testVEML6030, testVL53L0X;
RT_L2_DATA unsigned char dataWr[16];
RT_L2_DATA unsigned char dataRd[16];
RT_L2_DATA rt_i2c_t *i2c;

RT_L2_DATA unsigned char addr[1];


int test_sensor(rt_i2c_t * i2c, char *name, unsigned char cs, unsigned char idReg, unsigned char idVal, unsigned char ctrlReg, unsigned char ctrlVal1, unsigned char ctrlVal2)
{
  printf("*** Test %s ***\n", name);
  i2c->cs = cs;
  //end of I2C configuration

  //Read ID register
  iniBuf(dataRd, SIZE_BUF);
  addr[0] = idReg;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd, 1, 0, NULL);

  //read CTRL REG
  addr[0] = ctrlReg;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd+1, 1, 0, NULL);
  if (ctrlVal1 == dataRd[1])
    dataWr[1] = ctrlVal2;
  else
    dataWr[1] = ctrlVal1;

  //write CTRL REG
  dataWr[0] = ctrlReg;
  rt_i2c_write(i2c, dataWr, 2, 0, NULL);

  //read CTRL_REG to check write
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd+2, 1, 0, NULL);

  if ((idVal == dataRd[0]) &&(dataWr[1] == dataRd[2]))
  {
    printf("%s chip Id and Read/Write in control register ok\n\tchipID=0x%x expected 0x%x\n\tWritten in register 0x%x read 0x%x\n", name, dataRd[0], idVal, dataWr[1], dataRd[2]);
    return SUCCESS;
  }
  else
  {
    printf("%s test failed:\n\tchipID=0x%x expected 0x%x\n\tWritten in register 0x%x read 0x%x\n", name,dataRd[0], idVal, dataWr[1], dataRd[2]);
    return FAIL;
  }
}

int test_veml6030(rt_i2c_t * i2c)
{
  printf("*** Test VEML6030 ***\n");
  i2c->cs = VEML6030_ADDRESS;
  //end of I2C configuration VEML6030

  //Read ALS_WH_REG
  iniBuf(dataRd, SIZE_BUF);
  addr[0] = VEML6030_ALS_WH_REG;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd, 2, 0, NULL);
  printf("read1 0x%x 0x%x\n", dataRd[0], dataRd[1]);

  unsigned int rd = ((dataRd[0]<<8) | dataRd[1]); //receive MSB first so swap byte
  unsigned int expected;

  if (VEML6030_ALS_WH_LW == dataRd[1])
  {
    dataWr[2] = VEML6030_ALS_WH_LW2;
    expected = (VEML6030_ALS_WH_HI<<8) | VEML6030_ALS_WH_LW2;
  }
  else
  {
    dataWr[2] = VEML6030_ALS_WH_LW;
    expected = (VEML6030_ALS_WH_HI<<8) | VEML6030_ALS_WH_LW;
  }

  //write VEML6030_CTRL_REG1
  dataWr[0] = VEML6030_ALS_WH_REG;
  dataWr[1] = VEML6030_ALS_WH_HI;
  rt_i2c_write(i2c, dataWr, 3, 0, NULL);

  //read VEML6030_CTRL_REG1 to check write
  addr[0] = VEML6030_ALS_WH_REG;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd+2, 2, 0, NULL);
  printf("read 0x%x 0x%x\n", dataRd[2], dataRd[3]);
  unsigned int rd2 = ((dataRd[2]<<8) | dataRd[3]);

  if (expected == rd2)
  {
    printf("VEML6030 test ok:\n\tWritten in register 0x%x read 0x%x\n",  expected, rd2);
    return SUCCESS;
  }
  else
  {
    printf("VEML6030 test failed:\n\tWritten in register 0x%x read 0x%x\n",  VEML6030_ALS_WH_HI, rd2);
    return FAIL;
  }
}

int test_VL53L0X(rt_i2c_t * i2c)
{
  rt_event_t *call_event;

  printf("*** Test VL53L0X ***\n");
  i2c->cs = VL53L0X_ADDRESS;
  //end of I2C configuration VL53L0X

  //Read REG1 to REG3 register
  printf("Reading REG1 to REG3\n") ;
  iniBuf(dataRd, SIZE_BUF);
  addr[0] = VL53L0X_REF_REG1;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd, 3, 0, NULL);

  //Read REG4
  printf("Reading REG4\n") ;
  addr[0] = VL53L0X_REF_REG4;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd+3, 2, 0, NULL);
  unsigned int reg4 = ((dataRd[3]<<8) | dataRd[4]);

  //Read REG5
  printf("Reading REG5\n") ;
  addr[0] = VL53L0X_REF_REG5;
  rt_i2c_write(i2c, addr, 1, 1, NULL);
  rt_i2c_read(i2c, dataRd+5, 2, 0, NULL);
  unsigned int reg5 = ((dataRd[5]<<8) | dataRd[6]);

  if ((VL53L0X_REF_DEFAULT_VAL_REG1 == dataRd[0]) &&
      (VL53L0X_REF_DEFAULT_VAL_REG2 == dataRd[1]) &&
      (VL53L0X_REF_DEFAULT_VAL_REG3 == dataRd[2]) &&
      (VL53L0X_REF_DEFAULT_VAL_REG4 == reg4) &&
      (VL53L0X_REF_DEFAULT_VAL_REG5 == reg5))
  {
    printf("VL53L0X reference register read ok\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t 0x%x vs 0x%x\n", dataRd[0], VL53L0X_REF_DEFAULT_VAL_REG1, dataRd[1], VL53L0X_REF_DEFAULT_VAL_REG2, dataRd[2], VL53L0X_REF_DEFAULT_VAL_REG3, reg4, VL53L0X_REF_DEFAULT_VAL_REG4, reg5, VL53L0X_REF_DEFAULT_VAL_REG5);
    return SUCCESS;
  }
  else
  {
    printf("VL53L0X reference register read FAILED\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t0x%x vs 0x%x\n\t 0x%x vs 0x%x\n", dataRd[0], VL53L0X_REF_DEFAULT_VAL_REG1, dataRd[1], VL53L0X_REF_DEFAULT_VAL_REG2, dataRd[2], VL53L0X_REF_DEFAULT_VAL_REG3, reg4, VL53L0X_REF_DEFAULT_VAL_REG4, reg5, VL53L0X_REF_DEFAULT_VAL_REG5);
    return FAIL;
  }
}

int main()
{
  unsigned char i2c_ch = 6; //i2c1

  if (rt_event_alloc(NULL, 8)) return -1;

  rt_i2c_conf_init(&conf);
  conf.id = 0;
  conf.max_baudrate = 200000;
  i2c = rt_i2c_open(NULL, &conf, NULL);
  if (i2c == NULL)
  {
    printf ("Failed to open I2C\n");
    return FAIL;
  }

  testBMP280 = test_sensor(i2c, "BMP280", BMP280_ADDRESS, BMP280_VERSION_REG, BMP280_CHIPID, BMP280_CONFIG_REG, BMP280_CTRL_CFG, BMP280_CTRL_CFG2);
  testHTS221 = test_sensor(i2c, "HTS221", HTS221_ADDRESS, HTS221_WHO_AM_I_REG, HTS221_CHIPID, HTS221_CTRL_REG1, HTS221_VAL_CTRL_REG1, HTS221_VAL2_CTRL_REG1);
  testLIS3MDL = test_sensor(i2c, "LIS3MDL", LIS3MDL_ADDRESS, LIS3MDL_WHO_AM_I_REG, LIS3MDL_CHIPID, LIS3MDL_CTRL_REG1, LIS3MDL_VAL_CTRL_REG1, LIS3MDL_VAL2_CTRL_REG1);
  testLSM6DSL = test_sensor(i2c, "LSM6DSL", LSM6DSL_ADDRESS, LSM6DSL_WHO_AM_I_REG, LSM6DSL_CHIPID, LSM6DSL_CTRL1_XL_REG, LSM6DSL_VAL_CTRL1_REG, LSM6DSL_VAL2_CTRL1_REG);
  testVL53L0X = test_VL53L0X(i2c);
  testVEML6030 = test_veml6030(i2c);
  if (!testBMP280 && !testHTS221 && !testLIS3MDL &&
      !testLSM6DSL && !testTMP007 && !testVEML6030 && !testVL53L0X)
  {
    printf("///\nTest Successful, all sensors I2C link ok\n///\n");
    return SUCCESS;
  }
  else
  {
    printf("///\nTest failed, at least one sensors I2C link broken\n///\n");
    return FAIL;
  }
}
