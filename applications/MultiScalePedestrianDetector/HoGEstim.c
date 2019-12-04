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
#include "HoGEstimKernels.h"
#include "HoGEstimManage.h"
#include "ImgIO.h"
#include "Gap8.h"
#include "padframe.h"
#include "archi_gap8Gpio.h"
#include "pad.h"
#include "padframe.h"
#include "ili9341.h"
#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#define FROM_FILE 1
#define VERBOSE 0
#define SAVE_IMAGE 1

#define LCD_OFF_X 40
#define LCD_OFF_Y 10

#if HOG_ESTIM_TREE_DEPTH==1
#ifdef HOG_ESTIM_TD1
#include "ModelTreeTer1_1.def"
// #include "ModelTreeTer1_Ord.def"
// #include "ModelTree1_1.def"
// #include "ModelTreeBis1_1.def"
#else
#include "ModelTree1.def"
#endif
#elif HOG_ESTIM_TREE_DEPTH==2
#include "ModelTree2.def"
#else
#error HOG_ESTIM_TREE_DEPTH value not supported
#endif

L2_MEM unsigned short *image16;
L2_MEM unsigned char *ImageIn;
L2_MEM unsigned char *ImageIn1;
L2_MEM unsigned short *HoGFeatures;
L2_MEM unsigned char *EstimOut;
L1_CL_MEM unsigned char *CellLinesBuffer;
L1_CL_MEM unsigned char *HoGFeatColsBuffer;
L1_CL_MEM HoGWeakPredictorBis_T *WeakEstimModel;
L2_MEM unsigned int TotalCycles;

L2_MEM unsigned int TotalHOGCycles;
L2_MEM unsigned int TotalResizeCycles;
L2_MEM unsigned int TotalEstimCycles;

RT_FC_TINY_DATA rt_camera_t *camera1;
RT_FC_TINY_DATA rt_cam_conf_t cam1_conf;
RT_FC_TINY_DATA rt_spim_conf_t conf;


typedef struct ArgCluster {
	unsigned int W;
	unsigned int H;
	unsigned int W_real;
	unsigned int H_real;
} ArgCluster_T;

typedef struct CropArgCluster {
	unsigned char *ImageIn;
	unsigned char *ImageOut;
	unsigned int W;
	unsigned int H;
	unsigned int crop_w;
	unsigned int crop_h;

} CropArgCluster_T;

static void cam_param_conf(rt_cam_conf_t *_conf){
  _conf->resolution = QVGA;
  _conf->format = HIMAX_MONO_COLOR;
  _conf->fps = fps30;
  _conf->slice_en = DISABLE;
  _conf->shift = 0;
  _conf->frameDrop_en = DISABLE;
  _conf->frameDrop_value = 0;
  _conf->cpiCfg = UDMA_CHANNEL_CFG_SIZE_16;
  _conf->control_id = 1;
}

static int CoreCountDynamic = 1;
static int ActiveCore = 8;//rt_nb_pe();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = rt_nb_pe();
	Log2Core = gap8_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

rt_spim_t *init_spi_lcd(){
    rt_spim_conf_init(&conf);
    conf.wordsize = RT_SPIM_WORDSIZE_8;
    conf.big_endian = 1;
    conf.max_baudrate = 50000000;
    conf.polarity = 0;
    conf.phase = 0;
    conf.id = 1;
    conf.cs = 0;

    rt_spim_t *spim = rt_spim_open(NULL, &conf, NULL);
    if (spim == NULL) return NULL;

    return spim;
}

void DumpHOGFeatures(unsigned short int *Feat, unsigned int BlockW, unsigned int BlockH, unsigned BlockSize)

{
	unsigned int i, j, k;

	printf("unsigned short HOGFeatures[%d*%d*%d] = {\n", BlockW,BlockH,BlockSize);
	for (i=0;i<BlockH; i++) {
		for (j=0;j<BlockW; j++) {
			printf("/* L%d, B%d */ ", i, j);
			for (k=0; k<BlockSize; k++) {
				printf("%d,", Feat[i*BlockW*BlockSize + j*BlockSize + k]);
			}
			printf("\n");
		}
	}
	printf("};\n");
}

