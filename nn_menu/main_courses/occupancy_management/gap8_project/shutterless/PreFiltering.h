/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 *
 */

//Fixed point versions
int32_t shutterless_fixed_fc(int16_t*  img_input_fp16,int16_t* img_offset_fp16, int num_bad_px, int* out_min, int* out_max);
//Floating point versions
int32_t shutterless_float(int16_t*  img_input_fp16,int16_t* img_offset_fp16, int num_bad_px, int* out_min, int* out_max);
