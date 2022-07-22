#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BIT_INSERT(dst, src, size, off)     (((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define BIT_EXTRACT(x, size, off)           (((int)((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size))))>>(32-(size)))
#define BIT_EXTRACTU(x, size, off)          (((x)>>(off))&((unsigned int)(1<<(size))-1))

#define SPECIAL_SYMBOL (0xABBAABBA)

typedef struct {
    unsigned int Ctype:2;		// 0: uniform quantization, 1: optimal quantization, 2: optimal quantization + 1 bit
    unsigned int Qbits:4;		// Number of bits for quantized numbers
    unsigned int Sign:1;		// 0: expanded data is unsigned, 1: expanded data is signed
    unsigned int TargetType:2;	// Size in type of the expanded data 1: byte, 2: half word, 4: word
    unsigned int DataN:23;		// Number of items
} Header_T;

typedef struct {
    unsigned int InitSeed;
    unsigned int StartBit;
    unsigned int StartByte;
    unsigned int EncodeType;
    unsigned int Qbits;
    unsigned int Ncentroids;
    unsigned int Qcbits;
    unsigned int Nitems;
    unsigned int DataSize;
    unsigned int UnsignedData;
    char *OutFileName;
    FILE *File;
    unsigned int OutBitSize;
    char *InName;
    char *CentName;
    char *OutName;
} Control_T;

void Insert(
        char *Data,
        unsigned int At,	// Location in bits
        unsigned int Size,	// Number of bits
        int V			// Value to insert
        )

{
    unsigned int BytePos = At/8;
    unsigned int ByteOff = At%8;
    unsigned int *IntPt = (unsigned int *) (Data+BytePos);

    *IntPt = BIT_INSERT(*IntPt, V, Size, ByteOff);
}

unsigned int Extract(
        char *Data,
        unsigned int At,
        unsigned int Size,
        unsigned int Unsign
        )

{
    unsigned int BytePos = At/8;
    unsigned int ByteOff = At%8;
    unsigned int *IntPt = (unsigned int *) (Data+BytePos);

    if (Unsign)
    {
        return BIT_EXTRACTU(*IntPt, Size, ByteOff);
    }
    else
    {
        return BIT_EXTRACT((int)*IntPt, Size, ByteOff);
    }
}

unsigned int PrimeRNG(unsigned int Seed)

{
    if (Seed)
    {
        srand(Seed);
    }
    else
    {
        Seed = time(NULL);
        srand(Seed);
    }
    return Seed;
}

int GenRandN(int Min_num, int Max_num)

{
    int Result = 0, Low_num = 0, Hi_num = 0;

    if (Min_num < Max_num)
    {
        Low_num = Min_num; Hi_num = Max_num + 1; // include max_num in output
    }
    else
    {
        Low_num = Max_num + 1; Hi_num = Min_num; // include max_num in output
    }
    Result = (rand() % (Hi_num - Low_num)) + Low_num;
    return Result;
}

void GenPlainData(void **Data, unsigned int DataSize, unsigned int Unsign, unsigned Q, unsigned int N, int Single, int NonZero)

{
    char *PtB;
    short int *PtH;
    int *PtW;

    switch (DataSize) {
        case 1:
            PtB = (char*) malloc(sizeof(char)*N);
            for (int i=0; i<N; i++) {
                if (Single) {
                    int Unique = 0;
                    while (!Unique) {
                        Unique = 1;
                        char V = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                        if (Unsign) V = V&((1<<Q)-1);
                        if (NonZero&&(V==0)) Unique=0;
                        else for (int j=0; j<i; j++) if (V == PtB[i]) {Unique = 0; break;}
                        if (Unique) PtB[i] = V;
                    }
                } else {
                    PtB[i] = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                    if (Unsign) PtB[i] = PtB[i]&((1<<Q)-1);
                }
            }
            *Data = PtB;
            break;
        case 2:
            PtH = (short int*) malloc(sizeof(short int)*N);
            for (int i=0; i<N; i++) {
                if (Single) {
                    int Unique = 0;
                    while (!Unique) {
                        Unique = 1;
                        short int V = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                        if (Unsign) V = V&((1<<Q)-1);
                        if (NonZero&&(V==0)) Unique=0;
                        else for (int j=0; j<i; j++) if (V == PtH[i]) {Unique = 0; break;}
                        if (Unique) PtH[i] = V;
                    }
                } else {
                    PtH[i] = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                    if (Unsign) PtH[i] = PtH[i]&((1<<Q)-1);
                }
            }
            *Data = PtH;
            break;
        case 4:
            PtW = (int*) malloc(sizeof(int)*N);
            for (int i=0; i<N; i++) {
                if (Single) {
                    int Unique = 0;
                    while (!Unique) {
                        Unique = 1;
                        int V = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                        if (Unsign) V = V&((1<<Q)-1);
                        if (NonZero&&(V==0)) Unique=0;
                        else for (int j=0; j<i; j++) if (V == PtW[i]) {Unique = 0; break;}
                        if (Unique) PtW[i] = V;
                    }
                } else {
                    PtW[i] = GenRandN(-(1<<(Q-1)), (1<<(Q-1))-1);
                    if (Unsign) PtW[i] = PtW[i]&((1<<Q)-1);
                }
            }
            *Data = PtW;
            break;
        default: *Data = 0;
    }
}

