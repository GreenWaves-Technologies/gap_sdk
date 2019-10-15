#ifndef __INPUT_H__
#define __INPUT_H__

#ifndef DIM
# define DIM         16U
#endif

#if DIM == 16U
# include "input-matrix-16.h"
# define CHECKSUM (0x42fb43beU)

#elif DIM == 32U
# include "input-matrix-32.h"
# define CHECKSUM (0x44013f95U)

#elif DIM == 64U
# include "input-matrix-64.h"
# define CHECKSUM  (0x44ffc999U)

#elif DIM == 128U
# include "input-matrix-128.h"
# define CHECKSUM  (0x45ffc502U)

#elif DIM == 256U
# include "input-matrix-256.h"
# define CHECKSUM  (0x47003b9eU)

#else
# error WIDTH not supported
#endif

#ifndef CHECKSUM
#define CHECKSUM   (0x0U)
#endif


#endif /* __INPUT_H__*/