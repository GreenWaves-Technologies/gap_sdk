import numpy as np
from copy import deepcopy

def Rad2_FFT_DIF_Fix16(In_Data, Twiddles, N_fft):
    Data = deepcopy(In_Data)
    iLog2N = int(np.log2(N_fft))
    iL = 1
    iM = N_fft // 2
    for iCnt1 in range(iLog2N-3):
        iQ = 0
        for iCnt2 in range(0, iM):
            Wr = np.int64(Twiddles[0, iQ])
            Wi = np.int64(Twiddles[1, iQ])
            iA = iCnt2
            for iCnt3 in range(iL):
                iB = iA + iM
                Tmpr = np.int64(Data[0, iA]) - Data[0, iB]
                Tmpi = np.int64(Data[1, iA]) - Data[1, iB]
                Data[0, iA] = np.int16((np.int32(Data[0, iA]) + Data[0, iB]) >> 1)
                Data[1, iA] = np.int16((np.int32(Data[1, iA]) + Data[1, iB]) >> 1)
                Data[0, iB] = np.int16((((Tmpr*Wr) - (Tmpi*Wi)) >> 15) >> 1)
                Data[1, iB] = np.int16((((Tmpr*Wi) + (Tmpi*Wr)) >> 15) >> 1)
                iA = iA + 2 * iM
            iQ = iQ + iL
        iL *= 2
        iM //= 2

    iQ = 0
    iM = 4
    iL = int(N_fft) >> 3
    for iCnt2 in range(0, iM):
        Wr = Twiddles[0, iQ]
        Wi = Twiddles[1, iQ]
        iA = iCnt2
        for iCnt3 in range(iL):
            iB = iA + iM
            Tmpr = np.int64(Data[0, iA]) - Data[0, iB]
            Tmpi = np.int64(Data[1, iA]) - Data[1, iB]
            Data[0, iA] = np.int16((np.int32(Data[0, iA]) + Data[0, iB]))
            Data[1, iA] = np.int16((np.int32(Data[1, iA]) + Data[1, iB]))
            Data[0, iB] = np.int16((((Tmpr*Wr) - (Tmpi*Wi)) >> 15))
            Data[1, iB] = np.int16((((Tmpr*Wi) + (Tmpi*Wr)) >> 15))
            iA = iA + 2 * iM
        iQ += iL

    iQ = 0
    iM = 2
    iL = int(N_fft) >> 2
    for iCnt2 in range(0, iM):
        Wr = Twiddles[0, iQ]
        Wi = Twiddles[1, iQ]
        iA = iCnt2
        for iCnt3 in range(iL):
            iB = iA + iM
            Tmpr = np.int64(Data[0, iA]) - Data[0, iB]
            Tmpi = np.int64(Data[1, iA]) - Data[1, iB]
            Data[0, iA] = np.int16((np.int32(Data[0, iA]) + Data[0, iB]))
            Data[1, iA] = np.int16((np.int32(Data[1, iA]) + Data[1, iB]))
            Data[0, iB] = np.int16((((Tmpr*Wr) - (Tmpi*Wi)) >> 15))
            Data[1, iB] = np.int16((((Tmpr*Wi) + (Tmpi*Wr)) >> 15))
            iA = iA + 2 * iM
        iQ += iL

    iM = 1
    iL = N_fft // 2
    iA = 0
    for iCnt3 in range(iL):
        iB = iA + iM
        Tmpr = np.int32(Data[0, iA]) - Data[0, iB]
        Tmpi = np.int32(Data[1, iA]) - Data[1, iB]
        Data[0, iA] = np.int16((np.int32(Data[0, iA]) + Data[0, iB]))
        Data[1, iA] = np.int16((np.int32(Data[1, iA]) + Data[1, iB]))
        Data[0, iB] = np.int16(Tmpr)
        Data[1, iB] = np.int16(Tmpi)
        iA = iA + 2 * iM

    return Data


