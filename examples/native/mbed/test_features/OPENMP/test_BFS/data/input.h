#ifndef __INPUT_H__
#define __INPUT_H__

#ifndef DIM
# define DIM         1U
#endif

#if DIM == 1U
# include "rand-graph-1024.h"
# define CHECKSUM (0x1229U)

#elif DIM == 2U
# include "rand-graph-2048.h"
# define CHECKSUM (0x25a3U)

#elif DIM == 4U
# include "rand-graph-4096.h"
# define CHECKSUM (0x4fbaU)

#elif DIM == 8U
# include "rand-graph-8192.h"
# define CHECKSUM (0xa4ecU)

#elif DIM == 16U
# include "rand-graph-16384.h"
# define CHECKSUM  (0x17f5dU)

#elif DIM == 32U
# include "rand-graph-32768.h"
# define CHECKSUM  (0x309f8U)

#elif DIM == 64U
# include "rand-graph-65536.h"
# define CHECKSUM  (0x69bb6U)

#else
# error WIDTH not supported
#endif

#ifndef CHECKSUM
#define CHECKSUM   (0x0U)
#endif


#endif /* __INPUT_H__*/
