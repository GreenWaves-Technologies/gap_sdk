#ifndef __CNN_GENERATORS_H__
#define __CNN_GENERATORS_H__

extern void LoadCNNLibrary();

/*********************************************************************************************************************************************************************
    Generators for Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

    Template:
        Name:       Name of the generated user kernel

        In_DataSize:    1: byte, 2: half word, 4: word
        Filter_DataSize:1: byte, 2: half word, 4: word
        Bias_DataSize:  1: byte, 2: half word, 4: word
        Out_DataSize:   1: byte, 2: half word, 4: word

        In_InL3:    0: In is in L2, 1: In is in L3 memory
        Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
        Bias_InL3:  0: Bias is in L2, 1: Bias is in L3 memory
        Out_InL3:   0: Out is in L2, 1: Out is in L3 memory

        InFeat:     Number of input feature's maps
        OutFeat:    Number of output feature's maps
        Width:      Number of columns of a given feature map
        Height:     Number of lines of a given feature map

        FSc:        Dimension of the convolution (FSc x FSc)
        ConvStride: Stride between 2 points in the input feature map where convolution is evaluated
        ConvDoPad:  0: No padding, 1: Zero padding
        ConvDoReLU: Performs linear rectification after all convolutions for a given output feature's map have been evaluated

        FSp:        Size of the pooling region (FSp x FSp)
        PoolStride: Stride between 2 points in the input feature map where pooling is evaluated
        PoolDoPad:  0: No padding, 1: Zero padding
        PoolDoReLU: 0: No linear rectification, 1: Linear rectification after pooling
        DoPool:     0: No pooling, 1: Max pooling, 2: Average pooling

    Currently only homogeneous DataSize are supported for byte and half word

    CNN_LargeConvolutionPoolReLU_Hor
        For a given output feature's map all input feature's maps are traversed. A given input feature map is tiled and the evaluation of the tile is
        dispatched on all cores for parallel evaluation. Before starting the traversal of all input feature's maps bias is assigned to the output tile.
        Once all input feature's maps have been traversed the produced output tile is optionaly passed to a pooling layer and/or to a linear rectification layer.
        Note that with this approach all input featire's maps are traversed several time (OutFeat time).
        Tile orientation is horizontal.

    CNN_LargeConvolutionPoolReLU_Ver
        For a given output feature's map all input feature's maps are traversed. A given input feature map is tiled and the evaluation of the tile is
        dispatched on all cores for parallel evaluation. Before starting the traversal of all input feature's maps bias is assigned to the output tile.
        Once all input feature's maps have been traversed the produced output tile is optionaly passed to a pooling layer and/or to a linear rectification layer.
        Note that with this approach all input featire's maps are traversed several time (OutFeat time).
        Tile orientation is vertical.

    CNN_LargeParOutFeatConvolutionPoolReLU_Hor
        All output feature's maps are evaluated in parallel. A tile crosses the entire set of output feature's maps. Input feature's maps are traversed in sequence
        through tiles correspondingi, in the input maps, to the currently evalauted output feature map's tile and combined with corresponding filter (if filter does
        not fit into cluster's L1 then filter is tiled in the inner loop).
        Here input feature's maps are traversed only once therefore this generator is good when input features are in L3, the real constraints comes from the fact that the
        tile traversing all the input features must fit into cluster's L1.
        Tiling is horizontal.

    CNN_LargeParOutFeatConvolutionPoolReLU_Ver
        All output feature's maps are evaluated in parallel. A tile crosses the entire set of output feature's maps. Input feature's maps are traversed in sequence
        through tiles correspondingi, in the input maps, to the currently evalauted output feature map's tile and combined with corresponding filter (if filter does
        not fit into cluster's L1 then filter is tiled in the inner loop).
        Here input feature's maps are traversed only once therefore this generator is good when input features are in L3, the real constraints comes from the fact that the
        tile traversing all the input features must fit into cluster's L1.
        Tiling is vertical.

    CNN_SmallParOutFeatConvolutionPoolReLU
        Output feature's maps are evaluated in parallel, an output tile contains several full output feature's maps (prefered to be a multiple of number of cores for
        efficient parallelization), filter is the same iteration space. If cluster's L1 memory can accomodate the entire set of input feature's maps they are installed first,
        This generator is generally good when InFeat and OutFeat are large but Width and Height small (10 or bellow), if input can be promoted to buffer in cluster's L1
        then inputs are loaded only once, if not they will be loaded as many time as they are tiles in the output feature's maps (usually a small number).
        if not the set is tiled in the inner loop of the user's kernel, a tile contains a group of full input feature's maps.


*********************************************************************************************************************************************************************/

