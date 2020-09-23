/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* not needed, but we need separate linker scripts anyway */
OUTPUT_ARCH(riscv)
SEARCH_DIR(.)
__DYNAMIC  =  0;

ENTRY(reset_handler)

MEMORY
{
  FC_tcdm           : ORIGIN = 0x1B000000, LENGTH = 0x4000
  L2                : ORIGIN = 0x1C000000, LENGTH = 0x80000
  L1_sram           : ORIGIN = 0x10000000, LENGTH = 0x10000
}

/* Here are defined stack sizes for FC and cluster. */
__STACK_FC_SIZE      = 0x1000;
__STACK_IRQ_SIZE     = 0x400;

/* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

SECTIONS
{
    .l1FcTiny :
    {
        . = ALIGN(4);
        __l1FcShared_start = .;
        *(.l1FcTiny)
        *(.l1FcTiny.*)
        . = ALIGN(4);
        __l1FcShared_end = .;
    } > FC_tcdm

    __l1FcShared_size = __l1FcShared_end - __l1FcShared_start;

     .fcTcdmTiny :
     {
       *(.fcTcdmTiny)
       *(.fcTcdmTiny.*)
     } > FC_tcdm

     .fcTcdm :
     {
       *(.fcTcdm)
       *(.fcTcdm.*)
     } > FC_tcdm

     .fcTcdm_g :
     {
       *(.fcTcdm_g)
       *(.fcTcdm_g.*)
     } > FC_tcdm

    /* This section contains stack used by IRQ handler. */
    .stack_irq :
    {
        . = ALIGN(4);
        __stack_irq_end__ = .;
        . += __STACK_IRQ_SIZE;
        __stack_irq_start__ = .;
     } > FC_tcdm

    .heapfcram :
    {
        . = ALIGN(8);
        *(.heapfcram)
        *(.heapfcram.*)
        . = ALIGN(8);
        __heapfcram_start = .;
        __stack_fc_end__ = .;
        . += __STACK_FC_SIZE;
        __stack_fc_start__ = .;
    } > FC_tcdm

     __heapfcram_size = ORIGIN(FC_tcdm) + LENGTH(FC_tcdm) - __heapfcram_start;

    /* This section holds IRQ vectors table. */
    .vectors_irq :
    {
        . = ALIGN(4);
        __irq_vector_base_m__ = .;
        KEEP(*(.vectors_irq))
    } > L2

    /* This section holds code of program. */
    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text.reset)
        *(.text)
        *(.text.*)
        . = ALIGN(4);
        __cluster_text_start = .;
        *(.cluster.text)
        *(.cluster.text.*)
        __cluster_text_end = .;
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
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        *(.eh_frame*)
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
        . = ALIGN(4);
        *(.sdata)
        *(.sdata.*)
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
    } > L2
    __l1_preload_start_inL2 = ALIGN(4);

    .l1clusterTiny (ORIGIN(L1_sram)) :
    {
        /* Due to limitations on FPGA loader, loadable sections must have base and size aligned on 4 bytes */
        . = ALIGN(4);
        __l1_preload_start = . + 0x4;
        *(.l1clusterTiny)
        *(.l1clusterTiny.*)
        . = ALIGN(4);
    } > L1_sram AT> L2

    .l1cluster_g (ORIGIN(L1_sram) + SIZEOF(.l1clusterTiny)) :
    {
        . = ALIGN(4);
        __printf_lock_ptr = . + 0x4;
        . = __printf_lock_ptr + 0x4;
        *(.heapsram)
        *(.heapsram.*)
        . = ALIGN(4);
        *(.l1cluster_g)
        *(.l1cluster_g.*)
        *(.data_l1)
        *(.data_l1.*)
        . = ALIGN(4);
        *(.libgomp)
        *(.libgomp.*)
        . = ALIGN(8);
        __heapsram_start = .;
    } > L1_sram AT> L2

     __heapsram_size = ORIGIN(L1_sram) + LENGTH(L1_sram) - __heapsram_start;
     __l1_preload_size = __heapsram_start - ORIGIN(L1_sram);

    .heapl2ram (__l1_preload_start_inL2 + __l1_preload_size) :
    {
        . = ALIGN(8);
        *(.heapl2ram)
        *(.heapl2ram.*)
        . = ALIGN(8);
        __heapl2ram_start = .;
    } > L2

    __heapl2ram_size = LENGTH(L2) + ORIGIN(L2) - __heapl2ram_start;
    __heapl2ram_limit = __heapl2ram_size + __heapl2ram_start;
}
