/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ImgIO.h"
#include "Gap.h"

#ifdef __EMUL__
#define pi_l2_malloc(x) malloc(x)
#define pi_l2_free(x,y) free(x)
#endif

#ifdef LINK_IMAGE_HEADER
#include LINK_IMAGE_HEADER
#define __int_ssize_t unsigned int
static ssize_t seek_point = 0;
#define __OPEN(__name, __flags, __mode) 1
#define __CLOSE(__fd)
#define __SEEK(__fd, __pos) seek_point = __pos
static ssize_t fake_read(unsigned char * buf, ssize_t len) {
	int left = sizeof(LINK_IMAGE_NAME) - seek_point;
	if (len < left) left = len;
	for (int i=0; i<left; i++)
		buf[i] = LINK_IMAGE_NAME[seek_point+i];
	return left;
}
static ssize_t fake_write(unsigned char * buf, ssize_t len) {
	return 0;
}

#define __READ(__fd, __buf, __len) fake_read(__buf, __len)
#define __WRITE(__fd, __buf, __len) fake_write(__buf, __len)
#else
#ifdef __EMUL__
#include <unistd.h>
#define __OPEN(__name, __flags, __mode) open(__name, __flags, __mode)
#define __CLOSE(__fd) close(__fd)
#define __SEEK(__fd, __pos) lseek(__fd, __pos, SEEK_SET)
#define __READ(__fd, __buf, __len) read(__fd, __buf, __len)
#define __WRITE(__fd, __buf, __len) write(__fd, __buf, __len)
#define __int_ssize_t ssize_t
#else
#define __OPEN(__name, __flags, __mode) BRIDGE_Open(__name, __flags, __mode, NULL)
#define __CLOSE(__fd) BRIDGE_Close(__fd, NULL)
#define __SEEK(__fd, __pos) BRIDGE_Read(__fd, NULL, __pos, NULL);
#define __READ(__fd, __buf, __len) BRIDGE_Read(__fd, __buf, __len, NULL)
#define __WRITE(__fd, __buf, __len) BRIDGE_Write(__fd, __buf, __len, NULL)
#define __int_ssize_t unsigned int
#endif
#endif

#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#define CHUNK_SIZE 8192

#define PPM_HEADER 40

static void progress_bar(char * OutString, int n, int tot)
{
	int tot_chars = 30;
	printf("%s",OutString);
	printf(" [");
	int chars = (n*tot_chars)/tot;

	for(int i=0;i<tot_chars;i++){
		if(i<=chars)
			printf("#");
		else printf(" ");
	}
	printf("]");
	printf("\n");

}

static unsigned int SkipComment(unsigned char *Img, unsigned int Ind)
{
	while (Img[Ind] == '#') {
		while (Img[Ind] != '\n') {printf("%c", Img[Ind]);Ind++;}
		Ind++;
	}
	return Ind;
}

static unsigned int ReadPPMHeader(unsigned char *ImgIn, unsigned int *W, unsigned int *H, unsigned int *IsRGB, unsigned int *res)
{
#define IS_DIGIT(C) (((C) >= '0') && ((C) <= '9'))
	unsigned int Val, Ind = 0;


	if      (ImgIn[0] == 'P' && ImgIn[1] == '5' && ImgIn[2] == '\n') *IsRGB = 0;
	else if (ImgIn[0] == 'P' && ImgIn[1] == '6' && ImgIn[2] == '\n') *IsRGB = 1;
	else return 0;
	Ind = 3;

	Ind=SkipComment(ImgIn, Ind);
	while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
	while (IS_DIGIT(ImgIn[Ind])) {
		Val = Val*10 + (ImgIn[Ind] - 0x30);
		Ind++;
	}
	*W = Val;
	Ind=SkipComment(ImgIn, Ind);
	while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
	while (IS_DIGIT(ImgIn[Ind])) {
		Val = Val*10 + (ImgIn[Ind] - 0x30);
		Ind++;
	}
	*H = Val;

	Ind=SkipComment(ImgIn, Ind);
	while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
	while (IS_DIGIT(ImgIn[Ind])) {
		Val = Val*10 + (ImgIn[Ind] - 0x30);
		Ind++;
	}
	if (Val != 255 && Val!=65335) return 0;
	if (Val==255) *res=8;
	if (Val==65335) *res=16;
	
	while (ImgIn[Ind] != 0xA) Ind++;

	return (Ind+1);

#undef IS_DIGIT
}

static unsigned int GetInputImageInfos(char *Name, unsigned int *W, unsigned int *H, unsigned int *IsRGB, unsigned int *HeaderSize, unsigned int *res)
{
	static int Debug=1;
	int File = __OPEN(Name, O_RDONLY, 0);
	unsigned int Err = 0;

	*W = 0; *H = 0; *IsRGB = 0; *HeaderSize = 0;

	if (Debug) printf("File: %s open: %s\n", Name, (File?"Ok":"Failed"));
	if (File) {
		unsigned char *Header = (unsigned char *) pi_l2_malloc(256);
		Err |= (Header == 0);
		if (__READ(File, Header, 256) == 256) {
			unsigned int i;
			*HeaderSize = ReadPPMHeader(Header, W, H, IsRGB, res);
			if (Debug) {
				printf("Image %s:  [W: %d, H: %d] %s, HeaderSize: %d\n", Name, *W, *H, *IsRGB?"Color":"Gray", *HeaderSize);
				for (i=0; i<*HeaderSize;i++) printf("%c", Header[i]);
                printf("\n");
			}
		} else Err = 1;
		pi_l2_free(Header, 256);
		__CLOSE(File);
	}
	return Err;
}

