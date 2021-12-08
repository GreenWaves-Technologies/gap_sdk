
OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  L2           : ORIGIN = 0x1c000004, LENGTH = 0x0007fffc
  L2_aliased   : ORIGIN = 0x00000004, LENGTH = 0x00003ffc
}

__ZERO = DEFINED(__ZERO) ?  __ZERO : 0;
__USE_UART = DEFINED(__USE_UART) ?  __USE_UART : 0;
__RT_DEBUG_CONFIG = DEFINED(__RT_DEBUG_CONFIG) ?  __RT_DEBUG_CONFIG : 0;
__FC = DEFINED(__FC) ?  __FC : 1;
__L2 = DEFINED(__L2) ?  __L2 : 0x80000;
__L1Cl = DEFINED(__L1Cl) ?  __L1Cl : 0x10000;
__FETCH_ALL = DEFINED(__FETCH_ALL) ?  __FETCH_ALL : 0;
__ACTIVE_FC = DEFINED(__ACTIVE_FC) ?  __ACTIVE_FC : 1;
__rt_stack_size = DEFINED(__rt_stack_size) ?  __rt_stack_size : 0x800;
__rt_platform = DEFINED(__rt_platform) ?  __rt_platform : 3;
__rt_iodev = DEFINED(__rt_iodev) ?  __rt_iodev : 0;
__rt_iodev_uart_value = DEFINED(__rt_iodev_uart_value) ?  __rt_iodev_uart_value : 1;
__rt_iodev_uart_channel = DEFINED(__rt_iodev_uart_channel) ?  __rt_iodev_uart_channel : 0;
__rt_iodev_uart_baudrate = DEFINED(__rt_iodev_uart_baudrate) ?  __rt_iodev_uart_baudrate : 625000;
__rt_iodev_default_value = DEFINED(__rt_iodev_default_value) ?  __rt_iodev_default_value : 0;
__rt_cl_master_stack_size = DEFINED(__rt_cl_master_stack_size) ?  __rt_cl_master_stack_size : 0x400;
__rt_cl_slave_stack_size = DEFINED(__rt_cl_slave_stack_size) ?  __rt_cl_slave_stack_size : 0x400;
__rt_config = DEFINED(__rt_config) ?  __rt_config : 0x0;
__rt_debug_init_config = DEFINED(__rt_debug_init_config) ?  __rt_debug_init_config : 0x3;
__rt_debug_init_config_trace = DEFINED(__rt_debug_init_config_trace) ?  __rt_debug_init_config_trace : 0x0;


GROUP( -lgloss -lgcc )

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
    . = ALIGN(4);
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > L2_aliased AT> L2


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


  .stack : {
    . = ALIGN(4);
    . = ALIGN(16);
    stack_start = .;
    . = . + 0x800;
    stack = .;
  } > L2


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


  __l2_priv0_end = ALIGN(4);




  /*
   * L2 PRIVATE BANK1
   *
   * Contains FC code
   */

  .vectors MAX(0x1c008000,ALIGN(256)) :
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

  __l2_priv1_end = ALIGN(4);


  /*
   * L2 SHARED BANKS
   *
   * Contains other data such as peripheral data and cluster code and data
   */

  .text.cluster MAX(0x1c010000,ALIGN(4)) :
  {
    __cluster_text_start = .;
    *(.cluster.text)
    *(.cluster.text.*)
    __cluster_text_end = .;
    . = ALIGN(4);
  } > L2

  .l2_data : {
    . = ALIGN(4);
    *(.l2_data)
    *(.l2_data.*)
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    . = ALIGN(4);
  } > L2


  __l2_shared_end = LOADADDR(.l2_data) + SIZEOF(.l2_data);

}
