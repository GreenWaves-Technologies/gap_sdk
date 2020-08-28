#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

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
        AT_MEM_L3_HFLASH,
        AT_MEM_L3_QSPIFLASH,
        AT_MEM_L3_OSPIFLASH,
        AT_MEM_L3_MRAMFLASH,
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
#if 0
		case AT_MEM_L3_QSPIRAM:
			AT_QSPIRAM_CL_COPY((AT_QSPIRAM_T *) L3_Device, (AT_QSPIRAM_EXT_ADDR_TYPE) (Addr+Offset), (AT_QSPIRAM_INT_ADDR_TYPE) L2_BufferAddr, Size, 1, L3_Event);
			AT_QSPIRAM_CL_WAIT((AT_QSPIRAM_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_OSPIRAM:
			AT_OSPIRAM_CL_COPY((AT_OSPIRAM_T *) L3_Device, (AT_OSPIRAM_EXT_ADDR_TYPE) (Addr+Offset), (AT_OSPIRAM_INT_ADDR_TYPE) L2_BufferAddr, Size, 1, L3_Event);
			AT_OSPIRAM_CL_WAIT((AT_OSPIRAM_T *) L3_Device, L3_Event);
			break;
#endif
		case AT_MEM_L3_HFLASH:
			AT_HYPERFLASH_FS_CL_COPY((AT_HYPERFLASH_FS_T *) L3_Device, (AT_HYPERFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_HYPERFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 0, L3_Event);
			AT_HYPERFLASH_FS_CL_WAIT((AT_HYPERFLASH_FS_T *) L3_Device, L3_Event);
			break;
#if 0
		case AT_MEM_L3_QSPIFLASH:
			AT_QSPIFLASH_FS_CL_COPY((AT_QSPIFLASH_FS_T *) L3_Device, (AT_QSPIFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_QSPIFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 1, L3_Event);
			AT_QSPIFLASH_FS_CL_WAIT((AT_QSPIFLASH_FS_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_OSPIFLASH:
			AT_OSPIFLASH_FS_CL_COPY((AT_OSPIFLASH_FS_T *) L3_Device, (AT_OSPIFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_OSPIFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 1, L3_Event);
			AT_OSPIFLASH_FS_CL_WAIT((AT_OSPIFLASH_FS_T *) L3_Device, L3_Event);
			break;
		case AT_MEM_L3_MRAMFLASH:
			AT_EMRAMFLASH_FS_CL_COPY((AT_EMRAMFLASH_FS_T *) L3_Device, (AT_EMRAMFLASH_FS_EXT_ADDR_TYPE) (Addr+Offset), (AT_EMRAMFLASH_FS_INT_ADDR_TYPE) L2_BufferAddr, Size, 1, L3_Event);
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
								Val = *((char *) (Addr+Item));
								break;
							case 2:
								Val = *((short int *) (Addr+Item));
								break;
							case 4:
								Val = *((int *) (Addr+Item));
								break;
						}
						InBuffer -= ItemSize;
						Item += ItemSize;
						printf(" %d", Val);
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

/* Tile Padding */

typedef struct {
        char *__restrict__ In;          /**< Tile */
        short int W;                    /**< Tile width */
        short int H;                    /**< Tile Height */
        short int Feat;                 /**< Number of features */
        short int Size;                 /**< Tile data type size in bytes */
        short int Pad;                  /**< Number of lines to be cleared at bottom of each tile if horizontal tile, at right if vertical */
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
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Feat);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Feat);

	if (Arg->Orientation == 0) { /* Horizontal */
		int ClrSize = W*Pad*Size;
       		for (unsigned int i=First; i<Last; i++) {
			char *Base = (In + i*W*H*Size + W*(H-Pad)*Size);
			for (unsigned j=0; j<(ClrSize/4); j++) ((int*) Base)[j] = 0;
			for (unsigned j=(ClrSize/4)*4; j<ClrSize; j++) Base[j] = 0;
		}
	} else {
		int ClrSize = Pad*Size;
       		for (unsigned int i=First; i<Last; i++) {
			char *Base = (In + i*W*H*Size + (W-Pad)*Size);
			for (unsigned j=0; j<H; j++)
				for (unsigned k=0; k<ClrSize; k++) Base[W*Size*j + k] = 0;
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
	
	unsigned int CoreId = gap_coreid();
	if (Arg->Orientation == 0) { /* Horizontal */
		int ClrSize = W*Pad*Size;
		unsigned int Chunk = ChunkSize(ClrSize);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, ClrSize);
		char *Base = (In + W*(H-Pad)*Size);
       		for (unsigned int i=First; i<Last; i++) Base[i] = 0;
	} else {
		unsigned int Chunk = ChunkSize(H);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, H);
		char *Base = (In + (W-Pad)*Size);
		int ClrSize = Pad*Size;
       		for (unsigned int i=First; i<Last; i++) 
			for (unsigned k=0; k<ClrSize; k++) Base[W*Size*i + k] = 0;
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
	int Orientation		/* 0: Horizontal tile, 1: Vertical tile */
	)

{
	volatile AT_KerTileClear_T Arg;

	Arg.In = In; Arg.W = W; Arg.H = H; Arg.Size = Size; Arg.Feat = Feat; Arg.Pad = Pad; Arg.Orientation = Orientation;

	if (Feat > 1) {
		AT_FORK(gap_ncore(), (void *) AT_KerParTileClear, (void *) &Arg);
		__CALL(AT_KerParTileClear, (AT_KerTileClear_T *) &Arg);
	} else {
		AT_FORK(gap_ncore(), (void *) AT_KerTileClear, (void *) &Arg);
		__CALL(AT_KerTileClear, (AT_KerTileClear_T *) &Arg);
	}
}
