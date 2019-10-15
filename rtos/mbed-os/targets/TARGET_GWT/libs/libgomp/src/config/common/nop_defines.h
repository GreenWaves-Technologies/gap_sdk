/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         nop utilities */

#define __nop { \
  asm("l.nop"); \
}

#define _10_nop_block  { \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  asm("l.nop"); \
  }

#define _50_nop_block  { \
  _10_nop_block \
  _10_nop_block \
  _10_nop_block \
  _10_nop_block \
  _10_nop_block \
  }

#define _100_nop_block  { \
  _50_nop_block \
  _50_nop_block \
  }

#define _200_nop_block  { \
  _100_nop_block \
  _100_nop_block \
  }

#define _500_nop_block  { \
  _200_nop_block \
  _200_nop_block \
  _100_nop_block \
  }

#define _1000_nop_block  { \
  _500_nop_block \
  _500_nop_block \
  }

#define _2000_nop_block  { \
  _1000_nop_block \
  _1000_nop_block \
  }

#define _5000_nop_block  { \
  _2000_nop_block \
  _2000_nop_block \
  _1000_nop_block \
  }

#define _10000_nop_block  { \
  _5000_nop_block \
  _5000_nop_block \
  }

#define _20000_nop_block  { \
  _10000_nop_block \
  _10000_nop_block \
  }

#define _50000_nop_block  { \
  _10000_nop_block \
  _20000_nop_block \
  _20000_nop_block \
  }

#define _100000_nop_block  { \
  _50000_nop_block \
  _50000_nop_block \
  }
