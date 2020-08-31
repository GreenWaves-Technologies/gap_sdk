#ifndef __CNN_GENERATORS_H__
#define __CNN_GENERATORS_H__

/** @addtogroup groupCNN
@{ */

/**
 @brief Load Convolutional Neural Network Library

 Load Convolutional Neural Network Library
*/
extern void LoadSSDLibrary();

extern void CNN_SSD_PostProcess_SQ8(char *Name, CNN_GenControl_T *Ctrl, int n_anchors, int n_classes, int n_outbox, int max_bb_before_nn_max);


#endif //__CNN_GENERATORS_H__