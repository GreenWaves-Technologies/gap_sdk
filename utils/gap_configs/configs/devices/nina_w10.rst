Nina W10 model
--------------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             spim0             Optional
cs                  Chip select where the device is connected.           0                 Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.nina_w10]
  include = devices/nina_w10.json
  interface = spim0
  cs = 0