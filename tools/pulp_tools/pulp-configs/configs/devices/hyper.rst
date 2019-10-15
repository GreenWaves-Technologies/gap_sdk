Hyperchip
---------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             hyper0            Optional
cs                  Chip select where the device is connected.           0                 Optional
config.flash.size   Flash size in bytes                                  0x00800000        Optional
config.ram.size     Ram size in bytes                                    0x00800000        Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.hyper]
  include = devices/hyper.json
  interface = hyper0
  cs = 0
  config.ram.size = 0x00800000
  config.flash.size = 0x00800000