int InitializeData(void **InData, short int **Centroids, Control_T *Ctrl)

{
    unsigned int EncodeType = Ctrl->EncodeType;
    unsigned int DataSize = Ctrl->DataSize;
    unsigned int N = Ctrl->Nitems;

    // Add how many items will be added if we start from a different bit and byte offset
    unsigned int starting_offset = (Ctrl->StartByte * 8) + (Ctrl->StartBit);
    unsigned int offset_items_added = (starting_offset/Ctrl->Qbits) + 1;

    N += offset_items_added;

    unsigned int Q = Ctrl->Qbits;
    unsigned int NC = Ctrl->Ncentroids;

    // used in T3 mode only. Counts the number of elements that index LUT
    int D;

    switch (EncodeType) {
        case 0:
            GenPlainData(InData, DataSize, Ctrl->UnsignedData, Q, N, 0, 0); return (Q*N);
        case 1:
            *InData = (void *) malloc(DataSize*N);
            GenPlainData((void **)Centroids, 2, Ctrl->UnsignedData, Q, NC, 1, 0);
            for (int i=0; i<N; i++) {
                int V = GenRandN(0, NC-1);
                switch (DataSize) {
                    case 1: ((char *) (*InData))[i] = (*Centroids)[V]; break;
                    case 2: ((short int *) (*InData))[i] = (*Centroids)[V]; break;
                    case 4: ((int *) (*InData))[i] = (*Centroids)[V]; break;
                }
            }
            return (Q*N + starting_offset);
            break;
        case 2:
            N = Ctrl->Nitems;
            starting_offset = (Ctrl->StartByte * 8) + (Ctrl->StartBit);
            offset_items_added = (starting_offset/Ctrl->Qbits) * Ctrl->Qbits + 1;

            N += offset_items_added;
            *InData = (void *) malloc(DataSize*N);
            GenPlainData((void **)Centroids, 2, Ctrl->UnsignedData, Q, NC, 1, 1);
            D = 0;
            for (int i=0; i<N; i++) {
                int V = GenRandN(-NC, NC-1);
                switch (DataSize) {
                    case 1: ((char *) (*InData))[i] = (V>=0)?(*Centroids)[V]:SPECIAL_SYMBOL; if (V>=0) D++; break;
                    case 2: ((short int *) (*InData))[i] = (V>=0)?(*Centroids)[V]:SPECIAL_SYMBOL; if (V>=0) D++; break;
                    case 4: ((int *) (*InData))[i] = (V>=0)?(*Centroids)[V]:SPECIAL_SYMBOL; if (V>=0) D++; break;
                }
            }
            // For each D (Address to lookup table and plus 1 bit)
            // The reason for +1 bit is that bit with value 0 is special symbol and
            // when we find 1 we check qbit after to address the LUT
            return ((N-D)+(D*(Ctrl->Qcbits+1)) + starting_offset);
    }
    return 0;
}

void DumpData(char *Mess, void *Data, int DataSize, int Unsign, int N)

