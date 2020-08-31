#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "SSD_BasicKernels.h"
//#include "AutoTilerLibTypes.h"
/*#include "CNN_Generators.h"
#include "CNN_Generator_Util.h"*/
#include "Gap.h"

void LoadSSDLibrary(){

    LibKernel("Ker_SSD_Init", CALL_SEQUENTIAL_STRUCT,
        CArgs(1,
            TCArg("int16_t *", "bbox_idx")
            ),
        "Ker_SSD_Init_ArgT", NULL
    );
    LibKernel("Ker_SSD_Decoder", CALL_PARALLEL,
        CArgs(12,
        	TCArg("int8_t * __restrict__", "boxes_in"  ),
            TCArg("int8_t * __restrict__", "classes_in"),
            TCArg("int8_t * __restrict__", "anchors_in"),
            TCArg("bbox_t *"             , "bbox_out"  ),
            TCArg("int16_t "             , "Box_W"     ),
            TCArg("int16_t "             , "Class_W"   ),
            TCArg("int16_t "             , "H"         ),
            TCArg("uint8_t *"            , "in_scales" ),
            TCArg("uint8_t *"            , "in_norms"  ),
            TCArg("int8_t *"             , "infos"     ),
            TCArg("int16_t "             , "n_max_bb"  ),
            TCArg("int16_t *"            , "bbox_idx"  )
            ),
        "Ker_SSD_Decoder_ArgT", NULL
    );

    LibKernel("Ker_SSD_NMS", CALL_SEQUENTIAL_STRUCT,
        CArgs(4,
			TCArg("bbox_t *" , "bbox_out"),
			TCArg("int8_t *" , "infos"   ),
            TCArg("int16_t " , "n_max_bb"),
            TCArg("int16_t *", "bbox_idx")
            ),
        "Ker_SSD_NMS_ArgT", NULL
    );

}

//CNN_SSD_PostProcess_SQ8("S74_Op_TFLite_Detection_PostProcess_0_63", 0, 1917, 91, 10);
// n_anchors, n_classes, n_outbox
//    	AddNode("S74_Op_TFLite_Detection_PostProcess_0_63", 
//		Bindings(6, 
//			GNodeArg(GNA_IN, "S67_Output", 0), 
//			GNodeArg(GNA_IN, "S72_Output", 0), 
//			GNodeArg(GNA_IN, "S73_Anchors", 0), 
//			GNodeArg(GNA_OUT, "Output_1", 0), 
//			GNodeArg(GNA_IN, "S74_Ssd_scales", 0), 
//			GNodeArg(GNA_IN, "S74_Ssd_norms", 0)));

/*AddNode("S75_Op_TFLite_Detection_PostProcess_0_63", 
        Bindings(7, 
            GNodeArg(GNA_IN, "S68_Output", 0), 
            GNodeArg(GNA_IN, "S73_Output", 0), 
            GNodeArg(GNA_IN, "S74_Anchors", 0), 
            GNodeArg(GNA_OUT, "Output_1", 0), 
            GNodeArg(GNA_IN, "S75_Ssd_scales", 0), 
            GNodeArg(GNA_IN, "S75_Ssd_norms", 0), 
            GNodeArg(GNA_IN, "S75_Infos", 0)));

*/
// Q and info parameters
//uint8_t SSD_Inscales = [scale_x, scale_x_anc, scale_y, scale_y_anc, scale_h, scale_w, scale_ao, scale_score] uint8
//uint8_t norms        = [norm_x, norm_x_anc, norm_y, norm_y_anc, norm_h, norm_w, norm_ao, norm_score] uint8
//infos  = [IOU threshold (Q7), score threshold (Q of output), max detection (Q0), max detection per class (Q0)]


