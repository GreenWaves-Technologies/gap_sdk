
OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  /* 0x8000 are reserved at the end of the L2 for the boot rom */
  L2           : ORIGIN = 0x1c000008, LENGTH = 0x00190000 - 8
  L2_aliased   : ORIGIN = 0x00000008, LENGTH = 0x00003ff8
  L1                 : ORIGIN = 0x10000008, LENGTH = 0x0001fff8
  L1_aliased         : ORIGIN = 0x00000008, LENGTH = 0x0001fff8
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
  /*
   * L2 PRIVATE BANK0
   *
   * Contains FC data
   */

  /* This section is for tiny FC data which benefits 
   * from the alias at 0
   */
  .data_tiny_fc :
  {
    . = ALIGN(8);
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
    . = ALIGN(8);
  } > L2_aliased AT> L2


  .init :
  {
    . = ALIGN(8);
    KEEP( *(.init) )
    . = ALIGN(8);
  } > L2


  .fini :
  {
    . = ALIGN(8);
    KEEP( *(.fini) )
    . = ALIGN(8);
  } > L2


  .preinit_array : {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(8);
  } > L2


  .init_array : {
    . = ALIGN(8);
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
    . = ALIGN(8);
  } > L2


  .fini_array : {
    . = ALIGN(8);
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
    . = ALIGN(8);
  } > L2


  .boot : {
    . = ALIGN(8);
    *(.boot)
    *(.boot.data)
    . = ALIGN(8);
  } > L2


  .rodata : {
    . = ALIGN(8);
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
    . = ALIGN(8);
  } > L2


  .got : {
    . = ALIGN(8);
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
    . = ALIGN(8);
  } > L2


  .shbss : {
    . = ALIGN(8);
    *(.shbss)
    . = ALIGN(8);
  } > L2


  .talias : {
  } > L2


  .gnu.offload_funcs : {
    . = ALIGN(8);
    KEEP(*(.gnu.offload_funcs))
    . = ALIGN(8);
  } > L2


  .gnu.offload_vars : {
    . = ALIGN(8);
    KEEP(*(.gnu.offload_vars))
    . = ALIGN(8);
  } > L2


  .stack : {
    . = ALIGN(8);
    . = ALIGN(16);
    stack_start = .;
    . = . + 0x800;
    stack = .;
    . = ALIGN(8);
  } > L2


  .data : {
    . = ALIGN(8);
    sdata  =  .;
    _sdata  =  .;
    *(.data_fc)
    *(.data_fc.*)
    *(.data);
    *(.data.*)
    *(.sdata);
    *(.sdata.*)
    *(.heapl2ram)
    *(.fcTcdm)
    *(.fcTcdm.*)
    *(.fcTcdm_g)
    *(.fcTcdm_g.*)
    . = ALIGN(8);
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
    . = ALIGN(8);
    _bss_end = .;
  } > L2


  __l2_priv0_end = ALIGN(8);




  /*
   * L2 PRIVATE BANK1
   *
   * Contains FC code
   */

  .vectors MAX(0x1c008000,ALIGN(256)) :
  {
    __irq_vector_base = .;
    KEEP(*(.vectors))
    . = ALIGN(8);
  } > L2

  .text :
  {
    . = ALIGN(8);
    _stext = .;
    *(.text)
    *(.text.*)
    _etext  =  .;
    *(.lit)
    *(.shdata)
    _endtext = .;
    . = ALIGN(8);
  } > L2

  __l2_priv1_end = ALIGN(8);


  /*
   * L2 SHARED BANKS
   *
   * Contains other data such as peripheral data and cluster code and data
   */

  .l2_data MAX(0x1c010000,ALIGN(8)) :
  {
    . = ALIGN(8);
    __cluster_text_start = .;
    *(.cluster.text)
    *(.cluster.text.*)
    . = ALIGN(8);
    __cluster_text_end = .;
    *(.l2_data)
    *(.l2_data.*)
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    . = ALIGN(8);
  } > L2


  /* Following sections are keeping the cluster data
   * in L2 until the cluster is powered up */

  _l1_preload_start_inL2 = .;

  .data_tiny_l1 :
  {
    . = ALIGN(8);
    _l1_preload_start = .;
    *(.data_tiny_l1)
    *(.data_tiny_l1.*)
    *(.data_alias_l1)
    *(.data_alias_l1.*)
    . = ALIGN(8);
  } > L1_aliased AT> L2

  .l1cluster_g (ORIGIN(L1) + SIZEOF(.data_tiny_l1)): {
    . = ALIGN(8);
    *(.heapsram)
    *(.heapsram.*)
    *(.l1cluster_g)
    *(.l1cluster_g.*)
    *(.data_l1)
    *(.data_l1.*)
    . = ALIGN(8);
    _libgomp_start = .;
    *(.libgomp)
    *(.libgomp.*)
    . = ALIGN(8);
  } > L1 AT> L2

  .bss_l1 (ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g)) : {
    . = ALIGN(8);
    *(.bss_l1)
    *(.bss_l1.*)
    . = ALIGN(8);
  } > L1 AT> L2

  __l1_end = ALIGN(8);

  __l2_shared_end = LOADADDR(.bss_l1) + SIZEOF(.bss_l1);




  _l1_preload_size = SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g);

  __cluster_text_size = __cluster_text_end - __cluster_text_start;

  __l1_heap_start = ALIGN(8);
  __l1_heap_size = LENGTH(L1) - __l1_heap_start + ORIGIN(L1);
}