unsigned char *ReadImageFromFile(char *ImageName, unsigned int *W, unsigned int *H, unsigned char *InBuffer, unsigned int BuffSize)
{
	int File = 0;
	unsigned int IsRGB, HeaderSize, Size, AlignedSize, ReadSize=0;
	unsigned char *ImagePtr = 0;
	int Allocated = 0;
	unsigned int res=0;

	
	if (GetInputImageInfos(ImageName, W, H, &IsRGB, &HeaderSize, &res)) {
		printf("Failed to get input images infos, %s\n", ImageName); goto Fail;
	}
	if (IsRGB) {
		printf("Only Gray levels supported, found RGB\n"); goto Fail;
	}
	File = __OPEN(ImageName, O_RDONLY, 0);
	if (File == 0) {
		printf("Failed to open file, %s\n", ImageName); goto Fail;
	}
	Size = (*W)*(*H)*(IsRGB?3:1)*((res==16)?sizeof(short int):sizeof(unsigned char));
	printf("SIZE %d\n",Size);
	if (InBuffer && (BuffSize >= Size)) {
		AlignedSize = Size;
		ImagePtr = InBuffer;
	} else {
		Allocated = 1;
		AlignedSize = ALIGN(Size, 2);
		ImagePtr = (unsigned char *) pi_l2_malloc(AlignedSize);
	}
	if (ImagePtr == 0) {
		printf("Failed to allocate %d bytes for input image\n", AlignedSize); goto Fail;
	}
	__SEEK(File, HeaderSize);
	{
		unsigned char *TargetImg = ImagePtr;
		unsigned int RemainSize = AlignedSize;

		while (RemainSize > 0) {
			unsigned int Chunk = Min(4096, RemainSize);
			printf("LOAD IMAGE : RemainSize %d ReadSize %d Chunk %d\n",RemainSize,ReadSize,Chunk);
			__int_ssize_t R = __READ(File, TargetImg, Chunk);
			ReadSize += R;
			if (R != Chunk) break;
			TargetImg += Chunk; RemainSize -= Chunk;
		}
	}
	if (AlignedSize!=ReadSize) {
		printf("Error, expects %d bytes but got %d\n", AlignedSize, ReadSize); goto Fail;
	}
	__CLOSE(File);
	printf("Image %s, [W: %d, H: %d], Gray, Size: %d bytes, res=%d bits Loaded sucessfully\n", ImageName, *W, *H, AlignedSize,res);

	//for (int i=0;i<100;i++) printf("%d %d\n",i,((short int*)ImagePtr)[i]);

	return (ImagePtr);


	
Fail:
	if (ImagePtr && Allocated) pi_l2_free(ImagePtr, AlignedSize);
	__CLOSE(File);
	printf("Failed to load image %s from flash\n", ImageName);
	return 0;
}

static void WritePPMHeader(int FD, unsigned int W, unsigned int H)
{
  	unsigned int Ind = 0, x, i, L;
  	unsigned char *Buffer = (unsigned char *) pi_l2_malloc(PPM_HEADER * sizeof(unsigned char));

  	/* P5<cr>* */
  	Buffer[Ind++] = 0x50; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  	/* W <space> */
  	x = W; L=0;
  	while (x>0) { x = x/10; L++; }
  	x = W; i = 1;
  	while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  	Ind += L;
  	Buffer[Ind++] = 0x20;

  	/* H <cr> */
  	x = H; L=0;
  	while (x>0) { x = x/10; L++; }
  	x = H; i = 1;
  	while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  	Ind += L;
  	Buffer[Ind++] = 0xA;

  	/* 255 <cr> */
  	Buffer[Ind++] = 0x32; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  	for (unsigned int a=0; a<Ind; a++){
  		__WRITE(FD, &(Buffer[a]), sizeof(unsigned char));
	}

	pi_l2_free(Buffer, PPM_HEADER * sizeof(unsigned char));
}

int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *OutBuffer)
{
	int File = __OPEN(ImageName, O_RDWR | O_CREAT, S_IRWXU);
	int ret = 0;
	WritePPMHeader(File, W, H);

	int steps = (W*H) / CHUNK_SIZE;

	for(int i=0;i<steps;i++){
		progress_bar("Writing image ",i,steps);
		ret += __WRITE(File, OutBuffer +(CHUNK_SIZE*i), CHUNK_SIZE);
	}
	if(((W*H) % CHUNK_SIZE) != 0)
		ret += __WRITE(File, OutBuffer+(CHUNK_SIZE*steps), ((W*H) % CHUNK_SIZE)*sizeof(unsigned char));

	__CLOSE(File);

	return ret;
}


