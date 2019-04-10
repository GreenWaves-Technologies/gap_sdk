// 
// Copyright (C) 2015 ETH Zurich and University of Bologna
// All rights reserved.
//
// This software may be modified and distributed under the terms
// of the BSD license.  See the LICENSE file for details.
//
// Authors: Germain Haugou (germain.haugou@gmail.com)
//

#include "archi/pulp.h"

  .section .text

reset_handler:
  la   x2, stack
  j main
  
  .global changeStack
changeStack:
  mv   sp, a2
  jr   a1


  .section .vectors, "ax"
  .option norvc;
  .org 0x0
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler
  jal x0, default_handler

  //// reset vector
  .org 0x80
  jal x0, reset_handler

  .org 0x84
loop:
  j loop
  
default_handler:
  mret

  