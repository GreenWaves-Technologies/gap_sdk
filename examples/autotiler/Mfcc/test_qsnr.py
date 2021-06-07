import argparse
from scipy.io.wavfile import read
import numpy as np
import math
import librosa
import soundfile as sf
import json
import re 
import io
from librosa.core.spectrum import power_to_db
import scipy
import glob
import os
import pandas as pd

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
def gap_mulsRN(x, y, N):
    if np.isscalar(x) and np.isscalar(y):
        return int(int(x) * int(y) + (1<<(N-1))) >> N
    else:
        return (x.astype(np.int32) * y.astype(np.int32) + (1<<(N-1))).astype(np.int32) >> N
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

def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='mfcc_test')
    parser.add_argument('--use_power', action="store_true")
    return parser

def main():
    parser = create_parser()
    args = parser.parse_args()
    files_list = "samples/*.wav"
    executable = "mfcc_emul"

    with open("MfccConfig.json", "r") as f:
        config = json.load(f)

    dtype = config.get('dtype', "int")
    frame_size = config['frame_size']
    frame_step = config['frame_step']
    n_fft = config['n_fft']
    n_fft_int = n_fft if dtype == "fix32_scal" else n_fft // 2
    rad4 = round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4)
    n_mels = config['mfcc_bank_cnt']
    fmin = config['fmin']
    fmax = config['fmax']
    high_prec = dtype == "fix32_scal"
    use_power = args.use_power
    ndct = config.get("n_dct", 0)
    nfiles = len(glob.glob(files_list))

    first = True
    for i, file in enumerate(glob.glob(files_list)):
        print(f"File {i}/{nfiles}: {file}")
        ex_stream = os.popen("./{} {}".format(executable, file))

        data, samplerate = sf.read(file)
        # GT
        stft = librosa.core.spectrum.stft(data, n_fft, frame_step, frame_size, center=False, pad_mode="constant")
        spect = np.abs(stft) ** (1 if not use_power else 2)
        mel_basis = librosa.filters.mel(samplerate, n_fft, n_mels, fmin, fmax)
        mel_spect = np.dot(mel_basis, spect)
        logmel = power_to_db(mel_spect)
        mfcc = scipy.fftpack.dct(logmel, axis=0, type=2, norm=None)


        # Parse output
        GAP_Shift           = {"Matcher": re.compile(r"Shift\s=\s(?P<value_list>[-0-9]+)")                  , "values": []}
        GAP_out_preemph     = {"Matcher": re.compile(r"out_preemph_c = \[\t*(?P<value_list>[^\)\]]+)]\)")   , "values": []}
        GAP_out_window      = {"Matcher": re.compile(r"out_window_c = \[\t*(?P<value_list>[^\)\]]+)]\)")    , "values": []}
        GAP_out_fft         = {"Matcher": re.compile(r"out_rfft = \[\t*(?P<value_list>[^\)\]]+)]\)") , "values": []}
        GAP_out_fft_shift   = {"Matcher": re.compile(r"out_fft_shift = \[\t*(?P<value_list>[^\)\]]+)]\)"), "values": []}
        if not use_power:
            GAP_out_spect   = {"Matcher": re.compile(r"out_mag = \[\t*(?P<value_list>[^\)\]]+)]\)") , "values": []}
        else:
            GAP_out_spect   = {"Matcher": re.compile(r"out_power = \[\t*(?P<value_list>[^\)\]]+)]\)")       , "values": []}
        GAP_melspect        = {"Matcher": re.compile(r"out_melfilterbank = \[\t*(?P<value_list>[^\)\]]+)]\)"), "values": []}
        GAP_shift_BF        = {"Matcher": re.compile(r"shift_BF = \[\t*(?P<value_list>[^\)\]]+)]\)")        , "values": []}
        GAP_log             = {"Matcher": re.compile(r"out_log = \[\t*(?P<value_list>[^\)\]]+)\]\)")        , "values": []}
        GAP_Norm            = {"Matcher": re.compile(r"Norm\s=\s(?P<value_list>[0-9]+)")                    , "values": []}
        GAP_norm_clip       = {"Matcher": re.compile(r"out_norm_clip = \[\t*(?P<value_list>[^\)\]]+)]\)")   , "values": []}
        GAP_out_dct         = {"Matcher": re.compile(r"out_dct_ii = \[\t*(?P<value_list>[^\)\]]+)]\)")      , "values": []}

        OUT_LIST = [GAP_Shift, GAP_out_preemph, GAP_out_window, GAP_out_fft, GAP_out_spect, GAP_melspect, GAP_shift_BF, GAP_log, GAP_Norm, GAP_out_dct, GAP_out_fft_shift]
        str_replace = ""
        for line in ex_stream.readlines():
            str_replace += line.replace("np.array([\n", "[")
        buf = io.StringIO(str_replace)
        for line in buf:
            for match in OUT_LIST:
                m = match["Matcher"].search(line)
                if m:
                    match["values"].append(np.array(eval(m['value_list'])))

        # fixed point info extracted at runtime
        Shift = np.array(GAP_Shift['values'])
        fft_shift_buff = np.array(GAP_out_fft_shift['values'])
        Norm = np.array(GAP_Norm['values'])
        melspect_shift_buff = np.array(GAP_shift_BF['values'])

        # values
        gap_windows = np.array(GAP_out_window['values'])
        gap_fft = np.array(GAP_out_fft['values'])
        gap_spect = np.array(GAP_out_spect['values'])
        gap_melspect = np.array(GAP_melspect['values'])
        gap_log = np.array(GAP_log['values'])
        gap_dct = np.array(GAP_out_dct['values'])

        if stft.shape[1] < gap_windows.shape[0]:
            print(f"{file} has less than {gap_windows.shape[0]} frames")
            continue

        fft_qsnrs = []
        spect_qsnrs = []
        melspect_qsnrs = []
        logmel_qsnrs = []
        dct_qsnrs = []

        # QFORMAT
        if dtype == 'int' or dtype=="fix16" or dtype == "fix32_scal":
            QIN = 15
            QWin = QIN + Shift
            if not high_prec:
                if rad4:
                    QFFT = QIN + Shift - int(np.log2(n_fft)) + 4
                else:
                    QFFT = QIN + Shift - int(np.log2(n_fft)) + 3
            else:
                QFFT = (QIN + Shift[:,np.newaxis] + fft_shift_buff)[:,:n_fft//2+1]
            QSPECT = 15
            if not high_prec:
                QMEL = QSPECT + 15 - melspect_shift_buff
            else:
                QMEL = 15 - 2 + 2*Shift[:,np.newaxis] - melspect_shift_buff

            #QLOG = 15
            QLOG_NORM = 15 - Norm
            QDCT = QLOG_NORM - 7

            for i in range(gap_windows.shape[0]):
                fft_qsnrs.append(qsnr(np.abs(stft.T[i]), np.abs(gap_fft[i])[:n_fft//2+1] * 2.0**(-QFFT[i])))
                spect_qsnrs.append(qsnr(spect.T[i], gap_spect[i] * 2.0**(-QSPECT)))    
                melspect_qsnrs.append(qsnr(mel_spect.T[i], gap_melspect[i] * 2.0**(-QMEL[i])))
                logmel_qsnrs.append(qsnr(logmel.T[i], gap_log[i] * 2.0**(-QLOG_NORM[i])))
                if ndct > 0:
                    dct_qsnrs.append(qsnr(mfcc.T[i], gap_dct[i] * 2.0**(-QDCT[i]))) 
        else:
            for i in range(gap_windows.shape[0]):
                fft_qsnrs.append(qsnr(np.abs(stft.T[i]), np.abs(gap_fft[i])[:n_fft//2+1]))
                spect_qsnrs.append(qsnr(spect.T[i], gap_spect[i]))    
                melspect_qsnrs.append(qsnr(mel_spect.T[i], gap_melspect[i]))
                logmel_qsnrs.append(qsnr(logmel.T[i], gap_log[i]))
                if ndct > 0:
                    dct_qsnrs.append(qsnr(mfcc.T[i], gap_dct[i])) 

        fft_avg = sum(fft_qsnrs)/len(fft_qsnrs)
        fft_min = min(fft_qsnrs)
        fft_min_idx = fft_qsnrs.index(fft_min)
        spe_avg = sum(spect_qsnrs)/len(spect_qsnrs)
        spe_min = min(spect_qsnrs)
        spe_min_idx = spect_qsnrs.index(spe_min)
        mel_avg = sum(melspect_qsnrs)/len(melspect_qsnrs)
        mel_min = min(melspect_qsnrs)
        mel_min_idx = melspect_qsnrs.index(mel_min)
        log_avg = sum(logmel_qsnrs)/len(logmel_qsnrs)
        log_min = min(logmel_qsnrs)
        log_min_idx = logmel_qsnrs.index(log_min)
        dct_avg = sum(dct_qsnrs)/len(dct_qsnrs) if ndct > 0 else 0
        dct_min = min(dct_qsnrs) if ndct > 0 else 0
        dct_min_idx = dct_qsnrs.index(dct_min) if ndct > 0 else 0
        print(f"\tFFT:      avg: {fft_avg:.2f} min: {fft_min}@{fft_min_idx}")
        print(f"\tSPECT:    avg: {spe_avg:.2f} min: {spe_min}@{spe_min_idx}")
        print(f"\tMELSPECT: avg: {mel_avg:.2f} min: {mel_min}@{mel_min_idx}")
        print(f"\tLOGMEL:   avg: {log_avg:.2f} min: {log_min}@{log_min_idx}")
        print(f"\tDCT:      avg: {dct_avg:.2f} min: {dct_min}@{dct_min_idx}")
        df = pd.DataFrame([[file, fft_avg, fft_min, fft_min_idx, spe_avg, spe_min, spe_min_idx, mel_avg, mel_min, mel_min_idx, log_avg, log_min, log_min_idx, dct_avg, dct_min, dct_min_idx]],
                          columns=["file path", "fft_avg", "fft_min", "fft_min_idx", "spect_avg", "spect_min", "spect_min_idx", "melspect_avg", "melspect_min", "melspect_min_idx", "logmel_avg", "logmel_min", "logmel_min_idx", "dct_avg", "dct_min", "dct_min_idx"])
        if not first:
            df.append(df)
        csv_filename = f"qsnrs_{dtype}" + ("_high_prec" if high_prec else "") + ".csv"
        df.to_csv(csv_filename, mode="w" if first else "a", header=first)
        first = False

if __name__ == "__main__":
    main()
