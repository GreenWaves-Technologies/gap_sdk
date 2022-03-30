from scipy.io.wavfile import read
import numpy as np
import matplotlib.pyplot as plt
import math
import librosa
import soundfile as sf
import json
import re 
import io
from librosa.core.spectrum import power_to_db
import scipy

def FIX2FP(Val, Prec):
    if np.isscalar(Val):
        return float(Val) / float(1<<Prec)
    else:
        return Val.astype(np.float32) / float(1<<Prec)

file_path = "samples/yes.wav"
data, samplerate = sf.read(file_path)

with open("MfccConfig.json", "r") as f:
    config = json.load(f)

frame_size = config['frame_size']
frame_step = config['frame_step']
n_fft = config['n_fft']
n_mels = config['n_mels']
fmin = config['fmin']
fmax = config['fmax']
use_power = False
ndct = config.get("n_dct", 0)
dtype = config.get('dtype', "int")
if dtype == "float16":
    np_dtype = np.float16
elif dtype == "float32":
    np_dtype = np.float32
else:
    np_dtype = np.int16

# FORWARD MEL
stft = librosa.core.spectrum.stft(data, n_fft, frame_step, frame_size, center=False, pad_mode="constant")
spect = np.abs(stft) ** (1 if not use_power else 2)
mel_basis = librosa.filters.mel(samplerate, n_fft, n_mels, fmin, fmax)
mel_spect = np.dot(mel_basis, spect)
logmel = power_to_db(mel_spect)
mfcc = scipy.fftpack.dct(logmel, axis=0, type=2, norm=None)

# Data from GAP
MelOutGap = np.fromfile("GapMel.bin", np_dtype).reshape(-1, n_mels)
if dtype not in ["float32", "float16"]:
    NORM = 6
    MelOutGap = FIX2FP(MelOutGap, 15-NORM-7)
n_frames = MelOutGap.shape[0]

lines = (n_frames+9)//10
columns = 10
fig1, axes1 = plt.subplots(lines, columns)
for i in range(n_frames):
    axes1.flat[i].plot(mfcc.T[i])
    axes1.flat[i].plot(MelOutGap[i])
fig1.suptitle('Forward gap vs librosa - Click on frame to see it', fontsize=16)

def create_fig1(evt):
    if evt.inaxes:
        newfig, newax = plt.subplots()
        num = fig1.axes.index(evt.inaxes)
        newax.plot(mfcc.T[num], label=f"Frame {num} - Librosa")
        newax.plot(MelOutGap[num], '--', label=f"Frame {num} - Gap")
        newax.legend()
        newfig.show()

fig1.canvas.mpl_connect("button_press_event", create_fig1)
plt.show()
