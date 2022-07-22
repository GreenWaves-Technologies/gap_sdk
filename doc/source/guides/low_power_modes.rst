.. _guide_low_power_modes:

Using low power modes
=====================

.. warning::

    This guide is **only for GAP9**.

.. note::

    To have more details about low-power modes, please refer to GAP9 datasheet.

One of GAP9 goals is to have an ultra-low power consumption.
As such, the chip has a bunch of low power modes, including:

- Idle mode: A low-overhead mode to save power if no activity will occur during
  short amount of time.
- Light-sleep: Most power domains are turned off except for memory banks and
  IOs.
- Deep-Sleep Retentive: Every power domain is turned off, but some L2 memory
  banks can be saved depending on configuration.
- Deep-Sleep: Every power domain and memory bank is turned off.

Low power modes
---------------

Idle mode
"""""""""

.. warning::

   Not yet supported on FreeRTOS.

This mode is not exactly a sleep mode (the only wakeup source is a counter).
It only gates GAP9 clocks for some time in order to save power.
It reduces the overhead to enter/exit this mode but limits power-savings.

It only takes a couple of µs to do a full sleep-wakeup cycle.

You can see how to use this power mode in the :ref:`Idle mode example <example_power_idle_mode>`.

Light-Sleep
"""""""""""

.. warning::

   Not yet supported.

The chips power downs, keeps data in L2 memory banks and switchable IOs depending on
existing configuration.

It only takes a couple of hundred µs to do a full sleep-wakeup cycle.

You can see how to use this power mode in ``TODO this example``.

Deep-Sleep Retentive
""""""""""""""""""""

.. warning::

   Not yet supported on FreeRTOS.

The chip powers down but keeps data in L2 memory banks depending on configuration.
You can select which memory bank to keep and which one you want to turn off.
Power consumption in this mode depends on the configuration. The more memory you keep
retentive the more power consumed.

When waking-up from this mode all hardware configuration is lost.
Peripherals need to be reopened.

It takes about 1.2ms to do a full sleep-wakeup cycle.

You can see how to use this power mode in ``TODO this example``.

Deep-sleep
""""""""""

The chip completely powers down, including memory banks.

When waking-up from this mode all hardware configuration is lost.
Peripherals need to be reopened.

It takes about 2.5ms to do a full sleep-wakeup cycle.

You can see how to use this power mode in the :ref:`Deep Sleep example <example_power_deep_sleep>`.

Wakeup sources
--------------

In sleep mode, the wakeup process can be triggered from multiple sources.

Counter
"""""""

Before going to sleep, a counter is set to a determined amount of time.
Once in sleep mode this counter will increment with each reference clock cycle.
And when this counter is reached the wakeup sequence will start.

You can see how to use this wakeup source in ``TODO this example``.

RTC
"""

When the 32kHz slow clock is available, it can be configured to trigger interrupts at precise times over
long ranges to wake-up the chip.

You can see how to use this wakeup source in the :ref:`Deep Sleep example <example_power_deep_sleep>`.

SPI
"""

An SPI-slave can be set to listen during sleep mode for a wakeup command
(``0x3F``). Once this command is received the wakeup process will start.
To read the wakeup status, the master should send the status command (``0x05``).
This status will be 0 until GAP9 wakes up and turns it to 1.
To complete the wakeup and allows GAP9 to return to a nominal behavior the
master should send an handshake command (``0x71``).

You can see how to use this wakeup source in ``TODO this example``.

GPIO
""""

A GPIO can be set to trigger the wakeup on a specific event.
Once the event happens, the chip will wakeup.

The GPIOs that can be used as wakeup sources are detailed in the datasheet.

You can see how to use this wakeup source in ``TODO this example``.
