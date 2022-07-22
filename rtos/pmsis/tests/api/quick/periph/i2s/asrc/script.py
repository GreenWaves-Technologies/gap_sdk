#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack
import argparse

width = 24


parser = argparse.ArgumentParser(description='Check TX buffer')

parser.add_argument("--frequency", dest="frequency", default=None, type=int, help="Expected frequency")
parser.add_argument("--sample-rate", dest="sampling_rate", default=None, type=int, help="Sample rate")
parser.add_argument("--file", dest="file", default=None, help="File containing the values to check")

args = parser.parse_args()



def sign_extend(value, bits):
    sign_bit = 1 << (bits - 1)
    return (value & (sign_bit - 1)) - (value & sign_bit)


def check_freq(file, sample_freq, expected, margin=0.1):

    data = []
    drop_first_zeros = True

    with open(file, 'r') as fp:

        for line in fp.readlines():

            value = int(line, 0)

            if drop_first_zeros:
                if value == 0:
                    continue
                else:
                    drop_first_zeros = False

            value = sign_extend(value, 24)

            data.append(value)


    data = np.array(data, np.int32)

    w = np.fft.fft(data)
    freqs = np.fft.fftfreq(len(w))

    #print(freqs.min(), freqs.max())

    # Find the peak in the coefficients

    idx = np.argmax(np.abs(w))
    freq = freqs[idx]
    freq_in_hertz = abs(freq * sample_freq)

    if freq_in_hertz != 0:
        error = np.abs(expected - freq_in_hertz) / freq_in_hertz
    else:
        error = 1


    if False:
        # Number of samplepoints
        N = 600
        # sample spacing
        T = 1.0 / 800.0
        x = np.linspace(0.0, N*T, N)
        y = np.sin(50.0 * 2.0*np.pi*x) + 0.5*np.sin(80.0 * 2.0*np.pi*x)
        xf = np.linspace(0.0, 1.0/(2.0*T), N/2)

        fig, ax = plt.subplots()
        ax.plot(xf, 2.0/N * np.abs(w[:N//2]))
        plt.show()

    print("Required frequency: " + str(expected) + '\n')
    print("Extracted frequency: " + str(freq_in_hertz) + '\n')
    print("Required error: " + str(margin) + '\n')
    print("Got error: " + str(error) + '\n')

    return error < margin


retval = check_freq(args.file, args.sampling_rate, args.frequency)

if retval:
    print ('CHECK SUCCESS')
    exit(0)

print ('CHECK FAILED')

exit(1)