def Rad4_FFT_DIF_Fix16(In_Data, Twiddles, N_fft):
    Data = deepcopy(In_Data)
    iLog4N = int(np.log2(N_fft)) >> 1
    iL = 1
    iM = N_fft // 4
    for iCnt1 in range(iLog4N-2):
        iQ = 0
        for iCnt2 in range(0, iM):
            W1r = Twiddles[0, iQ]
            W1i = Twiddles[1, iQ]
            W2r = Twiddles[0, iQ*2]
            W2i = Twiddles[1, iQ*2]
            W3r = Twiddles[0, iQ*3]
            W3i = Twiddles[1, iQ*3]
            iA = iCnt2
            for iCnt3 in range(iL):
                A1r = ((np.int32(Data[0, iA]) + Data[0, iA+2*iM]) + (Data[0, iA+iM]  +  Data[0, iA+3*iM]))
                A1i = ((np.int32(Data[1, iA]) + Data[1, iA+2*iM]) + (Data[1, iA+iM]  +  Data[1, iA+3*iM]))
                B1r = ((np.int32(Data[0, iA]) - Data[0, iA+2*iM]) + (Data[1, iA+iM]  -  Data[1, iA+3*iM]))
                B1i = ((np.int32(Data[1, iA]) - Data[1, iA+2*iM]) - (Data[0, iA+iM]  -  Data[0, iA+3*iM]))
                C1r = ((np.int32(Data[0, iA]) + Data[0, iA+2*iM]) - (Data[0, iA+iM]  +  Data[0, iA+3*iM]))
                C1i = ((np.int32(Data[1, iA]) + Data[1, iA+2*iM]) - (Data[1, iA+iM]  +  Data[1, iA+3*iM]))
                D1r = ((np.int32(Data[0, iA]) - Data[0, iA+2*iM]) - (Data[1, iA+iM]  -  Data[1, iA+3*iM]))
                D1i = ((np.int32(Data[1, iA]) - Data[1, iA+2*iM]) + (Data[0, iA+iM]  -  Data[0, iA+3*iM]))

                Tmp = B1r
                B1r = np.int64(B1r*W1r - B1i*W1i) >> 15
                B1i = np.int64(Tmp*W1i + B1i*W1r) >> 15
                Tmp = C1r
                C1r = np.int64(C1r*W2r - C1i*W2i) >> 15
                C1i = np.int64(Tmp*W2i + C1i*W2r) >> 15
                Tmp = D1r
                D1r = np.int64(D1r*W3r - D1i*W3i) >> 15
                D1i = np.int64(Tmp*W3i + D1i*W3r) >> 15

                Data[0, iA     ] = np.int16(A1r >> 2)
                Data[1, iA     ] = np.int16(A1i >> 2)
                Data[0, iA+  iM] = np.int16(B1r >> 2)
                Data[1, iA+  iM] = np.int16(B1i >> 2)
                Data[0, iA+2*iM] = np.int16(C1r >> 2)
                Data[1, iA+2*iM] = np.int16(C1i >> 2)
                Data[0, iA+3*iM] = np.int16(D1r >> 2)
                Data[1, iA+3*iM] = np.int16(D1i >> 2)

                iA = iA + 4 * iM
            iQ += iL
        iL = iL * 4
        iM = iM // 4

    iQ = 0
    iM = 4
    iL = N_fft // 16
    for iCnt2 in range(0, iM):
        W1r = Twiddles[0, iQ]
        W1i = Twiddles[1, iQ]
        W2r = Twiddles[0, iQ*2]
        W2i = Twiddles[1, iQ*2]
        W3r = Twiddles[0, iQ*3]
        W3i = Twiddles[1, iQ*3]
        iA = iCnt2
        for iCnt3 in range(iL):
            A1r = ((np.int32(Data[0, iA]) + Data[0, iA+2*iM]) + (Data[0, iA+iM]  +  Data[0, iA+3*iM]))
            A1i = ((np.int32(Data[1, iA]) + Data[1, iA+2*iM]) + (Data[1, iA+iM]  +  Data[1, iA+3*iM]))
            B1r = ((np.int32(Data[0, iA]) - Data[0, iA+2*iM]) + (Data[1, iA+iM]  -  Data[1, iA+3*iM]))
            B1i = ((np.int32(Data[1, iA]) - Data[1, iA+2*iM]) - (Data[0, iA+iM]  -  Data[0, iA+3*iM]))
            C1r = ((np.int32(Data[0, iA]) + Data[0, iA+2*iM]) - (Data[0, iA+iM]  +  Data[0, iA+3*iM]))
            C1i = ((np.int32(Data[1, iA]) + Data[1, iA+2*iM]) - (Data[1, iA+iM]  +  Data[1, iA+3*iM]))
            D1r = ((np.int32(Data[0, iA]) - Data[0, iA+2*iM]) - (Data[1, iA+iM]  -  Data[1, iA+3*iM]))
            D1i = ((np.int32(Data[1, iA]) - Data[1, iA+2*iM]) + (Data[0, iA+iM]  -  Data[0, iA+3*iM]))

            Tmp = B1r
            B1r = np.int64(B1r*W1r - B1i*W1i) >> 15
            B1i = np.int64(Tmp*W1i + B1i*W1r) >> 15
            Tmp = C1r
            C1r = np.int64(C1r*W2r - C1i*W2i) >> 15
            C1i = np.int64(Tmp*W2i + C1i*W2r) >> 15
            Tmp = D1r
            D1r = np.int64(D1r*W3r - D1i*W3i) >> 15
            D1i = np.int64(Tmp*W3i + D1i*W3r) >> 15

            Data[0, iA     ] = np.int16(A1r)
            Data[1, iA     ] = np.int16(A1i)
            Data[0, iA+  iM] = np.int16(B1r)
            Data[1, iA+  iM] = np.int16(B1i)
            Data[0, iA+2*iM] = np.int16(C1r)
            Data[1, iA+2*iM] = np.int16(C1i)
            Data[0, iA+3*iM] = np.int16(D1r)
            Data[1, iA+3*iM] = np.int16(D1i)

            iA = iA + 4 * iM
        iQ += iL

    iM = 1
    iL = N_fft // 4
    iA = 0
    for iCnt3 in range(iL):
        Ar = np.int32(Data[0, iA])
        Ai = np.int32(Data[1, iA])
        Br = np.int32(Data[0, iA+iM])
        Bi = np.int32(Data[1, iA+iM])
        Cr = np.int32(Data[0, iA+2*iM])
        Ci = np.int32(Data[1, iA+2*iM])
        Dr = np.int32(Data[0, iA+3*iM])
        Di = np.int32(Data[1, iA+3*iM])

        Data[0, iA     ] = np.int16(Ar + Cr + Br + Dr)
        Data[1, iA     ] = np.int16(Ai + Ci + Bi + Di)
        Data[0, iA+  iM] = np.int16(Ar - Cr + Bi - Di)
        Data[1, iA+  iM] = np.int16(Ai - Ci - Br + Dr)
        Data[0, iA+2*iM] = np.int16(Ar + Cr - Br - Dr)
        Data[1, iA+2*iM] = np.int16(Ai + Ci - Bi - Di)
        Data[0, iA+3*iM] = np.int16(Ar - Cr - Bi + Di)
        Data[1, iA+3*iM] = np.int16(Ai - Ci + Br - Dr)

        iA = iA + 4 * iM
    return Data

