#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/ov5640.h"
#include "ov5640_config.h"
#include "ov5640_af_config.h"

static uint8_t OV5640_EXPOSURE_TBL[7][6]=
{
    0x10,0x08,0x10,0x08,0x20,0x10,//-3
    0x20,0x18,0x41,0x20,0x18,0x10,//-2
    0x30,0x28,0x61,0x30,0x28,0x10,//-1
    0x38,0x30,0x61,0x38,0x30,0x10,//0
    0x40,0x38,0x71,0x40,0x38,0x10,//+1
    0x50,0x48,0x90,0x50,0x48,0x20,//+2
    0x60,0x58,0xa0,0x60,0x58,0x20,//+3
};

//exposure: 0 - 6,
void OV5640_Exposure(struct pi_device *device, uint8_t exposure)
{
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value);	//start group 3
    pi_camera_reg_set(device,0x3a0f,&OV5640_EXPOSURE_TBL[exposure][0]);
    pi_camera_reg_set(device,0x3a10,&OV5640_EXPOSURE_TBL[exposure][1]);
    pi_camera_reg_set(device,0x3a1b,&OV5640_EXPOSURE_TBL[exposure][2]);
    pi_camera_reg_set(device,0x3a1e,&OV5640_EXPOSURE_TBL[exposure][3]);
    pi_camera_reg_set(device,0x3a11,&OV5640_EXPOSURE_TBL[exposure][4]);
    pi_camera_reg_set(device,0x3a1f,&OV5640_EXPOSURE_TBL[exposure][5]);
    reg_value = 0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value = 0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

static uint8_t OV5640_LIGHTMODE_TBL[5][7]=
{
    0x04,0x00,0x04,0x00,0x04,0x00,0x00,//Auto
    0x06,0x1C,0x04,0x00,0x04,0xF3,0x01,//Sunny
    0x05,0x48,0x04,0x00,0x07,0xCF,0x01,//Office
    0x06,0x48,0x04,0x00,0x04,0xD3,0x01,//Cloudy
    0x04,0x10,0x04,0x00,0x08,0x40,0x01,//Home
};

// light mode:
//      0: auto
//      1: sunny
//      2: office
//      3: cloudy
//      4: home
void OV5640_Light_Mode(struct pi_device *device, uint8_t mode)
{
    uint8_t i;
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value);	//start group 3
    for(i=0;i<7;i++)pi_camera_reg_set(device,0x3400+i,&OV5640_LIGHTMODE_TBL[mode][i]);
    reg_value = 0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value = 0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

static uint8_t OV5640_SATURATION_TBL[7][6]=
{
    0x0C,0x30,0x3D,0x3E,0x3D,0x01,//-3
    0x10,0x3D,0x4D,0x4E,0x4D,0x01,//-2
    0x15,0x52,0x66,0x68,0x66,0x02,//-1
    0x1A,0x66,0x80,0x82,0x80,0x02,//+0
    0x1F,0x7A,0x9A,0x9C,0x9A,0x02,//+1
    0x24,0x8F,0xB3,0xB6,0xB3,0x03,//+2
    0x2B,0xAB,0xD6,0xDA,0xD6,0x04,//+3
};


// Color Saturation:
//   sat:  0 - 6
void OV5640_Color_Saturation(struct pi_device *device, uint8_t sat)
{
    uint8_t i;
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value);	//start group 3
    reg_value = 0x1c;
    pi_camera_reg_set(device,0x5381,&reg_value);
    reg_value = 0x5a;
    pi_camera_reg_set(device,0x5382,&reg_value);
    reg_value = 0x06;
    pi_camera_reg_set(device,0x5383,&reg_value);
    for(i=0;i<6;i++)  pi_camera_reg_set(device,0x5384+i,&OV5640_SATURATION_TBL[sat][i]);
    reg_value = 0x98;
    pi_camera_reg_set(device,0x538b,&reg_value);
    reg_value = 0x01;
    pi_camera_reg_set(device,0x538a,&reg_value);
    reg_value = 0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value = 0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

//Brightness
//     bright:  0 - 8
void OV5640_Brightness(struct pi_device *device, uint8_t bright)
{
    uint8_t brtval;
    if(bright<4)brtval=4-bright;
    else brtval=bright-4;
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value);	//start group 3
    reg_value = brtval<<4;
    pi_camera_reg_set(device,0x5587,&reg_value);
    if(bright<4) reg_value=0x09;
    else reg_value=0x01;
    pi_camera_reg_set(device,0x5588,&reg_value);
    reg_value=0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value=0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

//Contrast:
//     contrast:  0 - 6
void OV5640_Contrast(struct pi_device *device, uint8_t contrast)
{
    uint8_t reg0val=0x00;
    uint8_t reg1val=0x20;
    switch(contrast)
    {
        case 0://-3
            reg1val=reg0val=0x14;
            break;
        case 1://-2
            reg1val=reg0val=0x18;
            break;
        case 2://-1
            reg1val=reg0val=0x1C;
            break;
        case 4://1
            reg0val=0x10;
            reg1val=0x24;
            break;
        case 5://2
            reg0val=0x18;
            reg1val=0x28;
            break;
        case 6://3
            reg0val=0x1C;
            reg1val=0x2C;
            break;
    }
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value); //start group 3
    pi_camera_reg_set(device,0x5585,&reg0val);
    pi_camera_reg_set(device,0x5586,&reg1val);
    reg_value=0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value=0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

