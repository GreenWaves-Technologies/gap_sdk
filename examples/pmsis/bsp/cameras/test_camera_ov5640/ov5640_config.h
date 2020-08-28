#ifndef __OV5640_CONFIGS_H__
#define __OV5640_CONFIGS_H__

#define OV5640_CHIPID_HIGH 0x300a
#define OV5640_CHIPID_LOW 0x300b

typedef struct ov5640_reg{
    uint16_t addr;
    uint8_t value;
} ov5640_reg_t;

void OV5640_Exposure(struct pi_device *device, uint8_t exposure);
void OV5640_Light_Mode(struct pi_device *device, uint8_t mode);
void OV5640_Color_Saturation(struct pi_device *device, uint8_t sat);
void OV5640_Brightness(struct pi_device *device, uint8_t bright);
void OV5640_Contrast(struct pi_device *device, uint8_t contrast);
void OV5640_Sharpness(struct pi_device *device, uint8_t sharp);
void OV5640_Special_Effects(struct pi_device *device, uint8_t eft);
uint8_t OV5640_Focus_Init(struct pi_device *device);
uint8_t OV5640_Auto_Focus(struct pi_device *device);

#endif
