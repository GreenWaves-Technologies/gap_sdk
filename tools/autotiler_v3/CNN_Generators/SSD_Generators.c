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

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "SSD_Generators.h"
#include "SSD_BasicKernels.h"
//#include "AutoTilerLibTypes.h"
/*#include "CNN_Generators.h"
#include "CNN_Generator_Util.h"*/
#include "Gap.h"

void LoadSSDLibrary(){

	LibKernel("Ker_SSD_Init", CALL_SEQUENTIAL_STRUCT,
		CArgs(3,
			TCArg("int16_t *", "bbox_idx"),
			TCArg("bbox_t *" , "bbox_buf"),
			TCArg("int16_t " , "n_max_bb")
			),
		"Ker_SSD_Init_ArgT", NULL
	);
	LibKernel("Ker_SSD_Decoder", CALL_PARALLEL,
		CArgs(12,
			TCArg("int8_t * __restrict__", "boxes_in"  ),
			TCArg("int8_t * __restrict__", "classes_in"),
			TCArg("int8_t * __restrict__", "anchors_in"),
			TCArg("bbox_t *"	     , "bbox_buf"  ),
			TCArg("int16_t "	     , "Box_W"	   ),
			TCArg("int16_t "	     , "Class_W"   ),
			TCArg("int16_t "	     , "H"	   ),
			TCArg("uint8_t *"	     , "in_scales" ),
			TCArg("uint8_t *"	     , "in_norms"  ),
			TCArg("int"	             , "ScoreThr"  ),
			TCArg("int16_t "	     , "n_max_bb"  ),
			TCArg("int16_t *"	     , "bbox_idx"  )
			),
		"Ker_SSD_Decoder_ArgT", NULL
	);

	LibKernel("Ker_SSD_NMS", CALL_SEQUENTIAL_STRUCT,
		CArgs(8,
			TCArg("bbox_t *" , "bbox_buf"  ),
			TCArg("int16_t"  , "n_out_box" ),
			TCArg("int16_t *", "bbox_out"  ),
			TCArg("int8_t *" , "scores_out"),
			TCArg("int8_t *" , "class_out" ),
                        TCArg("int"      , "NMSThr"    ),
			TCArg("int16_t " , "n_max_bb"  ),
			TCArg("int16_t *", "bbox_idx"  )
			),
		"Ker_SSD_NMS_ArgT", NULL
	);

}

