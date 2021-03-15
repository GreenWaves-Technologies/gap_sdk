#define NB_CONV 2
#define NB_DENSE 1
struct param_conv_layer convLayers[] = {{1,32,98,40,8,20,1,1,2,14},
{32,32,39,16,4,10,1,0,0,14},
};
struct param_dense_layer denseLayers[] = {{32,12,30,13,0,16,13},
};
