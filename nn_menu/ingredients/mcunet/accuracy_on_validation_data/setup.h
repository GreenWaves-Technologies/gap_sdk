#ifndef __SETUP_H__
#define __SETUP_H__

#ifdef SILENT
#define PRINTF(...) ((void) 0)
#else
#define PRINTF printf
#endif  /* DEBUG */

// Cluster core #0 stack size
#define CL_STACK_SIZE (6*1024)
// Other cluster cores stack size
#define CL_SLAVE_STACK_SIZE 1024

// Network input resolution
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 160
#define IMAGE_CHANNELS 3

#define INPUT_SIZE (IMAGE_WIDTH*IMAGE_HEIGHT*IMAGE_CHANNELS)

#define NETWORK_CLASSES 1000
#endif
