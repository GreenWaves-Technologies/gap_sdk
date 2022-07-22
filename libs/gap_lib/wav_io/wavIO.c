/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "gaplib/wavIO.h"

#ifndef SILENT
    #define PRINTF printf
#else
    #define PRINTF(...) ((void) 0)
#endif

#define WAV_HEADER_SIZE	44

#ifndef __EMUL__
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"
#define INTER_BUFF_SIZE     (1000*2)
static uint8_t *_tmp_buffer;
static int fs_write_from_L3(void *file, void *data, int size_total, struct pi_device *ram)
{
    int l3_index = 0;
    int size = 0;
    int rest_size = size_total;
    uint32_t _l3_buff = (uint32_t) (data);

    PRINTF("l3_buff: %x, size_total: %d\n", _l3_buff, size_total);

    _tmp_buffer = (uint8_t *) __ALLOC_L2((uint32_t) INTER_BUFF_SIZE);
    if (_tmp_buffer == NULL)
    {
        printf("TMP BUFFER malloc failed\n");
        return -1;
    }

    do
    {
        if(rest_size >= INTER_BUFF_SIZE)
            size = INTER_BUFF_SIZE;
        else
            size = INTER_BUFF_SIZE - rest_size;

        pi_ram_read(ram, (_l3_buff+l3_index), _tmp_buffer, (uint32_t) size);
        pi_fs_write(file, _tmp_buffer, size);

        l3_index += size;
        rest_size = rest_size - size;
    } while (rest_size > 0);
    
    __FREE_L2(_tmp_buffer, (uint32_t) INTER_BUFF_SIZE);

    return 0;
}
#endif

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

static int ReadWAVHeader(char *FileName, header_struct *HeaderInfo)
{
	// TODO use a struct for header
	switch_fs_t fs;
	__FS_INIT(fs);
	switch_file_t File = __OPEN_READ(fs, FileName);

	if (!File) {
		printf("Unable to open file %s\n", FileName);
		return 1;
	}

	unsigned int Err = 0;
	unsigned char *Header = (unsigned char *) __ALLOC_L2(WAV_HEADER_SIZE);
	Err |= (Header == 0);
	if ((__READ(File, Header, WAV_HEADER_SIZE) != WAV_HEADER_SIZE) || Err) return 1;

	HeaderInfo->FileSize      = Header[4]  | (Header[5]<<8)  | (Header[6]<<16)  |	(Header[7]<<24);
	HeaderInfo->format_type   = Header[20] | (Header[21]<<8);
	HeaderInfo->NumChannels   = Header[22] | (Header[23]<<8);
	HeaderInfo->SampleRate    = Header[24] | (Header[25]<<8) | (Header[26]<<16) |	(Header[27]<<24);
	HeaderInfo->byterate      = Header[32] | (Header[33]<<8);
	HeaderInfo->BitsPerSample = Header[34] | (Header[35]<<8);
	HeaderInfo->DataSize      = Header[40] | (Header[41]<<8) | (Header[42]<<16) |	(Header[43]<<24);
	__FREE_L2(Header, WAV_HEADER_SIZE);
	__CLOSE(File);
	__FS_DEINIT(fs);
	return Err;
}

static int ReadWavShort(switch_file_t File, short int* OutBuf, unsigned int NumSamples, unsigned int Channels)
{
	int ChunkSize = 1024;
	unsigned char *data_buf = (unsigned char *) __ALLOC_L2(ChunkSize*Channels*sizeof(short int));
	int BytesPerSample = Channels * sizeof(short int);
	if (data_buf==NULL) {
		printf("Error allocating\n");
		return 1;
	}
	int i;
	unsigned int ch;
	int RemainBytes = NumSamples*BytesPerSample;
	int read_size;
	while (RemainBytes>0){
		#ifndef SILENT
			progress_bar("Reading Wav ", NumSamples*BytesPerSample-RemainBytes, NumSamples*BytesPerSample);
		#endif
		if (RemainBytes > ChunkSize*BytesPerSample) read_size = ChunkSize*BytesPerSample;
		else 										read_size = RemainBytes;
		int len = __READ(File, data_buf, read_size);
		if (!len) return 1;
		RemainBytes -= len;
		int offset = 0;
		for (i=0; i<len/BytesPerSample; i++){
			int data_in_channel;
			for (ch=0; ch<Channels; ch++){
				data_in_channel = data_buf[offset*2] | (data_buf[offset*2+1] << 8);
				OutBuf[i*Channels + ch] = (short int) data_in_channel;
				offset += 1; //Bytes in each channel
			}
		}
		OutBuf += Channels*(len/BytesPerSample);
	}
	__FREE_L2(data_buf, ChunkSize*Channels*sizeof(short int));

	return 0;
}

