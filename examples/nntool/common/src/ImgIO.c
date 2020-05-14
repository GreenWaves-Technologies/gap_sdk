/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "ImgIO.h"
#include "Gap.h"

#ifndef PPM_HEADER
#define PPM_HEADER                          40
#endif

#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#define CHUNK_SIZE 8192

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

void SkipCommentAndWhiteSpace(unsigned char *pImg, int buf_len, int *i)
{
	int saw_nl = 1;
	while (*i < buf_len && (pImg[*i] == '#'||pImg[*i] == ' '||pImg[*i] == '\t'||pImg[*i] == '\r'||pImg[*i] == '\n')) {
		if (saw_nl && pImg[*i] == '#') {
			while (*i < buf_len && pImg[*i] != '\n') {
				printf("%c", pImg[*i]);
				(*i)++;
			}
		}
		saw_nl = (pImg[*i] == '\n');
		(*i)++;
	}
}

unsigned int ReadValue(unsigned char *pImg, int buf_len, int *i)
{
#define IS_DIGIT(C) (((C) >= '0') && ((C) <= '9'))
	unsigned int val = 0;
	SkipCommentAndWhiteSpace(pImg, buf_len, i);
	while (*i < buf_len && !IS_DIGIT(pImg[*i])) {
		(*i)++;
	}
	while (*i < buf_len && IS_DIGIT(pImg[*i])) {
		val = val * 10 + (pImg[*i] - 0x30);
		(*i)++;
	}
	return val;
#undef IS_DIGIT
}

static int ReadPPMHeader(unsigned char *ImgIn, unsigned int *W, unsigned int *H, unsigned int *BytesPerPixel, unsigned int *HeaderLen, int buf_len)
{
	*W = *H = *BytesPerPixel = *HeaderLen = 0;

	if      (ImgIn[0] == 'P' && ImgIn[1] == '5' && ImgIn[2] == '\n') *BytesPerPixel = 1;
	else if (ImgIn[0] == 'P' && ImgIn[1] == '6' && ImgIn[2] == '\n') *BytesPerPixel = 3;
	else return 1;

	int i = 3;

	*W = ReadValue(ImgIn, buf_len, &i);
	*H = ReadValue(ImgIn, buf_len, &i);
	unsigned int Val = ReadValue(ImgIn, buf_len, &i);

	if (Val != 255) return 1;

	while (ImgIn[i++] != 0xA) {};
	*HeaderLen = i;
	return 0;
}

static int GetInputImageInfos(char *Name, unsigned int *W, unsigned int *H, unsigned int *BytesPerPixel, unsigned int *HeaderSize)
{
	*W = 0; *H = 0; *BytesPerPixel = 0; *HeaderSize = 0;
	switch_fs_t fs;
	__FS_INIT(fs);
	switch_file_t File = __OPEN_READ(fs, Name);

	if (!File) {
		printf("Unable to open file %s\n", Name);
		return 1;
	}

	unsigned int Err = 0;
	unsigned char *Header = (unsigned char *) gap_allocL2(256);
	Err |= (Header == 0);
	if (__READ(File, Header, 256) == 256) {
		unsigned int i;
		if (ReadPPMHeader(Header, W, H, BytesPerPixel, HeaderSize, 256)) {
			printf("Unable to load header %s", Name);
			Err = 1;
		} else {
			printf("Image %s:  [W: %d, H: %d] Bytes per pixel %d, HeaderSize: %d\n", Name, *W, *H, *BytesPerPixel, *HeaderSize);
			for (i=0; i<*HeaderSize;i++) printf("%c", Header[i]);
			printf("\n");
		}
	} else {
		printf("Unable to read header %s", Name);
		Err = 1;
	}
	gap_freeL2(Header, 256);
	__CLOSE(File);
	__FS_DEINIT(fs);
	return Err;
}

static int ReadMultiChannelImage(switch_file_t File, unsigned char * InBuffer, int W, int H, int BytesPerPixel, int shift)
{
	unsigned int RowSize = W*BytesPerPixel, ChannelSize = W * H;
	unsigned char InputBuf[RowSize];
	unsigned char * pInBuffer = InBuffer;
	
	for (int CurRow=0; CurRow < H; CurRow++) {
		int RemainBytes = RowSize;
		unsigned char *pInpBuf = InputBuf;
		while (RemainBytes > 0) {
			__int_ssize_t len = __READ(File, pInpBuf, RemainBytes);
			if (!len) return 1;
			RemainBytes -= len;
			pInpBuf += len;
			
		}
		for (int i=0; i < W; i++) {
			for (int j=0; j < BytesPerPixel; j++) {
				pInBuffer[ChannelSize * j + W * CurRow + i] = (shift?InputBuf[i * BytesPerPixel + j] >> 1:InputBuf[i * BytesPerPixel + j]);
			}
		}
	}
	return 0;
}

