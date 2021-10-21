#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import os
import pandas as pd
import re

L1_MEM_USAGE = re.compile(r'\s*(?P<column>Shared L1 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L2_MEM_USAGE = re.compile(r'\s*(?P<column>L2 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L3_MEM_USAGE = re.compile(r'\s*(?P<column>L3 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L3_MEM_BW = re.compile(r'(?P<column>L3 Memory bandwidth for 1 graph run)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
L2_MEM_BW = re.compile(r'(?P<column>L2 Memory bandwidth for 1 graph run)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
KER_ARGS = re.compile(r'(?P<column>Sum of all Kernels arguments size)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
TIL_OH = re.compile(r'(?P<column>Tiling Bandwith overhead)\s+\:\s+(?P<value>[0-9.]+)\s+Move/KerArgSize\n')
L2_MEM_BW_PER = re.compile(r'(?P<column>Percentage of baseline BW for L2)\s+\:\s+(?P<value>[0-9.]+)\s+\%\n')
L3_MEM_BW_PER = re.compile(r'(?P<column>Percentage of baseline BW for L3)\s+\:\s+(?P<value>[0-9.]+)\s+\%\n')
KER_OPS = re.compile(r'(?P<column>Sum of all Kernels operations)\s+\:\s+(?P<value>[0-9]+)\s+Operations\n')
TOT_COEFFS = re.compile(r'(?P<column>Total amount of flash coefficients)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')

matches = [L1_MEM_USAGE, L2_MEM_USAGE, L3_MEM_USAGE, L3_MEM_BW, L2_MEM_BW, TIL_OH, KER_ARGS, L2_MEM_BW_PER, L3_MEM_BW_PER, KER_OPS, TOT_COEFFS]
SIZES = ["SMALL", "MEDIUM", "LARGE"]

def main(size_name):
	size_flags = {}
	for s in SIZES:
		size_flags[s] = 1 if s == size_name else 0
	L2_dict = {}
	frames = [] 
	for L2_MEM in range(190, 380, 10):
		columns = []
		cmd = 'make at_model SMALL={} MEDIUM={} LARGE={} MODEL_L2_MEMORY={}'.format(size_flags["SMALL"], size_flags["MEDIUM"], size_flags["LARGE"], L2_MEM*1000)
		print(cmd)
		stream = os.popen(cmd)

		out_log = stream.readlines()

		#print(output[-24:-12])
		out_dict = {}
		row = []
		for line in out_log:
			for match in matches:
				m = match.search(line)
				if m:
					if "Memory size" in m['column']:
						columns.append(m['column'] + ' Given')
						row.append(float(m['given']))
						columns.append(m['column'] + ' Used')
						row.append(float(m['used']))
					else:
						columns.append(m['column'])
						row.append(float(m['value']))
					continue
		df1 = pd.DataFrame.from_dict({L2_MEM: row}, orient='index', columns=columns)
		frames.append(df1)

	df = pd.concat(frames, sort=False)
	normalized_df=(df-df.min())/(df.max()-df.min())

	df.to_csv("at_log/{}.csv".format(size_name))
	normalized_df.cumsum()
	normalized_df.plot()
	plt.xlabel('L2 MEM [KB]')
	plt.ylabel('normalized between min-max')

	plt.show()

if __name__ == "__main__":
	if len(sys.argv) < 2:
		raise ValueError("should provide size")
	size = sys.argv[1]
	if not size in SIZES:
		raise ValueError("Size must be in ", SIZES)

	main(size)