static int ReadWavChar(switch_file_t File, char* OutBuf, unsigned int NumSamples, unsigned int Channels)
{
	int ChunkSize = 1024;
	unsigned char *data_buf = (unsigned char *) __ALLOC_L2(ChunkSize*Channels*sizeof(char));
	int BytesPerSample = Channels * sizeof(char);
	if (data_buf==NULL) {
		printf("Error allocating\n");
		return 1;
	}
	int i;
	unsigned int ch;
	int RemainBytes = NumSamples*BytesPerSample;
	int read_size;
	while (RemainBytes>0){
		#ifndef SILENT
			progress_bar("Reading Wav ", NumSamples*BytesPerSample-RemainBytes, NumSamples*BytesPerSample);
		#endif
		if (RemainBytes > ChunkSize*BytesPerSample) read_size = ChunkSize*BytesPerSample;
		else 										read_size = RemainBytes;
		int len = __READ(File, data_buf, read_size);
		if (!len) return 1;
		RemainBytes -= len;
		int offset = 0;
		for (i=0; i<read_size; i++){
			int data_in_channel;
			for (ch=0; ch<Channels; ch++){
				data_in_channel = data_buf[offset];
				OutBuf[offset*Channels + ch] = (short int) data_in_channel;
				offset += 1; //Bytes in each channel
			}
		}
		OutBuf += len/BytesPerSample;
	}
	return 0;
}

int ReadWavFromFile(char *FileName, void* OutBuf, unsigned int BufSize, header_struct *HeaderInfo) 
{
	if (ReadWAVHeader(FileName, HeaderInfo)) return 1;
	switch_file_t File = (switch_file_t) 0;
	switch_fs_t fs;
	__FS_INIT(fs);
	File = __OPEN_READ(fs, FileName);
	if (File == 0) {
		printf("Failed to open file, %s\n", FileName); goto Fail;
	}

	int NumSamples = HeaderInfo->DataSize * 8 / (HeaderInfo->NumChannels * HeaderInfo->BitsPerSample);
	int SizeOfEachSample = (HeaderInfo->NumChannels * HeaderInfo->BitsPerSample) / 8;

	__SEEK(File, WAV_HEADER_SIZE);

	int SamplesShort;
	if (HeaderInfo->BitsPerSample == 16) SamplesShort = 1;
	else if (HeaderInfo->BitsPerSample == 8) SamplesShort = 0;
	else {printf("BytesPerSample %d not supported\n", HeaderInfo->BitsPerSample); return 1;}

	if (BufSize < HeaderInfo->DataSize){
		printf("Buffer Size too small: %d required, %d given", HeaderInfo->DataSize, BufSize);
		return 1;
	}

	int res;
	if (HeaderInfo->BitsPerSample == 16)
		res = ReadWavShort(File, (short int *) OutBuf, NumSamples, HeaderInfo->NumChannels);
	else if (HeaderInfo->BitsPerSample == 8)
		res = ReadWavChar(File, (char *) OutBuf, NumSamples, HeaderInfo->NumChannels);
	else goto Fail;

	if (res) {
		printf("Input ended unexpectedly or bad format, %s\n", FileName); goto Fail;
	}
	__CLOSE(File);
	__FS_DEINIT(fs);
	PRINTF("\n\nFile: %s, FileSize: %d, NumChannels: %d, SampleRate: %d, BitsPerSample: %d, DataSize: %d, NumSamples: %d\n", \
		    FileName, HeaderInfo->DataSize, HeaderInfo->NumChannels, HeaderInfo->SampleRate, HeaderInfo->BitsPerSample, HeaderInfo->DataSize, NumSamples);

	return 0;
Fail:
	__CLOSE(File);
	__FS_DEINIT(fs);
	printf("Failed to load file %s from flash\n", FileName);
	return 1;

}

#ifndef __EMUL__
int WriteWavFromL3ToFile(char *FileName, int BytesPerSample, int SampleRate, int NumChannels, void *data, int Size, struct pi_device *ram)
{
	return WriteWavToFileNew(FileName, BytesPerSample, SampleRate, NumChannels, data, Size, 1, ram);
}
#endif

