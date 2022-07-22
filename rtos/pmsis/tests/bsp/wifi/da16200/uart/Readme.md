# Test/Example uart with the WiFi DA16200 module

## Infos:

Since the DA16200 module keeps last uart configuration, especially baudrate, when powered down, this test/example changes the uart baudrate and put it back to the default value, 115200.

If you lose the current uart baudrate, run this test with differents values for `conf.uart_baudrate` before calling `pi_open_from_conf`. Possible values: 9600/19200/38400/57600/115200/230400/460800/921600. If the value is correct, the test will finish (and the current baudrate will be the default one, 115200), otherwhise, the test will never finish.

## Setup:

Connect DA16200 module to GAP9_EVK via the UART1 interface of the MikroeBUS.

### Pins used:

> TX/RX;<br>
> RST for DA16200 HW reset;<br>
> 3V3 and GND.<br>

If flow control (activated by default in software!):
> PWM for CTS, and CS for RTS;

## Build and launch:
```bash
cmake -B BUILD                             # Create BUILD folder
cmake --build BUILD/ --target menuconfig   # Manage configuration (flags, ...)
cmake --build BUILD/ --target run          # (Build if needed) and launch test
```
