# TX output(Mem -> Out)

This example shows how to use I2S interface to send data from GAP9.

## Configuration

The number of slots/channels used are configured using with `NB_CHANNELS` and `NB_ACTIVE_CHANNELS`.

Data are sent on TX using RX pin(default).

Optional :
* `PI_I2S_OPT_EXT_CLK` : use external clock for SCK/BCLK.
* `PI_I2S_OPT_EXT_WS` : use external clock for WS/LRCLK. This option is to be used be used with `PI_I2S_OPT_EXT_CLK`.
* `PI_I2S_OPT_FULL_DUPLEX` : RX and TX are on two different pins.
These options should be set before opening i2s device.

## FPGA connection

SCK, WS and TX pins should be linked to whatever device is at the other end.

External clocks should be provided to GAP9 on the right pins. Pins of each interfaces are listed below.

#### Full duplex mode
Default RX and TX are muxed on the same pin, RX. Therefore, data can not be sent and received at the same time.

Enabling full duplex mode allows to send and receive data at the same time using different pins for RX and TX.

#### Pin mapping
* I2S_0 :
  * `J4-5` : SCK/BCLK
  * `J4-4` : RX or TX(when not in full duplex mode)
  * `J4-3` : WS/FS
  * `J2-10` : TX in full duplex mode

* I2S_1 :
  * `J5-5` : SCK/BCLK
  * `J5-4` : RX or TX(when not in full duplex mode)
  * `J5-3` : WS/FS
  * `J2-12` : TX in full duplex mode

* I2S_2 :
   * `J6-5` : SCK/BCLK
   * `J6-4` : RX or TX(when not in full duplex mode)
   * `J6-3` : WS/FS
   * `J2-14` : TX in full duplex mode
