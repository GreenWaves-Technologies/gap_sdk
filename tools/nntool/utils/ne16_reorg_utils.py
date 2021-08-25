import numpy as np

def ne16_conv_weight_layout(W, w_bits, is_depthwise=False):
    if W.shape[1:3] == (3, 3):
        if is_depthwise:
            return ne16_conv_dw_3x3_weight_layout(W, w_bits)
        return ne16_conv_3x3_weight_layout(W, w_bits)
    elif W.shape[1:3] == (1, 1):
        return ne16_conv_1x1_weight_layout(W, w_bits)
    raise NotImplementedError(f"NE16 weights generation for {W.shape} weight")

def ne16_conv_3x3_weight_layout(W, w_bits=8, is_depthwise=False):
    tp_in = 16
    # tp_out = 32
    chan_out = W.shape[0]
    chan_in = W.shape[3]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    # w_binary_1 = np.zeros((chan_out, subtile_nb_ki, w_bits, 3 * 3, tp_in//8), dtype=np.uint8)
    # for k_out in range(chan_out):
    #     for k_in_major in range(subtile_nb_ki):
    #         for q in range(w_bits):
    #             for fy in range(3):
    #                 for fx in range(3):
    #                     for k_in_minor in range(tp_in):
    #                         if k_in_major*tp_in+k_in_minor >= chan_in:
    #                             w_val = 0
    #                         else:
    #                             w_val = W[k_out, fy, fx, k_in_major*tp_in+k_in_minor]
    #                         w_binary_1[k_out, k_in_major, q, fy*3+fx, k_in_minor//8] |= (0x1 & (w_val >> q)) << (k_in_minor%8)

    height = W.shape[1]
    width = W.shape[2]
    w_binary = np.zeros((chan_out*subtile_nb_ki*w_bits, height*width, 8, tp_in//8), dtype=np.uint8)
    for k_out in range(chan_out):
        for k_in_major in range(subtile_nb_ki):
            for filt_h in range(height):
                for filt_w in range(width):
                    for k_in_minor in range(tp_in):
                        w_layout_idx = k_out * subtile_nb_ki * w_bits + k_in_major * w_bits
                        if k_in_major*tp_in+k_in_minor >= chan_in:
                            for i in range(w_bits):
                                w_binary[w_layout_idx+i, filt_h*width+filt_w,
                                         k_in_minor % 8, k_in_minor//8] = 0
                        else:
                            for i in range(w_bits):
                                w_binary[w_layout_idx+i, filt_h*width+filt_w, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                                    (W[k_out, filt_h, filt_w, k_in_major*tp_in+k_in_minor]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((chan_out, subtile_nb_ki, w_bits, height, width, 2))
    return w_layout

def ne16_conv_dw_3x3_weight_layout(W, w_bits=8):
    tp_in = 16
    chan_in = W.shape[0]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    w_binary = np.zeros((subtile_nb_ki*w_bits, 3*3, 8, tp_in//8), dtype=np.uint8)
    for k_in_major in range(subtile_nb_ki):
        for filt_h in range(3):
            for filt_w in range(3):
                for k_in_minor in range(tp_in):
                    w_layout_idx = k_in_major * w_bits
                    if k_in_major*tp_in+k_in_minor >= chan_in:
                        for i in range(w_bits):
                            w_binary[w_layout_idx+i, filt_h*3+filt_w,
                                     k_in_minor % 8, k_in_minor//8] = 0
                    else:
                        for i in range(w_bits):
                            w_binary[w_layout_idx+i, filt_h*3+filt_w, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                                (W[k_in_major*tp_in+k_in_minor, filt_h, filt_w, 0]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((1, subtile_nb_ki, w_bits, 3, 3, 2))
    return w_layout

def ne16_conv_1x1_weight_layout(W, w_bits=8):
    tp_in = 16
    # tp_out = 32
    chan_out = W.shape[0]
    chan_in = W.shape[3]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    w_binary = np.zeros((chan_out*subtile_nb_ki, w_bits,
                         8, tp_in//8), dtype=np.uint8)
    for k_out in range(chan_out):
        for k_in_major in range(subtile_nb_ki):
            for k_in_minor in range(tp_in):
                w_layout_idx = k_out * subtile_nb_ki + k_in_major
                if k_in_major*tp_in+k_in_minor >= chan_in:
                    for i in range(w_bits):
                        w_binary[w_layout_idx, i, k_in_minor %
                                 8, k_in_minor//8] = 0
                else:
                    for i in range(w_bits):
                        w_binary[w_layout_idx, i, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                            (W[k_out, 0, 0, k_in_major*tp_in+k_in_minor]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((chan_out, subtile_nb_ki, w_bits, 2))
    return w_layout

def ne16_linear_weight_layout(W, w_bits=8):
    chan_out = W.shape[0]
    chan_in = W.shape[1]
    nb_ki = chan_in // 16 + (1 if chan_in % 16 != 0 else 0)
    w_binary = np.zeros((chan_out * w_bits * nb_ki, 2), dtype=np.uint8)
    for ko in range(chan_out):
        for ki in range(chan_in):
            kimaj = ki // 16
            kimin = ki % 16
            byte = kimin // 8
            shift = kimin % 8
            for q in range(w_bits):
                index = ko*nb_ki*w_bits + q*nb_ki + kimaj
                w_binary[index, byte] = w_binary[index, byte] | (
                    1 << shift if (W[ko, ki] & (1 << q) != 0) else 0)
    return w_binary
