import sys
import numpy as np
import math
import librosa
import soundfile as sf
import json
from librosa.core.spectrum import power_to_db
import scipy

file_path = sys.argv[1]
data, samplerate = sf.read(file_path)
#data = np.clip(data*3, -1, 1)

with open("MfccConfig.json", "r") as f:
    config = json.load(f)

frame_size = config['frame_size']
frame_step = config['frame_step']
n_fft = config['n_fft']
n_mels = config['n_mels']
fmin = config['fmin']
fmax = config['fmax']
dtype = config.get('dtype', "int")
use_power = False
rad4 = round(math.log(n_fft//2, 4)) == math.log(n_fft//2, 4)
ndct = config.get('n_dct', False)

from librosa.filters import get_window
from librosa import util
librosa_fft_window = get_window("hann", frame_size, fftbins=True)
# Pad the window out to n_fft size
librosa_fft_window = util.pad_center(librosa_fft_window, n_fft)

stft = librosa.core.spectrum.stft(data, n_fft, frame_step, frame_size, center=False, pad_mode="constant")
spect = np.abs(stft) ** (1 if not use_power else 2)
mel_basis = librosa.filters.mel(samplerate, n_fft, n_mels, fmin, fmax)
mel_spect = np.dot(mel_basis, spect)
logmel = power_to_db(mel_spect, top_db=None)
mfcc = scipy.fftpack.dct(logmel, axis=0, type=2, norm=None)
with open("ground_truth.h", "w") as f:
    f.write(f"float ground_truth[] = {{\n")
    for elem in mfcc.T.flatten():
        f.write(f"{elem}, ")
    f.write("};\n")