void CNN_SSD_PostProcess_SQ8(char *Name, CNN_GenControl_T *Ctrl, int n_anchors, int n_classes, int n_outbox, int max_bb_before_nn_max)
{

    Kernel_T *Kernel;
   	Kernel = UserKernel(Name,
        KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, 1),IterTiledSpace(KER_ITER_TILE0)),
        TILE_HOR,
        CArgs(7,
          TCArg("int8_t * __restrict__", "boxes_in"  ),
          TCArg("int8_t * __restrict__", "classes_in"),
          TCArg("int8_t * __restrict__", "anchors_in"),
          TCArg("bbox_t * "            , "bbox_out"  ),
          TCArg("uint8_t * "           , "in_scales" ),
          TCArg("uint8_t * "           , "in_norms"  ),
          TCArg("int8_t * "            , "infos"     )
        ),
        Calls(3,
            Call("Ker_SSD_Init", LOC_LOOP_PROLOG,
                Bindings(1,
                    K_Arg("bbox_idx",KER_ARG)
                )
            ),
            Call("Ker_SSD_Decoder", LOC_LOOP,
                Bindings(12,
                    K_Arg("boxes_in"  , KER_ARG_TILE  ),
                    K_Arg("classes_in", KER_ARG_TILE  ),
                    K_Arg("anchors_in", KER_ARG_TILE  ),
                    K_Arg("bbox_out"  , KER_ARG       ),
                    K_Arg("boxes_in"  , KER_ARG_TILE_W),
                    K_Arg("classes_in", KER_ARG_TILE_W),
                    K_Arg("classes_in", KER_ARG_TILE_H),
                    K_Arg("in_scales" , KER_ARG       ),
                    K_Arg("in_norms"  , KER_ARG       ),
                    K_Arg("infos"     , KER_ARG       ),
                    Imm(max_bb_before_nn_max          ),
                    K_Arg("bbox_idx"  , KER_ARG       )
                )
            ),
            Call("Ker_SSD_NMS", LOC_LOOP_EPILOG,
                Bindings(4,
                    K_Arg("bbox_out", KER_ARG),
                    K_Arg("infos",KER_ARG),
                    Imm(max_bb_before_nn_max),
                    K_Arg("bbox_idx",KER_ARG)
                )
            )
        ),
		KerArgs(8,
			KerArg("boxes_in"  , KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , 4                   , n_anchors, sizeof(int8_t) , 0, 0, 0, "boxes_in"   ),
			KerArg("classes_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , n_classes           , n_anchors, sizeof(int8_t) , 0, 0, 0, "classes_in" ),
			KerArg("anchors_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_CONST|O_DB    , 4                   , n_anchors, sizeof(int8_t) , 0, 0, 0, "anchors_in" ),
			KerArg("bbox_out"  , KerArgSpace(1,KER_ITER_D0)   , O_OUT                , max_bb_before_nn_max, 1        , sizeof(bbox_t) , 0, 0, 0, "bbox_out"   ),
			KerArg("bbox_idx"  , KerArgSpace(1,KER_ITER_D0)   , O_BUFF|O_NDB|O_NTILED, 1                   , 1        , sizeof(int16_t), 0, 0, 0, ""           ),
            KerArg("in_scales" , KerArgSpace(1,KER_ITER_D0)   , O_IN|O_CONST         , 8                   , 1        , sizeof(uint8_t), 0, 0, 0, "in_scales"  ),
			KerArg("in_norms"  , KerArgSpace(1,KER_ITER_D0)   , O_IN|O_CONST         , 8                   , 1        , sizeof(uint8_t), 0, 0, 0, "in_norms"   ),
            KerArg("infos"     , KerArgSpace(1,KER_ITER_D0)   , O_IN|O_CONST         , 5                   , 1        , sizeof(int8_t) , 0, 0, 0, "infos"      )
        )
    );

    if (Kernel) {
        //AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
        //AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

        AddKernelArgDim(Name, "boxes_in"  , 3, 4,                    n_anchors, 1);
        AddKernelArgDim(Name, "classes_in", 3, n_classes,            n_anchors, 1);
        AddKernelArgDim(Name, "anchors_in", 3, 4,                    n_anchors, 1);
        AddKernelArgDim(Name, "bbox_out"  , 3, 4,                    n_outbox,  2);
        AddKernelArgDim(Name, "in_scales" , 3, 8,                    1,         1);
        AddKernelArgDim(Name, "in_norms"  , 3, 8,                    1,         1);
        AddKernelArgDim(Name, "infos"     , 3, 5,                    1,         1);
    }
    return (Kernel!=0);
}
