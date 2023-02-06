# FFT on GAP

This project contains a benchmark of the FFT for GAP processors. It uses the FFT_Library.c contained in the Autotiler.

By default it runs the complex FFT with 64, 128, 256, 512, 1024 and 2048 bins in FP32, Fixed point 16 bits and, if target is GAP9, also FP16.

Input data are synthetically generated in InitData.c as 4 cosine summed together. The code measures the MeanSquared Error for each run.

## Usage

To run from 64 to 2048 bins FFT in all Data types:

	make clean all run

To run only specific FFT dimensions, i.e. 128:

	make clean all run MINDIM=128 MAXDIM=128

To run specific data type and specific dimension (DATA_TYPE: 0: FP32, 1: Fixed point 16, 2: FP16). In this mode GAP9 can run also MAXDIM=4096 FFT:

	make clean all run MINDIM=128 MAXDIM=128 DATA_TYPE=1

If you want to debug specific input data you can replace the In_Data.h with what you want and change the code accordingly. In FFTRunTest.c define DEBUG=1 to print the input output of each FFT that you can copy and paste in a python script (with numpy) and look at the waveforms, i.e. Test.ipynb.

## Emul Mode

The application can be compiled for x86 (faster execution and no memory limits) with Emul.mk:

	make -f Emul.mk clean all [FLAGS as before]
	./fft_emul
