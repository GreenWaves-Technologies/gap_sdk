/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "gaplib/ImgIO.h"
#include "stdint.h"


#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#define CHUNK_SIZE 8192

unsigned char *img_rgb888;


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
	unsigned char *Header = (unsigned char *) __ALLOC_L2(256);
	Err |= (Header == 0);
	if (__READ(File, Header, 256) == 256) {
		unsigned int i;
		if (ReadPPMHeader(Header, W, H, BytesPerPixel, HeaderSize, 256)) {
			printf("Unable to load header %s", Name);
			Err = 1;
		} else {
			PRINTF("Image %s:  [W: %d, H: %d] Bytes per pixel %d, HeaderSize: %d\n", Name, *W, *H, *BytesPerPixel, *HeaderSize);
			for (i=0; i<*HeaderSize;i++) PRINTF("%c", Header[i]);
			PRINTF("\n");
		}
	} else {
		printf("Unable to read header %s", Name);
		Err = 1;
	}
	__FREE_L2(Header, 256);
	__CLOSE(File);
	__FS_DEINIT(fs);
	return Err;
}

static int ReadMultiChannelImageRGB565(switch_file_t File, unsigned short * InBuffer, int W, int H)
{
	unsigned int RowSize = W*3;
	unsigned char InputBuf[RowSize];
	unsigned short * pInBuffer = InBuffer;

	for (int CurRow=0; CurRow < H; CurRow++) {
		int RemainBytes = RowSize;
		unsigned char *pInpBuf = InputBuf;
		while (RemainBytes > 0) {
			__int_ssize_t len = __READ(File, pInpBuf, RemainBytes);
			if (!len) return 1;
			RemainBytes -= len;
			pInpBuf += len;
		}
		for (int j=0, i=0; i < W; i++) {
			pInBuffer[W * CurRow + i] = ((((uint16_t)InputBuf[j]&0xf8)<<8)|(((uint16_t)InputBuf[j+1]&0xfc)<<3)|(((uint16_t)InputBuf[j+2]&0xf8)>>3));
            j+=3;
		}
	}
	return 0;
}

static int ReadMultiChannelImageTranspose2CHW(switch_file_t File, signed char * InBuffer, int W, int H, int BytesPerPixel)
{
	unsigned int RowSize = W*BytesPerPixel, ChannelSize = W * H;
	unsigned char InputBuf[RowSize];
	signed char * pInBuffer = InBuffer;

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
				pInBuffer[ChannelSize * j + W * CurRow + i] = InputBuf[i * BytesPerPixel + j];
			}
		}
	}
	return 0;
}

static int ReadMultiChannelImage(switch_file_t File, signed char * InBuffer, int W, int H, int BytesPerPixel)
{
	unsigned int RowSize = W*BytesPerPixel, ChannelSize = W * H;
	unsigned char InputBuf[RowSize];
	signed char * pInBuffer = InBuffer;

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
				pInBuffer[RowSize * CurRow + i * BytesPerPixel + j] = InputBuf[i * BytesPerPixel + j];
			}
		}
	}
	return 0;
}

static int ReadMultiChannelImageShortTranspose2CHW(switch_file_t File, short int * InBuffer, int W, int H, int BytesPerPixel)
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
				pInBuffer[ChannelSize * j + W * CurRow + i] = (short int) (InputBuf[i * BytesPerPixel + j]);
			}
		}
	}
	return 0;
}

static int ReadMultiChannelImageShort(switch_file_t File, short int * InBuffer, int W, int H, int BytesPerPixel)
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
				pInBuffer[RowSize * CurRow + i * BytesPerPixel + j] = (short int) (InputBuf[i * BytesPerPixel + j]);
			}
		}
	}
	return 0;
}

int ReadImageFromFile(char *ImageName, unsigned int DesiredW, unsigned int DesiredH, unsigned int DesiredBytesPerPixel, void *InBuffer, unsigned int BuffSize, img_io_out_t out_type, int Transpose2CHW) 
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
	if (out_type == IMGIO_OUTPUT_RGB565) {
		if (BuffSize < W*H*2) {
			printf("Buffer is too small, %s\n", ImageName); goto Fail;
		}
	} else {
		if (BuffSize < Size) {
			printf("Buffer is too small, %s\n", ImageName); goto Fail;
		}
	}
	__SEEK(File, HeaderSize);
	int res;
	switch (out_type) {
		case IMGIO_OUTPUT_CHAR:
			if (Transpose2CHW){
				res = ReadMultiChannelImageTranspose2CHW(File, (signed char *)InBuffer, W, H, BytesPerPixel);
			} else {
				res = ReadMultiChannelImage(File, (signed char *)InBuffer, W, H, BytesPerPixel);
			}
			break;
		case IMGIO_OUTPUT_SHORT:
			if (Transpose2CHW){
				res = ReadMultiChannelImageShortTranspose2CHW(File, (short int *)InBuffer, W, H, BytesPerPixel);
			} else {
				res = ReadMultiChannelImageShort(File, (short int *)InBuffer, W, H, BytesPerPixel);
			}
			break;
		case IMGIO_OUTPUT_RGB565:
			res = ReadMultiChannelImageRGB565(File, (unsigned short *)InBuffer, W, H);
			break;
		default:
			res = 1;
	}
	if (res) {
		printf("Input ended unexpectedly or bad format, %s\n", ImageName); goto Fail;
	}
	__CLOSE(File);
	__FS_DEINIT(fs);
	PRINTF("Image %s, [W: %d, H: %d], Bytes per pixel %d, Size: %d bytes, Loaded successfully\n", ImageName, W, H, BytesPerPixel, Size);

	return 0;
