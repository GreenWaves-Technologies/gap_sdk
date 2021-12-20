import numpy as np

def gen_fft_twiddles_lut(Nfft, Inverse=False, rad4=False):
    Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft)
    if Inverse:
        Twiddles_real = np.cos(Phi)
        Twiddles_imag = np.sin(Phi)
    else:
        Twiddles_real = np.cos(-Phi)
        Twiddles_imag = np.sin(-Phi)
    if rad4:
        twid = np.empty((2 * int(3*Nfft/4), ), dtype=Twiddles_real.dtype)
        twid[0::2] = Twiddles_real[:int(3*Nfft/4)]
        twid[1::2] = Twiddles_imag[:int(3*Nfft/4)]
        return twid
    twid = np.empty((2 * int(Nfft//2), ), dtype=Twiddles_real.dtype)
    twid[0::2] = Twiddles_real[:int(Nfft//2)]
    twid[1::2] = Twiddles_imag[:int(Nfft//2)]
    return twid

def gen_rfft_twiddles_lut(Nfft):
    Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft//2)
    Twiddles_real = np.sin(Phi)
    Twiddles_imag = np.cos(Phi)
    twid = np.empty((Twiddles_real.size + Twiddles_imag.size, ), dtype=Twiddles_real.dtype)
    twid[0::2] = Twiddles_real
    twid[1::2] = Twiddles_imag
    return twid

def gen_fft_swaptable_lut(Ni, rad4=False):
    if rad4:
        log4 = int(np.log2(Ni) / 2)
        iL = Ni / 4
        iM = 1
        SwapTable = np.zeros(Ni)
        for i in range(log4):
            for j in range(iM):
                SwapTable[j +   iM] = SwapTable[j] +   iL
                SwapTable[j + 2*iM] = SwapTable[j] + 2*iL
                SwapTable[j + 3*iM] = SwapTable[j] + 3*iL
            iL /= 4
            iM *= 4    
    else:
        log2 = int(np.log2(Ni))
        iL = Ni / 2
        iM = 1
        SwapTable = np.zeros(Ni)
        for i in range(log2):
            for j in range(iM):
                SwapTable[j + iM] = SwapTable[j] + iL
            iL /= 2
            iM *= 2
    return SwapTable


def gen_dct_twiddles_lut(Ndct, dct_type=2, norm=None):
    norm_factor = np.ones((Ndct, Ndct))
    if norm == "ortho" and dct_type == 2:
        norm_factor   *= np.sqrt(1/(2*Ndct))
        norm_factor[0] = np.sqrt(1/(4*Ndct))
    if norm == "ortho" and dct_type == 3:
        norm_factor   *= np.sqrt(1/(2*Ndct))
        norm_factor[0] = np.sqrt(1/(Ndct))
    DCT_Coeff = np.zeros((Ndct, Ndct))
    for k in range(Ndct):
        for i in range(Ndct):
            if dct_type == 2:
                coeff = 2*np.cos(np.pi / (2*Ndct) * k * (2*i + 1))
            elif dct_type == 3:
                coeff = 1 if i == 0 else 2*np.cos(np.pi / (2*Ndct) * i * (2*k + 1))
            else:
                raise NotImplementedError("DCT type 2 and 3 only supported")
            DCT_Coeff[k, i] = coeff

    return (DCT_Coeff * norm_factor)
