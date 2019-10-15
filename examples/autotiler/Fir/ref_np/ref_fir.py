
from __future__ import print_function
import time

import numpy as np
from numpy import convolve as np_convolve
from scipy.signal import fftconvolve, lfilter, firwin
from scipy.signal import convolve as sig_convolve
from scipy.ndimage import convolve1d
import matplotlib.pyplot as plt

# fixed point accuracy (frac)
nbit_frac = 15
#In [10]:
# Create the m by n data to be filtered.
m = 1
n = 2 ** 15
np.random.seed(0)
x = np.random.random(size=(m, n))

npconv_time = []

#ntaps_list = 2 ** np.arange(2, 6)
ntaps_list = {40,20,10}

for ntaps in ntaps_list:
    # Create a FIR filter.
    b = np.array(firwin(ntaps, [0.05, 0.48], width=0.05, pass_zero=False))

    # --- numpy.convolve ---
    tstart = time.time()
    npconv_result = np.array([np_convolve(xi, b, mode='valid') for xi in x])
    npconv_time.append(time.time() - tstart)

    in_fp = (x * (2**nbit_frac - 1)) + 0.5
    # coeffs are multiplied by 2: for example FP15*FP16 => FP31 if results are truncated to 16bits => FP15
    coeff_fp = (b * (2**(nbit_frac+1) - 1)) + 0.5
    out_fp = (npconv_result * (2**nbit_frac - 1)) + 0.5
    format = "%d"
#    format = '%d'

    np.savetxt("fir_in" + str(n) + ".h", in_fp, fmt=format, delimiter=",\n")
    np.savetxt("fir_out" + str(ntaps) + ".h", out_fp, fmt=format, delimiter=",\n")
    np.savetxt("fir_coeff" + str(ntaps) + ".h", coeff_fp, fmt=format, newline=",\n")