static int ReadMultiChannelImageShort(switch_file_t File, short int * InBuffer, int W, int H, int BytesPerPixel, int shift)
{
	unsigned int RowSize = W*BytesPerPixel, ChannelSize = W * H;
	unsigned char InputBuf[RowSize];
	short int * pInBuffer = InBuffer;
	
	for (int CurRow=0; CurRow < H; CurRow++) {
		int RemainBytes = RowSize;
		unsigned char *pInpBuf = InputBuf;
		while (RemainBytes > 0) {
			__int_ssize_t len = __READ(File, pInpBuf, RemainBytes);
			if (!len) return 1;
			RemainBytes -= len;
			pInpBuf += len;
		}
		for (int i=0; i < W; i++) {
			for (int j=0; j < BytesPerPixel; j++) {
				pInBuffer[ChannelSize * j + W * CurRow + i] = (short int) (shift?InputBuf[i * BytesPerPixel + j] >> 1:InputBuf[i * BytesPerPixel + j]);
			}
		}
	}
	return 0;
}

int ReadImageFromFile(char *ImageName, unsigned int DesiredW, unsigned int DesiredH, unsigned int DesiredBytesPerPixel, void *InBuffer, unsigned int BuffSize, int shift, int shorts)
{
	switch_file_t File = (switch_file_t) 0;
	unsigned int BytesPerPixel, W, H, HeaderSize, Size, ReadSize=0;

	if (GetInputImageInfos(ImageName, &W, &H, &BytesPerPixel, &HeaderSize)) {
		printf("Failed to get input images infos, %s\n", ImageName); goto Fail;
	}
	if (BytesPerPixel != DesiredBytesPerPixel) {
		printf("Expecting %d bytes per pixel image, %s\n", BytesPerPixel, ImageName); goto Fail;
	}
	if (DesiredH != H || DesiredW != W) {
		printf("Expecting [%dx%d] image, got [%dx%d] %s\n", DesiredW, DesiredH, W, H, ImageName); goto Fail;
	}
	switch_fs_t fs;
	__FS_INIT(fs);
	File = __OPEN_READ(fs, ImageName);
	if (File == 0) {
		printf("Failed to open file, %s\n", ImageName); goto Fail;
	}
	Size = W*H*BytesPerPixel;
	if (BuffSize < Size) {
		printf("Buffer is too small, %s\n", ImageName); goto Fail;
	}
	__SEEK(File, HeaderSize);
	if ((shorts?ReadMultiChannelImageShort(File, (short int *)InBuffer, W, H, BytesPerPixel, shift):ReadMultiChannelImage(File, (unsigned char *)InBuffer, W, H, BytesPerPixel, shift))) {
		printf("Input ended unexpectedly, %s\n", ImageName); goto Fail;
	}
	__CLOSE(File);
	__FS_DEINIT(fs);
	printf("Image %s, [W: %d, H: %d], Bytes per pixel %d, Size: %d bytes, Loaded successfully\n", ImageName, W, H, BytesPerPixel, Size);

	return 0;
Fail:
	__CLOSE(File);
	__FS_DEINIT(fs);
	printf("Failed to load image %s from flash\n", ImageName);
	return 1;
}

static void WritePPMHeader(switch_file_t FD, unsigned int W, unsigned int H)
{
  	unsigned int Ind = 0, x, i, L;
  	unsigned char *Buffer = (unsigned char *) gap_allocL2(PPM_HEADER * sizeof(unsigned char));

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

	gap_freeL2(Buffer, PPM_HEADER * sizeof(unsigned char));
}

int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *OutBuffer)
{
	#define CHUNK_NUM 10
	switch_fs_t fs;
	__FS_INIT(fs);
	switch_file_t File = __OPEN_WRITE(fs, ImageName);
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
	__FS_DEINIT(fs);

	return ret;
}