{
    printf("----------%s----------------\n", Mess);
    for (int i=0; i<N; i++) {
        int V;
        switch (DataSize) {
            case 1:
                if (!Unsign)
                {
                    V = ((signed char *)Data)[i];
                }
                else
                {
                    V = ((unsigned char *)Data)[i];
                }
                break;
            case 2:
                if (!Unsign)
                {
                    V = ((short int *)Data)[i];
                }
                else
                {
                    V = ((unsigned short int *)Data)[i];
                }
                break;
            case 4:
                if (!Unsign)
                {
                    V = ((int *)Data)[i];
                }
                else
                {
                    V = ((unsigned int *)Data)[i];
                }
                break;
        }
        printf("%3d: %d\n", i, V);
    }
}

char *TypeName(Control_T *Ctrl)

{
    switch (Ctrl->DataSize) {
        case 1:
            if (!Ctrl->UnsignedData)
            {
                return "signed char";
            }
            else
            {
                return "unsigned char";
            }
            break;
        case 2:
            if (!Ctrl->UnsignedData)
            {
                return "signed short";
            }
            else
            {
                return "unsigned short";
            }
            break;
        case 4:
            if (!Ctrl->UnsignedData)
            {
                return "int";
            }
            else
            {
                return "unsigned int";
            }
            break;
    }
    return "BadType";
}


int GetData(void *Data, int DataSize, int Unsign, int At)

{
    int V=0;
    switch (DataSize) {
        case 1:
            if (!Unsign)
            {
                V = ((signed char *)Data)[At];
            }
            else
            {
                V = ((unsigned char *)Data)[At];
            }
            break;
        case 2:
            if (!Unsign)
            {
                V = ((short int *)Data)[At];
            }
            else
            {
                V = ((unsigned short int *)Data)[At];
            }
            break;
        case 4:
            if (!Unsign)
            {
                V = ((int *)Data)[At];
            }
            else
            {
                V = ((unsigned int *)Data)[At];
            }
            break;
    }
    return V;
}

void PutData(void *Data, int DataSize, int Unsign, int At, int V)

{
    switch (DataSize) {
        case 1:
            if (!Unsign)
            {
                ((signed char *)Data)[At] = V;
            }
            else
            {
                ((unsigned char *)Data)[At] = V;
            }
            break;
        case 2: if (!Unsign)
                {
                    ((short int *)Data)[At] = V;
                }
                else
                {
                    ((unsigned short int *)Data)[At] = V;
                }
                break;
        case 4: if (!Unsign)
                {
                    ((int *)Data)[At] = V;
                }
                else
                {
                    ((unsigned int *)Data)[At] = V;
                }
                break;
    }
}

void Dump2File(void *In, short int *Cent, void *Out, Control_T *Ctrl)

{
    unsigned int OutWordSize = (Ctrl->OutBitSize+31)/32;

    fprintf(Ctrl->File, "PI_L1 %s %s[%d] = {\n", TypeName(Ctrl), Ctrl->InName, Ctrl->Nitems);
    for (int i=0; i<Ctrl->Nitems; i++) fprintf(Ctrl->File, "\t%d,\n", GetData(In, Ctrl->DataSize, Ctrl->UnsignedData, i));
    fprintf(Ctrl->File, "};\n");
    if (Ctrl->EncodeType==1||Ctrl->EncodeType==2) {
        fprintf(Ctrl->File, "%s %s[%d] = {\n", "PI_L1 short int", Ctrl->CentName, Ctrl->Ncentroids);
        for (int i=0; i<Ctrl->Ncentroids; i++) fprintf(Ctrl->File, "\t%d,\n", Cent[i]);
        fprintf(Ctrl->File, "};\n");
    }
    fprintf(Ctrl->File, "PI_L2 %s %s[%d] = {\n", "unsigned int", Ctrl->OutName, OutWordSize);
    for (int i=0; i<OutWordSize; i++) fprintf(Ctrl->File, "\t0x%X,\n", ((unsigned int *) Out)[i]);
    fprintf(Ctrl->File, "};\n");
}

int Clookup(short int *Centroids, unsigned int NC, int V)

{
    for (int i=0; i<NC; i++)
    {
        if (Centroids[i]==V)
        {
            return i;
        }
    }
    return -1;
}

void EncodeData(void *InData, short int *Centroids, unsigned int **OutData, Control_T *Ctrl)

