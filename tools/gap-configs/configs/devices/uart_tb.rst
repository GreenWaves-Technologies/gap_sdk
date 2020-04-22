Uart model
----------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             uart0             Optional
config.baudrate     Baudrate used by the model                           625000            Optional
config.loopback     Connect TX to RX if set to true                      false             Optional
config.stdout       Dump received characters to terminal stdout if set   true              Optional
                    to true, otherwise
config.tx_file      Dump received characters to the specified file if it tx_uart.log       Optional
                    is not an empty string
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.uart]
  include = devices/uart_tb.json
  interface = uart0
  config.baudrate = 625000
  config.loopback = false
  config.stdout = true
  config.tx_file = tx_uart.log
