#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"

void MnistModel(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{

    KernelOper_T Cop = KOP_CONV_DP;

    SetSymbolDynamics();

    SetUsedFilesNames(0, 2, "CNN_BasicKernels.h", "MnistKernels.h");
    SetGeneratedFilesNames("MnistKernels.c", "MnistKernels.h");
    SetMemoryDeviceInfos(4,
        AT_MEM_L1, L1Memory, "Mnist_L1_Memory", 0, 0,
        AT_MEM_L2, L2Memory, "Mnist_L2_Memory", 0, 0,
        AT_MEM_L3_HRAM, L3Memory, "Mnist_L3_Memory", 0, 0,
        AT_MEM_L3_HFLASH, L3Flash, "0", "Mnist_L3_Flash_Const.dat", 0
    );
    
    LoadCNNLibrary();
    //Convolutional Layer
    CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_0", 0, 2,2,2,2, 12,14,14,12, 1,1,1,1, 1,32,28,28,
        Cop, 5, 5, 1, 1, 1, 1, 0,
        KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);
    //Convolutional Layer
    CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_1", 0, 2,2,2,2, 12,14,14,12, 1,1,1,1, 32,64,12,12,
        Cop, 5, 5, 1, 1, 1, 1, 0,
        KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);
    //Linear Layer 
    CNN_LinearReLU("LinearLayerReLU_0", 0, 2,2,2,2, 12,12,8,11, 2,2,2,2, 1024, 10,
        KOP_LINEAR, KOP_NONE);
    //Sofmax
    CNN_SoftMax("SoftMax_0", 0, 2,2, 15,15,1,1, 10, KOP_SOFTMAX);

#define GRAPH
#ifdef GRAPH
    //Open Graph Creation
    CreateGraph("MnistCNN",
        /* Arguments either passed or globals */
        //Here goes input, output and layers parameters(weights and biases)
        //ConstInfo function takes as input a tensor in CxHxW format and create a single binary file charged in gap8 flash
        //It can take in input float and automaticcaly convert in fixed point format
        CArgs(8,
            TCArgInfo("short int *__restrict__", "Input0", ARG_SCOPE_ARG, ARG_DIR_IN, AT_MEM_L2, AT_MEM_UNDEF, 0),
            TCArgInfo("short int *__restrict__", "Step1Weights", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step1Weights.tensor", 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Step1Biases",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step1Biases.tensor"  , 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Step2Weights", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step2Weights.tensor", 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Step2Biases",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step2Biases.tensor"  , 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Step3Weights", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step3Weights.tensor", 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Step3Biases",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, AT_MEM_L3_HFLASH, AT_MEM_UNDEF, ConstInfo("model/Step3Biases.tensor"  , 1, 1, 16, 0)),
            TCArgInfo("short int *__restrict__", "Output0", ARG_SCOPE_ARG, ARG_DIR_OUT, AT_MEM_UNDEF, AT_MEM_L2, 0)
        ),
        //Graph internal buffer between layers
        /* Locals, allocated dynamically */
        CArgs(3,
            TCArgInfo("short int *__restrict__", "OutputStep2", ARG_SCOPE_LOCAL, ARG_DIR_INOUT, AT_MEM_UNDEF, AT_MEM_UNDEF, 0),
            TCArgInfo("short int *__restrict__", "OutputStep3", ARG_SCOPE_LOCAL, ARG_DIR_INOUT, AT_MEM_UNDEF, AT_MEM_UNDEF, 0),
            TCArgInfo("short int *__restrict__", "OutputStep4", ARG_SCOPE_LOCAL, ARG_DIR_INOUT, AT_MEM_UNDEF, AT_MEM_UNDEF, 0)
        )
    );
    //Node Connections with arguments
    AddNode("Conv5x5ReLUMaxPool2x2_0", //Name of the Generated Layer
            Bindings(4,                //Number of parameters that genereted layers has and that needs to be connected
                                                        //void Conv5x5ReLUMaxPool2x2_0(
                GNodeArg(GNA_IN, "Input0", 0),          //short int * __restrict__ In,
                GNodeArg(GNA_IN, "Step1Weights", 0),    //short int * __restrict__ Filter,
                GNodeArg(GNA_IN, "Step1Biases", 0),     //short int * __restrict__ Bias,
                GNodeArg(GNA_OUT, "OutputStep2", 0)    //short int * __restrict__ Out
                )
            );

    AddNode("Conv5x5ReLUMaxPool2x2_1", Bindings(4, GNodeArg(GNA_IN, "OutputStep2", 0), GNodeArg(GNA_IN, "Step2Weights", 0), GNodeArg(GNA_IN, "Step2Biases", 0), GNodeArg(GNA_OUT, "OutputStep3",0)));
    AddNode("LinearLayerReLU_0", Bindings(4, GNodeArg(GNA_IN, "OutputStep3", 0), GNodeArg(GNA_IN, "Step3Weights", 0), GNodeArg(GNA_IN, "Step3Biases", 0), GNodeArg(GNA_OUT, "OutputStep4",0)));
    AddNode("SoftMax_0", Bindings(2, GNodeArg(GNA_IN, "OutputStep4", 0), GNodeArg(GNA_OUT, "Output0", 0)));
    //Close Graph creation
    CloseGraph();
#endif
}

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) {
        printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
    }
    MnistModel(48000, 300*1024, 8*1024*1024, 20*1024*1024);
    GenerateTilingCode();
    return 0;
}
