/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include "Gap.h"
#include "../CNN_Libraries_fp16/CNN_FloatType.h"
#include "CNN_AT_Misc.h"



#ifdef __pulp__
#define Abs(a)          __builtin_pulp_abs((a))
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)          (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif


static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

/* Tensor Dump */
typedef enum {
	AT_MEM_UNDEF,
	AT_MEM_L3_HRAM,
	AT_MEM_L3_QSPIRAM,
	AT_MEM_L3_OSPIRAM,
	AT_MEM_L3_DEFAULTRAM,
	AT_MEM_L3_HFLASH,
	AT_MEM_L3_QSPIFLASH,
	AT_MEM_L3_OSPIFLASH,
	AT_MEM_L3_MRAMFLASH,
	AT_MEM_L3_DEFAULTFLASH,
	AT_MEM_L2,
	AT_MEM_L1,
	AT_MEM_LAST
} AT_MemLocation_T;

static void *AT_TensorGetNextPage(
	int Loc,
	void *L3_Device,
	void *L3_Event,
	unsigned int Size,
	void *L2_BufferAddr,
	void *Addr,
	int Offset)

{
	switch (Loc) {
		case AT_MEM_L3_HRAM:
			AT_HYPERRAM_CL_COPY((AT_HYPERRAM_T *) L3_Device, (AT_HYPERRAM_EXT_ADDR_TYPE) (Addr+Offset), (AT_HYPERRAM_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_HYPERRAM_CL_WAIT((AT_HYPERRAM_T *) L3_Device, L3_Event);
			break;
#ifdef __gap9__
		case AT_MEM_L3_QSPIRAM:
			AT_QSPIRAM_CL_COPY((AT_QSPIRAM_T *) L3_Device, (AT_QSPIRAM_EXT_ADDR_TYPE) (Addr+Offset), (AT_QSPIRAM_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_QSPIRAM_CL_WAIT((AT_QSPIRAM_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_OSPIRAM:
			AT_OSPIRAM_CL_COPY((AT_OSPIRAM_T *) L3_Device, (AT_OSPIRAM_EXT_ADDR_TYPE) (Addr+Offset), (AT_OSPIRAM_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_OSPIRAM_CL_WAIT((AT_OSPIRAM_T *) L3_Device, L3_Event);
			break;
#endif
		case AT_MEM_L3_HFLASH:
			AT_HYPERFLASH_FS_CL_COPY((AT_HYPERFLASH_FS_T *) L3_Device, (AT_HYPERFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_HYPERFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_HYPERFLASH_FS_CL_WAIT((AT_HYPERFLASH_FS_T *) L3_Device, L3_Event);
			break;
#ifdef __gap9__
		case AT_MEM_L3_QSPIFLASH:
			AT_QSPIFLASH_FS_CL_COPY((AT_QSPIFLASH_FS_T *) L3_Device, (AT_QSPIFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_QSPIFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_QSPIFLASH_FS_CL_WAIT((AT_QSPIFLASH_FS_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_OSPIFLASH:
			AT_OSPIFLASH_FS_CL_COPY((AT_OSPIFLASH_FS_T *) L3_Device, (AT_OSPIFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_OSPIFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_OSPIFLASH_FS_CL_WAIT((AT_OSPIFLASH_FS_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_MRAMFLASH:
			AT_EMRAMFLASH_FS_CL_COPY((AT_EMRAMFLASH_FS_T *) L3_Device, (AT_EMRAMFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_EMRAMFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_EMRAMFLASH_FS_CL_WAIT((AT_EMRAMFLASH_FS_T *) L3_Device, L3_Event);
			break;
#endif
		case AT_MEM_L2:
		case AT_MEM_L1:
			return Addr;
	}
	return L2_BufferAddr;
}

void AT_DumpTensor(
	char *NodeName,
	char *ArgName,
	int Loc,
	void *L3_Device,
	void *L3_Event,
	int ItemSize,
	int Dim,
	int D0,
	int D1,
	int D2,
	int D3,
	int D4,
	void *L2_BufferAddr,
	unsigned int L2_BufferSize,
	void *Addr)
{
	int MAX_PER_LINE = 30;
	int IsFloat = 0;
	if (ItemSize<0) {
		IsFloat = 1;
		ItemSize = -ItemSize;
	}
	int SizeToRead = D0*D1*D2*D3*D4*ItemSize;
	int InBuffer=0;
	if (L2_BufferSize==0) L2_BufferSize = SizeToRead;
	int Item = 0;
	int ReadSoFar = 0;
	void *BaseAddr = Addr;

	printf("Node: %s, Argument: %s, Dim: %d, [%d][%d][%d][%d][%d] ItemSize: %d\n", NodeName, ArgName, Dim, D0,D1,D2,D3,D4, ItemSize);
	for (int d0=0; d0<D0; d0++) {
		if (Dim>=5) printf("D%d: %d\n", Dim-5, d0);
		for (int d1=0; d1<D1; d1++) {
			if (Dim>=4) printf("D%d: %d\n", Dim-4, d1);
			for (int d2=0; d2<D2; d2++) {
				if (Dim>=3) printf("D%d: %d\n", Dim-3, d2);
				for (int d3=0; d3<D3; d3++) {
					int Nprinted = 0;
					if (Dim>=2) printf("D%d: %d - D%d:0..%d\n", Dim-2, d3, Dim-1, D4);
					else printf("D%d:0..%d\n", Dim-1, D4);
					for (int d4=0; d4<D4; d4++) {
						int Val = 0;
						float FVal;
						if (InBuffer==0) {
							int Size = Min(L2_BufferSize, SizeToRead);
							Addr = AT_TensorGetNextPage(Loc, L3_Device, L3_Event, Size, L2_BufferAddr, BaseAddr, ReadSoFar);
							InBuffer = Size;
							SizeToRead -= Size;
							ReadSoFar += Size;
							Item = 0;
						}
						switch (ItemSize) {
							case 1:
								Val = *((signed char *) (Addr+Item));
								break;
							case 2:
								if (IsFloat) FVal = *((F16 *) (Addr+Item)); else Val = *((short int *) (Addr+Item));
								break;
							case 4:
								if (IsFloat) FVal = *((float *) (Addr+Item)); else Val = *((int *) (Addr+Item));
								break;
						}
						InBuffer -= ItemSize;
						Item += ItemSize;
						if (IsFloat) printf(" %f", FVal); else printf(" %d", Val);
						Nprinted++;
						if (Nprinted==MAX_PER_LINE) {
							printf("\n"); Nprinted=0;
						}
					}
					if (Nprinted) printf("\n");
				}
			}
		}
	}
}

void AT_DumpTensorCompressed(
	char *NodeName,
	char *ArgName,
	int Loc,
	void *L3_Device,
	void *L3_Event,
	int LUTBits,
	int Sparse,
	int Dim,
	int D0,
	int D1,
	int D2,
	int D3,
	int D4,
	void *L2_BufferAddr,
	unsigned int L2_BufferSize,
	char *Addr,
	int *ItemAddresses)
{
	int MAX_PER_LINE = 30;

	int NumItems = D0*D1*D2*D3*D4;
	int TileSizeToRead = 0, TileItemsToRead = 0;
	int InBuffer=0, Val=0, CurVal=0;
	int Byte = 0, ReadingBit, Mask;
	int ReadSoFar = 0, ReadSparse = Sparse;
	char *BaseAddr = Addr;

	printf("Node: %s, Argument: %s, Dim: %d, [%d][%d][%d][%d][%d] ItemSize: %d\n", NodeName, ArgName, Dim, D0,D1,D2,D3,D4, (Sparse&&LUTBits==8?2:1));
	for (int d0=0; d0<D0; d0++) {
		if (Dim>=5) printf("D%d: %d\n", Dim-5, d0);
		for (int d1=0; d1<D1; d1++) {
			if (Dim>=4) printf("D%d: %d\n", Dim-4, d1);
			for (int d2=0; d2<D2; d2++) {
				if (Dim>=3) printf("D%d: %d\n", Dim-3, d2);
				for (int d3=0; d3<D3; d3++) {
					int Nprinted = 0;
					if (Dim>=2) printf("D%d: %d - D%d:0..%d\n", Dim-2, d3, Dim-1, D4);
					else printf("D%d:0..%d\n", Dim-1, D4);
					for (int d4=0; d4<D4; d4++) {
						int ReadVal = 0;
						while(!ReadVal) {
							/* Need data if at end of tile or buffer */
							if (!TileItemsToRead||InBuffer==0) {
								if (!TileItemsToRead||!TileSizeToRead) { /* End of Tile */
									TileSizeToRead = ItemAddresses[1];
									TileItemsToRead = ItemAddresses[2];
									ItemAddresses += 3;
									ReadingBit = 0;
								}
								int Size = L2_BufferSize?Min(L2_BufferSize, TileSizeToRead):TileSizeToRead;
								Addr = AT_TensorGetNextPage(Loc, L3_Device, L3_Event, Size, L2_BufferAddr, BaseAddr, ReadSoFar);
								TileSizeToRead -= Size;
								ReadSoFar += Size;
								Byte = 0; InBuffer = Size; Mask = 0x80;
							}
							if (Mask==0x80) Val = *((signed char *) (Addr+Byte));
							if (ReadSparse) {
								if (Mask&Val) {
									ReadSparse = 0;
									CurVal = 1;
								} else {
									printf(" 0");
									ReadVal = 1;
								}
							} else {
								CurVal = (CurVal<<1) + (Mask&Val?1:0);
								ReadingBit++;
								if (ReadingBit>=LUTBits) {
									printf(" %d", CurVal);
									CurVal = 0; ReadingBit = 0;
									ReadVal = 1; ReadSparse = Sparse;
								}
							}
							Mask>>=1;
							if (!Mask) {
								Byte++; InBuffer--; Mask = 0x80;
							}
						}
						TileItemsToRead--;
						Nprinted++;
						if (Nprinted==MAX_PER_LINE) {
							printf("\n"); Nprinted = 0;
						}
					}
					if (Nprinted) printf("\n");
				}
			}
		}
	}
}

void AT_ChecksumTensor(
	char *NodeName,
	char *ArgName,
	int Loc,
	void *L3_Device,
	void *L3_Event,
	int ItemSize,
	int Dim,
	int D0,
	int D1,
	int D2,
	int D3,
	int D4,
	void *L2_BufferAddr,
	unsigned int L2_BufferSize,
	void *Addr)
{
	long int Checksum = 0;
	float FChecksum = 0.0;
	int MAX_PER_LINE = 30;
	int SizeToRead = D0*D1*D2*D3*D4*ItemSize;
	int InBuffer=0;
	if (L2_BufferSize==0) L2_BufferSize = SizeToRead;
	int Item = 0;
	int ReadSoFar = 0;
	void *BaseAddr = Addr;
	int IsFloat = 0;
	if (ItemSize<0) {
		IsFloat = 1;
		ItemSize = -ItemSize;
	}

	for (int d0=0; d0<D0; d0++) {
		for (int d1=0; d1<D1; d1++) {
			for (int d2=0; d2<D2; d2++) {
				for (int d3=0; d3<D3; d3++) {
					for (int d4=0; d4<D4; d4++) {
						int Val = 0;
						float FVal = 0.0;
						if (InBuffer==0) {
							int Size = Min(L2_BufferSize, SizeToRead);
							Addr = AT_TensorGetNextPage(Loc, L3_Device, L3_Event, Size, L2_BufferAddr, BaseAddr, ReadSoFar);
							InBuffer = Size;
							SizeToRead -= Size;
							ReadSoFar += Size;
							Item = 0;
						}
						switch (ItemSize) {
							case 1:
								Val = *((signed char *) (Addr+Item));
								break;
							case 2:
								if (IsFloat) FVal = *((F16 *) (Addr+Item)); else Val = *((short int *) (Addr+Item));
								break;
							case 4:
								if (IsFloat) FVal = *((float *) (Addr+Item)); else Val = *((int *) (Addr+Item));
								break;
						}
						InBuffer -= ItemSize;
						Item += ItemSize;
						if (IsFloat) FChecksum += FVal;
						else         Checksum  += Val;
					}
				}
			}
		}
	}
	if (IsFloat) printf("Node: %s Argument: %s Size: %d ItemSize: %d\n\tChecksum = %f\n", NodeName, ArgName, D0*D1*D2*D3*D4, ItemSize, FChecksum);
	else         printf("Node: %s Argument: %s Size: %d ItemSize: %d\n\tChecksum = %ld\n", NodeName, ArgName, D0*D1*D2*D3*D4, ItemSize, Checksum);
}

/* Tile Padding */

typedef struct {
        char *__restrict__ In;          /**< Tile */
        short int W;                    /**< Tile width */
        short int H;                    /**< Tile Height */
        short int Feat;                 /**< Number of features */
        short int Size;                 /**< Tile data type size in bytes */
        short int Pad;                  /**< Number of lines to be cleared at bottom of each tile if horizontal tile, at right if vertical */
        short int PadValue;             /**< Number of lines to be cleared at bottom of each tile if horizontal tile, at right if vertical */
        short int Orientation;          /**< Tile orientation. 0: Horizontal, 1: vertical */
} AT_KerTileClear_T;

static void AT_KerParTileClear(AT_KerTileClear_T *Arg)

{
	char * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int Feat = Arg->Feat;
	int Size = Arg->Size;
	int Pad = Arg->Pad;
	int PadVal = Arg->PadValue;
	int PadVal4 = PadVal | (PadVal<<8) | (PadVal<<16) | (PadVal<<24);
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Feat);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Feat);

	if (Arg->Orientation == 0) { /* Horizontal */
		int ClrSize = W*Pad*Size;
       		for (unsigned int i=First; i<Last; i++) {
			char *Base = (In + i*W*H*Size + W*(H-Pad)*Size);
			for (unsigned j=0; j<(ClrSize/4); j++) ((int*) Base)[j] = PadVal4;
			for (unsigned j=(ClrSize/4)*4; j<ClrSize; j++) Base[j] = PadVal;
		}
	} else {
		int ClrSize = Pad*Size;
       		for (unsigned int i=First; i<Last; i++) {
			char *Base = (In + i*W*H*Size + (W-Pad)*Size);
			for (unsigned j=0; j<H; j++)
				for (unsigned k=0; k<ClrSize; k++) Base[W*Size*j + k] = PadVal;
		}
	}
	gap_waitbarrier(0);
}

static void AT_KerParTileClear_HWC(AT_KerTileClear_T *Arg)

{
	char * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int Feat = Arg->Feat;
	int Size = Arg->Size;
	int Pad = Arg->Pad;
	int PadVal = Arg->PadValue;
	int PadVal4 = PadVal | (PadVal<<8) | (PadVal<<16) | (PadVal<<24);
	
	int CoreId = gap_coreid();

	if (Arg->Orientation == 0) { /* Horizontal */
		int Chunk = ChunkSize(Pad*W);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Pad*W);
		int ClrSize = Feat;
       		for (int i=First; i<Last; i++) {
			char *Base = (In + W*(H-Pad)*Feat*Size + i*Feat*Size);
			for (unsigned j=0; j<(ClrSize/4); j++) ((int*) Base)[j] = PadVal4;
			for (unsigned j=(ClrSize/4)*4; j<ClrSize; j++) Base[j] = PadVal;
		}
	} else {
		int Chunk = ChunkSize(H);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, H);
		int ClrSize = Pad*Feat;
       		for (int i=First; i<Last; i++) {
			char *Base = (In + W*i*Feat*Size + (W-Pad)*Feat*Size);
			for (unsigned j=0; j<(ClrSize/4); j++) ((int*) Base)[j] = PadVal4;
			for (unsigned j=(ClrSize/4)*4; j<ClrSize; j++) Base[j] = PadVal;
		}
	}
	gap_waitbarrier(0);
}

static void AT_KerTileClear(AT_KerTileClear_T *Arg)

{
	char * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int Size = Arg->Size;
	int Pad = Arg->Pad;
	int PadVal = Arg->PadValue;
	
	unsigned int CoreId = gap_coreid();
	if (Arg->Orientation == 0) { /* Horizontal */
		int ClrSize = W*Pad*Size;
		unsigned int Chunk = ChunkSize(ClrSize);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, ClrSize);
		char *Base = (In + W*(H-Pad)*Size);
       		for (unsigned int i=First; i<Last; i++) Base[i] = PadVal;
	} else {
		unsigned int Chunk = ChunkSize(H);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, H);
		char *Base = (In + (W-Pad)*Size);
		int ClrSize = Pad*Size;
       		for (unsigned int i=First; i<Last; i++) 
			for (unsigned k=0; k<ClrSize; k++) Base[W*Size*i + k] = PadVal;
	}
	gap_waitbarrier(0);
}

void AT_TileClear(
	char *__restrict__ In,	/* Tile */
	int W,			/* Tile width */
	int H,			/* Tile height */
	int Feat,		/* Number of features */
	int Size,		/* Tile element size in bytes */
	int Pad,		/* Height or width of the area to be 0 padded */
	int PadValue,		/* Pad Value */
	int Orientation,	/* 0: Horizontal tile, 1: Vertical tile */
        int IsHWC               /* 0: CHW, 1: HWC */
	)

{
	volatile AT_KerTileClear_T Arg;

	Arg.In = In; Arg.W = W; Arg.H = H; Arg.Size = Size; Arg.Feat = Feat; Arg.Pad = Pad; Arg.PadValue = PadValue; Arg.Orientation = Orientation;

	if (Feat > 1) {
		if (IsHWC) {
			AT_FORK(gap_ncore(), (void *) AT_KerParTileClear_HWC, (void *) &Arg);
			__CALL(AT_KerParTileClear_HWC, (AT_KerTileClear_T *) &Arg);
		} else {
			AT_FORK(gap_ncore(), (void *) AT_KerParTileClear, (void *) &Arg);
			__CALL(AT_KerParTileClear, (AT_KerTileClear_T *) &Arg);			
		}
	} else {
		AT_FORK(gap_ncore(), (void *) AT_KerTileClear, (void *) &Arg);
		__CALL(AT_KerTileClear, (AT_KerTileClear_T *) &Arg);
	}
}

#pragma GCC diagnostic pop