extern void CNN_LargeConvolutionPoolReLU_Hor(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

                        unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool
            );

extern void CNN_LargeConvolutionPoolReLU_Ver(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

                        unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool
            );

extern void CNN_LargeParOutFeatConvolutionPoolReLU_Hor(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

                        unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool
            );

extern void CNN_LargeParOutFeatConvolutionPoolReLU_Ver(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

                        unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool
            );


extern void CNN_SmallParOutFeatConvolutionPoolReLU(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,

            unsigned int InFeat,
            unsigned int OutFeat,
            unsigned int Width,
            unsigned int Height,

            unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool     // 0: NoPool, 1: Max Pool, 2: Average Pool
            );


/*********************************************************************************************************************************************************************
    Generators for DepthWise Convolutions, followed by an optional linear rectification (ReLU).

    Template:
        Name:       Name of the generated user kernel

        In_DataSize:    1: byte, 2: half word, 4: word
        Filter_DataSize:1: byte, 2: half word, 4: word
        Bias_DataSize:  1: byte, 2: half word, 4: word
        Out_DataSize:   1: byte, 2: half word, 4: word

        In_InL3:        0: In is in L2, 1: In is in L3 memory
        Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
        Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
        Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

        InOutFeat:  Number of input and output feature's maps
        Width:      Number of columns of a given feature map
        Height:     Number of lines of a given feature map

        FSc:        Dimension of the convolution (FSc x FSc)
        ConvStride: Stride between 2 points in the input feature map where convolution is evaluated
        ConvDoPad:  0: No padding, 1: Zero padding
        ConvDoReLU: Performs linear rectification after all convolutions for a given output feature's map have been evaluated

    Currently only homegeneous data size are supported (bytes and hald words)

    CNN_LargeDepthWiseConvolutionReLU_Hor
        A given input feature map is tiled and the evaluation of the tile is dispatched on all cores for parallel evaluation. 
        Before starting the traversal of all input feature's maps bias is assigned to the output tile.
        Tile orientation is horizontal.

    CNN_LargeDepthWiseConvolutionReLU_Ver
        A given input feature map is tiled and the evaluation of the tile is dispatched on all cores for parallel evaluation. 
        Before starting the traversal of all input feature's maps bias is assigned to the output tile.
        Tile orientation is vertical.

*********************************************************************************************************************************************************************/

void CNN_LargeDepthWiseConvolutionReLU_Hor(
            char         *Name,
    
            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,
    
            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,
    
            unsigned int InOutFeat,
            unsigned int Width,
            unsigned int Height,
    
            unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU
            );

void CNN_LargeDepthWiseConvolutionReLU_Ver(
            char         *Name,
    
            unsigned int In_DataSize,
            unsigned int Filter_DataSize,
            unsigned int Bias_DataSize,
            unsigned int Out_DataSize,
    
            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Filter_InL3,
            unsigned int Bias_InL3,
            unsigned int Out_InL3,
    
            unsigned int InOutFeat,
            unsigned int Width,
            unsigned int Height,
    
            unsigned int FSc,
            unsigned int ConvStride,
            int          ConvDoPad,
            int          ConvDoReLU
            );


