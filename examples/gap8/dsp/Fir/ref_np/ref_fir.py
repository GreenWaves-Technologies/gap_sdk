import numpy
from numpy import cos, sin, pi, absolute, arange
from scipy.signal import kaiserord, lfilter, firwin, freqz
from pylab import figure, clf, plot, xlabel, ylabel, xlim, ylim, title, grid, axes, show


#------------------------------------------------
# Create a signal for demonstration.
#------------------------------------------------

sample_rate = 100.0
nsamples    = 32768
t = arange(nsamples) / sample_rate
x = cos(2*pi*0.5*t) + 0.2*sin(2*pi*2.5*t+0.1) + \
        0.2*sin(2*pi*15.3*t) + 0.1*sin(2*pi*16.7*t + 0.1) + \
            0.1*sin(2*pi*23.45*t+.8)


#------------------------------------------------
# Create a FIR filter and apply it to x.
#------------------------------------------------

# The Nyquist rate of the signal.
nyq_rate = sample_rate / 2.0

# The desired width of the transition from pass to stop,
# relative to the Nyquist rate.  We'll design the filter
# with a 5 Hz transition width.
width = 5.0/nyq_rate

# The desired attenuation in the stop band, in dB.
ripple_db = 60.0

# Compute the order and Kaiser parameter for the FIR filter.
N, beta = kaiserord(ripple_db, width)

# The cutoff frequency of the filter.
cutoff_hz = 10.0

# Use firwin with a Kaiser window to create a lowpass FIR filter.
taps10 = firwin(10,0.2, window=('kaiser', beta))
taps20 = firwin(20,0.2, window=('kaiser', beta))
taps40 = firwin(40,0.2, window=('kaiser', beta))

# Use lfilter to filter x with the FIR filter.
out10 = lfilter(taps10, 1.0, x)
out20 = lfilter(taps20, 1.0, x)
out40 = lfilter(taps40, 1.0, x)



#np.round(Twiddles_real * ((1<<FFT_TWIDDLE_DYN)-1)).astype(numpy.int), np.round(Twiddles_imag * ((1<<FFT_TWIDDLE_DYN)-1)).astype(np.int)

x_fix      = numpy.round(x      * ((1<<14)-1)).astype(numpy.int)
taps10_fix = numpy.round(taps10 * ((1<<14)-1)).astype(numpy.int)
taps20_fix = numpy.round(taps20 * ((1<<14)-1)).astype(numpy.int)
taps40_fix = numpy.round(taps40 * ((1<<14)-1)).astype(numpy.int)
out10_fix  = numpy.round(out10  * ((1<<14)-1)).astype(numpy.int)
out20_fix  = numpy.round(out20  * ((1<<14)-1)).astype(numpy.int)
out40_fix  = numpy.round(out40  * ((1<<14)-1)).astype(numpy.int)

# print(out10)
# print(out10_fix)

numpy.savetxt("../fir_in32768.h", [x_fix], fmt='%i',delimiter =',\n')
numpy.savetxt("../fir_coeff10.h", [taps10_fix], fmt='%i',delimiter =",\n")
numpy.savetxt("../fir_coeff20.h", [taps20_fix], fmt='%i',delimiter =",\n")
numpy.savetxt("../fir_coeff40.h", [taps40_fix], fmt='%i',delimiter =",\n")
numpy.savetxt("../fir_out10.h", [out10_fix], fmt='%i',delimiter =",\n")
numpy.savetxt("../fir_out20.h", [out20_fix], fmt='%i',delimiter =",\n")
numpy.savetxt("../fir_out40.h", [out40_fix], fmt='%i',delimiter =",\n")



# print("input signal")
# print(x.size)
# print(x)
# print("taps")
# print(taps.size)
# print(taps)
# print("Output signal")
# print(filtered_x.size)
# print(filtered_x)