// Q and info parameters
//uint8_t SSD_Inscales = [scale_x, scale_x_anc, scale_y, scale_y_anc, scale_h, scale_w, scale_ao, scale_score] uint8
//uint8_t norms		= [norm_x, norm_x_anc, norm_y, norm_y_anc, norm_h, norm_w, norm_ao, norm_score] uint8
//infos  = [IOU threshold (Q7), score threshold (Q of output), max detection (Q0), max detection per class (Q0)]
int CNN_SSD_PostProcess_SQ8(
        char *Name,
        CNN_GenControl_T *Ctrl,
        int n_anchors,
        int n_classes,
        int n_outbox,
        int max_bb_before_nn_max,
        float DecScoreThr,
        float NMSThr)
{

        int64_t LayerOp = max_bb_before_nn_max * 8 + max_bb_before_nn_max*max_bb_before_nn_max;
        int64_t LayerBandwidth = n_anchors*(n_classes + 4 + 4) + n_outbox*(4+2);
	Kernel_T *Kernel;
   	Kernel = UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, 1),IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(8,
                        TCArg("int8_t * __restrict__", "boxes_in"  ),
                        TCArg("int8_t * __restrict__", "classes_in"),
                        TCArg("int8_t * __restrict__", "anchors_in"),
                        TCArg("int16_t *"	     , "bbox_out"  ),
                        TCArg("int8_t *"             , "class_out" ),
                        TCArg("int8_t *"             , "scores_out"),
                        TCArg("uint8_t * "           , "in_scales" ),
                        TCArg("int8_t * "            , "in_norms"  )
                        ),
                Calls(3,
                        Call("Ker_SSD_Init", LOC_LOOP_PROLOG,
                                Bindings(3,
                                        K_Arg("bbox_idx",KER_ARG),
                                        K_Arg("bbox_buf",KER_ARG),
                                        Imm(max_bb_before_nn_max)
                                        )
                                ),
                        Call("Ker_SSD_Decoder", LOC_LOOP,
                                Bindings(12,
                                        K_Arg("boxes_in"  , KER_ARG_TILE  ),
                                        K_Arg("classes_in", KER_ARG_TILE  ),
                                        K_Arg("anchors_in", KER_ARG_TILE  ),
                                        K_Arg("bbox_buf"  , KER_ARG       ),
                                        K_Arg("boxes_in"  , KER_ARG_TILE_W),
                                        K_Arg("classes_in", KER_ARG_TILE_W),
                                        K_Arg("classes_in", KER_ARG_TILE_H),
                                        K_Arg("in_scales" , KER_ARG       ),
                                        K_Arg("in_norms"  , KER_ARG       ),
                                        Imm(FP2FIX(DecScoreThr, 7)        ),
                                        Imm(max_bb_before_nn_max          ),
                                        K_Arg("bbox_idx"  , KER_ARG       )
                                        )
                                ),
                        Call("Ker_SSD_NMS", LOC_LOOP_EPILOG,
                                Bindings(8,
                                        K_Arg("bbox_buf",   KER_ARG),
                                        Imm(n_outbox               ),
                                        K_Arg("bbox_out",   KER_ARG),
                                        K_Arg("scores_out", KER_ARG),
                                        K_Arg("class_out",  KER_ARG),
                                        Imm(FP2FIX(NMSThr, 7)      ),
                                        Imm(max_bb_before_nn_max   ),
                                        K_Arg("bbox_idx",   KER_ARG)
                                        )
                                )
                ),
		KerArgs(10,
                        KerArg("boxes_in"  , KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , 4                   , n_anchors, sizeof(int8_t) , 0, 0, 0, "boxes_in"   ),
			KerArg("classes_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , n_classes           , n_anchors, sizeof(int8_t) , 0, 0, 0, "classes_in" ),
			KerArg("anchors_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_CONST|O_DB    , 4                   , n_anchors, sizeof(int8_t) , 0, 0, 0, "anchors_in" ),
			KerArg("bbox_buf",   KerArgSpace(1,KER_ITER_D0)   , O_BUFF|O_NDB|O_NTILED, max_bb_before_nn_max, 1        , sizeof(bbox_t) , 0, 0, 0, ""           ),
                        KerArg("bbox_out",   KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 4        , sizeof(int16_t), 0, 0, 0, "bbox_out"   ),
                        KerArg("scores_out", KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 1        , sizeof(int8_t) , 0, 0, 0, "scores_out" ),
                        KerArg("class_out",  KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 1        , sizeof(int8_t) , 0, 0, 0, "class_out"  ),
			KerArg("bbox_idx"  , KerArgSpace(1,KER_ITER_D0)   , O_BUFF|O_NDB|O_NTILED, 1                   , 1        , sizeof(int16_t), 0, 0, 0, ""           ),
                        KerArg("in_scales" , KerArgSpace(1,KER_ITER_D0)   , O_IN|O_CONST         , 8                   , 1        , sizeof(uint8_t), 0, 0, 0, "in_scales"  ),
			KerArg("in_norms"  , KerArgSpace(1,KER_ITER_D0)   , O_IN|O_CONST         , 8                   , 1        , sizeof(uint8_t), 0, 0, 0, "in_norms"   )
                )
        );

        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "boxes_in"  , 3, 4,                    n_anchors, 1);
                AddKernelArgDim(Name, "classes_in", 3, n_classes,            n_anchors, 1);
                AddKernelArgDim(Name, "anchors_in", 3, 4,                    n_anchors, 1);
                AddKernelArgDim(Name, "bbox_out"  , 3, 4,                    n_outbox,  2);
                AddKernelArgDim(Name, "scores_out", 3, 1,                    n_outbox,  1);
                AddKernelArgDim(Name, "class_out" , 3, 1,                    n_outbox,  1);
                AddKernelArgDim(Name, "in_scales" , 3, 8,                    1,         1);
                AddKernelArgDim(Name, "in_norms"  , 3, 8,                    1,         1);
        }
        return (Kernel!=0);
}