/*********************************************************************************************************************************************************************
    Generators for Pooling (Max or Average) followed by an optional linear rectification (ReLU) or linear rectification only

    Template:
        Name:       Name of the generated user kernel

        In_DataSize:    1: byte, 2: half word, 4: word
        Out_DataSize:   1: byte, 2: half word, 4: word

        In_InL3:    0: In is in L2, 1: In is in L3 memory
        Out_InL3:   0: Out is in L2, 1: Out is in L3 memory

        InFeat:     Number of input feature's maps
        OutFeat:    Number of output feature's maps (InFeat has to be equal to OutFeat for these generators)
        Width:      Number of columns of a given feature map
        Height:     Number of lines of a given feature map

        FSp:        Size of the pooling region (FSp x FSp)
        PoolStride: Stride between 2 points in the input feature map where pooling is evaluated
        PoolDoPad:  0: No padding, 1: Zero padding
        PoolDoReLU: 0: No linear rectification, 1: Linear rectification. If DoPool=1 ReLU is performed after pooling otherwise only ReLU is performed.
        DoPool:     0: No pooling, 1: Max pooling, 2: Average pooling. If DoPool=0 then PoolDoReLU must be 1

    Currently only homogeneous DataSize are supported for byte and half word

    CNN_LargePoolReLU
        Input feature's maps are evaluated one after the other. A given feature map is tiled and the evaluation of a tile is dispatched on all cores for
        parallel evaluation. Tiling is horizontal.

    CNN_LargeParOutFeatPoolReLU
        Input feature's maps are processed as a group, a tile crosses all input feature maps and then each core is given a subset of input feature's maps.
        Tiling is horizontal.

    CNN_SmallParOutFeatPoolReLU:
        Input feature's maps are processed as a group but it is assumed than an entire feature maps can fit into cluster's L1 memory. A tile is a group of
        full input feature's maps, the size of the group is constrained to be a multiple of number of cores if possible.

*********************************************************************************************************************************************************************/

extern void CNN_LargePoolReLU(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool     // 0: NoPool, 1: Max Pool, 2: Average Pool
            );

extern void CNN_LargeParOutFeatPoolReLU(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool     // 0: NoPool, 1: Max Pool, 2: Average Pool
            );

extern void CNN_SmallParOutFeatPoolReLU(
            char         *Name,

            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,       // 1 if In comes from L3, 0 if it comes from L2
            unsigned int Out_InL3,

                        unsigned int InFeat,
                        unsigned int OutFeat,
                        unsigned int Width,
                        unsigned int Height,

            unsigned int FSp,
            unsigned int PoolStride,
            int          PoolDoPad,
            int          PoolDoReLU,

            int      DoPool     // 0: NoPool, 1: Max Pool, 2: Average Pool
            );


/*********************************************************************************************************************************************************************
    Generators for Addition followed by an optional linear rectification (ReLU)

    Template:
        Name:       Name of the generated user kernel

        In_DataSize:    1: byte, 2: half word, 4: word
        Out_DataSize:   1: byte, 2: half word, 4: word

        In_InL3:    0: In is in L2, 1: In is in L3 memory
        Out_InL3:   0: Out is in L2, 1: Out is in L3 memory

        InOutFeat:      Number of input and output feature's maps (for add must be equal)
        Width:      Number of columns of a given feature map
        Height:     Number of lines of a given feature map
        
        DoReLU: 0: No linear rectification, 1: Linear rectification. 

    Currently only homegeneous data size are supported (bytes and hald words)

    CNN_LargeAddReLU
        Input feature's maps are evaluated one after the other. A given feature map is tiled and the evaluation of a tile is dispatched on all cores for
        parallel evaluation. Tiling is horizontal.
    CNN_LargeParOutFeatAddReLU
        Input feature's maps are processed as a group, a tile crosses all input feature maps and then each core is given a subset of input feature's maps.
        Tiling is horizontal.
    CNN_SmallParOutFeatPoolReLU:
        Input feature's maps are processed as a group but it is assumed than an entire feature maps can fit into cluster's L1 memory. A tile is a group of
        full input feature's maps, the size of the group is constrained to be a multiple of number of cores if possible.



*********************************************************************************************************************************************************************/


