/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tf2gap_data/weights_bias.c"


void main(int argc, char* argv[]) {
  int i,j,k;
  FILE *of,*fin;
  float max=0.0;
  int QN_WL0=14;
  int QN_BL0=14;
  int QN_WL1=8;
  int QN_BL1=14;  
  int QN_WL2=8;
  int QN_BL2=14;
  
  //if (argc==1) {printf("usage: read_file filename QN: exit\n");exit(0);}
  //fin = fopen(argv[1],"r");
  of = fopen("coeff_hwce.h","w");
  //QN = atoi(argv[1]);
  fprintf(of,"RT_L2_DATA short int  L2_W_0[]={\n");
  for (i=0;i<32;i++) {
    //printf("%d %f\n",QN,L2_W_0[i]);

    // 4 first 5x5 kernels
    for(j=0;j<20;j++) {
      for (k=0;k<5;k++) fprintf(of,"%d, ",(int)floor(L2_W_0[i*8*20 + j*8 + k]*(float)((1<<QN_WL0)-1) + 0.5));
      // align on words
      if (!((j+1)%5)) fprintf(of,"0,\n");
      else fprintf(of,"\n");
    }
    // 4 (3+2)*5
    for(j=0;j<20;j++) {
      for (k=0;k<3;k++) fprintf(of,"%d, ",(int)floor(L2_W_0[i*8*20 + j*8 + k+5]*(float)((1<<QN_WL0)-1) + 0.5));
      // align on words
      if (!((j+1)%5)) fprintf(of,"0,");
      fprintf(of,"0, 0,\n");
    }

  }
  fprintf(of,"};\n");
  
  fprintf(of,"RT_L2_DATA short int  L2_B_0[]={\n");
  for (i=0;i<32;i++) {
    if (fabs(L2_B_0[i])>max) max = fabs(L2_B_0[i]);
    fprintf(of,"%d, ",(int)floor(L2_B_0[i]*(float)((1<<QN_BL0)-1) + 0.5));
    if (!((i+1)%30)) fprintf(of,"\n");

  }
  fprintf(of,"};\n");
  printf("max L0= %f %x\n",max,(int) (max*(1<<QN_WL0)));

  max=0;
  fprintf(of,"RT_L2_DATA short int  L2_B_1[]={\n");
  for (i=0;i<32;i++) {
    if (fabs(L2_B_1[i])>max) max = fabs(L2_B_1[i]);
    fprintf(of,"%d, ",(int)floor(L2_B_1  [i]*(float)((1<<QN_BL1)-1) + 0.5));
    if (!((i+1)%30)) fprintf(of,"\n");

  }
  fprintf(of,"};\n");
  printf("max L1_B1= %f %x\n",max,(int) (max*(1<<QN_BL1)));

  max=0;
  fprintf(of,"RT_L2_DATA short int  L2_B_2[]={\n");
  for (i=0;i<12;i++) {
    if (fabs(L2_B_2[i])>max) max = fabs(L2_B_2[i]);
    fprintf(of,"%d, ",(int)floor(L2_B_2[i]*(float)((1<<QN_BL2)-1) + 0.5));
    if (!((i+1)%30)) fprintf(of,"\n");

  }
  fprintf(of,"};\n");
  printf("max L2_B2= %f %x\n",max,(int) (max*(1<<QN_BL2)));




  // L2_W_2 2*4bits
  // L2_W_1 8 bits
  fprintf(of,"RT_L2_DATA Word8  L2_W_2[]={\n");
  k=0;
  for (i=0;i<12*32*30*13;i+=2) {
    short int datint;
    short int dat=0;

    
    dat =  (((short int) (int)floor(L2_W_2[i]*(float)((1<<QN_WL2)-1) + 0.5)) ) ;
    //printf("%d %x ",i,dat);
    if (dat>(short int)0x7) dat=0x7;
    if (dat<(short int)0xfff8) dat=0x8;
    //printf("-- %x ",dat);
    dat = dat & 0xf;

    k++;
    if (k==7) {
      datint = 0;
      k=0;
      i=i-1;
    }
    else
      datint = (((short int) (int)floor(L2_W_2[i+1]*(float)((1<<QN_WL2)-1) + 0.5)) );
    
    //printf("%d %x ",i,datint);
    if (datint>(short int)0x7) datint=0x7;
    if (datint<(short int)0xfff8) datint=0x8;
    datint = datint << 4;
    //printf("-- %x",datint);
    
    dat |= datint;
    //printf("-- %x\n",dat);
    fprintf(of,"%d, ",dat);
    if (!((i+2)%30)) fprintf(of,"\n");

  }
  fprintf(of,"};\n");

  // coeff as 2 (4+1)*5 kernels
  max=0;
  fprintf(of,"RT_L2_DATA short int  L2_W_1[]={\n");
   for (i=0;i<32*32;i++) {
    for(j=0;j<10;j++) {
      for (k=0;k<4;k++) fprintf(of,"%d, ",(int)floor(L2_W_1[i*4*10 + j*4 + k]*(float)((1<<QN_WL1)-1) + 0.5));
      fprintf(of,"0,\n");
      // align on words
      if (!((j+1)%5)) fprintf(of,"0,\n");    }    

  }
  fprintf(of,"};\n");
    

  
}
