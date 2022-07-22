# Test the connection between the WiFi DA16200 module and a phone

## Infos:

TCP connection;<br>
Server: DA16200 / Client: phone.

## Setup:

Connect DA16200 module to GAP9_EVK via the UART1 interface of the MikroeBUS.

### Pins used:

> TX/RX;<br>
> RST for DA16200 HW reset;<br>
> 3V3 and GND.<br>

If flow control (activated by default in software!):
> PWM for CTS, and CS for RTS;

### Sockets:

Open an access point, and connect devices on it. **Check addresses and ports !** 

## Build and launch:
```bash
cmake -B BUILD                             # Create BUILD folder
cmake --build BUILD/ --target menuconfig   # Manage configuration (flags, ...)
cmake --build BUILD/ --target run          # (Build if needed) and launch test
```