unsigned char *CreateNentryBuffer(unsigned int N, unsigned EntrySize)

{
	unsigned char *Buffer = (unsigned char *) rt_alloc(RT_ALLOC_CL_DATA, N*sizeof(unsigned short **) + N*EntrySize);
	unsigned int i;

	if (Buffer==0) return 0;
	for (i=0; i<N; i++)
		((unsigned short **) Buffer)[i] = (unsigned short *) (Buffer + N*sizeof(unsigned short **) + i*EntrySize);
	return Buffer;
}

void ReportHoGConfiguration()

{
	int EstiW=  1+(((HOG_ESTIM_WIDTH)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));
	int EstiH = 1+(((HOG_ESTIM_HEIGHT)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));

	printf("========================= Configuration ============================================\n");
	printf("HoG Cell:              [%3d x %3d      ] Pixels\n", HOG_CELL_SIZE, HOG_CELL_SIZE);
	printf("HoG Block:             [%3d x %3d      ] Cells\n", HOG_BLOCK_SIZE, HOG_BLOCK_SIZE);
	printf("HoG Block Overlap:     [%3d            ] Cells\n", HOG_BLOCK_OVERLAP);
	printf("HoG Angular Resolution: %d degree, Number of bins: %d\n", HOG_ANGLE_STEP, HOG_NBINS);
	printf("Estimation Window:     [%4d x %3d      ] In Features: [%3d x %3d]\n", HOG_ESTIM_WIDTH, HOG_ESTIM_HEIGHT, EstiW, EstiH);
	printf("Model, Weak predictors [%4d x %3d      ], %6d bytes, TreeDepth: %d\n",
		sizeof(Model)/sizeof(HoGWeakPredictorBis_T), sizeof(HoGWeakPredictorBis_T), sizeof(Model), HOG_ESTIM_TREE_DEPTH);
	printf("====================================================================================\n\n");
}

void ReportHoGLevelConfiguration(unsigned int Level, unsigned int W, unsigned H)

