from scipy.io.wavfile import read
import numpy as np
#%matplotlib widget
import matplotlib.pyplot as plt
import math
import librosa
import soundfile as sf
import json
import re 
import io
import scipy

def FP2FIX(Val, Prec):
    if np.isscalar(Val):
        return int(Val * ((1<<Prec) - 1))
    else:
        return (Val * ((1<<Prec) - 1)).astype(np.int32)

def FIX2FP(Val, Prec):
    if np.isscalar(Val):
        return float(Val) / float(1<<Prec)
    else:
        return Val.astype(np.float32) / float(1<<Prec)

def qsnr(orig, quant):
    """Calculate the QSNR between two tensors
    """
    qerr = orig - quant
    sum_err = np.sum(qerr * qerr)
    sum_orig = np.sum(orig * orig)
    if sum_err > 0:
        if sum_orig < sum_err:
            if sum_orig == 0:
                return -math.inf
            else:
                # Means error is larger than signal
                return -int(round(10 * math.log10(sum_err/sum_orig), 0))
        # Error portion of signal
        return int(round(10 * math.log10(sum_orig/sum_err), 0))
    # Means no error
    return math.inf

file_path = "samples/yes.wav"
data, samplerate = sf.read(file_path)

with open("MelConfig.json", "r") as f:
    config = json.load(f)

frame_size = config['frame_size']
window_size = config.get('window_size', frame_size)
frame_step = config['frame_step']
n_fft = config['n_fft']
n_mels = config['n_mels']
fmin = config['fmin']
fmax = config['fmax']
dtype = config.get('dtype', "int")
if dtype == "float16":
    mel_dtype = np.float16
    imel_dtype = np.float16
    QMel = 0
    QIMel = 0
elif dtype == "float32":
    mel_dtype = np.float32
    imel_dtype = np.float32
    QMel = 0
    QIMel = 0
elif dtype == "fix16":
    mel_dtype = np.int32
    imel_dtype = np.int16
    QMel = 16
    QIMel = 11
else:
    raise ValueError(f"Not supported {dtype}")

# FORWARD MEL
stft = librosa.core.spectrum.stft(data, n_fft, frame_step, window_size, center=False, pad_mode="constant")
spect = np.abs(stft)
mel_basis = librosa.filters.mel(samplerate, n_fft, n_mels, fmin, fmax)
mel_spect = np.dot(mel_basis, spect)

# INVERSE MEL
inv_mel_spect, _, _, _ = np.linalg.lstsq(mel_basis, mel_spect)
# A.T*b = A.T*A*x^
# x^ = (A.T*A)^-1 * A.T * b
# inv_mel_basis_matrix = np.matmul(np.linalg.inv(np.matmul(mel_basis, mel_basis.T)), mel_basis)
# spect_rec = np.dot(mel_spect.T, inv_mel_basis_matrix)

# Data from GAP
MelOutGap = np.fromfile("GapMel.bin", mel_dtype).reshape(-1, n_mels).astype(np.float32) * 2.0**(-QMel)
InvMelGap = np.fromfile("GapInvMel.bin", imel_dtype).reshape(-1, (n_fft//2+1)).astype(np.float32) * 2.0**(-QIMel)
n_frames = MelOutGap.shape[0]

lines = (n_frames+9)//10
columns = 10
fig1, axes1 = plt.subplots(lines, columns)
for i in range(n_frames):
    axes1.flat[i].plot(mel_spect.T[i])
    axes1.flat[i].plot(MelOutGap[i])
fig1.suptitle(f'Forward gap vs librosa - Click on frame to see it (Overall QSNR: {qsnr(mel_spect.T[:n_frames], MelOutGap)})', fontsize=16)

fig2, axes2 = plt.subplots(lines, columns)
for i in range(n_frames):
    axes2.flat[i].plot(inv_mel_spect.T[i])
    axes2.flat[i].plot(InvMelGap[i])
    axes2.flat[i].plot(spect.T[i], "--")
fig2.suptitle(f'Inverse gap vs librosa - Click on frame to see it (Overall QSNR: {qsnr(spect.T[:n_frames], InvMelGap)})', fontsize=16)

def create_fig1(evt):
    if evt.inaxes:
        newfig, newax = plt.subplots()
        num = fig1.axes.index(evt.inaxes)
        newax.plot(mel_spect.T[num], label=f"Frame {num} - Librosa")
        newax.plot(MelOutGap[num], label=f"Frame {num} - Gap")
        newax.legend()
        newfig.suptitle(f'Frame {num} (QSNR: {qsnr(mel_spect.T[:n_frames][num], MelOutGap[num])})', fontsize=16)
        newfig.show()

def create_fig2(evt):
    if evt.inaxes:
        newfig, newax = plt.subplots()
        num = fig2.axes.index(evt.inaxes)
        newax.plot(inv_mel_spect.T[num], label=f"Frame {num} - Librosa")
        newax.plot(InvMelGap[num], label=f"Frame {num} - Gap")
        newax.plot(spect.T[num], "--", label=f"Frame {num} - Original")
        newax.legend()
        newfig.suptitle(f'Frame {num} (QSNR: {qsnr(spect.T[:n_frames][num], InvMelGap[num])})', fontsize=16)
        newfig.show()

fig1.canvas.mpl_connect("button_press_event", create_fig1)
fig2.canvas.mpl_connect("button_press_event", create_fig2)

plt.show()
