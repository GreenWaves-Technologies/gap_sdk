import os
import sys
import argparse

import re
import numpy as np

parser = argparse.ArgumentParser(description='Evaluate in emul mode for some images')
parser.add_argument('--head_thr', type=float, default=0.5, help='head/no head probability threshold')
parser.add_argument('--look_thr', type=float, default=0.5, help='look/no look probability threshold')
parser.add_argument('--model_info_header', default='BUILD_MODEL_SQ8BIT/vww17Info.h', help='model prefix')

args = parser.parse_args()
SCALE_HEAD = re.compile(r'#define S68_Op_output_1_OUT_SCALE\t(?P<scale_head>[0-9.]+)')
SCALE_LOOK = re.compile(r'#define S72_Op_output_2_OUT_SCALE\t(?P<scale_look>[0-9.]+)')
with open(args.model_info_header, "r") as f:
	for line in f.readlines():
		m_head = SCALE_HEAD.search(line)
		m_look = SCALE_LOOK.search(line)
		if m_head:
			head_scale = float(m_head['scale_head'])
		if m_look:
			look_scale = float(m_look['scale_look'])

	out_lin_thr_head = np.round(np.log(args.head_thr / (1 - args.head_thr)) / head_scale)
	out_lin_thr_look = np.round(np.log(args.look_thr / (1 - args.look_thr)) / look_scale)

print("\n\nThis are the thresholds converted that should be copy and pasted in the main.c code:")
print("#define HEAD_DETECTOR_THRESHOLD\t{} //({})".format(int(out_lin_thr_head), args.head_thr))
print("#define LOOK_THRESHOLD\t{} //({})\n\n\n".format(int(out_lin_thr_look), args.look_thr))
