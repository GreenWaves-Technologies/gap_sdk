#include <stdint.h>
#include <stdio.h>

// AutoTiler Libraries
#include "AutoTilerLib.h"

void LoadLibKernels(){
    LibKernel("KerSDD3Dto2DShort", CALL_PARALLEL,
        CArgs(8,
            TCArg("short * __restrict__", "In"),
            TCArg("unsigned int ",        "Win"),
            TCArg("unsigned int ",        "Hin"),
            TCArg("short * __restrict__", "Out"),
            TCArg("unsigned int ",        "Wout"),
            TCArg("unsigned int ",        "Hout"),
            TCArg("unsigned char",        "Q"),
            TCArg("unsigned short",       "n_classes")
            ),
        "KerSDD3Dto2DShort_ArgT", NULL
    );

    LibKernel("KerPredecoderShort", CALL_PARALLEL,
        CArgs(10,
            TCArg("short * __restrict__", "Classes"),
            TCArg("short * __restrict__", "Boxes"),
            TCArg("unsigned int ",        "Classes_W"),
            TCArg("unsigned int ",        "Classes_H"),
            TCArg("unsigned int ",        "Classes_TileIndex"),
            TCArg("unsigned int ",        "Classes_Std_H"),
            TCArg("void  * __restrict__", "Ancor_layer"),
            TCArg("void  * __restrict__", "BoundingBoxes"),
            TCArg("int",                  "Boxes_Q"),
            TCArg("unsigned int ",        "n_classes")
            ),
        "KerPredecoderShort_ArgT", NULL
    );

}

void GenerateSSD3Dto2D(char *Name, int Wi, int Hi, int Fi, int classes, int DoSoftmax)
{
    //DoSoftmax argument is not used, we choose at runtime

    UserKernel(Name,
        KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
        TILE_HOR,
        CArgs(4, 
            TCArg("short *", "In"), 
            TCArg("short *", "Out"),
            TCArg("unsigned char", "Q"),
            TCArg("unsigned short", "n_classes")
        ),
        
        Calls(1, Call("KerSDD3Dto2DShort", LOC_LOOP,
            
            Bindings(8, K_Arg("In", KER_ARG_TILE),
                        K_Arg("In", KER_ARG_TILE_W),
                        K_Arg("In", KER_ARG_TILE_H),
                        K_Arg("Out", KER_ARG_TILE),
                        K_Arg("Out", KER_ARG_TILE_W),
                        K_Arg("Out", KER_ARG_TILE_H),
                        C_Arg("Q"),
                        C_Arg("n_classes")
                        ))),
        KerArgs(2,
            KerArg("In",  KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB_L2DB,  Wi*Hi, Fi, sizeof(short), 0, OBJ_CONSTRAINTS_TILE_VER, 0, "In"),
            KerArg("Out", KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB_L2DB, Fi, Wi*Hi,sizeof(short), 0, 0, 0, "Out")
        )
    );
}

void GeneratePredecoder(char *Name, int FeaturesMap_W, int FeaturesMap_H, int n_ancors, int offsets,int classes)
{   

    //Classes can be used to contrain the input
    UserKernel(Name,
        KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
        TILE_HOR,
        CArgs(5, 
            TCArg("short *", "Classes"), 
            TCArg("short *", "Boxes"),
            TCArg("void  *", "Ancor_layer"),
            TCArg("void  *", "BoundingBoxes"),
            TCArg("int"    , "Boxes_Q")
        ),

        Calls(1, Call("KerPredecoderShort", LOC_LOOP,
            Bindings(10, K_Arg("Classes",    KER_ARG_TILE),
                        K_Arg("Boxes",      KER_ARG_TILE),
                        K_Arg("Classes",    KER_ARG_TILE_W),
                        K_Arg("Classes",    KER_ARG_TILE_H),
                        K_Arg("Classes",    KER_ARG_TILEINDEX),
                        K_Arg("Classes",    KER_ARG_TILE_H0),
                        C_Arg("Ancor_layer"),
                        C_Arg("BoundingBoxes"),
                        C_Arg("Boxes_Q"),
                        Imm(classes)
                        ))),
        KerArgs(2,
            KerArg("Classes", KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB_L2DB, classes, FeaturesMap_W * FeaturesMap_H*n_ancors, sizeof(short), 0, 0, 0, "Classes"),
            KerArg("Boxes",   KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB_L2DB, offsets, FeaturesMap_W * FeaturesMap_H*n_ancors, sizeof(short), 0, 0, 0, "Boxes")
        )
    );
}




int main(int argc, char **argv)
{
    // This will parse AutoTiler options and perform various initializations
    if (TilerParseOptions(argc, argv))
    {
        printf("Failed to initialize or incorrect output arguments directory.\n");
        return 1;
    }

    int n_classes = 2;
    int n_offsets = 4;
    int n_anchors;

    int L1Memory = 38000;
    int L2Memory = 100000;
    int L3Memory = 1024*1024;


    SetInlineMode(ALWAYS_INLINE);
    SetSymbolNames("SSDKernels_L1_Memory", "SSDKernels_L2_Memory");
    SetSymbolDynamics();
    SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

    SetUsedFilesNames(0, 1, "SSDBasicKernels.h");

    SetGeneratedFilesNames("SSDKernels.c", "SSDKernels.h");

    SetL1MemorySize(L1Memory);
    SetMemorySizes(L1Memory,L2Memory,L3Memory);

    SetMemoryDeviceInfos(3,
        AT_MEM_L1, L1Memory, "SSDKernels_L1_Memory", 0, 0,
        AT_MEM_L2, L2Memory, "SSDKernels_L2_Memory", 0, 0,
        AT_MEM_L3_HRAM, L3Memory, "SSDKernels_L3_Memory", 0, 1
    );


    LoadLibKernels();

    n_anchors = 4;
    GenerateSSD3Dto2D("SDD3Dto2DSoftmax_80_60_12", 80, 60, 8, n_classes,1);
    GenerateSSD3Dto2D("SDD3Dto2D_80_60_24",        80, 60, 16, n_classes,0);
    GeneratePredecoder("Predecoder80_60",          80, 60, n_anchors, n_offsets, n_classes);
    
    GenerateSSD3Dto2D("SDD3Dto2DSoftmax_40_30_14", 40, 30, 8, n_classes,1);
    GenerateSSD3Dto2D("SDD3Dto2D_40_30_28",        40, 30, 16, n_classes,0);
    GeneratePredecoder("Predecoder40_30",          40, 30, n_anchors, n_offsets, n_classes);
    
    GenerateSSD3Dto2D("SDD3Dto2DSoftmax_20_15_16", 20, 15, 8, n_classes,1);
    GenerateSSD3Dto2D("SDD3Dto2D_20_15_32",        20, 15, 16, n_classes,0);
    GeneratePredecoder("Predecoder20_15",          20, 15, n_anchors, n_offsets, n_classes);

    // Now that we are done with model parsing we generate the code
    GenerateTilingCode();
    return 0;
}
