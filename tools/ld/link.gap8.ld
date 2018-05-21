
OUTPUT_ARCH(riscv)
ENTRY( _start )
MEMORY
{
  L2                 : ORIGIN = 0x1c000000, LENGTH = 0x00080000
  fc_tcdm            : ORIGIN = 0x1b000000, LENGTH = 0x00004000
  fc_tcdm_aliased    : ORIGIN = 0x00000004, LENGTH = 0x00003ffc
  L1                 : ORIGIN = 0x10000004, LENGTH = 0x00010000
  L1_aliased         : ORIGIN = 0x00000004, LENGTH = 0x0000fffc
}

SECTIONS
{
  .vectors :
  {
    . = ALIGN(4);
    __irq_vector_base = .;KEEP(*(.vectors))
  } > L2

  .text :
  {
    . = ALIGN(4);
    _stext = .;
    *(.text._start)
    *(.text)
    *(.text.*)
    _etext  =  .;
    *(.lit)
    *(.shdata)
    _endtext = .;
  } > L2

  .l1FcTiny : AT(ORIGIN(fc_tcdm))
  {
    . = ALIGN(4);
    _l1FcShared_start = .;
    *(.l1FcTiny)
    *(.l1FcTiny.*)
    *(.fcTcdmTiny)
    *(.fcTcdmTiny.*)
    . = ALIGN(4);
    _l1FcShared_end = .;
  } > fc_tcdm_aliased

  .data_tiny_fc (ADDR(.l1FcTiny) + SIZEOF(.l1FcTiny)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny))
  {
    . = ALIGN(4);
    *(.data_tiny_fc)
    *(.data_tiny_fc.*)
  } > fc_tcdm_aliased

  .cluster.text :
  {
    . = ALIGN(4);
    __cluster_text_start = .;
    *(.cluster.text)
    *(.cluster.text.*)
    __cluster_text_end = .;
  } > L2

  __fc_code_end = ALIGN(8);

  .init (ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc))
  {
    . = ALIGN(4);
    KEEP( *(.init) )
  } > fc_tcdm

  .fini (ADDR(.init) + SIZEOF(.init)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init))
  {
    . = ALIGN(4);
    KEEP( *(.fini) )
  } > fc_tcdm

  .preinit_array (ADDR(.fini) + SIZEOF(.fini)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini))
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > fc_tcdm

  .init_array (ADDR(.preinit_array) + SIZEOF(.preinit_array)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array))
  {
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
  } > fc_tcdm

  .fini_array (ADDR(.init_array) + SIZEOF(.init_array)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array))
  {
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
  } > fc_tcdm

  .boot (ADDR(.fini_array) + SIZEOF(.fini_array)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array))
  {
    . = ALIGN(4);
    *(.boot)
    *(.boot.data)
  } > fc_tcdm

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata);
    *(.rodata.*)
    *(.srodata);
    *(.srodata.*)
    *(.eh_frame*)
  } > L2

  .got (ADDR(.boot) + SIZEOF(.boot)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot))
  {
    . = ALIGN(4);
    *(.got.plt) * (.igot.plt) *(.got) *(.igot)
  } > fc_tcdm

  .shbss (ADDR(.got) + SIZEOF(.got)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got))
  {
    . = ALIGN(4);
    *(.shbss)
  } > fc_tcdm

  .talias (ADDR(.shbss) + SIZEOF(.shbss)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss))
  {
  } > fc_tcdm

  .gnu.offload_funcs (ADDR(.talias) + SIZEOF(.talias)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias))
  {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_funcs))
  } > fc_tcdm

  .gnu.offload_vars (ADDR(.gnu.offload_funcs) + SIZEOF(.gnu.offload_funcs)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs))
  {
    . = ALIGN(4);
    KEEP(*(.gnu.offload_vars))
  } > fc_tcdm

  .stack (ADDR(.gnu.offload_vars) + SIZEOF(.gnu.offload_vars)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars))
  {
    . = ALIGN(4);
    . = ALIGN(16);
    . = . + 0x800;
    stack = .;
  } > fc_tcdm

  .data_fc (ADDR(.stack) + SIZEOF(.stack)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack))
  {
    . = ALIGN(4);
    *(.data_fc)
    *(.data_fc.*)
  } > fc_tcdm

  .data_fc_shared (ADDR(.data_fc) + SIZEOF(.data_fc)): AT(ORIGIN(fc_tcdm) + SIZEOF(.l1FcTiny) + SIZEOF(.data_tiny_fc) + SIZEOF(.init) + SIZEOF(.fini) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array) + SIZEOF(.boot) + SIZEOF(.got) + SIZEOF(.shbss) + SIZEOF(.talias) + SIZEOF(.gnu.offload_funcs) + SIZEOF(.gnu.offload_vars) + SIZEOF(.stack) + SIZEOF(.data_fc))
  {
    . = ALIGN(4);
    *(.data_fc_shared)
    *(.data_fc_shared.*)
    __fc_tcdm_end = ABSOLUTE(.);
  } > fc_tcdm

  .data_tiny_l1 : AT(_l1_preload_start_inL2)
  {
    . = ALIGN(4);
    _l1_preload_start = .;
    *(.data_tiny_l1)
    *(.data_tiny_l1.*)
    *(.data_alias_l1)
    *(.data_alias_l1.*)
  } > L1_aliased

  .tls (ADDR(.data_tiny_l1) + SIZEOF(.data_tiny_l1)): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1))
  {
    . = ALIGN(4);
    _tls_start = .;
    *(.tls)
    *(.tls.*)
    . = ALIGN(4);
    _tls_end = .;
  } > L1_aliased

  .heapsram (ADDR(.tls) + SIZEOF(.tls) * __NB_ACTIVE_PE): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE)
  {
    . = ALIGN(4);
    *(.heapsram)
    *(.heapsram.*)
  } > L1_aliased

  .l1cluster_g (ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram)): AT(_l1_preload_start_inL2 + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram))
  {
    . = ALIGN(4);
    *(.l1cluster_g)
    *(.l1cluster_g.*)
    *(.data_l1)
    *(.data_l1.*)
    . = ALIGN(4);
    _libgomp_start = .;
    *(.libgomp)
    *(.libgomp.*)
    . = ALIGN(4);
    _heapsram_start = .;
  } > L1

  __l1_heap_start = ALIGN(4);

  __l1_heap_size = LENGTH(L1) - __l1_heap_start + ORIGIN(L1);

  _heapsram_size = LENGTH(L1) - _heapsram_start + ORIGIN(L1);

  _l1_preload_size = SIZEOF(.data_tiny_l1) + SIZEOF(.tls) + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g);

  .bss_l1 (ADDR(.l1cluster_g) + SIZEOF(.l1cluster_g)): AT(ORIGIN(L1) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g))
  {
    . = ALIGN(4);
    *(.bss_l1)
    *(.bss_l1.*)
    _heapsram_start = .;
    __L1_end = ABSOLUTE(.);
  } > L1

  .data_tiny_l1_s (ADDR(.rodata) + SIZEOF(.rodata)):
  {
    . = ALIGN(4);
    _l1_preload_start_inL2 = .;
  } > L2

  .tls_s (ADDR(.rodata) + SIZEOF(.rodata) + SIZEOF(.data_tiny_l1)):
  {
  } > L2

  .heapsram_s (ADDR(.rodata) + SIZEOF(.rodata) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE):
  {
  } > L2

  .l1cluster_g_s (ADDR(.rodata) + SIZEOF(.rodata) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram)):
  {
  } > L2

  .data (ADDR(.rodata) + SIZEOF(.rodata) + SIZEOF(.data_tiny_l1) + SIZEOF(.tls) * __NB_ACTIVE_PE + SIZEOF(.heapsram) + SIZEOF(.l1cluster_g)):
  {
    . = ALIGN(4);
    sdata  =  .;
    _sdata  =  .;
    *(.data);
    *(.data.*)
    *(.sdata);
    *(.sdata.*)
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .;
  } > L2

  .heapl2ram (ADDR(.data) + SIZEOF(.data)):
  {
    . = ALIGN(4);
    *(.heapl2ram)
    *(.fcTcdm)
    *(.fcTcdm.*)
    *(.fcTcdm_g)
    *(.fcTcdm_g.*)
  } > L2

  .bss (ADDR(.heapl2ram) + SIZEOF(.heapl2ram)):
  {
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

  .shared (ADDR(.bss) + SIZEOF(.bss)):
  {
    . = ALIGN(4);
    *(.l2_shared_data)
    *(.l2_shared_data.*)
    __L2_end = ABSOLUTE(.);
  } > L2

  __l2_data_end = ALIGN(8);

  __l2_heap_start = ALIGN(4);

  __l2_heap_size = LENGTH(L2) - __l2_heap_start + ORIGIN(L2);

  __fc_tcdm_heap_start = __fc_tcdm_end;

  __fc_tcdm_heap_size = LENGTH(fc_tcdm) - __fc_tcdm_heap_start + ORIGIN(fc_tcdm);

}