{
    unsigned int OutWordSize = (Ctrl->OutBitSize+31)/32;
    unsigned int *Out = (unsigned int *) malloc(sizeof(unsigned int)*OutWordSize);
    unsigned int CurPos;

    *OutData = Out;
    switch (Ctrl->EncodeType) {
        case 0:
            CurPos = (Ctrl->StartByte * 8) + (Ctrl->StartBit);
            for (int i=0; i < Ctrl->Nitems; i++)
            {
                int Value = GetData(InData, Ctrl->DataSize, Ctrl->UnsignedData, i);
                Insert((char *)Out, CurPos, Ctrl->Qbits, Value); CurPos+=Ctrl->Qbits;
            }
            break;
        case 1:
            CurPos = (Ctrl->StartByte * 8) + (Ctrl->StartBit);
            for (int i=0; i < Ctrl->Nitems; i++)
            {
                int Value = Clookup(Centroids, Ctrl->Ncentroids, GetData(InData, Ctrl->DataSize, Ctrl->UnsignedData, i));
                Insert((char *)Out, CurPos, Ctrl->Qcbits, Value); CurPos+=Ctrl->Qcbits;
            }
            break;
        case 2:
            CurPos = (Ctrl->StartByte * 8) + (Ctrl->StartBit);
            for (int i=0; i < Ctrl->Nitems; i++)
            {
                long long mask = ((1LL << (Ctrl->DataSize * 8)) - 1);
                int Value = GetData(InData, Ctrl->DataSize, Ctrl->UnsignedData, i);
                if (   (Value          & mask)
                    == (SPECIAL_SYMBOL & mask))
                {
                    Insert((char *)Out, CurPos, 1, 0); CurPos+=1;
                }
                else
                {
                    Insert((char *)Out, CurPos, 1, 1); CurPos+=1;
                    Value = Clookup(Centroids, Ctrl->Ncentroids, Value);
                    Insert((char *)Out, CurPos, Ctrl->Qcbits, Value); CurPos+=Ctrl->Qcbits;
                }
            }
            break;
    }

}

void *DecodeData(unsigned int *OutData, short int *Cent, void **DecData, Control_T *Ctrl)

{
    unsigned int OutWordSize = (Ctrl->OutBitSize+31)/32;
    unsigned int CurPos= (Ctrl->StartByte * 8) + (Ctrl->StartBit);
    void *OutDecData = (void *) malloc(Ctrl->DataSize*Ctrl->Nitems);

    switch (Ctrl->EncodeType) {
        case 0:
            for (int i=0; i<Ctrl->Nitems; i++)
            {
                if ((CurPos+Ctrl->Qbits)>Ctrl->OutBitSize)
                {
                    printf("Fetching item %d after end of encoded stream\n", i);
                }
                int V = Extract((char *)OutData, CurPos, Ctrl->Qbits, Ctrl->UnsignedData); CurPos += Ctrl->Qbits;
                PutData(OutDecData, Ctrl->DataSize, Ctrl->UnsignedData, i, V);
            }
            break;
        case 1:
            for (int i=0; i<Ctrl->Nitems; i++)
            {
                if ((CurPos+Ctrl->Qcbits)>Ctrl->OutBitSize)
                {
                    printf("Fetching item %d after end of encoded stream\n", i);
                }
                int V = Extract((char *)OutData, CurPos, Ctrl->Qcbits, 1); CurPos += Ctrl->Qcbits;
                V = Cent[V];
                PutData(OutDecData, Ctrl->DataSize, Ctrl->UnsignedData, i, V);
            }
            break;
        case 2:
            for (int i=0; i<Ctrl->Nitems; i++)
            {
                if ((CurPos+1)>Ctrl->OutBitSize)
                {
                    printf("Fetching item %d after end of encoded stream\n", i);
                }
                int V = Extract((char *)OutData, CurPos, 1, Ctrl->UnsignedData); CurPos++;
                if (V != 0)
                {
                    if ((CurPos+Ctrl->Qcbits)>Ctrl->OutBitSize)
                    {
                        printf("Fetching item %d after end of encoded stream\n", i);
                    }
                    V = Extract((char *)OutData, CurPos, Ctrl->Qcbits, 1); CurPos += Ctrl->Qcbits;
                    V = Cent[V];
                }
                else
                {
                    V = SPECIAL_SYMBOL;
                }
                PutData(OutDecData, Ctrl->DataSize, Ctrl->UnsignedData, i, V);
            }
            break;
    }
    *DecData = OutDecData;
}