int WriteWavToFile(char *FileName, int BytesPerSample, int SampleRate, int NumChannels, void *data, int Size)
{
	return WriteWavToFileNew(FileName, BytesPerSample, SampleRate, NumChannels, data, Size, 0, 0);
}

int WriteWavToFileNew(char *FileName, int BytesPerSample, int SampleRate, int NumChannels, void *data, int Size, int fromL3, struct pi_device *ram)
{
	switch_fs_t fs;
	__FS_INIT(fs);

    void *File = __OPEN_WRITE(fs, FileName);

    unsigned int idx = 0;
    unsigned int sz = WAV_HEADER_SIZE + Size;

    unsigned char *header_buffer = (unsigned char *) __ALLOC_L2(WAV_HEADER_SIZE * sizeof(char));

    // 4 bytes "RIFF"
    header_buffer[idx++] = 'R';
    header_buffer[idx++] = 'I';
    header_buffer[idx++] = 'F';
    header_buffer[idx++] = 'F';

    // 4 bytes File size - 8bytes 32kS 0x10024 - 65408S 0x1ff24
    //header_buffer[idx++] = 0x24;
    //header_buffer[idx++] = 0xff;
    //header_buffer[idx++] = 0x01;
    //header_buffer[idx++] = 0x00;
    header_buffer[idx++] = (unsigned char) (sz & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((sz & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((sz & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((sz & 0xff000000) >> 24);

    // 4 bytes file type: "WAVE"
    header_buffer[idx++] = 'W';
    header_buffer[idx++] = 'A';
    header_buffer[idx++] = 'V';
    header_buffer[idx++] = 'E';

    // 4 bytes format chunk: "fmt " last char is trailing NULL
    header_buffer[idx++] = 'f';
    header_buffer[idx++] = 'm';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = ' ';

    // 4 bytes length of format data below, until data part
    header_buffer[idx++] = 0x10;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 2 bytes type of format: 1 (PCM)
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    // 2 bytes nb of channels: 1 or 2
    //header_buffer[idx++] = 0x02;
    //header_buffer[idx++] = 0x01;
    header_buffer[idx++] = NumChannels;
    header_buffer[idx++] = 0x00;

    // 4 bytes sample rate in Hz:
    header_buffer[idx++] = (SampleRate >> 0) & 0xff;
    header_buffer[idx++] = (SampleRate >> 8) & 0xff;
    header_buffer[idx++] = (SampleRate >> 16) & 0xff;
    header_buffer[idx++] = (SampleRate >> 24) & 0xff;

    // 4 bytes (Sample Rate * BitsPerSample * Channels) / 8:
    // (8000*16*1)/8=0x3e80 * 2
    // (16000*16*1)/8=32000 or 0x6F00
    // (22050*16*1)/8=0xac44
    // (22050*16*2)/8=0x15888
    int rate = (SampleRate * BytesPerSample * NumChannels) / 8;
    header_buffer[idx++] = (rate >> 0) & 0xff;
    header_buffer[idx++] = (rate >> 8) & 0xff;
    header_buffer[idx++] = (rate >> 16) & 0xff;
    header_buffer[idx++] = (rate >> 24) & 0xff;

    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    rate = (BytesPerSample * NumChannels) / 8;
    header_buffer[idx++] = (rate >> 0) & 0xff;
    header_buffer[idx++] = (rate >> 8) & 0xff;

    // 2 bytes bit per sample:
    header_buffer[idx++] = BytesPerSample;
    header_buffer[idx++] = 0x00;

    // 4 bytes "data" chunk
    header_buffer[idx++] = 'd';
    header_buffer[idx++] = 'a';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = 'a';

    // 4 bytes size of data section in bytes:
    header_buffer[idx++] = (unsigned char) (Size & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((Size & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((Size & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((Size & 0xff000000) >> 24);

    int ret = 0;
    ret += __WRITE(File, header_buffer, WAV_HEADER_SIZE);
    #ifndef __EMUL__
	    if(fromL3)
	        ret += fs_write_from_L3(File, data, Size, ram);
	    else
	        ret += __WRITE(File, data, Size);
    #else
	    ret += __WRITE(File, data, Size);
	#endif

    __CLOSE(File);

    __FS_DEINIT(fs);
    return ret;
}
