#include <stdio.h>
#include <stdlib.h>
#include "MFCC.h"

#define WIN_H		MFCC_BANK_CNT
#define WIN_W		350
#define H_SCALE		5
#define W_SCALE		5

typedef unsigned short ImgType;

ImgType Img_MFCC   [3*WIN_W*W_SCALE*WIN_H*H_SCALE];
ImgType Img_MFCC_D1[3*WIN_W*W_SCALE*WIN_H*H_SCALE];
ImgType Img_MFCC_D2[3*WIN_W*W_SCALE*WIN_H*H_SCALE];
ImgType MergedImg  [3*WIN_W*W_SCALE*WIN_H*H_SCALE*3];

static void writePPM(const char *filename, ImgType *img, int W, int H, int isRGB)

{
        FILE *fp;
	int Levels = (sizeof(ImgType)==1)?255:65535;
	int Bytes = sizeof(ImgType);
        //open file for output
        fp = fopen(filename, "w");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename); exit(1);
        }
        //write the header file
        //image format
        if (isRGB) {
                fprintf(fp, "P6\n");
                //image size
                fprintf(fp, "%d %d\n", W, H);
                // rgb component depth, here 255 grey levels
                fprintf(fp, "%d\n",Levels);
                // pixel data
                fwrite(img, 3*Bytes, W*H, fp);
        } else {
                fprintf(fp, "P5\n");
                //image size
                fprintf(fp, "%d %d\n", W, H);
                // rgb component depth, here 255 grey levels
                fprintf(fp, "%d\n",Levels);
                // pixel data
                fwrite(img, 1*Bytes, W*H, fp);
        }
        fclose(fp);
}

void Magnify(ImgType *Img, int W, int H, int IsRGB)

{
#define Abs(x)		(((x)<0)?-(x):(x))
#define Max(x, y)	(((x)>(y))?(x):(y))
	int i, j;
	int MaxP=1;
	int MaxDyn = (sizeof(ImgType)==1)?255:65535;
	float Coeff;

	if (IsRGB==0) {
		for (i=0; i<H; i++) {
			for (j=0; j<W; j++) {
				int P = Abs(Img[i*W+j]);
				MaxP = Max(MaxP, P);
			}
		}
		
		Coeff = MaxDyn/(float)MaxP;
printf("MaxP: %d, MaxDyn: %d, Coeff=%f\n", MaxP, MaxDyn, Coeff);
		for (i=0; i<H; i++) {
			for (j=0; j<W; j++) {
				Img[i*W+j] = (ImgType) (Img[i*W+j] * Coeff);
			}
		}
	} else {
	}
#undef Abs
#undef Max
}

void PushFrame(short int *MFCC, short int *MFCC_D1, short int *MFCC_D2, int Flush, int Precision)

{
	int i, j, k, l, m;
	static int FrameCount = 0;
	static int Win = 0;
	int W = WIN_W;
	int H = WIN_H;
	char I_Name[30];
	int Dyn = (sizeof(ImgType)==1)?8:16;
	float Shrink;
	int Offset;
	int Up_H = H_SCALE;
	int Up_W = W_SCALE;

	if (Flush) {
		if (FrameCount == 0) return;
		for (i=0; i<H; i++) {
			for (j=FrameCount; j<W; j++) {
				for (l=0; l<Up_H; l++) {
					for (m=0; m<Up_H; m++) {	
						for (k=0; k<3; k++) {
							Img_MFCC   [3*((Up_H*i+l)*W*Up_W + Up_W*j+m)+k] = 0;
							Img_MFCC_D1[3*((Up_H*i+l)*W*Up_W + Up_W*j+m)+k] = 0;
							Img_MFCC_D2[3*((Up_H*i+l)*W*Up_W + Up_W*j+m)+k] = 0;
						}
					}
				}
			}
		}
		FrameCount = W;
	} else {
		if (Precision < Dyn) Shrink = (float) (1<<(Dyn-Precision));
		else Shrink = 1.0/(float) (1<<(Precision-Dyn));
		
		for (i=0; i<H; i++) {
			for (j=0; j<Up_H; j++) {
				for (k=0; k<Up_H; k++) {
					Img_MFCC   [3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+0] = ((1<<Dyn)-1) - (int) (MFCC[i]*Shrink);
					Img_MFCC   [3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+1] = ((1<<Dyn)-1) - (int) (MFCC[i]*Shrink);
					Img_MFCC   [3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+2] = ((1<<Dyn)-1) - (int) (MFCC[i]*Shrink);
				if (MFCC_D1[i] < 0) {
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+0] = 0;
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+1] = 0;
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+2] = (int) ((-MFCC_D1[i])*Shrink);
				} else {
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+0] = (int) ((MFCC_D1[i])*Shrink);
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+1] = 0;
					Img_MFCC_D1[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+2] = 0;
				}
				if (MFCC_D2[i] < 0) {
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+0] = 0;
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+1] = 0;
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+2] = (int) ((-MFCC_D2[i])*Shrink);
				} else {
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+0] = (int) ((MFCC_D2[i])*Shrink);
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+1] = 0;
					Img_MFCC_D2[3*((Up_H*(H-1-i)+j)*W*Up_W+Up_W*FrameCount+k)+2] = 0;
				}
				}
			}
		}
		FrameCount++;
	}
	if (FrameCount == W) {
		sprintf(I_Name, "Results/Win%d.ppm",   Win);
		// Magnify(Img_MFCC, W, H, 0);
/*
		for (i=0; i<W*W_SCALE; i++) {
			Img_MFCC[i] = 0;
			Img_MFCC[Up_H*(H-1)*W*Up_W + i] = 0;
		}
		for (i=0; i<H*H_SCALE; i++) {
			Img_MFCC[i*W*Up_W] = 0;
			Img_MFCC[i*W*Up_W*Up_H] = 0;
		}
*/
		
		writePPM(I_Name, Img_MFCC, W*W_SCALE, H*H_SCALE, 1);

		sprintf(I_Name, "Results/WinD1%d.ppm", Win);
		writePPM(I_Name, Img_MFCC_D1, W*W_SCALE, H*H_SCALE, 1);

		sprintf(I_Name, "Results/WinD2%d.ppm", Win);
		writePPM(I_Name, Img_MFCC_D2, W*W_SCALE, H*H_SCALE, 1);

		Offset = 0;
		for (i=0; i<H*H_SCALE; i++) {
			for (j=0; j<W*W_SCALE; j++) {
				for (k=0; k<3; k++)
					MergedImg[3*(i*W*W_SCALE + j)+k] = Img_MFCC[3*(i*W*W_SCALE + j)+k];
			}
		}
		Offset += 3*H*H_SCALE*W*W_SCALE;
		for (i=0; i<H*H_SCALE; i++) {
			for (j=0; j<W*W_SCALE; j++) {
				for (k=0; k<3; k++)
					MergedImg[Offset + 3*(i*W*W_SCALE + j)+k] = Img_MFCC_D1[3*(i*W*W_SCALE + j)+k];
			}
		}
		Offset += 3*H*H_SCALE*W*W_SCALE;
		for (i=0; i<H*H_SCALE; i++) {
			for (j=0; j<W*W_SCALE; j++) {
				for (k=0; k<3; k++)
					MergedImg[Offset + 3*(i*W*W_SCALE + j)+k] = Img_MFCC_D2[3*(i*W*W_SCALE + j)+k];
			}
		}
		sprintf(I_Name, "Results/WinAll%d.ppm", Win);
		writePPM(I_Name, MergedImg, W*W_SCALE, 3*H*H_SCALE, 1);
		FrameCount = 0; Win++;
	}
	
}

