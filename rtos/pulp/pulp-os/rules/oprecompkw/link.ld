
OUTPUT_ARCH(riscv)
ENTRY( _start )

/* We reserve 0x100 bytes in L1 for the boot code stack as we can return to it
 * after the test has finished execution */

MEMORY
{
  L2           : ORIGIN = 0x1c000000, LENGTH = 0x00080000
  L2_aliased   : ORIGIN = 0x00000000, LENGTH = 0x00004000
  L1                 : ORIGIN = 0x10000004, LENGTH = 0x0000fefc
  L1_aliased         : ORIGIN = 0x00000004, LENGTH = 0x0000fefc
}

/*
 * This linker script try to put FC data in L2 private bank0 and FC code 
 * in L2 private bank1 to avoid contention between FC code and data
 * as FC has no instruction cache and is so often accessing L2 to
 * get instructions. Everything can be shifted in case one bank is full.
 *
 * Cluster code and initialized data are put in shared banks to not polute
 * private banks which are quite small, and also avoid contentions between
 * cluster cache refill and FC.
 */


SECTIONS
{
  .vectors :
  {
    __irq_vector_base = .;
    KEEP(*(.vectors))
  } > L2

  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text)
    *(.text.*)
    _etext  =  .;
    *(.lit)
    *(.shdata)
    _endtext = .;
    . = ALIGN(4);
  } > L2

  .text.cluster :
  {
    __cluster_text_start = .;
    *(.cluster.text)
    *(.cluster.text.*)
    __cluster_text_end = .;
    . = ALIGN(4);
  } > L2



  .init :
  {
    . = ALIGN(4);
    KEEP( *(.init) )
  } > L2


  .fini :
  {
    . = ALIGN(4);
    KEEP( *(.fini) )
  } > L2


  .preinit_array : {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > L2


  .init_array : {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    __CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    KEEP(*(.ctors.start))
    KEEP(*(.ctors))
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    LONG(0)
    __CTOR_END__ = .;
    PROVIDE_HIDDEN (__init_array_end = .);
  } > L2


  .fini_array : {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    __DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    KEEP(*(.dtors.start))
    KEEP(*(.dtors))
    LONG(0)
    __DTOR_END__ = .;
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > L2


  .boot : {
    . = ALIGN(4);
    *(.boot)
    *(.boot.data)
  } > L2


  .rodata : {
    . = ALIGN(4);
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
  } > L2


  .got : {
    . = ALIGN(4);
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
  } > L2


  .shbss : {
    . = ALIGN(4);
    *(.shbss)
  } > L2


  .talias : {
  } > L2


  .gnu.offload_funcs : {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_funcs))
  } > L2


  .gnu.offload_vars : {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_vars))
  } > L2


  .data : {
    . = ALIGN(4);
    sdata  =  .;
    _sdata  =  .;
    *(.data);
    *(.data.*)
    *(.sdata);
    *(.sdata.*)
    *(.heapl2ram)
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .;
  } > L2


  .bss : {
    . = ALIGN(8);
    _bss_start = .;
    *(.bss)
    *(.bss.*)
    *(.sbss)
    *(.sbss.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > L2


  .l2_data : {
    . = ALIGN(4);
    *(.l2_data)
    *(.l2_data.*)
    . = ALIGN(4);
  } > L2

  __l2_data_end = ALIGN(8);

  __cluster_text_size = __cluster_text_end - __cluster_text_start;

  __l2_heap_start = ALIGN(4);

  __l2_heap_size = LENGTH(L2) - __l2_heap_start + ORIGIN(L2);



  /* Following sections are keeping the cluster data
   * in L2 until the cluster is powered up */

  _l1_preload_start_inL2 = ALIGN(4);

  .data_tiny_l1 :
  {
    . = ALIGN(4);
    _l1_preload_start = .;
    *(.data_tiny_l1)
    *(.data_tiny_l1.*)
    *(.data_alias_l1)
    *(.data_alias_l1.*)
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > L1_aliased AT> L1

  .l1cluster_g (ORIGIN(L1) + SIZEOF(.data_tiny_l1)): {
    . = ALIGN(4);
    *(.heapsram)
    *(.heapsram.*)
    *(.l1cluster_g)
    *(.l1cluster_g.*)
    *(.data_l1)
    *(.data_l1.*)
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    *(.data_fc)
    *(.data_fc.*)
    *(.fcTcdm)
    *(.fcTcdm.*)
    *(.fcTcdm_g)
    *(.fcTcdm_g.*)
    . = ALIGN(4);
    _libgomp_start = .;
    *(.libgomp)
    *(.libgomp.*)
    . = ALIGN(4);
  } > L1

  .stack (ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g)) : {
    . = ALIGN(4);
    . = ALIGN(16);
    stack_start = .;
    . = . + 0x800;
    stack = .;
  } > L1

  .bss_l1 (ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g)) + SIZEOF(.stack) : {
    . = ALIGN(4);
    *(.bss_l1)
    *(.bss_l1.*)
    . = ALIGN(4);
  } > L1

  _l1_preload_size = SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g);

  __l1_heap_start = ALIGN(4);
  __l1_heap_size = LENGTH(L1) - __l1_heap_start + ORIGIN(L1);
}
