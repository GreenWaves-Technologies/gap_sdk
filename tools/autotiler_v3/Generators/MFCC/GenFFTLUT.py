import math
from SetupLUT import FP2FIX, SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4

swap_c_string = "#include \"Gap.h\"\n"
swap_h_string = "\n"
twid_c_string = "#include \"Gap.h\"\n"
twid_h_string = "\n"
for dtype in ["short int", "f16", "float"]:
	head = ""
	tail = ""
	if dtype == "f16":
		head = "#ifdef __gap9__\n"
		tail = "#endif\n"
	twid_c_string += head + f"/* FFT Twiddles for {dtype} data type */\n"
	twid_h_string += head + f"/* FFT Twiddles for {dtype} data type */\n"
	for nfft in [16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192]:
		d = "fix" if dtype == "short int" else dtype
		twid_h_string += f"extern {dtype} R2_Twiddles_{d}_{nfft}[{nfft}];\n"
		twid_c_string += f"L2_MEM {dtype} R2_Twiddles_{d}_{nfft}[{nfft}] = {{\n"
		swap_h_string += f"extern short int R2_SwapTable_{d}_{nfft}[{nfft}];\n"
		swap_c_string += f"L2_MEM short int R2_SwapTable_{d}_{nfft}[{nfft}] = {{\n"

		SwapTable = SetupSwapTable(nfft);
		Twiddles_cos, Twiddles_sin = SetupTwiddlesLUT(nfft, dtype=dtype)
		for i in range(len(Twiddles_cos[:nfft//2])):
			if dtype == "short int":
				twid_c_string += "\t{}, {}, \n".format(FP2FIX(Twiddles_cos[i], 15), FP2FIX(Twiddles_sin[i], 15))
			else:
				twid_c_string += "\t{}, {}, \n".format(Twiddles_cos[i], Twiddles_sin[i])
		for i in range(len(SwapTable)//2):
			swap_c_string += "\t{}, {}, \n".format(int(SwapTable[2*i]), int(SwapTable[2*i+1]))
		twid_c_string += "};\n"
		swap_c_string += "};\n"

		if round(math.log(nfft, 4)) == math.log(nfft, 4):
			twid_h_string += f"extern {dtype} R4_Twiddles_{d}_{nfft}[{int(2*3/4*nfft)}];\n"
			twid_c_string += f"L2_MEM {dtype} R4_Twiddles_{d}_{nfft}[{int(2*3/4*nfft)}] = {{\n"
			swap_h_string += f"extern short int R4_SwapTable_{d}_{nfft}[{nfft}];\n"
			swap_c_string += f"L2_MEM short int R4_SwapTable_{d}_{nfft}[{nfft}] = {{\n"
			SwapTable = SetupSwapTableR4(nfft)
			for i in range(len(Twiddles_cos[:int(3/4*nfft)])):
				if dtype == "short int":
					twid_c_string += "\t{}, {}, \n".format(FP2FIX(Twiddles_cos[i], 15), FP2FIX(Twiddles_sin[i], 15))
				else:
					twid_c_string += "\t{}, {}, \n".format(Twiddles_cos[i], Twiddles_sin[i])
			for i in range(len(SwapTable)//2):
				swap_c_string += "\t{}, {}, \n".format(int(SwapTable[2*i]), int(SwapTable[2*i+1]))
			twid_c_string += "};\n"
			swap_c_string += "};\n"	
	twid_c_string += tail
	twid_h_string += tail


with open("SwapTablesDef.c", "w") as f:
	f.write(swap_c_string)
with open("SwapTablesDef.h", "w") as f:
	f.write(swap_h_string)

with open("TwiddlesDef.c", "w") as f:
	f.write(twid_c_string)
with open("TwiddlesDef.h", "w") as f:
	f.write(twid_h_string)