{
       	unsigned int BlockW = (((((W-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);   // Number of blocks in a line
       	unsigned int BlockH = (((((H-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);   // Number of blocks in a row
	int EstiW=  1+(((HOG_ESTIM_WIDTH)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));
	int EstiH = 1+(((HOG_ESTIM_HEIGHT)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));

	printf("\n===== Level %2d/8 ========== Configuration ============================================\n", Level);
	printf("Input Image:           [%4d x %3d      ], %6d bytes\n", W, H, W*H);
	printf("Hog Features:          [%4d x %3d x %3d], %6d bytes\n", BlockW, BlockH,
		HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS,
		BlockW*BlockH*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS*sizeof(unsigned short));

	// printf("Estimation Out:        [%4d x %3d      ], %6d bytes\n", 1 + (BlockW - EstiW), 1 + (BlockH - EstiH), (1 + (BlockW - EstiW))*(1 + (BlockH - EstiH)));
	printf("\n");
}

void ReportPositive(unsigned char *Out, unsigned int West, unsigned int Hest, unsigned int Wref, unsigned int W, unsigned int Href, unsigned int H)

{
        unsigned int Ratio = (Wref<<12)/W;
        unsigned int FeatPix = (HOG_BLOCK_SIZE-HOG_BLOCK_OVERLAP)*HOG_CELL_SIZE;

        printf("Reference Image [%d x %d], Resized Image [%d x %d]\n", Wref, Href, W, H);
        for (unsigned int y=0; y<Hest; y++) {
                for (unsigned int x=0; x<West; x++) {
                        if (Out[West*y+x]) {
                                printf("\tMatch at [x:%3d=>Pix:%3d=>PixRef:%3d, y:%3d=>Pix:%3d=>PixRef:%3d], Weight: %d\n",
                                       x, x*FeatPix, gap8_muluRN(x*FeatPix, Ratio, 12), y, y*FeatPix, gap8_muluRN(y*FeatPix, Ratio, 12), Out[West*y+x]);
                                printf("\t\tIn ref Image: Origin: [X:%3d, Y:%3d], Window: [%3d x %3d]\n",
                                       gap8_muluRN(x*FeatPix, Ratio, 12), gap8_muluRN(y*FeatPix, Ratio, 12),
                                       gap8_muluRN(HOG_ESTIM_WIDTH, Ratio, 12), gap8_muluRN(HOG_ESTIM_HEIGHT, Ratio, 12));
                        }
                }
        }
        for (unsigned int y=0; y<Hest; y++) {
                for (unsigned int x=0; x<West; x++) {
                        if (Out[West*y+x]) {
                                printf("\t{%3d, %3d, %3d, %3d, %3d},\n",
                                       gap8_muluRN(x*FeatPix, Ratio, 12), gap8_muluRN(y*FeatPix, Ratio, 12),
                                       gap8_muluRN(HOG_ESTIM_WIDTH, Ratio, 12), gap8_muluRN(HOG_ESTIM_HEIGHT, Ratio, 12), Out[West*y+x]);
                        }
                }
        }

}

int CountPositive(unsigned char *Out, unsigned int West, unsigned int Hest, unsigned int Wref, unsigned int W, unsigned int Href, unsigned int H)

{
	int Cnt=0;

	for (unsigned int i=0; i<(West*Hest); i++) Cnt = Cnt + (Out[i]>0);
	if (Cnt) ReportPositive(Out, West, Hest, Wref, W, Href, H);
	return Cnt;
}

int ProcessOneLevel(int Level, unsigned char *ImageInRef, unsigned char *ImageIn, unsigned int Wref, unsigned int Href, unsigned int W, unsigned int H,
		    void (*MyHOG)(uint8_t*, uint16_t**, uint16_t*, uint32_t, uint32_t, Kernel_Exec_T *),
            void (*MyResize)(uint8_t*, uint8_t*, Kernel_Exec_T*),
            void (*MyWeakHOGEstim)(uint16_t *, uint16_t **, uint32_t , HoGWeakPredictorBis_T*, uint32_t, Kernel_Exec_T * ))

{
    unsigned int BlockW = (((((W-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/(HOG_BLOCK_CELL_STRIDE)) + 1);   // Number of blocks in a line
    unsigned int BlockH = (((((H-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/(HOG_BLOCK_CELL_STRIDE)) + 1);   // Number of blocks in a row
	unsigned int EstiW = 1+(((HOG_ESTIM_WIDTH)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));
	unsigned int EstiH = 1+(((HOG_ESTIM_HEIGHT)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));
	unsigned int Ti;
	#if VERBOSE
	ReportHoGLevelConfiguration(Level, W, H);
	#endif
	SetBBAspectRatio(Wref, W);
	if (MyResize) {
		Ti = gap8_readhwtimer();
		//MyResize(ImageInRef, ImageIn, ((Wref-1)<<16)/W, ((Href-1)<<16)/H, ConfResize);
		MyResize(ImageInRef, ImageIn, NULL);
		Ti = gap8_readhwtimer() - Ti;
		#if VERBOSE
		printf("Resize [%3d x %3d] to [%3d x %3d]:                           %10d cycles. %3d cycles per pixel\n", Wref, Href, W, H, Ti, Ti/(W*H));
		#endif
		TotalCycles += Ti;
		TotalResizeCycles += Ti;
	} else ImageIn = ImageInRef;

	CellLinesBuffer = CreateNentryBuffer(HOG_BLOCK_SIZE, ((W-2)/HOG_CELL_SIZE)*HOG_NBINS*sizeof(unsigned short));
	if (CellLinesBuffer == 0) {
		printf("Failed to allocated cell lines buffe: %d Bytes\n", ((W-2)/HOG_CELL_SIZE)*HOG_NBINS*sizeof(unsigned short));
		return 1;
	}

	Ti = gap8_readhwtimer();
	MyHOG(ImageIn, (unsigned short **) CellLinesBuffer, HoGFeatures,
              W*HOG_BLOCK_SIZE*HOG_CELL_SIZE, BlockW*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS, 0);
	Ti = gap8_readhwtimer() - Ti;
	#if VERBOSE
	printf("Hog Features extraction [W:%d, H:%d]:                      %10d cycles. %3d cycles per pixel\n", W, H, Ti, Ti/(W*H));
	#endif
	rt_free(RT_ALLOC_CL_DATA,(void*)(unsigned int) CellLinesBuffer, HOG_BLOCK_SIZE*sizeof(unsigned short **) + (HOG_BLOCK_SIZE*(((W-2)/HOG_CELL_SIZE)*HOG_NBINS*sizeof(unsigned short))));
	TotalCycles += Ti;
	TotalHOGCycles += Ti;
/*
	if (Level==0) {
		DumpHOGFeatures(HoGFeatures, BlockW, BlockH, HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS);
	}
*/
	HoGFeatColsBuffer = CreateNentryBuffer(EstiW, BlockH*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS*sizeof(unsigned short));
	if (HoGFeatColsBuffer == 0) {
		printf("Failed to allocated HOG Features columns buffer: %d Bytes\n",
			EstiW*(BlockH*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS*sizeof(unsigned short) + sizeof(unsigned short *)));
		return 1;
	}
	Ti = gap8_readhwtimer();
	MyWeakHOGEstim(HoGFeatures, (uint16_t ** restrict) HoGFeatColsBuffer, BlockH,
		       WeakEstimModel, sizeof(Model)/sizeof(HoGWeakPredictorBis_T), 0);
	Ti = gap8_readhwtimer() - Ti;
	#if VERBOSE
	printf("Estimation on %3d [%3d x %3d x %3d] windows of HOG features: %10d cycles. %3d cycles per weak predictor.\n",
		(1+BlockW-EstiW)*(1+BlockH-EstiH), EstiW, EstiH, HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS, Ti,
		Ti/((1+BlockW-EstiW)*(1+BlockH-EstiH)*(sizeof(Model)/sizeof(HoGWeakPredictorBis_T))));
	#endif
	rt_free(RT_ALLOC_CL_DATA, (void*) (unsigned int) HoGFeatColsBuffer, EstiW*sizeof(unsigned short **) + (EstiW*(BlockH*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS*sizeof(unsigned short))));
	TotalCycles += Ti;
	TotalEstimCycles += Ti;

	return 0;
}

void DumpImage(unsigned char *Img, unsigned int W, unsigned int H)

{
	int i, j, Cnt=0;

	printf("P5\n");
	printf("%d %d\n", W, H);
	printf("255\n");
	printf("unsigned char Img[%d*%d] = {\n", W, H);
	for (unsigned i=0; i<H; i++) {
		for (unsigned j=0; j<W; j++) {
			printf("%d,", Img[W*i+j]);
			if (Cnt==15) {
				Cnt=0; printf("\n");
			} else Cnt++;
		}
	}
	printf("\n};\n");
}



void crop_cluster(CropArgCluster_T *ArgC){

	unsigned char* in  = ArgC->ImageIn;
	unsigned char* out = ArgC->ImageOut;
  	unsigned int width = ArgC->W;
  	unsigned int height = ArgC->H;
	unsigned int crop_h = ArgC->crop_h;
	unsigned int crop_w = ArgC->crop_w;
	unsigned int crop_h_half = (ArgC->crop_h)/2;
	unsigned int crop_w_half = (ArgC->crop_w)/2;

  	unsigned int CoreId = rt_core_id();
  	unsigned int ChunkCell = ChunkSize(width);
  	unsigned int First = CoreId*ChunkCell, Last  = Min(width, First+ChunkCell);

  	for(unsigned int i=0;i<height;i++){
		for(unsigned int j=First;j<Last;j++){
  			out[i*width+j] = in[(i+crop_h_half)*(width+crop_w)+j+crop_w_half];
  		}
  		rt_team_barrier(); //we need to be sure a line is finished before starting the new one.
  	}

}

L2_MEM CropArgCluster_T arg;

void crop_image(unsigned char* in,unsigned char* out,unsigned int W,unsigned int H,unsigned int crop_w,int crop_h){

	if(crop_h%2!=0 || crop_w%2!=0){
		printf("Copped pixels must be multiple of 2!\n");
		return;
	}

	arg.ImageIn=in;
	arg.ImageOut=out;
	arg.H=H;
	arg.W=W;
	arg.crop_h=crop_h;
	arg.crop_w=crop_w;

	rt_team_fork(__builtin_pulp_CoreCount(), (void *) crop_cluster, (void *) &arg);

}


void gray8_to_RGB565_cluster_half(ArgCluster_T *ArgC){

  	unsigned int width = ArgC->W;
  	unsigned int height = ArgC->H;

  	unsigned int CoreId = rt_core_id();
  	unsigned int ChunkCell = ChunkSize(height);
  	unsigned int First = CoreId*ChunkCell, Last  = Min(height, First+ChunkCell);
	for(unsigned int i=First;i<Last;i++){
   		for(unsigned int j=0;j<width;j++){
    		image16[((i/2)*(width/2))+(j/2)] = ((ImageIn[((i)*width)+j] >> 3 ) << 3) | ((ImageIn[((i)*width)+j] >> 5) ) | (((ImageIn[((i)*width)+j] >> 2 ) << 13) )|   ((ImageIn[((i)*width)+j] >> 3) <<8);	}
	}
}




void cluster_main(ArgCluster_T* arg){
		unsigned int W = arg->W;
		unsigned int H = arg->H;


		//TODO cal this just once
		InstallModelBis(Model, WeakEstimModel, sizeof(Model)/sizeof(HoGWeakPredictorBis_T));

		gap8_resethwtimer();
		#if FROM_FILE

		#if VERBOSE
			ReportHoGConfiguration();
		#endif

		#else
			crop_image (ImageIn, ImageIn, W, H,2,2);
		#endif


		ProcessOneLevel( 0, ImageIn, ImageIn,  W, H, W-0*W/HOG_ESTIM_SCALE_FACTOR, H-0*H/HOG_ESTIM_SCALE_FACTOR, MyHOG,              0, MyWeakHOGEstim);
		ProcessOneLevel(-1, ImageIn, ImageIn1, W, H, W-1*W/HOG_ESTIM_SCALE_FACTOR, H-1*H/HOG_ESTIM_SCALE_FACTOR, MyHOG_N1, MyResize_N1, MyWeakHOGEstim_N1);
		ProcessOneLevel(-2, ImageIn, ImageIn1, W, H, W-2*W/HOG_ESTIM_SCALE_FACTOR, H-2*H/HOG_ESTIM_SCALE_FACTOR, MyHOG_N2, MyResize_N2, MyWeakHOGEstim_N2);
		ProcessOneLevel(-3, ImageIn, ImageIn1, W, H, W-3*W/HOG_ESTIM_SCALE_FACTOR, H-3*H/HOG_ESTIM_SCALE_FACTOR, MyHOG_N3, MyResize_N3, MyWeakHOGEstim_N3);

		ProcessOneLevel(+1, ImageIn, ImageIn1, W, H, W+1*W/HOG_ESTIM_SCALE_FACTOR, H+1*H/HOG_ESTIM_SCALE_FACTOR, MyHOG_1, MyResize_1, MyWeakHOGEstim_1);
		ProcessOneLevel(+2, ImageIn, ImageIn1, W, H, W+2*W/HOG_ESTIM_SCALE_FACTOR, H+2*H/HOG_ESTIM_SCALE_FACTOR, MyHOG_2, MyResize_2, MyWeakHOGEstim_2);

		RemoveNonMaxBB();

		#if FROM_FILE
		DumpBBList();
		DrawBBList(ImageIn,W,H);
		#else
		DrawBBList(ImageIn,W,H);
  		rt_team_fork(__builtin_pulp_CoreCount(), (void *) gray8_to_RGB565_cluster_half, (void *) arg);
  		ResetBBList();
  		#endif
}

int main()
{
	// char *ImageName = "ValveBW.ppm";
	// char *ImageName = "ValveBW_QVGA.ppm";
	//char *ImageName = "../../../Pedestrian.ppm";
	//char *ImageName = "../../../Pedestrian_324.pgm";

	char *Imagefile = "img_OUT5.pgm";
	char imageName[64];
	sprintf(imageName, "../../../testImages/GWT_dataset/%s", Imagefile);

	ArgCluster_T clusterArg;

	unsigned int MaxUpScale = 2;
	unsigned int W_real = 324, H_real = 244;
	unsigned int W = 322, H = 242;

	unsigned int MaxW = (W) + MaxUpScale*(W)/HOG_ESTIM_SCALE_FACTOR;
	unsigned int MaxH = (H) + MaxUpScale*(H)/HOG_ESTIM_SCALE_FACTOR;
	unsigned int MaxW_real = (W_real) + MaxUpScale*(W_real)/HOG_ESTIM_SCALE_FACTOR;
	unsigned int MaxH_real = (H_real) + MaxUpScale*(H_real)/HOG_ESTIM_SCALE_FACTOR;

    unsigned int BlockW = 1+((((MaxW-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE);   // Number of blocks in a line
    unsigned int BlockH = 1+((((MaxH-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE);   // Number of blocks in a row
	unsigned int EstiW  = 1+(((HOG_ESTIM_WIDTH)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));
	unsigned int EstiH  = 1+(((HOG_ESTIM_HEIGHT)/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP));

	unsigned int HOGFeatSize = BlockW*BlockH*HOG_NBINS*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*sizeof(unsigned short);
	unsigned int ImgSize     = MaxW*MaxH*sizeof(unsigned char);
	unsigned int ImgSize_real     = MaxH_real*MaxW_real*sizeof(unsigned char);
	//unsigned int AllocSize = Max(ImgSize, HOGFeatSize);
	unsigned int AllocSize = Max(ImgSize_real, HOGFeatSize);

	TotalCycles = 0; TotalHOGCycles = 0;
	TotalResizeCycles = 0; TotalEstimCycles = 0;

	//Allocating Events
	if (rt_event_alloc(NULL, 8)) return -1;
	//Setting FC to 250MhZ
	rt_freq_set(RT_FREQ_DOMAIN_FC, 250000000);

	image16 	= (unsigned short *) rt_alloc(RT_ALLOC_L2_CL_DATA, (322/2)*(242/2)*sizeof(unsigned short));
	ImageIn     = (unsigned char *)  rt_alloc(RT_ALLOC_L2_CL_DATA,ImgSize_real);
	HoGFeatures = (unsigned short *) rt_alloc(RT_ALLOC_L2_CL_DATA,HOGFeatSize);
	ImageIn1    = (unsigned char *)  rt_alloc(RT_ALLOC_L2_CL_DATA,ImgSize);


	if (ImageIn==0 || HoGFeatures==0 || ImageIn1==0) {
		printf("Failed to allocate Memory for Image (%d bytes) or HoGFeatures (%d bytes) or Image clone (%d bytes)\n",
			ImgSize, HOGFeatSize, ImgSize);
		return 1;
	}


#if FROM_FILE

	{

		unsigned int Wi, Hi;
		if ((ReadImageFromFile(imageName, &Wi, &Hi, ImageIn, AllocSize)==0) || (Wi!=W) || (Hi!=H))

		{
			printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", imageName, W, H, Wi, Hi);
			return 1;
		}
	}

#else


	//Init SPI and ili9341 LCD screen
	rt_spim_t *spim = init_spi_lcd();
	ILI9341_begin(spim);
	setRotation(spim,2);

	cam_param_conf(&cam1_conf);
	camera1 = rt_camera_open("camera", &cam1_conf, 0);
	if (camera1 == NULL) return -1;

    //Init Camera Interface
    rt_cam_control(camera1, CMD_INIT, 0);
    rt_time_wait_us(1000000); //Wait camera calibration

#endif

	rt_cluster_mount(MOUNT, CID, 0, NULL);
	//Setting Cluster freq to 175MHz
	rt_freq_set(RT_FREQ_DOMAIN_FC, 175000000);

	if (AllocateBBList(HOG_ESTIM_MAX_BB))
	{
		printf("Failed to allocate BB List\n"); return 1;
	}

	HOG_L1_Memory = (char *) rt_alloc(RT_ALLOC_CL_DATA,_HOG_L1_Memory_SIZE);
	if (HOG_L1_Memory == 0) { printf("Failed to allocate %d bytes for L1_memory\n", _HOG_L1_Memory_SIZE); return 1; }
	WeakEstimModel = (HoGWeakPredictorBis_T *) rt_alloc(RT_ALLOC_CL_DATA,sizeof(Model));
	if (WeakEstimModel == 0) { printf("Failed to allocate %d bytes for Estimation Model\n", sizeof(Model)); return 1; }


	void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
	if (stacks == NULL) return -1;

	clusterArg.W = W;
	clusterArg.H = H;
	clusterArg.W_real = W_real;
	clusterArg.H_real = H_real;

	//Setting GPIO for Measurements
	gap8SetOnePadAlternate(PLP_PAD_TIMER0_CH0, PLP_PAD_GPIO_ALTERNATE1);
    gap8SetGPIODir(gap8GiveGpioNb(PLP_PAD_TIMER0_CH0), GPIO_DIR_OUT);

	#if !FROM_FILE
	//Configuring Camera Registers
	himaxRegWrite(camera1,0x1003, 0x00);             //  Black level target

    himaxRegWrite(camera1,AE_TARGET_MEAN, 0xAC);      //AE target mean [Def: 0x3C]
    himaxRegWrite(camera1,AE_MIN_MEAN,    0x0A);      //AE min target mean [Def: 0x0A]
    himaxRegWrite(camera1,INTEGRATION_H,  0x87);      //Integration H [Def: 0x01]
    himaxRegWrite(camera1,INTEGRATION_L,  0x08);      //Integration L [Def: 0x08]
    himaxRegWrite(camera1,ANALOG_GAIN,    0x00);      //Analog Global Gain
    himaxRegWrite(camera1,DAMPING_FACTOR, 0x20);      //Damping Factor [Def: 0x20]
    himaxRegWrite(camera1,DIGITAL_GAIN_H, 0x01);      //Digital Gain High [Def: 0x01]
    himaxRegWrite(camera1,DIGITAL_GAIN_L, 0x00);      //Digital Gain Low [Def: 0x00]

	while(1){
        //Enable Camera Interface
        rt_cam_control(camera1, CMD_START, 0);
        // Get an event from the free list, which can then be used for a blocking wait using rt_event_wait.
        rt_event_t *event_0 = rt_event_get_blocking(NULL);
        // Programme the camera capture job to udma
        rt_camera_capture (camera1, ImageIn, W_real*H_real, event_0);
        // Start to receive the frame.
        // Wait until the capture finished
        rt_event_wait(event_0);
        // Stop the camera, camera interface is clock gated.
        rt_cam_control(camera1, CMD_PAUSE, 0);

	#endif
  		gap8WriteGPIO(gap8GiveGpioNb(PLP_PAD_TIMER0_CH0), 0x1);
		rt_cluster_call(NULL, CID, (void (*)(void *))  cluster_main, &clusterArg, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);
		gap8WriteGPIO(gap8GiveGpioNb(PLP_PAD_TIMER0_CH0), 0x0);

	#if !FROM_FILE
		//Send image to ili9341 LCD
		lcd_pushPixels(spim, LCD_OFF_X, LCD_OFF_Y, W/2,H/2, image16);
	}

	#else
	#if SAVE_IMAGE
		sprintf(imageName, "../../../%s", Imagefile);
    	printf("imgName: %s\n", imageName);
    	WriteImageToFile(imageName,W,H,(ImageIn));
	#endif
	#endif

	rt_cluster_mount(UNMOUNT, CID, 0, NULL);

#if FROM_FILE
	#if VERBOSE
		printf("\n\nHOG Pyramid Features extraction and estimation done.\n\n");
		printf("\tInput Image: [%d, %d], 7 scales in total. Upscale: [2, step 1/%d], Downscale: [3, Step: -1/%d]\n", W, H,HOG_ESTIM_SCALE_FACTOR,HOG_ESTIM_SCALE_FACTOR);
		printf("\tTotal Resize cycles                 : %d\n", TotalResizeCycles);
		printf("\tTotal HOG Features extraction cycles: %d\n", TotalHOGCycles);
		printf("\tTotal Weak boost estimation cycles  : %d\n", TotalEstimCycles);
		printf("\n");
	#endif
	printf("\tGrand Total cycles                  : %d\n", TotalCycles);
#endif


	return 0;
}