int Check(void *In, void *DecEncIn, Control_T *Ctrl)

{
    int Err=0;
    for (int i=0; i<Ctrl->Nitems; i++)
    {
        int V0 = GetData(In, Ctrl->DataSize, Ctrl->UnsignedData, i);
        int V1 = GetData(DecEncIn, Ctrl->DataSize, Ctrl->UnsignedData, i);
        if (V0!=V1)
        {
            fprintf(stderr, "At index %d In=%d, Dec(Enc(In))=%d\n", i, V0, V1);
            Err++;
        }
    }
    return Err;
}
void UsageDetails()

{
    fprintf(stderr, "\t-s rngseed    : Seed for random number generation, if 0 or not passed will use time()\n");
    fprintf(stderr, "\t-b startbit   : Starting bit to start (0 <= starbit <= 7)\n");
    fprintf(stderr, "\t-B startbyte  : Starting byte to start (0 <= starbyte <= 3)\n");
    fprintf(stderr, "\t-E enctype    : Encoding type, 0 uniform quantization, 1 optimal quantization, 2 optimal quantization b'0{Ci} if In!=0, b'1 if In=0\n");
    fprintf(stderr, "\t-N nitems     : Number of input elements, must be >0\n");
    fprintf(stderr, "\t-Q quantbits  : Number of bits given for an input, must be in [1..15] if signed, [1..16] if unsigned\n");
    fprintf(stderr, "\t-C ncentroids : When optimal quantization is used, number of centroids, must be in [1..256], must be provided is enctype=1 or 2\n");
    fprintf(stderr, "\t-S datasize   : Input is byte: 1, half word: 2, word: 4\n");
    fprintf(stderr, "\t-U            : Input is unsigned, signed if ommitted\n");
    fprintf(stderr, "\t-O outfile    : Output file name, stdout if ommitted\n");
    fprintf(stderr, "\t-i name       : Name of the C var to be used for Input, default is DataIn\n");
    fprintf(stderr, "\t-o name       : Name of the C var to be used for Output, default is DataOut\n");
    fprintf(stderr, "\t-c name       : Name of the C var to be used for Centroids LUT, default is Centroid\n");
}
void ParseArguments(int argc, char *argv[], Control_T *Ctrl)

