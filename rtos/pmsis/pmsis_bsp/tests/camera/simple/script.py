#!/usr/bin/env python3

with open("imgTest0.pgm", 'wb') as file:
    file.write(b'P5\n')
    file.write(b'324 244\n')
    file.write(b'255\n')
    for i in range(0, 324*244):
        file.write((i & 0x7f).to_bytes(1, byteorder='big'))

with open("imgTest1.pgm", 'wb') as file:
    file.write(b'P5\n')
    file.write(b'324 244\n')
    file.write(b'255\n')
    for i in range(0, 324*244):
        file.write(((i | 0x80) & 0xff).to_bytes(1, byteorder='big'))
