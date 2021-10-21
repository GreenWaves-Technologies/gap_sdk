import sys
import numpy as np
import scipy.io.wavfile as wav
import os
from base_MODEL_MFCC import *


winlen = 0.03
winstep = 0.01
ncoeff = 40
n_win = 98
dct_coeff_count = 40
batch_size = 10
directory = "images"
directory_wav = "audio_samples"

for filename in os.listdir(directory_wav):


    print ("***>", filename)
    name,ext = filename.split('.')
    (rate,sig) = wav.read(os.path.join(directory_wav,filename))
    sigint = (np.array(sig))

    tab2D = mfcc(sig,samplerate=rate,winlen=winlen,winstep=winstep,nfilt=ncoeff,numcep=dct_coeff_count, winfunc = numpy.hamming)


    # dump file in a 40*98 pgm image with  16bits pixels
    s_16b = np.array(ncoeff*n_win)
    #shift left by 6 bits
    s_16b = (tab2D*64+0.5).astype(int)

    if not os.path.exists(directory):
        os.makedirs(directory)
    with open(os.path.join(directory, name+".dat"), 'wb') as f:
        np.uint16(s_16b).tofile(f)
    with open(os.path.join(directory, name+".pgm"), 'wb') as f:
        hdr =  'P5' + '\n' + str(ncoeff) + '  ' + str(n_win) + '  ' + str(65535) + '\n'
        f.write(hdr.encode())
        np.uint16(s_16b).tofile(f)
    
