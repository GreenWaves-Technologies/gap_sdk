#ifndef __NNTOOL_EXTRA_GENERATORS__
#define __NNTOOL_EXTRA_GENERATORS__
#include <stdint.h>
#include "AutoTilerLib.h"

typedef enum {
	NNTOOL_KOP_RGB565,
	NNTOOL_KOP_RGB888,
	NNTOOL_KOP_RGB16,
	NNTOOL_KOP_BW,
	NNTOOL_KOP_BW16
} nntool_kop_t;

void LoadNNTools_Extra_Library();
int CNN_Norm(
	char *Name,

	int Width,
	int Height,
	int DoOffset,
	nntool_kop_t kop
);

#endif
