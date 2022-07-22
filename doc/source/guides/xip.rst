.. _guide_xip:

Using eXecute In Place to extend internal memory
================================================

.. warning::

    All instructions in this document are only valid with GAP9, **GAP8 has no
    support for eXecute In Place (XIP)**.

.. note::

    Data accesses are supported via XIP too, but at the cost of fairly
    increased latency, please only use it outside of critical paths.

Introduction
------------

GAP9 comes with XIP support by default. This can help you to use either internal
eMRAM flash or an external Flash/RAM as if it was standard internal memory.
This is particularly useful if your application is big (in terms of code), or
if you have a lot of support data that does not require high-speed accesses
(a device tree as an example).
This document will present both code only and code+data XIP usage.

Limitations
-----------


- It is **not** possible to write in flash (whether eMRAM flash or external
  flash) from XIP.

- When using XIP, actions that would lock it (closing the supporting device,
  reconfiguring XIP) can not be performed from XIP executed code. For those,
  some methods will need to be present in L2 memory.

Using XIP for code execution
----------------------------

Using XIP for code execution requires two things:

- First, flash application binary in a supported flash (eMRAM or select
  OctoSPI flash)
- Second, when running, the boot device and XIP config need to be given to
  the runner.

Both of those can be done with the following command:

.. code-block:: bash

    make clean all run CONFIG_XIP=1 CONFIG_BOOT_DEVICE=mram

:code:`CONFIG_XIP=1` instructs the runner to use XIP linker scripts and flash
generators. It will also set a compile-time macro with the same name in the
code, used to protect memory controller access in firmware.
:code:`CONFIG_BOOT_DEVICE=mram` instructs the runner to flash the code in
chosen flash, here the eMRAM flash.

And that's all, you now have XIP executed code running on board.
A sample linker script for XIP can be found in
:code:`$GAP_SDK_ROOT/rtos/freeRTOS/vendors/gwt/gap9/src/device/ld/GAP9_xip.ld`.
It is to be noted that the ldscript only sees XIP addresses, which are virtual.
There are no such things as eMRAM addresses, or external flash addresses,
as those are `not` memory mapped in GAP architecture. Outside of XIP, those are
only available via manual DMA accesses. As such, a binary compiled for XIP can
execute indifferently from any external memory, as long as XIP code is properly
configured to align :code:`0x20000000` virtual memory address with the binary starting point in
external memory.
If using standard ROM boot this is done automatically, if using an SSBL, the
SSBL programmer is responsible for it.


Using XIP for data accesses
---------------------------

Using XIP for data accesses can be done on any flash/ram device. However, it
can't be used to write in flashes (including internal eMRAM) and latency is
higher than an L2 access.
Thus, its main use case is for data that is not accessed in the application's
critical paths. A typical use case would be a device tree, or any data which won't be
intensively reused, and would thus only waste precious L2 space.
Another use case would be to load a full piece of foreign code (client or
provider binary blob).

In that case, the client or provider can compile their full application using XIP
base addresses, which the firmware may load in external RAM.
This, compiled with user-mode execution can allow full isolation of binaries
from the rest of the system, as well as easier binary blob distribution.


Using XIP on data is based on existing memory controller drivers
(eMRAM, Octospi notably), and an ad-hoc XIP driver.

On the memory controller driver, one only needs to add :code:`xip_en = 1` to the
configuration. Here is an example:

.. code-block:: c++

    pi_default_ram_conf_t ram_conf;
    pi_device_t ram;
    pi_default_ram_conf_init(&ram_conf);
    ram_conf.xip_en = 1; // activate xip support on the device
    pi_open_from_conf(&ram,&ram_conf);
    pi_flash_open(&ram);
    uint32_t ram_buffer = pi_ram_alloc(BUFFER_SIZE);

Here, XIP support has been requested, and a buffer in ram (ram physical address)
has been reserved.

Then, the XIP itself needs to be configured:

.. code-block:: c++

    uint32_t page_mask = 0;
    pi_device_t *xip_device = pmsis_l2_malloc(sizeof(pi_device_t));
    for (int i = 0; i<XIP_NB_DCACHE_PAGE; i++)
    {
        // Allocate L2 memory chunks for XIP pages
        pi_xip_dcache_page_alloc(i, XIP_PAGE_SIZE);
    }
    // request a certain number of those pages
    pi_xip_free_page_mask_get(&page_mask,XIP_NB_DCACHE_PAGE);
    pi_xip_conf_t xip_conf = {0};
    // ram is not read only
    xip_conf.ro = 0;
    // adapt depending on the interface, octospi0 is 0, octospi1 is 1, mram is 2
    xip_conf.per_id = XIP_DEVICE;
    // choose a page size (see xip driver documenation for more detail)
    xip_conf.page_size = XIP_PAGE_SIZE;
    // use the pages that were requested
    xip_conf.page_mask = page_mask;
    // Prepare a mount size, expressed as a number of pages
    // real page size is equal to PAGE_SIZE << 9 - smallest page size is 512B
    uint32_t mount_size =  BUFFER_SIZE / (PAGE_SIZE << 9);
    // now mount the area on desired memory controller, at first xip virtual address
    // as such, ram_buffer will be accssible via xip at address 0x20000000
    pi_xip_mount(xip_device,0x20000000,ram_buffer,mount_size,0)
    // access can then simply be done with:
    *((uint32_t*)0x20000000) = SOME_VALUE; // Warn: only possible to write in ram

Using two devices on the same interface
---------------------------------------

In the case where two devices (one on each available Chip Select) are used on
the same interface, some care needs to be taken.
Firstly, it is only possible to map one area on one device at a given time.
As such, two possibilities arise:

- Either map each area in turn (umount one, mount the other)
- Map the full devices together

In case both devices are mounted together, the mount will begin with external
address :code:`0x0`, and with a size of :code:`CS1_BASE + CS1_SIZE`, the base
being the mapping base address given as input for the device on CS1.
Due to a hardware limitation, :code:`CS1_BASE` must be a multiple of
:code:`CS1_SIZE`.
As such if :code:`CS1_BASE` is bigger than :code:`CS0_SIZE` there will be a
"hole" in the mapping between :code:`XIP_BASE+CS0_SIZE` and
:code:`XIP_BASE+CS1_BASE`.

