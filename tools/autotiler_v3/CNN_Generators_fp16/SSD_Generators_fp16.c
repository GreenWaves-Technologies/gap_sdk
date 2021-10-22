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
#include "SSD_Generators_fp16.h"
#include "Gap.h"

#define SIZE_OF_BBOX_F16_T 12

void LoadSSDLibrary_fp16(){

	LibKernel("Ker_SSD_Init_f16", CALL_SEQUENTIAL_STRUCT,
		CArgs(3,
			TCArg("short int *" , "bbox_idx"),
			TCArg("bbox_f16_t *", "bbox_buf"),
			TCArg("short int"   , "n_max_bb")
			),
		"Ker_SSD_Init_Arg_f16_T", NULL
	);
	LibKernel("Ker_SSD_Decoder_f16", CALL_PARALLEL,
		CArgs(9,
			TCArg("F16 * __restrict__", "boxes_in"  ),
			TCArg("F16 * __restrict__", "classes_in"),
			TCArg("F16 * __restrict__", "anchors_in"),
			TCArg("bbox_f16_t *"	  , "bbox_buf"  ),
			TCArg("short int "        , "N_Classes" ),
			TCArg("short int "	  , "N_Anchors" ),
			TCArg("short int "	  , "n_max_bb"  ),
                        TCArg("F16 "              , "ScoreThr"  ),
			TCArg("short int *"	  , "bbox_idx"  )
			),
		"Ker_SSD_Decoder_Arg_f16_T", NULL
	);

	LibKernel("Ker_SSD_NMS_f16", CALL_SEQUENTIAL_STRUCT,
		CArgs(8,
			TCArg("bbox_f16_t *", "bbox_buf"  ),
			TCArg("short int"   , "n_out_box" ),
			TCArg("F16 *"       , "bbox_out"  ),
			TCArg("F16 *"       , "scores_out"),
			TCArg("F16 *"       , "class_out" ),
			TCArg("short int "  , "n_max_bb"  ),
                        TCArg("F16 "        , "NMSThr"  ),
			TCArg("short int *" , "bbox_idx"  )
			),
		"Ker_SSD_NMS_Arg_f16_T", NULL
	);

}

// Q and info parameters
//uint8_t SSD_Inscales = [scale_x, scale_x_anc, scale_y, scale_y_anc, scale_h, scale_w, scale_ao, scale_score] uint8
//uint8_t norms		= [norm_x, norm_x_anc, norm_y, norm_y_anc, norm_h, norm_w, norm_ao, norm_score] uint8
//infos  = [IOU threshold (Q7), score threshold (Q of output), max detection (Q0), max detection per class (Q0)]
int CNN_SSD_PostProcess_fp16(
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
        UserSymbols(2, US_Float("DecScoreThr", DecScoreThr), US_Float("NMSThr", NMSThr));
   	Kernel = UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, 1),IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(6,
                        TCArg("F16 * __restrict__", "boxes_in"  ),
                        TCArg("F16 * __restrict__", "classes_in"),
                        TCArg("F16 * __restrict__", "anchors_in"),
                        TCArg("F16 *"	          , "bbox_out"  ),
                        TCArg("F16 *"             , "class_out" ),
                        TCArg("F16 *"             , "scores_out")
                        ),
                Calls(3,
                        Call("Ker_SSD_Init_f16", LOC_LOOP_PROLOG,
                                Bindings(3,
                                        K_Arg("bbox_idx",KER_ARG),
                                        K_Arg("bbox_buf",KER_ARG),
                                        Imm(max_bb_before_nn_max)
                                        )
                                ),
                        Call("Ker_SSD_Decoder_f16", LOC_LOOP,
                                Bindings(9,
                                        K_Arg("boxes_in"  , KER_ARG_TILE  ),
                                        K_Arg("classes_in", KER_ARG_TILE  ),
                                        K_Arg("anchors_in", KER_ARG_TILE  ),
                                        K_Arg("bbox_buf"  , KER_ARG       ),
                                        K_Arg("classes_in", KER_ARG_TILE_W),
                                        K_Arg("classes_in", KER_ARG_TILE_H),
                                        Imm(max_bb_before_nn_max          ),
                                        BindKExpr("DecScoreThr"           ),
                                        K_Arg("bbox_idx"  , KER_ARG       )
                                        )
                                ),
                        Call("Ker_SSD_NMS_f16", LOC_LOOP_EPILOG,
                                Bindings(8,
                                        K_Arg("bbox_buf",   KER_ARG),
                                        Imm(n_outbox               ),
                                        K_Arg("bbox_out",   KER_ARG),
                                        K_Arg("scores_out", KER_ARG),
                                        K_Arg("class_out",  KER_ARG),
                                        Imm(max_bb_before_nn_max   ),
                                        BindKExpr("NMSThr"         ),
                                        K_Arg("bbox_idx",   KER_ARG)
                                        )
                                )
                ),
		KerArgs(8,
                        KerArg("boxes_in"  , KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , 4                   , n_anchors, sizeof(short) , 0, 0, 0, "boxes_in"   ),
			KerArg("classes_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_DB            , n_classes           , n_anchors, sizeof(short) , 0, 0, 0, "classes_in" ),
			KerArg("anchors_in", KerArgSpace(1,KER_ITER_TILE0), O_IN|O_CONST|O_DB    , 4                   , n_anchors, sizeof(short) , 0, 0, 0, "anchors_in" ),
			KerArg("bbox_buf",   KerArgSpace(1,KER_ITER_D0)   , O_BUFF|O_NDB|O_NTILED, max_bb_before_nn_max, 1        , SIZE_OF_BBOX_F16_T,0,0,0,""           ),
                        KerArg("bbox_out",   KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 4        , sizeof(short) , 0, 0, 0, "bbox_out"   ),
                        KerArg("scores_out", KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 1        , sizeof(short) , 0, 0, 0, "scores_out" ),
                        KerArg("class_out",  KerArgSpace(1,KER_ITER_D0)   , O_OUT                , n_outbox            , 1        , sizeof(short) , 0, 0, 0, "class_out"  ),
			KerArg("bbox_idx"  , KerArgSpace(1,KER_ITER_D0)   , O_BUFF|O_NDB|O_NTILED, 1                   , 1        , sizeof(short) , 0, 0, 0, ""           )
                )
        );

        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelFloatArgDim(Name, "boxes_in"  , 3, 4,         n_anchors, 2);
                AddKernelFloatArgDim(Name, "classes_in", 3, n_classes, n_anchors, 2);
                AddKernelFloatArgDim(Name, "anchors_in", 3, 4,         n_anchors, 2);
                AddKernelFloatArgDim(Name, "bbox_out"  , 3, 4,         n_outbox,  2);
                AddKernelFloatArgDim(Name, "scores_out", 3, 1,         n_outbox,  2);
                AddKernelFloatArgDim(Name, "class_out" , 3, 1,         n_outbox,  2);
        }
        return (Kernel!=0);
}
