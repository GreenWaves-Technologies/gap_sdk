import sys
import numpy as np
import scipy.io.wavfile as wav

print ("***> %s" %((sys.argv)[1]))
(rate,sig) = wav.read(sys.argv[1])
sigint = (np.array(sig))
np.savetxt("wav.txt",sigint.astype(int),fmt='%d',delimiter=",")