extern void CNN_LargeAddReLU(
            char         *Name,
            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,
            unsigned int Out_InL3,

            unsigned int InOutFeat,
            unsigned int Width,
            unsigned int Height,

            int          DoReLU
            );

extern void CNN_LargeParOutFeatAddReLU(
            char         *Name,
            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,
            unsigned int Out_InL3,

            unsigned int InOutFeat,
            unsigned int Width,
            unsigned int Height,

            int          DoReLU
            );

extern void CNN_SmallParOutFeatAddReLU(
            char         *Name,
            unsigned int In_DataSize,
            unsigned int Out_DataSize,

            unsigned int In_InL3,
            unsigned int Out_InL3,

            unsigned int InOutFeat,
            unsigned int Width,
            unsigned int Height,

            int          DoReLU
            );

/*********************************************************************************************************************************************************************
    Generators for Linear layers followed by an optional linear rectification (ReLU)

    Template:
        Name:       Name of the generated user kernel

        In_DataSize:    1: byte, 2: half word, 4: word
        Filter_DataSize:1: byte, 2: half word, 4: word
        Bias_DataSize:  1: byte, 2: half word, 4: word
        Out_DataSize:   1: byte, 2: half word, 4: word

        In_InL3:    0: In is in L2, 1: In is in L3 memory
        Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
        Bias_InL3:  0: Bias is in L2, 1: Bias is in L3 memory
        Out_InL3:   0: Out is in L2, 1: Out is in L3 memory

        InDim:      Number of inputs
        OutDim:     Number of outputs

        DoReLU:     0: No linear rectification after linear layer, 1: Linear rectification applied after linear layer

    CNN_ParOutFeatLinearLayerReLU
        Input feature's maps are evaluated one after the other. A given feature map is tiled and the evaluation of a tile is dispatched on all cores for
        parallel evaluation. Tiling is horizontal.

    CNN_LinearLayerReLU
        Input feature's maps are processed as a group, a tile crosses all input feature maps and then each core is given a subset of input feature's maps.
        Tiling is horizontal.
*********************************************************************************************************************************************************************/

extern void CNN_ParOutFeatLinearLayerReLU(
    char *Name,

    unsigned int In_DataSize,
    unsigned int Filter_DataSize,
    unsigned int Bias_DataSize,
    unsigned int Out_DataSize,

    unsigned int In_InL3,
    unsigned int Filter_InL3,
    unsigned int Bias_InL3,
    unsigned int Out_InL3,

    unsigned int InDim,
    unsigned int OutDim,

    unsigned int DoReLU
    );

extern void CNN_LinearLayerReLU(
    char *Name,

    unsigned int In_DataSize,
    unsigned int Filter_DataSize,
    unsigned int Bias_DataSize,
    unsigned int Out_DataSize,

    unsigned int In_InL3,
    unsigned int Filter_InL3,
    unsigned int Bias_InL3,
    unsigned int Out_InL3,

    unsigned int InDim,
    unsigned int OutDim,

    unsigned int DoReLU
    );

/*********************************************************************************************************************************************************************
        Generators for SoftMax layers

        Template:
                Name:           Name of the generated user kernel

                In_DataSize:    1: byte, 2: half word,
                Out_DataSize:   1: byte, 2: half word

                In_InL3:        0: In is in L2, 1: In is in L3 memory
                Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

                Dim:            Number of inputs

        CNN_SoftMaxLayer
                Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to
                a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15
        if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)

*********************************************************************************************************************************************************************/

