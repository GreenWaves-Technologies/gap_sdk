import sounddevice as sd
from scipy.io.wavfile import write, read
import os
import sys

fs = 16000  # this is the frequency sampling; also: 4999, 64000
seconds = 1  # Duration of recording
 
myrecording = sd.rec(int(seconds * fs), samplerate=fs, channels=1, dtype='int16')
print("Starting: Speak now!")
sd.wait()  # Wait until recording is finished
print("finished")
print(myrecording)
write('from_pc/from_pc.wav', fs, myrecording)  # Save as WAV file
