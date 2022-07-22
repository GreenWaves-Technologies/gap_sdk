
import numpy as np

def do_conv_im2col(
    in_tensor_padded: np.ndarray,
    weights: np.ndarray,
    bias: np.ndarray,
    in_c,
    out_h,
    out_w,
    out_c,
    dilated_filter_h,
    dilated_filter_w,
    filt_h,
    filt_w,
    filt_dil_h,
    filt_dil_w,
    filt_str_h,
    filt_str_w,
    const_h,
    const_w,
    calc_dtype
    ) -> np.ndarray:

    in_tensor_padded = in_tensor_padded.astype(calc_dtype)
    weights_mat = weights.astype(calc_dtype).reshape(-1, out_c) #.transpose((1, 0))

    im2col_buff = np.empty((out_h * out_w, filt_h*filt_w*in_c))
    for o_h in range(out_h):
        in_h_pointer_i = o_h * filt_str_h
        in_h_pointer_e = o_h * filt_str_h + dilated_filter_h
        for o_w in range(out_w):
            in_w_pointer_i = o_w * filt_str_w
            in_w_pointer_e = o_w * filt_str_w + dilated_filter_w
            im2col_buff[o_h * out_w + o_w, :] = in_tensor_padded[
                    in_h_pointer_i:in_h_pointer_e:filt_dil_h,
                    in_w_pointer_i:in_w_pointer_e:filt_dil_w,
                    :
                    ].flatten()

    result = np.matmul(im2col_buff, weights_mat).transpose((1, 0))
    result = result.reshape(out_c, out_h, out_w) + bias

    min_acc = np.min(result)
    max_acc = np.max(result)
    return result.astype(calc_dtype), min_acc, max_acc

def do_grouped_conv_im2col(
    in_tensor_padded: np.ndarray,
    weights: np.ndarray,
    bias: np.ndarray,
    in_c,
    out_h,
    out_w,
    out_c,
    dilated_filter_h,
    dilated_filter_w,
    filt_h,
    filt_w,
    filt_dil_h,
    filt_dil_w,
    filt_str_h,
    filt_str_w,
    const_h,
    const_w,
    groups,
    calc_dtype
    ) -> np.ndarray:

    in_c_per_group = in_c // groups
    out_c_per_group = out_c // groups
    result = np.empty((out_c, out_h, out_w))
    for group in range(groups):

        in_tensor_padded_slice = in_tensor_padded[:,:,(in_c_per_group*group):(in_c_per_group*(group+1))].astype(calc_dtype)
        weights_mat = weights.astype(calc_dtype).reshape(-1, out_c)[:,(out_c_per_group*group):(out_c_per_group*(group+1))] #.transpose((1, 0))

        im2col_buff = np.empty((out_h * out_w, filt_h*filt_w*in_c_per_group))
        for o_h in range(out_h):
            in_h_pointer_i = o_h * filt_str_h
            in_h_pointer_e = o_h * filt_str_h + dilated_filter_h
            for o_w in range(out_w):
                in_w_pointer_i = o_w * filt_str_w
                in_w_pointer_e = o_w * filt_str_w + dilated_filter_w
                im2col_buff[o_h * out_w + o_w, :] = in_tensor_padded_slice[
                        in_h_pointer_i:in_h_pointer_e:filt_dil_h,
                        in_w_pointer_i:in_w_pointer_e:filt_dil_w,
                        :
                        ].flatten()

        partial_result = np.matmul(im2col_buff, weights_mat).transpose((1, 0))
        partial_result = partial_result.reshape(out_c_per_group, out_h, out_w) + bias[(out_c_per_group*group):(out_c_per_group*(group+1)),:,:]
        result[(out_c_per_group*group):(out_c_per_group*(group+1)),:,:] = partial_result

    min_acc = np.min(result)
    max_acc = np.max(result)
    return result.astype(calc_dtype), min_acc, max_acc

def do_conv_depthwise(
    in_tensor_padded: np.ndarray,
    weights: np.ndarray,
    bias: np.ndarray,
    in_c,
    out_h,
    out_w,
    out_c,
    dilated_filter_h,
    dilated_filter_w,
    filt_h,
    filt_w,
    filt_dil_h,
    filt_dil_w,
    filt_str_h,
    filt_str_w,
    const_h,
    const_w,
    calc_dtype
    ) -> np.ndarray:

    in_tensor_padded = in_tensor_padded.astype(calc_dtype)
    weights_mat = weights.astype(calc_dtype).reshape(filt_h, filt_w, out_c)

    result = bias.transpose((1, 2, 0))
    #pylint: disable=not-an-iterable
    for cur_h in range(filt_h):
        for cur_w in range(filt_w):
            # selects all elements that the filter element needs to multiply
            slabhw = np.multiply(in_tensor_padded[cur_h * filt_dil_h:
                                            const_h + cur_h * filt_dil_h:
                                            filt_str_h,
                                            cur_w * filt_dil_w:
                                            const_w + cur_w * filt_dil_w:
                                            filt_str_w,
                                            :],
                                    weights_mat[cur_h, cur_w, :]).astype(calc_dtype)
            # add to the previous filter elements
            result += slabhw

    min_acc = np.min(result)
    max_acc = np.max(result)

    return result.transpose((2, 0, 1)).astype(calc_dtype), min_acc, max_acc

def fast_numpy_conv(
    in_tensor_padded: np.ndarray,
    weights: np.ndarray,
    bias: np.ndarray,
    in_c: int,
    out_h: int,
    out_w: int,
    out_c: int,
    dilated_filter_h: int,
    dilated_filter_w: int,
    filt_h: int,
    filt_w: int,
    filt_dil_h: int,
    filt_dil_w: int,
    filt_str_h: int,
    filt_str_w: int,
    const_h: int,
    const_w: int,
    groups: int = 1,
    is_depthwise: bool = False,
    calc_dtype: np.dtype = np.float32
    ) -> np.ndarray:

    if is_depthwise:
        return do_conv_depthwise(in_tensor_padded, weights, bias, in_c, out_h, out_w, out_c, dilated_filter_h, dilated_filter_w, filt_h, filt_w, filt_dil_h, filt_dil_w, filt_str_h, filt_str_w, const_h, const_w, calc_dtype)
    elif groups == 1:
        return do_conv_im2col(in_tensor_padded, weights, bias, in_c, out_h, out_w, out_c, dilated_filter_h, dilated_filter_w, filt_h, filt_w, filt_dil_h, filt_dil_w, filt_str_h, filt_str_w, const_h, const_w, calc_dtype)
    else:
        return do_grouped_conv_im2col(in_tensor_padded, weights, bias, in_c, out_h, out_w, out_c, dilated_filter_h, dilated_filter_w, filt_h, filt_w, filt_dil_h, filt_dil_w, filt_str_h, filt_str_w, const_h, const_w, groups, calc_dtype)