extern void CNN_SoftMaxLayer(
        char *Name,

        unsigned int In_DataSize,
        unsigned int Out_DataSize,

        unsigned int In_InL3,
        unsigned int Out_InL3,

        unsigned int Dim
        );


/** @brief Load CNN related HWCE accelerated basic kernels.

Load CNN related HWCE accelerated basic kernels.
*/
void CNN_LoadHWCEKernelLibrary();

/* @brief HWCE enabled Pure convolution, no bias setting and no accumulation.

HWCE enabled Pure convolution, no bias setting and no accumulation.
*/
void CNN_TiledPlainConvNxN_HWCE_fp(
    char *Name,         /**< Name of the generated tiled kernel */
    unsigned int FS,        /**< Convolution dimension, 3, 5, or 7 */
    unsigned int Width,     /**< Input plane width, Width should be even */
    unsigned int Height     /**< Input plane height */
    );

/* @brief HWCE enabled Convolution layer, bias setting and accumulation.

HWCE enabled Convolution layer, bias setting and accumulation.
*/
void CNN_TiledConvNxN_HWCE_fp(
    char *Name,         /**< Name of the generated tiled kernel */
    unsigned int FS,        /**< Convolution dimension, 3, 5 or 7*/
    unsigned int InPlane,       /**< Number of input planes */
    unsigned int OutPlane,      /**< Number of output planes */
    unsigned int Width,     /**< Input plane width, Width should be even */
    unsigned int Height     /**< Input plane height */
    );

/* @brief HWCE enabled Convolution layer, 3x3 convolution multiple output mode, bias setting and accumulation.

HWCE enabled Convolution layer, 3x3 convolution multiple output mode, bias setting and accumulation.
*/
void CNN_TiledConv3x3_HWCE_MultiOut_fp(
    char *Name,         /**< Name of the generated tiled kernel */
    unsigned int Nout,      /**< Number of output processed in parallel: 1, 2 or 3 */
    unsigned int InPlane,       /**< Number of input planes */
    unsigned int OutPlane,      /**< Number of output planes */
    unsigned int Width,     /**< Input plane width, Width should be even */
    unsigned int Height     /**< Input plane height */
    );

/* @brief HWCE enabled composite kernels: NxN convolution (3/5/7), ReLU and Max or Average Pooling 2x2 -> 1.

HWCE enabled composite kernels: NxN convolution (3/5/7), ReLU and Max or Average Pooling 2x2 -> 1.
*/
void CNN_TiledConvNxNReLUPool2x2_HWCE_fp(
    char *Name,         /**< Name of the generated tiled kernel */
    unsigned int FS,        /**< Convolution dimension, 3, 5 or 7 */
    unsigned int InPlane,       /**< Number of input planes */
    unsigned int OutPlane,      /**< Number of output planes */
    unsigned int Width,     /**< Input plane width, Width should be even */
    unsigned int Height,        /**< Input plane height */
    unsigned int PoolMax        /**< 0: ReLU then Average 2x2 pooling, 1: ReLU then Max 2x2 pooling, 2: Average 2x2 pooling, 3: Max 2x2 pooling. */
    );

/* @brief HWCE enabled composite kernel: 3x3 convolution, ReLU and Max Pooling 2x2 -> 1, multiple out mode.

HWCE enabled composite kernel: 3x3 convolution, ReLU and Max Pooling 2x2 -> 1, multiple out mode.
*/
void CNN_TiledConv3x3ReLUPool2x2_HWCE_MultiOut_fp(
    char *Name,         /**< Name of the generated tiled kernel */
    unsigned int Nout,      /**< Number of simultaneously produced outputs, 1, 2 or 3 */
        unsigned int InPlane,       /**< Number of input planes */
    unsigned int OutPlane,      /**< Number of output planes */
    unsigned int Width,     /**< Input plane width, Width should be even */
    unsigned int Height,        /**< Input plane height */
        unsigned int PoolMax        /**< 0: Average 2x2 pooling. 1: Max 2x2 pooling */
    );

#endif
