JTAG proxy
----------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
config.active       Activate the proxy if set to true (this will open    false             Optional
                    the socket)
config.port         Socket port number where another tool can connect to 37539             Optional
                    interact
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.jtag_proxy]
  include = devices/jtag_proxy.json
  config.port = 37539