{
    int Opt;

    Ctrl->InitSeed = 0;
    Ctrl->EncodeType = 0;
    Ctrl->Qbits = 0;
    Ctrl->Ncentroids = 0;
    Ctrl->Qcbits = 0;
    Ctrl->Nitems = 0;
    Ctrl->DataSize = 0;
    Ctrl->UnsignedData = 0;
    Ctrl->OutBitSize = 0;
    Ctrl->File = 0;
    Ctrl->OutFileName = 0;
    Ctrl->StartBit = 0;
    Ctrl->StartByte = 0;

    Ctrl->InName = "DataIn";
    Ctrl->OutName = "DataOut";
    Ctrl->CentName = "Centroid";

    while ((Opt=getopt(argc, argv, "s:b:B:E:Q:C:N:S:UO:i:o:c:"))!=-1) {
        switch (Opt) {
            case 's': Ctrl->InitSeed = atoi(optarg); break;
            case 'b': Ctrl->StartBit = atoi(optarg); break;
            case 'B': Ctrl->StartByte = atoi(optarg); break;
            case 'E': Ctrl->EncodeType = atoi(optarg); break;
            case 'Q': Ctrl->Qbits = atoi(optarg); break;
            case 'C': Ctrl->Ncentroids = atoi(optarg); break;
            case 'N': Ctrl->Nitems = atoi(optarg); break;
            case 'S': Ctrl->DataSize = atoi(optarg); break;
            case 'U': Ctrl->UnsignedData=1; break;
            case 'O': Ctrl->OutFileName = optarg; break;
            case 'i': Ctrl->InName = optarg; break;
            case 'o': Ctrl->OutName = optarg; break;
            case 'c': Ctrl->CentName = optarg; break;
            default:
                      fprintf(stderr, "Usage: %s [-s val] [-b val] [-B val] [-E val] [-N val] [-Q val] [-C val] [-S val] [-U] [-O name] [-i name] [-o name] [-c name]\n", argv[0]);
                      UsageDetails();
                      exit(EXIT_FAILURE);

        }
    }
    if ( 	(Ctrl->EncodeType<0||Ctrl->EncodeType>2) ||
            (Ctrl->Qbits<1||Ctrl->Qbits>16) ||
            (Ctrl->StartBit < 0) || (Ctrl->StartBit > 7) ||
            (Ctrl->StartByte < 0) || (Ctrl->StartByte > 3) ||
            ((Ctrl->EncodeType!=0)&&(Ctrl->Ncentroids<1||Ctrl->Ncentroids>256)) ||
            (Ctrl->Nitems<1) ||
            (Ctrl->DataSize!=1&&Ctrl->DataSize!=2&&Ctrl->DataSize!=4)) {
        fprintf(stderr, "Usage: %s [-s val] [-b val] [-B val] [-E val] [-N val] [-Q val] [-C val] [-S val] [-U] [-O name] [-i name] [-o name] [-c name]\n", argv[0]);
        UsageDetails();
        exit(EXIT_FAILURE);
    }
    if (Ctrl->Qbits>((Ctrl->DataSize*8)-(Ctrl->UnsignedData==0))) {
        fprintf(stderr, "Quantization bits: %d > InputDataSize: %d bits\n", Ctrl->Qbits, Ctrl->DataSize*8-(Ctrl->UnsignedData!=0));
        exit(EXIT_FAILURE);
    }
    if (Ctrl->Ncentroids)
    {
        //Ctrl->Qcbits = 32-__builtin_clz(Ctrl->Ncentroids);
        Ctrl->Qcbits = Ctrl->Qbits;
    }

    if (Ctrl->OutFileName) {
        Ctrl->File = fopen(Ctrl->OutFileName, "w");
        if (Ctrl->File==0) {
            fprintf(stderr, "Cannot open output file.\n");
            fprintf(stderr, "Usage: %s [-s val] [-b val] [-B val] [-E val] [-N val] [-Q val] [-C val] [-S val] [-U] [-O name] [-i name] [-o name] [-c name]\n", argv[0]);
            UsageDetails();
            exit(EXIT_FAILURE);
        }
    } else Ctrl->File = stdout;
}


void ReportConfiguration(Control_T *Ctrl)

{
    fprintf(Ctrl->File, "/* Configuration\n");
    fprintf(Ctrl->File, "\tInit Seed   : %d\n", Ctrl->InitSeed);
    fprintf(Ctrl->File, "\tEncode Type : %d\n", Ctrl->EncodeType);
    fprintf(Ctrl->File, "\tQ bits      : %d\n", Ctrl->Qbits);
    fprintf(Ctrl->File, "\tStart bit   : %d\n", Ctrl->StartBit);
    fprintf(Ctrl->File, "\tStart byte  : %d\n", Ctrl->StartByte);
    fprintf(Ctrl->File, "\tN centroids : %d (%d bits)\n", Ctrl->Ncentroids, Ctrl->Qcbits);
    fprintf(Ctrl->File, "\tN items     : %d\n", Ctrl->Nitems);
    fprintf(Ctrl->File, "\tData Size   : %d\n", Ctrl->DataSize);
    fprintf(Ctrl->File, "\tUnsigned    : %s\n", (Ctrl->UnsignedData)?"Yes":"No");
    fprintf(Ctrl->File, "\tOut File    : %s\n", Ctrl->OutFileName);
    fprintf(Ctrl->File, "\tN out bits  : %d\n", Ctrl->OutBitSize);
    fprintf(Ctrl->File, "*/\n");
}

void MakeDriverConfiguration(Control_T *Ctrl)