Fail:
	__CLOSE(File);
	__FS_DEINIT(fs);
	printf("Failed to load image %s from flash\n", ImageName);
	return 1;
}

static void WritePPMHeader(void *FD, unsigned int W, unsigned int H, unsigned char imgFormat)
{
    // BYPASS mode, no need any header
    if (imgFormat == BYPASS_IO)
        return ;

    unsigned int Ind = 0, x, i, L;
    unsigned char *Buffer = (unsigned char *) __ALLOC_L2(PPM_HEADER * sizeof(unsigned char));

    /* P5<cr>* */
    Buffer[Ind++] = 0x50;                                   // P
    if (imgFormat == GRAY_SCALE_IO) Buffer[Ind++] = 0x35;   // 5
    else Buffer[Ind++] = 0x36;                              // 6
    Buffer[Ind++] = 0xA;                                    // <cr>

    /* W <space> */
    x = W; L=0;
    while (x>0)
    {
        x = x/10;
        L++;
    }
    x = W; i = 1;
    while (x>0)
    {
        Buffer[Ind+L-i] = 0x30 + (x%10);
        i++;
        x=x/10;
    }
    Ind += L;
    Buffer[Ind++] = 0x20;

    /* H <cr> */
    x = H; L=0;
    while (x>0)
    {
        x = x/10;
        L++;
    }
    x = H; i = 1;
    while (x>0)
    {
        Buffer[Ind+L-i] = 0x30 + (x%10);
        i++;
        x=x/10;
    }
    Ind += L;
    Buffer[Ind++] = 0xA;

    /* 255 <cr> */
    Buffer[Ind++] = 0x32;
    Buffer[Ind++] = 0x35;
    Buffer[Ind++] = 0x35;
    Buffer[Ind++] = 0xA;

    for (unsigned int a=0; a<Ind; a++)
    {
        __WRITE(FD,&(Buffer[a]), sizeof(unsigned char));
    }

    __FREE_L2(Buffer, PPM_HEADER * sizeof(unsigned char));
}

static void rgb565_to_rgb888 (unsigned char *input, unsigned int input_size, unsigned char *output )
{
    unsigned char red, green, blue; // 8-bits each
    unsigned short pixel;
    unsigned int ind = 0;

    // Just a simplest implementation, need to be optimized the performance
    for (unsigned int i = 0; i < input_size; i+=2) {
        pixel = *(unsigned short *) (input + i);
        red = (unsigned short)((pixel & 0xF800) >> 11);  // 5
        green = (unsigned short)((pixel & 0x07E0) >> 5); // 6
        blue = (unsigned short)(pixel & 0x001F);         // 5

        output[ind] = red << 3;  /* red */
        output[ind+1] = green << 2;  /* green */
        output[ind+2] = blue << 3;  /* blue */
        ind += 3;
    }
}


int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char PixelSize, unsigned char *OutBuffer, unsigned char imgFormat)
{

	switch_fs_t fs;
	__FS_INIT(fs);

    void *File = __OPEN_WRITE(fs, ImageName);

    int ret = 0;
    WritePPMHeader(File,W,H, imgFormat);

    if(imgFormat == RGB565_IO)
    {
        unsigned int rgb888_size = (CHUNK_SIZE/2)*3;     // size of 888 image in byte
        img_rgb888 = (unsigned char *) __ALLOC_L2(rgb888_size);

        int steps = (W*H*PixelSize) / CHUNK_SIZE;             // convert and fs write times

        for(int i=0;i<steps;i++){
        	#ifndef SILENT
	            progress_bar("Writing image ",i,steps);
	        #endif
            rgb565_to_rgb888(OutBuffer+(CHUNK_SIZE*i), CHUNK_SIZE, img_rgb888);
            ret+=__WRITE(File, img_rgb888, rgb888_size);
        }
        if(((W*H*PixelSize) % CHUNK_SIZE) != 0)
        {
            rgb888_size = ((W*H*PixelSize) % CHUNK_SIZE)/2*3;
            rgb565_to_rgb888((OutBuffer+(CHUNK_SIZE*steps)),((W*H*PixelSize) % CHUNK_SIZE) ,img_rgb888);
            ret+=__WRITE(File, img_rgb888, rgb888_size);
        }

        __FREE_L2(img_rgb888, (CHUNK_SIZE/2)*3);
    }
    else
    {
        int steps = (W*H*PixelSize) / CHUNK_SIZE;

        for(int i=0;i<steps;i++){
            #ifndef SILENT
	            progress_bar("Writing image ",i,steps);
	        #endif
            ret+=__WRITE(File,OutBuffer +(CHUNK_SIZE*i), CHUNK_SIZE);
        }
        if(((W*H*PixelSize) % CHUNK_SIZE) != 0)
            ret+=__WRITE(File,OutBuffer+(CHUNK_SIZE*steps) , ((W*H*PixelSize) % CHUNK_SIZE)*sizeof(unsigned char));
    }

    __CLOSE(File);
    __FS_DEINIT(fs);

    return ret;
}
