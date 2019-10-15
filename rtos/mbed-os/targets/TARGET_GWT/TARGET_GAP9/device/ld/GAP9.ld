/* ---------------------------------------------------------------------------------------*/
/*  @file:    GAP9.ld                                                                     */
/*  @purpose: CMSIS RV32IMC Core Device link File                                         */
/*            GAP9                                                                        */
/*  @version: 1.0                                                                         */
/*  @date:    2018-4-30                                                                  */
/*  @build:                                                                               */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright (c) 2018 , GreenWaves Technologies, Inc.                                     */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* Redistribution and use in source and binary forms, with or without modification,       */
/* are permitted provided that the following conditions are met:                          */
/*                                                                                        */
/* o Redistributions of source code must retain the above copyright notice, this list     */
/*   of conditions and the following disclaimer.                                          */
/*                                                                                        */
/* o Redistributions in binary form must reproduce the above copyright notice, this       */
/*   list of conditions and the following disclaimer in the documentation and/or          */
/*   other materials provided with the distribution.                                      */
/*                                                                                        */
/* o Neither the name of GreenWaves Technologies, Inc. nor the names of its               */
/*   contributors may be used to endorse or promote products derived from this            */
/*   software without specific prior written permission.                                  */
/*                                                                                        */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND        */
/* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED          */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                 */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR       */
/* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;           */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON         */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS          */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/******************************************************************************************/

/* not needed, but we need separate linker scripts anyway */
OUTPUT_ARCH(riscv)
SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
  L2_priv0          : ORIGIN = 0x1C000000, LENGTH = 0x8000
  L2_priv1          : ORIGIN = 0x1C008000, LENGTH = 0x8000
  L2                : ORIGIN = 0x1C010000, LENGTH = 0x170000
  L1_sram           : ORIGIN = 0x10000000, LENGTH = 0x20000
}

__L1_STACK_SIZE      = 0x400;
__L2_STACK_SIZE      = 0x1000;

/* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

SECTIONS
{
    .vectors_U :
    {
        . = ALIGN(4);
        IRQ_U_Vector_Base = .;
        KEEP(*(.vectors_U))
    } > L2_priv1

    .vectors_M :
    {
        . = ALIGN(4);
        IRQ_M_Vector_Base = .;
        KEEP(*(.vectors_M))
    } > L2_priv1

    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text.reset)
        *(.text)
        *(.text.*)
        _etext  =  .;
        *(.lit)
        *(.shdata)
        _endtext = .;
    }  > L2

    .ctors :
     {
        . = ALIGN(4);
        __CTOR_LIST__ = .;
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           from the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        __CTOR_END__ = .;
    } > L2

    .dtors : {
        . = ALIGN(4);
        __DTOR_LIST__ = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        __DTOR_END__ = .;
    }  > L2

    /*--------------------------------------------------------------------*/
    /* Global constructor/destructor segement                             */
    /*--------------------------------------------------------------------*/
    .preinit_array :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array*))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } > L2

    .init_array :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
    } > L2

    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } > L2


    .rodata : {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        *(.rodata);
        *(.rodata.*)
        . = ALIGN(4);
    } > L2

    /* or1k-specific */
    .got : { *(.got.plt) * (.igot.plt) *(.got) *(.igot) } > L2
    .shbss :
    {
        . = ALIGN(4);
        *(.shbss)
    } > L2

    .talias :
    {
    }  > L2

    /* GCC Offload table of offloaded functions and variables */
    .gnu.offload_funcs :
    {
        . = ALIGN(4);
        KEEP(*(.gnu.offload_funcs))
    } > L2

    .gnu.offload_vars :
    {
        . = ALIGN(4);
        KEEP(*(.gnu.offload_vars))
    } > L2

    .data : {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        __DATA_RAM = .;
        __data_start__ = .;      /* create a global symbol at data start */
        *(.data);
        *(.data.*)
        KEEP(*(.jcr*))
        . = ALIGN(4);
        __data_end__ = .;        /* define a global symbol at data end */
    } > L2

    .bss :
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        __START_BSS = .;
        __bss_start__ = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        __END_BSS = .;
        __l1_preload_start_inL2 = .;
    } > L2

    __bss_size__ = __bss_end__ - __bss_start__;

    .libgomp (ORIGIN(L1_sram) ): AT(__l1_preload_start_inL2)
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        __l1_preload_start = .;
        _libgomp_start = .;
         *(.libgomp)
        *(.libgomp.*)
        . = ALIGN(4);
    } > L1_sram

    .l1cluster_g (ORIGIN(L1_sram) + SIZEOF(.libgomp)) : AT(__l1_preload_start_inL2 + SIZEOF(.libgomp))
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        *(.l1clusterTiny)
        *(.l1clusterTiny.*)
        *(.l1cluster_g)
        *(.l1cluster_g.*)
        *(.data_l1)
        *(.data_l1.*)
        . = ALIGN(4);
        __l1_preload_end = .;
     } > L1_sram

     __l1_preload_size = __l1_preload_end - ORIGIN(L1_sram);

    .heapsram (ORIGIN(L1_sram)  + SIZEOF(.l1cluster_g) + SIZEOF(.libgomp)) : AT(__l1_preload_start_inL2 + SIZEOF(l1cluster_g) + SIZEOF(.libgomp))
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        *(.heapsram)
        *(.heapsram.*)
        . = ALIGN(4);
        __heapsram_start = .;
    } > L1_sram

     __heapsram_size = LENGTH(L1_sram) + ORIGIN(L1_sram) - __heapsram_start - __L1_STACK_SIZE;

     .stacks (ORIGIN(L1_sram) + LENGTH(L1_sram) - __L1_STACK_SIZE) :
     {
       . = ALIGN(4);
       stacks = . + __L1_STACK_SIZE;
     } > L1_sram

    .heapl2ram (__l1_preload_start_inL2 + __l1_preload_size) :
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */

        . = ALIGN(4);
        __end__ = .;
        PROVIDE(end = .);
        *(.heapl2ram)

        . = ALIGN(4);
        __heapl2ram_start = .;
    } > L2

    __heapl2ram_size = LENGTH(L2) + ORIGIN(L2) - __heapl2ram_start - __L2_STACK_SIZE;
    __heapl2ram_limit = __heapl2ram_size + __heapl2ram_start;
    __HeapLimit = __heapl2ram_limit;

    .l2Stack :
    {
      __StackTop = ORIGIN(L2) + LENGTH(L2);
      __StackLimit = __StackTop - __L2_STACK_SIZE;
    } > L2

    ASSERT(__StackLimit >= __heapl2ram_start, "Region L2 overflowed with stack and heap")

    .stab  0 (NOLOAD) :
    {
        [ .stab ]
    }

    .stabstr  0 (NOLOAD) :
    {
        [ .stabstr ]
    }
}