{
    fprintf(Ctrl->File, "\n");
    fprintf(Ctrl->File, "pi_cl_dma_decompressor_cmd_t cmd = {\n");

    fprintf(Ctrl->File, "    .ext = (uint32_t) %s,\n", Ctrl->OutName);
    fprintf(Ctrl->File, "    .loc = (uint32_t) %s,\n", Ctrl->InName);
    fprintf(Ctrl->File, "    .conf.items_to_transfer = %d,\n", Ctrl->Nitems);
    switch(Ctrl->EncodeType)
    {
        case 0:
            fprintf(Ctrl->File, "    .conf.decompr_mode = PI_CL_DMA_DECOMPRESSOR_MODE_T1,\n");
            break;
        case 1:
            fprintf(Ctrl->File, "    .conf.decompr_mode = PI_CL_DMA_DECOMPRESSOR_MODE_T2,\n");
            break;
        case 2:
            fprintf(Ctrl->File, "    .conf.decompr_mode = PI_CL_DMA_DECOMPRESSOR_MODE_T3,\n");
            break;
        default:
            fprintf(stderr, "Invalid Encode type\n");
            exit(EXIT_FAILURE);
            break;

    }
    fprintf(Ctrl->File, "    .conf.decompr_direction = PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC,\n");
    fprintf(Ctrl->File, "    .transf_type = PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN,\n");

    switch(Ctrl->DataSize)
    {
        case 1:
            fprintf(Ctrl->File, "    .conf.extension_type = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_8,\n");
            break;
        case 2:
            fprintf(Ctrl->File, "    .conf.extension_type = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_16,\n");
            break;
        case 4:
            fprintf(Ctrl->File, "    .conf.extension_type = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_32,\n");
            break;
        default:
            fprintf(stderr, "Invalid DataSize\n");
            exit(EXIT_FAILURE);
            break;
    }

    fprintf(Ctrl->File, "    .conf.sign_extension = %s,\n", (Ctrl->UnsignedData)
                                                       ? "PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_UNSIGNED"
                                                       : "PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_SIGNED");

    fprintf(Ctrl->File, "    .conf.item_bit_width = %d,\n", Ctrl->Qbits);
    fprintf(Ctrl->File, "    .conf.start_bit = %d,\n", Ctrl->StartBit);
    fprintf(Ctrl->File, "    .conf.start_byte = %d,\n", Ctrl->StartByte);

    fprintf(Ctrl->File, "    .lut_size = %d,\n", Ctrl->Ncentroids);

    if (Ctrl->Ncentroids > 0)
    {
        fprintf(Ctrl->File, "    .lut = (void*) %s,\n", Ctrl->CentName);
        fprintf(Ctrl->File, "    .flag.update_lut = 1,\n");
    }
    else
    {
        fprintf(Ctrl->File, "    .lut = NULL,\n");
        fprintf(Ctrl->File, "    .flag.update_lut = 0,\n");
    }

    fprintf(Ctrl->File, "    .l2_2d.count = 0,\n");
    fprintf(Ctrl->File, "    .l2_2d.stride = 0,\n");
    fprintf(Ctrl->File, "    .tcdm_2d.count = 0,\n");
    fprintf(Ctrl->File, "    .tcdm_2d.stride = 0,\n");

    fprintf(Ctrl->File, "    .special_symbol = 0x%x,\n", SPECIAL_SYMBOL);
    fprintf(Ctrl->File, "    .done = 0,\n");

    fprintf(Ctrl->File, "    .flag.update_2d_ext = 0,\n");
    fprintf(Ctrl->File, "    .flag.update_2d_loc = 0,\n");

    fprintf(Ctrl->File, "};\n\n");
}

int main(int argc, char *argv[])

{
    Control_T Ctrl;
    int *IN;
    int *DECOUT;
    short int *CENT;
    unsigned int *OUT;
    int Err;

    ParseArguments(argc, argv, &Ctrl);

    Ctrl.InitSeed = PrimeRNG(Ctrl.InitSeed);
    Ctrl.OutBitSize = InitializeData((void **) &IN, &CENT, &Ctrl);

    EncodeData(IN, CENT, &OUT, &Ctrl);
    DecodeData(OUT, CENT, (void **) &DECOUT, &Ctrl);
    Err = Check(IN, DECOUT, &Ctrl);
    ReportConfiguration(&Ctrl);
    Dump2File(IN, CENT, OUT, &Ctrl);
    MakeDriverConfiguration(&Ctrl);

    if (Err)
    {
        fprintf(stderr, "Internal error, %d incorrect decoded output out of %d input items\n", Err, Ctrl.Nitems);
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Decode(Encode(In)) OK\n");
    }

    return 0;
}

