import os
import json
import numpy as np
import sys
import argparse
from SetupLUT import FP2FIX, SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4, SetupDCTTable, SetupLiftCoeff, MFCC_COEFF_DYN


def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='mfcc_lut_gen')

	parser.add_argument('--params_json', default=None,
						help="Path to .json file where to get parameters")
	parser.add_argument('--win_lut_file', required="--params_json" not in sys.argv,
						help="path to fft lut file")
	parser.add_argument('--win_func', default="hanning", type=str,
						help="numpy window function (e.g. hanning)")
	parser.add_argument('--frame_size', required="--params_json" not in sys.argv, type=int,
						help="size in number of samples of one frame")
	parser.add_argument('--dtype', default="int")
	return parser

def main():
	parser = create_parser()
	args = parser.parse_args()

	if args.params_json:
		with open(args.params_json, "r") as f:
			models_params = json.load(f)
	else:
		models_params = {}
	print(models_params)

	win_lut_file     = args.win_lut_file     if not "win_lut_file"	   in models_params else models_params["win_lut_file"]
	frame_size       = args.frame_size       if not "frame_size"	   in models_params else models_params["frame_size"]
	window_fn        = args.win_func         if not "win_func"		   in models_params else models_params["win_func"]
	dtype     		 = args.dtype 		     if not "dtype"	   		   in models_params else models_params["dtype"]

	if dtype == "int":
		data_type = "short int"
	elif dtype == "float16":
		data_type = "F16_DSP"
	elif dtype == "float32":
		data_type = "float"
	else:
		raise NotImplemented(f"dtype = {dtype} not implemeted, available 'int' 'float32' 'foat16'")

	print(dtype)
	win_func = getattr(np, window_fn)
	if dtype == "int":
		Window = (win_func(frame_size) * 2**(15)).astype(np.int16)
	else:
		Window = win_func(frame_size).astype(np.float16)

	################################ WRITE TO FILE #######################################
	Out_str = ""
	# Window
	win_name = os.path.splitext(os.path.split(win_lut_file)[1])[0]
	Out_str += f"PI_L2 {data_type} {win_name}[{frame_size}] = {{\n\t"
	for i, elem in enumerate(Window):
		Out_str += str(elem) + ", "
		if (i+1)%12 == 0:
			Out_str += "\n\t"
	Out_str += "\n}; \n"

	with open(win_lut_file, 'w') as f:
		f.write(Out_str)

if __name__ == "__main__":
	main()
