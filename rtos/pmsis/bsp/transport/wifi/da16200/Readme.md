# Informations about WiFi DA16200 module


### Hardware setup
For UART interface, the firmware flashed uses the following pins:

- 3V3 and GND;
- CS for TX, and SCK for RX;
- TX for RTS, and RX for CTS.

### Notes

- Flow control is activated by default in software !

- If module doesn't send startup message, pay attention to the uart baudrate. It can be the source of the issue. Indeed, if any former test change it, it doesn't go back to the default one (115200). You have to retreive the actual baudrate then.
