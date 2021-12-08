
OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  L2_scm_0             : ORIGIN = 0x1c006000, LENGTH = 0x00002000
  L2_scm_1             : ORIGIN = 0x1c080000, LENGTH = 0x00002000
}

/*
 * This linker script put FC data in L2 private bank0 and FC code 
 * in L2 private bank1 to avoid contention between FC code and data
 * as FC has no instruction cache and is so often accessing L2 to
 * get instructions.
 * Cluster code and initialized data are also put in private bank1
 * to let the fc and cluster grows until the end of the L2.
 * This should not generate a lot of contention as cluster code
 * is supposed to quickly fit into the cluster instruction cache.
 */


SECTIONS
{
  __l2_priv0_end = ALIGN(4);

  .vectors :
  {
    . = ALIGN(4);
    __irq_vector_base = .;KEEP(*(.vectors))
  } > L2_scm_0

  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text)
    *(.text.*)
    . = ALIGN(4);
    __cluster_text_start = .;*(.cluster.text)
    *(.cluster.text.*)
    __cluster_text_end = .;_etext  =  .;
    *(.lit)
    *(.shdata)
    _endtext = .;
    . = ALIGN(4);
  } > L2_scm_0

  .data_tiny_fc :
  {
    . = ALIGN(4);
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > L2_scm_1


  .init :
  {
    . = ALIGN(4);
    KEEP( *(.init) )
  } > L2_scm_1


  .fini :
  {
    . = ALIGN(4);
    KEEP( *(.fini) )
  } > L2_scm_1


  .preinit_array : {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > L2_scm_1


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
  } > L2_scm_1


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
  } > L2_scm_1


  .boot : {
    . = ALIGN(4);
    *(.boot)
    *(.boot.data)
  } > L2_scm_1


  .rodata : {
    . = ALIGN(4);
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
  } > L2_scm_1


  .got : {
    . = ALIGN(4);
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
  } > L2_scm_1


  .shbss : {
    . = ALIGN(4);
    *(.shbss)
  } > L2_scm_1


  .talias : {
  } > L2_scm_1


  .gnu.offload_funcs : {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_funcs))
  } > L2_scm_1


  .gnu.offload_vars : {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_vars))
  } > L2_scm_1


  .stack : {
    . = ALIGN(4);
    . = ALIGN(16);
    . = . + 0x800;
    stack = .;
  } > L2_scm_1


  .data : {
    . = ALIGN(4);
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
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .;
  } > L2_scm_1


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
  } > L2_scm_1


  .l2_data : {
    . = ALIGN(4);
    *(.l2_data)
    *(.l2_data.*)
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    . = ALIGN(4);
  } > L2_scm_1


  /* Following sections are keeping the cluster data
   * in L2 until the cluster is powered up */

  .data_tiny_l1 :
  {
    . = ALIGN(4);
    *(.data_tiny_l1)
    *(.data_tiny_l1.*)
    *(.data_alias_l1)
    *(.data_alias_l1.*)
  } > L2_scm_1

  .l1cluster_g : {
    . = ALIGN(4);
    *(.heapsram)
    *(.heapsram.*)
    *(.l1cluster_g)
    *(.l1cluster_g.*)
    *(.data_l1)
    *(.data_l1.*)
    . = ALIGN(4);
    _libgomp_start = .;
    *(.libgomp)
    *(.libgomp.*)
    . = ALIGN(4);
  } > L2_scm_1

  .bss_l1 : {
    . = ALIGN(4);
    *(.bss_l1)
    *(.bss_l1.*)
    . = ALIGN(4);
  } > L2_scm_1

  __L2_scm_end = LOADADDR(.bss_l1) + SIZEOF(.bss_l1);


  __l2_priv1_end = MAX(0x1c008000,ALIGN(4));

  __l2_shared_end = MAX(0x1c010000,ALIGN(4));


}
