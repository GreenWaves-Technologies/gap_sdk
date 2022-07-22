# Compressor

# How to compile

`gcc -o Qenc --std=c99 Compress.c`

# How to use

Argument details: `./Qenc ?`

Generate in file Out.dat uncompressed and compressed (uniform quantized) data (20 of them) with 10 centroids, input data is 10 bit quantized, input is half word
`./Qenc -E 1 -C 10 -N 20 -Q 10 -S 2 -O Out.dat`
