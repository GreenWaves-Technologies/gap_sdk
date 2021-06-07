.. _guide_app_makefile_options:

Application Makefile options
============================

Generic options
---------------

To compile and run your application, use ``make clean all run``.
You can also add options to this command.
Here is a description of these options:

- ``platform=<option>`` is used to change the platform running the application.
  ``<option>`` includes:

  - ``gvsoc`` to run your application on GVSoC
  - ``board`` to run your application on a board

- ``PMSIS_OS=<os_name>`` is used to change the OS running the application.
  ``<os_name>`` includes:

  - ``pulpos``
  - ``freertos``

- ``GAPY_OPENOCD_CABLE=<cable>`` to change the adapter used by OpenOCD to connect to the board.
  ``<cable>`` includes but not limited to:

  - ``interface/ftdi/olimex-arm-usb-ocd-h.cfg``
  - ``interface/jlink.cfg``
  - ``interface/ftdi/sipeed-rv-debugger.cfg``

- ``io=<option>`` to change printf output type. ``<option>`` includes:

  - ``host`` to use JTAG output
  - ``uart`` to use UART output

- ``runner_args=<options>`` to specify arguments to the platform runner. Several arguments
  can be specified. Check the documentation of the platform to know the available arguments.

- ``config_args=<options>`` to specify options to customizer the platform configuration.
  Check the documentation of the platform to know the available options.

- ``LINK_SCRIPT=<path>`` to specify a specific link script.

- ``runner_args="<options>"`` to pass runner specific options.

- ``CONFIG_BOOT_DEVICE=<value>`` to specify the device from which to boot. ``<value>`` includes:

  - ``spiflash`` to boot from SPI flash
  - ``hyperflash`` to boot from hyperflash
  - ``mram`` to boot from mram
  - ``spislave`` to boot from SPI slave


Gap9 options
------------

- ``CONFIG_XIP=<value>`` to choose booting with xip or not. ``<value>`` includes:

  - ``1`` to boot using XIP
  - ``0`` to boot using standard L2 boot

- ``CONFIG_BOOT_DEVICE_FREQUENCY=<value>`` to specify the frequency used for booting from flash device. ``<value>``
  gives the frequency in Hz.

- ``CONFIG_FREQUENCY_PERIPH=<value>`` to specify the frequency for the peripheral clock domain. ``<value>``
  gives the frequency in Hz.

- ``CONFIG_FREQUENCY_FC=<value>`` to specify the frequency for the soc clock domain. ``<value>``
  gives the frequency in Hz.

- ``CONFIG_FREQUENCY_CLUSTER=<value>`` to specify the frequency for the cluster clock domain. ``<value>``
  gives the frequency in Hz.

- ``CONFIG_FREQUENCY_SFU=<value>`` to specify the frequency for the sfu clock domain. ``<value>``
  gives the frequency in Hz.


GVSOC options
-------------


- ``GV_PROXY=1`` to activate GVSOC telnet proxy to control it remotly.

- ``GV_PROXY_PORT=<value>`` to specify GVSOC telnet proxy port (default is 42951).

- ``GV_DEBUG_MODE=1`` to activate GVSOC debug mode, which is required to allow trace activation.
  This is by default disabled, to simulate at maximum speed, and is automatically enabled
  when traces are enabled on the command-line. This mode can be enabled manually with this
  option when traces are enabled dynamically.
