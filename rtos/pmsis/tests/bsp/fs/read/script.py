#!/usr/bin/env python3

with open("flash_file_0.bin", 'wb') as file:
    for i in range(0, 1024*16):
        file.write((i & 0x7f).to_bytes(1, byteorder='big'))

with open("flash_file_1.bin", 'wb') as file:
    for i in range(0, 1024*16):
        file.write(((i & 0x7f) | 0x80).to_bytes(1, byteorder='big'))