// Sharpness:
//    sharp: 0 - 33   (0: close , 33: auto , other: Sharpness)

void OV5640_Sharpness(struct pi_device *device, uint8_t sharp)
{
    uint8_t reg_value;
    if(sharp<33)
    {
        reg_value=0x65;
        pi_camera_reg_set(device,0x5308,&reg_value);
        pi_camera_reg_set(device,0x5302,&sharp);
    }else	// auto
    {
        reg_value=0x25;
        pi_camera_reg_set(device,0x5308,&reg_value);
        reg_value=0x08;
        pi_camera_reg_set(device,0x5300,&reg_value);
        reg_value=0x30;
        pi_camera_reg_set(device,0x5301,&reg_value);
        reg_value=0x10;
        pi_camera_reg_set(device,0x5302,&reg_value);
        reg_value=0x00;
        pi_camera_reg_set(device,0x5303,&reg_value);
        reg_value=0x08;
        pi_camera_reg_set(device,0x5309,&reg_value);
        reg_value=0x30;
        pi_camera_reg_set(device,0x530a,&reg_value);
        reg_value=0x04;
        pi_camera_reg_set(device,0x530b,&reg_value);
        reg_value=0x06;
        pi_camera_reg_set(device,0x530c,&reg_value);
    }
}

static uint8_t OV5640_EFFECTS_TBL[7][3]=
{
    0x06,0x40,0x10, // normal
    0x1E,0xA0,0x40,
    0x1E,0x80,0xC0,
    0x1E,0x80,0x80,
    0x1E,0x40,0xA0,
    0x40,0x40,0x10,
    0x1E,0x60,0x60,
};

void OV5640_Special_Effects(struct pi_device *device, uint8_t eft)
{
    uint8_t reg_value = 0x03;
    pi_camera_reg_set(device,0x3212,&reg_value); //start group 3
    pi_camera_reg_set(device,0x5580,&OV5640_EFFECTS_TBL[eft][0]);
    pi_camera_reg_set(device,0x5583,&OV5640_EFFECTS_TBL[eft][1]);// sat U
    pi_camera_reg_set(device,0x5584,&OV5640_EFFECTS_TBL[eft][2]);// sat V
    reg_value = 0x08;
    pi_camera_reg_set(device,0x5003,&reg_value);
    reg_value = 0x13;
    pi_camera_reg_set(device,0x3212,&reg_value); //end group 3
    reg_value = 0xa3;
    pi_camera_reg_set(device,0x3212,&reg_value); //launch group 3
}

uint8_t OV5640_Focus_Init(struct pi_device *device)
{
    uint16_t i;
    uint16_t addr=0x8000;
    uint8_t state=0x8F;
    uint8_t reg_value = 0x20;
    pi_camera_reg_set(device,0x3000, &reg_value);	//reset
    for(i=0;i<sizeof(OV5640_AF_Config);i++)
    {
        pi_camera_reg_set(device,addr,&OV5640_AF_Config[i]);
        addr++;
    }
    reg_value = 0x00;
    pi_camera_reg_set(device,0x3022,&reg_value);
    pi_camera_reg_set(device,0x3023,&reg_value);
    pi_camera_reg_set(device,0x3024,&reg_value);
    pi_camera_reg_set(device,0x3025,&reg_value);
    pi_camera_reg_set(device,0x3026,&reg_value);
    pi_camera_reg_set(device,0x3027,&reg_value);
    pi_camera_reg_set(device,0x3028,&reg_value);
    reg_value = 0x7f;
    pi_camera_reg_set(device,0x3029,&reg_value);
    reg_value = 0x00;
    pi_camera_reg_set(device,0x3000,&reg_value);
    i=0;
    do
    {
        pi_camera_reg_get(device,(0x3029), &state);
        pi_time_wait_us(5000);
        i++;
        if(i>1000)return 1;
    }while(state!=0x70);
    return 0;
}

uint8_t OV5640_Auto_Focus(struct pi_device *device)
{
    uint8_t temp=0;
    uint16_t retry=0;
    uint8_t reg_value = 0x01;
    pi_camera_reg_set(device,0x3023,&reg_value);
    reg_value = 0x08;
    pi_camera_reg_set(device,0x3022,&reg_value);
    do
    {
        pi_camera_reg_get(device,(0x3023),&temp);
        retry++;
        if(retry>1000)return 2;
        pi_time_wait_us(5000);
    } while(temp!=0x00);
    reg_value = 0x01;
    pi_camera_reg_set(device,0x3023,&reg_value);
    reg_value = 0x04;
    pi_camera_reg_set(device,0x3022,&reg_value);
    retry=0;
    do
    {
        pi_camera_reg_get(device,(0x3023), &temp);
        retry++;
        if(retry>1000)return 2;
        pi_time_wait_us(5000);
    }while(temp!=0x00);
    return 0;
}



