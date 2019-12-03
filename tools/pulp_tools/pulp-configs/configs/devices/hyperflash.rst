Hyperflash
----------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             hyper0            Optional
cs                  Chip select where the device is connected.           0                 Optional
config.size         Flash size in bytes                                  0x00800000        Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.hyperflash]
  include = devices/hyperflash.json
  interface = hyper0
  cs = 0
  config.size = 0x00800000
