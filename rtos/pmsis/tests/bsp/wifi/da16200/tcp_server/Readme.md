# Test WiFi DA16200 in TCP server mode

## Infos:

TCP connection;<br>
Server: DA16200 / Client: PC (cf *tcp_client.py* file).

## Setup:

Connect DA16200 module to GAP9_EVK via the UART1 interface of the MikroeBUS.

### Pins used:

> TX/RX;<br>
> RST for DA16200 HW reset;<br>
> 3V3 and GND.<br>

If flow control:
> PWM for CTS, and CS for RTS;

### Sockets:

Open an access point, and connect devices on it. **Check addresses and ports !** 

## Build and launch:

### 1. Client

```
python3 tcp_client.py
```

### 2. Test

```
make all run
```
