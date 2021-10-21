void MobileNetCNN(){
	CNN_GenControl_T CtrlH;
	CNN_InitGenCtrl(&CtrlH);
	CNN_SetGenCtrl(&CtrlH, "EnableIm2Col", AT_OPT_ON);
	CNN_SetGenCtrl(&CtrlH, "PADTYPE", (void *) PAD_BALANCED_RIGHT);

	CNN_ConvolutionMulBiasPoolReLU("Layer0", &CtrlH, 1,1,1,1,1,0,0,-7,16,0,1,1,1,0,1,3,32,224,224,KOP_CONV_DP,3,3,1,1,2,2, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer1", &CtrlH, 1,1,1,1,1,0,0,-3,9,0,1,1,1,0,1,32,32,112,112,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer2", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,32,64,112,112,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer3", &CtrlH, 1,1,1,1,1,0,0,-2,8,0,1,1,1,0,1,64,64,112,112,KOP_CONV_DWDP,3,3,1,1,2,2, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer4", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,64,128,56,56,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer5", &CtrlH, 1,1,1,1,1,0,0,-6,11,0,1,1,1,0,1,128,128,56,56,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer6", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,128,128,56,56,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer7", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,128,128,56,56,KOP_CONV_DWDP,3,3,1,1,2,2, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer8", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,128,256,28,28,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer9", &CtrlH, 1,1,1,1,1,0,0,-6,12,0,1,1,1,0,1,256,256,28,28,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer10", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,256,256,28,28,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer11", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,256,256,28,28,KOP_CONV_DWDP,3,3,1,1,2,2, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer12", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,256,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer13", &CtrlH, 1,1,1,1,1,0,0,-7,12,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer14", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer15", &CtrlH, 1,1,1,1,1,0,0,-7,12,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer16", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer17", &CtrlH, 1,1,1,1,1,0,0,-6,12,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer18", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer19", &CtrlH, 1,1,1,1,1,0,0,-6,12,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer20", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer21", &CtrlH, 1,1,1,1,1,0,0,-7,13,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer22", &CtrlH, 1,1,1,1,1,0,0,-7,15,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer23", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,512,512,14,14,KOP_CONV_DWDP,3,3,1,1,2,2, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer24", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,512,1024,7,7,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer25", &CtrlH, 1,1,1,1,1,0,0,-3,9,0,1,1,1,0,1,1024,1024,7,7,KOP_CONV_DWDP,3,3,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_ConvolutionMulBiasPoolReLU("Layer26", &CtrlH, 1,1,1,1,1,0,0,-7,14,0,1,1,1,0,1,1024,1024,7,7,KOP_CONV_DP,1,1,1,1,1,1, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU);
	CNN_PoolReLU("Layer27",&CtrlH, 1,1,0,0, 1,1, 1024,1024, 7,7,KOP_AVGPOOL,7,7, 1,1,7,7, 1, KOP_NONE);
	CNN_LinearReLU("Layer28",&CtrlH, 1,1,2,2,0,0,0,0,1,1,1,1,1024,1000,KOP_LINEAR, KOP_NONE);

	CreateGraph("MobileNetCNN",
		CArgs(85,
			TCArgInfo("signed char *__restrict", "In", ARG_SCOPE_ARG, ARG_DIR_IN, 1,AT_MEM_L3_HRAM, 0),
			TCArgInfo ("signed char *__restrict", "FL0",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L0_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL0",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L0_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML0",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L0_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL1",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L1_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL1",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L1_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML1",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L1_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL2",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L2_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL2",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L2_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML2",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L2_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL3",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L3_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL3",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L3_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML3",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L3_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL4",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L4_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL4",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L4_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML4",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L4_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL5",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L5_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL5",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L5_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML5",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L5_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL6",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L6_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL6",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L6_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML6",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L6_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL7",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L7_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL7",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L7_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML7",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L7_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL8",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L8_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL8",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L8_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML8",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L8_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL9",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L9_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL9",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L9_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML9",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L9_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL10",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L10_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL10",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L10_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML10",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L10_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL11",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L11_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL11",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L11_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML11",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L11_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL12",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L12_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL12",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L12_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML12",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L12_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL13",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L13_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL13",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L13_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML13",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L13_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL14",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L14_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL14",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L14_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML14",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L14_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL15",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L15_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL15",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L15_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML15",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L15_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL16",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L16_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL16",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L16_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML16",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L16_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL17",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L17_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL17",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L17_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML17",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L17_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL18",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L18_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL18",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L18_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML18",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L18_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL19",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L19_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL19",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L19_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML19",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L19_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL20",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L20_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL20",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L20_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML20",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L20_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL21",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L21_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL21",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L21_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML21",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L21_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL22",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L22_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL22",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L22_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML22",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L22_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL23",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L23_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL23",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L23_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML23",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L23_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL24",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L24_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL24",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L24_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML24",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L24_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL25",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L25_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL25",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L25_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML25",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L25_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL26",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L26_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "signed char * __restrict__", "BL26",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L26_bias_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "ML26",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L26_M0_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ("signed char *__restrict", "FL28",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("./binFiles/L28_weight_L3.bin", 1, 1, 8, 0)),
			TCArgInfo ( "short int * __restrict__", "BL28",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "./binFiles/L28_weight_L3.bin", 1, 1, 8, 0)),
            TCArgInfo ("short int *__restrict", "Out", 	   ARG_SCOPE_ARG, ARG_DIR_OUT, AT_MEM_L2, AT_MEM_L2, 0)

		),
		CArgs(28,
			TCArgInfo ("signed char *__restrict", "OutL0", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL1", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL2", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL3", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL4", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL5", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL6", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL7", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL8", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL9", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL10", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL11", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL12", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL13", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL14", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL15", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL16", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL17", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL18", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL19", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL20", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL21", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL22", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL23", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL24", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL25", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL26", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0),
			TCArgInfo ("signed char *__restrict", "OutL27", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0)
		)
	);
	AddNode("Layer0",Bindings(5,GNodeArg(GNA_IN, "In", 0),GNodeArg(GNA_IN, "FL0", 0),GNodeArg(GNA_IN, "BL0", 0),GNodeArg(GNA_IN, "ML0", 0),GNodeArg(GNA_OUT, "OutL0", 0) ));
	AddNode("Layer1",Bindings(5,GNodeArg(GNA_IN, "OutL0", 0),GNodeArg(GNA_IN, "FL1", 0),GNodeArg(GNA_IN, "BL1", 0),GNodeArg(GNA_IN, "ML1", 0),GNodeArg(GNA_OUT, "OutL1", 0) ));
	AddNode("Layer2",Bindings(5,GNodeArg(GNA_IN, "OutL1", 0),GNodeArg(GNA_IN, "FL2", 0),GNodeArg(GNA_IN, "BL2", 0),GNodeArg(GNA_IN, "ML2", 0),GNodeArg(GNA_OUT, "OutL2", 0) ));
	AddNode("Layer3",Bindings(5,GNodeArg(GNA_IN, "OutL2", 0),GNodeArg(GNA_IN, "FL3", 0),GNodeArg(GNA_IN, "BL3", 0),GNodeArg(GNA_IN, "ML3", 0),GNodeArg(GNA_OUT, "OutL3", 0) ));
	AddNode("Layer4",Bindings(5,GNodeArg(GNA_IN, "OutL3", 0),GNodeArg(GNA_IN, "FL4", 0),GNodeArg(GNA_IN, "BL4", 0),GNodeArg(GNA_IN, "ML4", 0),GNodeArg(GNA_OUT, "OutL4", 0) ));
	AddNode("Layer5",Bindings(5,GNodeArg(GNA_IN, "OutL4", 0),GNodeArg(GNA_IN, "FL5", 0),GNodeArg(GNA_IN, "BL5", 0),GNodeArg(GNA_IN, "ML5", 0),GNodeArg(GNA_OUT, "OutL5", 0) ));
	AddNode("Layer6",Bindings(5,GNodeArg(GNA_IN, "OutL5", 0),GNodeArg(GNA_IN, "FL6", 0),GNodeArg(GNA_IN, "BL6", 0),GNodeArg(GNA_IN, "ML6", 0),GNodeArg(GNA_OUT, "OutL6", 0) ));
	AddNode("Layer7",Bindings(5,GNodeArg(GNA_IN, "OutL6", 0),GNodeArg(GNA_IN, "FL7", 0),GNodeArg(GNA_IN, "BL7", 0),GNodeArg(GNA_IN, "ML7", 0),GNodeArg(GNA_OUT, "OutL7", 0) ));
	AddNode("Layer8",Bindings(5,GNodeArg(GNA_IN, "OutL7", 0),GNodeArg(GNA_IN, "FL8", 0),GNodeArg(GNA_IN, "BL8", 0),GNodeArg(GNA_IN, "ML8", 0),GNodeArg(GNA_OUT, "OutL8", 0) ));
	AddNode("Layer9",Bindings(5,GNodeArg(GNA_IN, "OutL8", 0),GNodeArg(GNA_IN, "FL9", 0),GNodeArg(GNA_IN, "BL9", 0),GNodeArg(GNA_IN, "ML9", 0),GNodeArg(GNA_OUT, "OutL9", 0) ));
	AddNode("Layer10",Bindings(5,GNodeArg(GNA_IN, "OutL9", 0),GNodeArg(GNA_IN, "FL10", 0),GNodeArg(GNA_IN, "BL10", 0),GNodeArg(GNA_IN, "ML10", 0),GNodeArg(GNA_OUT, "OutL10", 0) ));
	AddNode("Layer11",Bindings(5,GNodeArg(GNA_IN, "OutL10", 0),GNodeArg(GNA_IN, "FL11", 0),GNodeArg(GNA_IN, "BL11", 0),GNodeArg(GNA_IN, "ML11", 0),GNodeArg(GNA_OUT, "OutL11", 0) ));
	AddNode("Layer12",Bindings(5,GNodeArg(GNA_IN, "OutL11", 0),GNodeArg(GNA_IN, "FL12", 0),GNodeArg(GNA_IN, "BL12", 0),GNodeArg(GNA_IN, "ML12", 0),GNodeArg(GNA_OUT, "OutL12", 0) ));
	AddNode("Layer13",Bindings(5,GNodeArg(GNA_IN, "OutL12", 0),GNodeArg(GNA_IN, "FL13", 0),GNodeArg(GNA_IN, "BL13", 0),GNodeArg(GNA_IN, "ML13", 0),GNodeArg(GNA_OUT, "OutL13", 0) ));
	AddNode("Layer14",Bindings(5,GNodeArg(GNA_IN, "OutL13", 0),GNodeArg(GNA_IN, "FL14", 0),GNodeArg(GNA_IN, "BL14", 0),GNodeArg(GNA_IN, "ML14", 0),GNodeArg(GNA_OUT, "OutL14", 0) ));
	AddNode("Layer15",Bindings(5,GNodeArg(GNA_IN, "OutL14", 0),GNodeArg(GNA_IN, "FL15", 0),GNodeArg(GNA_IN, "BL15", 0),GNodeArg(GNA_IN, "ML15", 0),GNodeArg(GNA_OUT, "OutL15", 0) ));
	AddNode("Layer16",Bindings(5,GNodeArg(GNA_IN, "OutL15", 0),GNodeArg(GNA_IN, "FL16", 0),GNodeArg(GNA_IN, "BL16", 0),GNodeArg(GNA_IN, "ML16", 0),GNodeArg(GNA_OUT, "OutL16", 0) ));
	AddNode("Layer17",Bindings(5,GNodeArg(GNA_IN, "OutL16", 0),GNodeArg(GNA_IN, "FL17", 0),GNodeArg(GNA_IN, "BL17", 0),GNodeArg(GNA_IN, "ML17", 0),GNodeArg(GNA_OUT, "OutL17", 0) ));
	AddNode("Layer18",Bindings(5,GNodeArg(GNA_IN, "OutL17", 0),GNodeArg(GNA_IN, "FL18", 0),GNodeArg(GNA_IN, "BL18", 0),GNodeArg(GNA_IN, "ML18", 0),GNodeArg(GNA_OUT, "OutL18", 0) ));
	AddNode("Layer19",Bindings(5,GNodeArg(GNA_IN, "OutL18", 0),GNodeArg(GNA_IN, "FL19", 0),GNodeArg(GNA_IN, "BL19", 0),GNodeArg(GNA_IN, "ML19", 0),GNodeArg(GNA_OUT, "OutL19", 0) ));
	AddNode("Layer20",Bindings(5,GNodeArg(GNA_IN, "OutL19", 0),GNodeArg(GNA_IN, "FL20", 0),GNodeArg(GNA_IN, "BL20", 0),GNodeArg(GNA_IN, "ML20", 0),GNodeArg(GNA_OUT, "OutL20", 0) ));
	AddNode("Layer21",Bindings(5,GNodeArg(GNA_IN, "OutL20", 0),GNodeArg(GNA_IN, "FL21", 0),GNodeArg(GNA_IN, "BL21", 0),GNodeArg(GNA_IN, "ML21", 0),GNodeArg(GNA_OUT, "OutL21", 0) ));
	AddNode("Layer22",Bindings(5,GNodeArg(GNA_IN, "OutL21", 0),GNodeArg(GNA_IN, "FL22", 0),GNodeArg(GNA_IN, "BL22", 0),GNodeArg(GNA_IN, "ML22", 0),GNodeArg(GNA_OUT, "OutL22", 0) ));
	AddNode("Layer23",Bindings(5,GNodeArg(GNA_IN, "OutL22", 0),GNodeArg(GNA_IN, "FL23", 0),GNodeArg(GNA_IN, "BL23", 0),GNodeArg(GNA_IN, "ML23", 0),GNodeArg(GNA_OUT, "OutL23", 0) ));
	AddNode("Layer24",Bindings(5,GNodeArg(GNA_IN, "OutL23", 0),GNodeArg(GNA_IN, "FL24", 0),GNodeArg(GNA_IN, "BL24", 0),GNodeArg(GNA_IN, "ML24", 0),GNodeArg(GNA_OUT, "OutL24", 0) ));
	AddNode("Layer25",Bindings(5,GNodeArg(GNA_IN, "OutL24", 0),GNodeArg(GNA_IN, "FL25", 0),GNodeArg(GNA_IN, "BL25", 0),GNodeArg(GNA_IN, "ML25", 0),GNodeArg(GNA_OUT, "OutL25", 0) ));
	AddNode("Layer26",Bindings(5,GNodeArg(GNA_IN, "OutL25", 0),GNodeArg(GNA_IN, "FL26", 0),GNodeArg(GNA_IN, "BL26", 0),GNodeArg(GNA_IN, "ML26", 0),GNodeArg(GNA_OUT, "OutL26", 0) ));
	AddNode("Layer27",Bindings(2,GNodeArg(GNA_IN, "OutL26", 0),GNodeArg(GNA_OUT, "OutL27", 0)));
	AddNode("Layer28",Bindings(4,GNodeArg(GNA_IN, "OutL27", 0),GNodeArg(GNA_IN, "FL28", 0),GNodeArg(GNA_IN, "BL28", 0),GNodeArg(GNA_OUT, "Out", 0)));
	CloseGraph();
}