def SwapSamples(In_Data, SwapTable):
    Data = deepcopy(In_Data)
    for i, swap_idx in enumerate(SwapTable):
        if i < swap_idx:
            Sr = Data[0, i]
            Si = Data[1, i]
            Data[0, i] = Data[0, swap_idx]
            Data[1, i] = Data[1, swap_idx]
            Data[0, swap_idx] = Sr
            Data[1, swap_idx] = Si
    return Data

def RFFT_Step_Fix16(In_Data, RTwiddles, N_fft):
    RFFT_Out = np.zeros((2, N_fft//2+1))

    xBR = np.int32(In_Data[0, 0]) >> 2
    xBI = np.int32(In_Data[1, 0]) >> 2
    xAR = np.int32(In_Data[0, 0]) >> 2
    xAI = np.int32(In_Data[1, 0]) >> 2
    RFFT_Out[0, 0] = (xBR + xAR + xBI + xAI)
    RFFT_Out[1, 0] = (xAI - xBI + xBR - xAR)

    k = N_fft // 2 - 1
    for i in range(k):
        twr, twi = np.int64(RTwiddles[0, 1+i]), np.int64(RTwiddles[1, 1+i])
        xar, xai = np.int32(In_Data  [0, 1+i]), np.int32(In_Data  [1, 1+i])
        xbr, xbi = np.int32(In_Data  [0, k-i]), np.int32(In_Data  [1, k-i])

        t1r = np.int32((-xar + xbr) >> 2)
        t1i = np.int32((-xai - xbi) >> 2)
        t2r = np.int32(( xar + xbr) >> 2)
        t2i = np.int32(( xai - xbi) >> 2)

        RFFT_Out[0, i+1] = np.int16(((twr*t1r - twi*t1i) >> 15) + t2r)
        RFFT_Out[1, i+1] = np.int16(((twr*t1i + twi*t1r) >> 15) + t2i)

    xBR = np.int32(In_Data[0, k-(k-1)]) >> 2
    xBI = np.int32(In_Data[1, k-(k-1)]) >> 2
    xAR = np.int32(In_Data[0, 1+(k-1)]) >> 2
    xAI = np.int32(In_Data[1, 1+(k-1)]) >> 2
    RFFT_Out[0, k+1] = np.int16( xBR + xAR - xBI - xAI )
    RFFT_Out[1, k+1] = np.int16(0) #( xAI - xBI - xBR + xAR )
    return RFFT_Out
