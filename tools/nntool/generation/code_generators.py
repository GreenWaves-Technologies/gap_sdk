# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# pylint: disable=line-too-long

import logging

from graph.dim import PadDim

from .code_block import CodeBlock
from .kernel_parameters import (NO_ACTIVATION, NO_CONV, NO_POOL,
                                ActivationATParam, ConvATParam, GenCtrl,
                                GlobalPoolATParam, GroupedConvATParam,
                                LinearATParam, MatrixAddATParam,
                                MatScaleATParam, PoolATParam, SoftMaxATParam,
                                ThreeDTensorTransposeATParam)

LOG = logging.getLogger("nntool." + __name__)

GEN_CONV_POOL_RELU = "CNN_ConvolutionPoolReLU"
GEN_GROUPED_CONV_POOL_RELU = "CNN_GroupedConvolutionPoolReLU"
GEN_POOL_RELU = "CNN_PoolReLU"
GEN_LINEAR_RELU = "CNN_LinearReLU"
GEN_SOFTMAX = "CNN_SoftMax"
GEN_MATADD = "CNN_MatAdd"
GEN_MATADDDYN = "CNN_MatAddDynAdjust"
GEN_3D_TRANSPOSE = "CNN_3DTensorPermute"
GEN_GLOBALPOOL = "CNN_GlobalPool"
GEN_MATSCALE = "CNN_MatScale"


def gen_ctrl_call(api, op, val, code_block):
    if isinstance(val, str):
        val = 'AT_VAL("%s")' % val
    elif isinstance(val, bool):
        val = val and 'AT_OPT_ON' or 'AT_OPT_OFF'
    elif isinstance(val, int):
        val = 'AT_VAL(%s)' % val
    else:
        raise ValueError()

    code_block.write('{}({}, {});', api, op, val)


def gen_kernel_ctrl(op, val, code_block):
    gen_ctrl_call('AT_SetKernelCtrl', op, val, code_block)


def gen_graph_ctrl(op, val, code_block):
    gen_ctrl_call('AT_SetGraphCtrl', op, val, code_block)

# TCArgInfo("int *__restrict__",         "JumboOut",       ARG_SCOPE_ARG,    ARG_DIR_OUT,     ARG_LOC_L2,      ARG_LOC_L2)


def gen_output_decl(eparams, in_q, home_location, exec_location, code_block, allocate=False):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", {2}, ARG_DIR_OUT, {3}, {4}, 0)'
                     .format(in_q.ctype, eparams.name, "ARG_SCOPE_ARG_ALLOC" if allocate else "ARG_SCOPE_ARG", home_location, exec_location))

# TCArgInfo("short int *__restrict__",   "JumboIn",        ARG_SCOPE_ARG,    ARG_DIR_IN,      ARG_LOC_L2,      ARG_LOC_L2),


def gen_input_decl(eparams, out_q, home_location, exec_location, code_block, allocate=False):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", {2}, ARG_DIR_IN, {3}, {4}, 0)'
                     .format(out_q.ctype, eparams.name, "ARG_SCOPE_ARG_ALLOC" if allocate else "ARG_SCOPE_ARG", home_location, exec_location))

# TCArgInfo("short int *__restrict__",   "JumboL1_Filter", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, ARG_LOC_L2,      ARG_LOC_L3_HFLASH),

# ConstInfo(
# 	char *FileName,			/**< Name of the file containing the list of values for this constant vector */
# 	int Format,			/**< Format of the list of value, 0: floating point, 1: fixed point */
# 	int Size,			/**< Size of the fixed point container */
# 	int Fract			/**< Fractional part size, Qx.Fract, note that x+Fract < Size */
# 	);

# ConstInit_T *ConstInfo(
# 	char *FileName,			/**< Name of the file containing the list of values for this constant vector */
# 	int Format,			/**< Format of the list of value, 0: floating point, 1: fixed point */
# 	int Binary,			/**< If 1 file content is binary, if 0 file content is text */
# 	int Size,			/**< Size of the fixed point container */
# 	int Fract			/**< Fractional part size, Qx.Fract, note that x+Fract < Size */
# 	);


FMT_TYPES = {
    'float': 0,
    'fixed': 1
}


def gen_str(name):
    return '"%s"' % name


def gen_const_info(fname, qtype, fmt="fixed"):
    fmt = FMT_TYPES[fmt]
    return 'ConstInfo("{0}", {1}, 1, {2}, {3})'.format(fname, fmt, qtype.bits, qtype.q)


def gen_global_decl(name, qtype, home_location, exec_location, code_block, const_info=None):
    if const_info is None:
        const_info = "0"
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, {2}, {3}, {4})'
                     .format(qtype.ctype, name, home_location, exec_location, const_info))


def gen_stack_decl(out_name, in_names, code_block):
    code_block.write('AddStackedTensors("{}", {}, {});'
                     .format(out_name, len(in_names), ', '.join([gen_str(in_name) for in_name in in_names])))


def gen_local_decl(eparams, qtype, location, code_block):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_LOCAL, ARG_DIR_INOUT, {2}, {2}, 0)'
                     .format(qtype.ctype, eparams.name, location))


def gen_gnode_arg(direction, name):
    return 'GNodeArg({}, "{}", 0)'.format(direction, name)


def gen_g_arg(name):
    return 'GArg("{}")'.format(name)


def gen_g_node_c_arg(name):
    return 'GNodeCArg("{}")'.format(name)


def gen_imm_arg(symbol):
    return "Imm({})".format(symbol)

# AddNode("L0_1__3_16_224_224_C3x3_S1_PM2x2_S2_H",   Bindings(5, GNodeArg(GNA_IN, "DarknetIn",0),         GNodeArg(GNA_IN, "DarknetL0_1_Filter",0),   GNodeArg(GNA_IN, "DarknetL0_1_Bias",0),   GNodeArg(GNA_OUT, "DarknetL0_1_Out",0),   Imm(Norm)));


def gen_at_bindings(name, binding_list, code_block):
    code_block.write('AddNode("{0}", Bindings({1}, {2}));'
                     .format(name, len(binding_list), ", ".join(binding_list)))


def gen_at_func_bindings(name, func_name, where, binding_list, code_block):
    code_block.write('AddCallToNode("{0}", {1}, "{2}", Bindings({3}, {4}));'
                     .format(name, where, func_name, len(binding_list), ", ".join(binding_list)))


def is_dp(conv_q):
    # if conv_q.calc_q == conv_q.acc_q and\
    #     conv_q.acc_q.bits > conv_q.out_qs[0].bits:
    #     cop = "KOP_CONV_DP"
    # else:
    #     cop = "KOP_CONV"
    return True


def gen_conv_at_params(params, conv_q, pad_compatibilities, do_dp=False):
    if params.is_depthwise_conv():
        assert params.multiplier == 1, "Multiplier not supported"
        assert not do_dp, "No DP output for DW convolution"
        cop = is_dp(conv_q) and "KOP_CONV_DWDP" or "KOP_CONV_DW"
    elif params.is_grouped_conv():
        cop = is_dp(conv_q) and "KOP_CONV_DP" or "KOP_CONV"
        return GroupedConvATParam(
            ConvOper=cop,
            GroupIn=params.groups,
            GroupOut=params.multiplier,
            Fcx=params.filter.w,
            Fcy=params.filter.h,
            Dcx=params.dilation.w,
            Dcy=params.dilation.h,
            Scx=params.stride.w,
            Scy=params.stride.h,
            ConvPad=params.has_at_zero_pad() and 1 or 0
        )
    else:
        cop = is_dp(conv_q) and "KOP_CONV_DP" or "KOP_CONV"

    pad_compatibilities.append(params.padding.pad_compatibility)
    return ConvATParam(
        ConvOper=cop,
        Fcx=params.filter.w,
        Fcy=params.filter.h,
        Dcx=params.dilation.w,
        Dcy=params.dilation.h,
        Scx=params.stride.w,
        Scy=params.stride.h,
        ConvPad=params.has_at_zero_pad() and 1 or 0
    )


def gen_pool_at_params(params, pad_compatibilities):
    if params.pool_type == "average":
        pop = "KOP_AVGPOOL"
    elif params.pool_type == "max":
        pop = "KOP_MAXPOOL"
    else:
        raise NotImplementedError()

    pad_compatibilities.append(params.padding.pad_compatibility)
    return PoolATParam(
        PoolOper=pop,
        Fpx=params.filter.w,
        Fpy=params.filter.h,
        Dpx=1,
        Dpy=1,
        Spx=params.stride.w,
        Spy=params.stride.h,
        PoolPad=params.has_at_zero_pad() and 1 or 0
    )


def at_bits(qtype):
    if qtype is None:
        return 0
    # 1: byte, 2: half word, 4: word
    if qtype.bits == 8:
        return 1
    if qtype.bits == 16:
        return 2
    if qtype.bits == 32:
        return 4
    raise NotImplementedError("unsupported number of bits")


def at_q(qtype):
    if qtype is None:
        return 0
    return qtype.q


def at_bits_and_q(qtype):
    return "{}, {}".format(at_bits(qtype), qtype.q)


def gen_activation_op(activation):
    if activation is None or activation == "none":
        aop = "KOP_NONE"
    elif activation == "relu":
        aop = "KOP_RELU"
    elif activation == "relu6":
        aop = "KOP_RELUN"
    elif activation == "relun":
        aop = "KOP_RELUN"
    elif activation == "sigmoid" or activation == "hsigmoid":
        aop = "KOP_HSIGMOID"
    elif activation == "swish" or activation == "hswish":
        aop = "KOP_HSWISH"
    else:
        raise NotImplementedError("activation type %s not implemented" % activation)
    return aop


def gen_active_at_params(params):
    return ActivationATParam(
        ReLUOper=gen_activation_op(params.activation)
    )


def gen_linear_at_params(_):
    return LinearATParam(
        LinearOper="KOP_LINEAR"
    )


def gen_softmax_at_params(_):
    return SoftMaxATParam(
        SoftMaxOper="KOP_SOFTMAX"
    )


def gen_globalpool_at_params(params):
    return GlobalPoolATParam(
        GlobalPoolOper="KOP_GLOBAL_AVGPOOL" if params.pool_type == "average" else "KOP_GLOBAL_MAXPOOL"
    )


def gen_matrixadd_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD"
    )


def gen_matrixadddyn_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD_DYNADJUST"
    )


def gen_matscale_at_params(params):
    # KOP_MATSCALE_VECTOR, KOP_MATSCALE_SCALAR or KOP_MATSCALE_VECTOR_SCALAR
    if params.fusion_type == "vec_scalar":
        ms_op = 'KOP_MATSCALE_VECTOR_SCALAR'
    elif params.fusion_type == "vector":
        ms_op = 'KOP_MATSCALE_VECTOR'
    elif params.fusion_type == "scalar":
        ms_op = 'KOP_MATSCALE_SCALAR'
    else:
        raise NotImplementedError("unknown fusion type %s" % params.fusion_type)
    return MatScaleATParam(
        ScaleOper=ms_op,
        ReLUOper=gen_activation_op(params.activation)
    )


def gen_3d_transpose_at_params(params):
    if params.transpose_dimension == 2:
        perm = params.permute(['H', 'W'])
        permop = "KOP_MATPERM_CHW2C{}".format("".join(perm))
    else:
        perm = params.permute(['C', 'H', 'W'])
        permop = "KOP_MATPERM_CHW2{}".format("".join(perm))
    return ThreeDTensorTransposeATParam(
        MatPermOper=permop
    )

# extern void CNN_PoolReLU(
# 	char         *Name,

# 	CNN_GenControl_T *Ctrl,

# 	int In_DataSize,
# 	int Out_DataSize,

# 	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
# 	int Out_InL3,

# 	int InFeat,
# 	int OutFeat,
# 	int Width,
# 	int Height,

# 	KernelOper_T PoolOper,
# 	int Fpx,
# 	int Fpy,
# 	int Dpx,
# 	int Dpy,
# 	int Spx,
# 	int Spy,
# 	int          PoolPad,

# 	KernelOper_T ReLUOper
# 	);

# pylint: disable=too-many-arguments


def gen_at_pool_relu(code_block, name, in_q, out_q, in_dim,
                     out_dim, at_pool, at_active, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_pool.PoolOper == 'KOP_NONE':
        if in_dim.is_named and in_dim.has_keys(['c', 'w', 'h']):
            dims = [in_dim.c, in_dim.h, in_dim.w, in_dim.c]
        else:
            dims = in_dim.shape.copy()
            dims = dims + [1] * (4 - len(dims))

        if out_dim.is_named and out_dim.has_key('c'):
            dims[3] = out_dim.c
        else:
            dims[3] = dims[0]
    else:
        dims = [in_dim.c, in_dim.h, in_dim.w, out_dim.c]

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {},',
                         GEN_POOL_RELU, name, gen_ctrl, at_bits(in_q), at_bits(out_q),
                         dims[0], dims[3], dims[2], dims[1])
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {},',
                         GEN_POOL_RELU, name, gen_ctrl, at_bits(
                             in_q), at_bits(out_q), in_q.q, out_q.q,
                         dims[0], dims[3], dims[2], dims[1])
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});',
                     at_pool.PoolOper, at_pool.Fpx, at_pool.Fpy,
                     at_pool.Dpx, at_pool.Dpy, at_pool.Spx, at_pool.Spy,
                     at_pool.PoolPad, at_active.ReLUOper)
    code_block.deindent()

# extern void CNN_ConvolutionPoolReLU(
# 	char         *Name,

# 	CNN_GenControl_T *Ctrl,

# 	int In_DataSize,
# 	int Filter_DataSize,
# 	int Bias_DataSize,
# 	int Out_DataSize,

# 	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
# 	int Filter_InL3,
# 	int Bias_InL3,
# 	int Out_InL3,

# 	int InFeat,
# 	int OutFeat,
# 	int Width,
# 	int Height,

# 	KernelOper_T ConvOper,
# 	int Fcx,
# 	int Fcy,
# 	int Dcx,
# 	int Dcy,
# 	int Scx,
# 	int Scy,
# 	int          ConvPad,

# 	KernelOper_T PoolOper,
# 	int Fpx,
# 	int Fpy,
# 	int Dpx,
# 	int Dpy,
# 	int Spx,
# 	int Spy,
# 	int          PoolPad,

#        	KernelOper_T ReLUOper
# 	);

# pylint: disable=too-many-arguments


def gen_at_conv_pool_relu(code_block: CodeBlock, name, in_q, out_q,
                          filt_q, bias_q, in_dim, out_dim,
                          at_conv, at_pool, at_active, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_CONV_POOL_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_CONV_POOL_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_q.q, filt_q.q, bias_q.q, out_q.q,
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     at_conv.ConvOper, at_conv.Fcx, at_conv.Fcy,
                     at_conv.Dcx, at_conv.Dcy, at_conv.Scx, at_conv.Scy,
                     at_conv.ConvPad)
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});',
                     at_pool.PoolOper, at_pool.Fpx, at_pool.Fpy,
                     at_pool.Dpx, at_pool.Dpy, at_pool.Spx, at_pool.Spy,
                     at_pool.PoolPad, at_active.ReLUOper)
    code_block.deindent()

# extern void CNN_ConvolutionPoolReLU(
# 	char         *Name,

# 	CNN_GenControl_T *Ctrl,

#   GroupIn:        Size of the group for input features
#   GroupOut:       Size of the group for output features

# 	int In_DataSize,
# 	int Filter_DataSize,
# 	int Bias_DataSize,
# 	int Out_DataSize,

# 	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
# 	int Filter_InL3,
# 	int Bias_InL3,
# 	int Out_InL3,

# 	int InFeat,
# 	int OutFeat,
# 	int Width,
# 	int Height,

# 	KernelOper_T ConvOper,
# 	int Fcx,
# 	int Fcy,
# 	int Dcx,
# 	int Dcy,
# 	int Scx,
# 	int Scy,
# 	int          ConvPad,

# 	KernelOper_T PoolOper,
# 	int Fpx,
# 	int Fpy,
# 	int Dpx,
# 	int Dpy,
# 	int Spx,
# 	int Spy,
# 	int          PoolPad,

#        	KernelOper_T ReLUOper
# 	);

# pylint: disable=too-many-arguments


def gen_at_grouped_conv_pool_relu(code_block: CodeBlock, name, in_q, out_q,
                                  filt_q, bias_q, in_dim, out_dim,
                                  at_conv, at_pool, at_active, gen_ctrl=None,
                                  at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_GROUPED_CONV_POOL_RELU, name, gen_ctrl,
                         at_conv.GroupIn, at_conv.GroupOut,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_GROUPED_CONV_POOL_RELU, name, gen_ctrl,
                         at_conv.GroupIn, at_conv.GroupOut,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_q.q, filt_q.q, bias_q.q, out_q.q,
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     at_conv.ConvOper, at_conv.Fcx, at_conv.Fcy,
                     at_conv.Dcx, at_conv.Dcy, at_conv.Scx, at_conv.Scy,
                     at_conv.ConvPad)
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});',
                     at_pool.PoolOper, at_pool.Fpx, at_pool.Fpy,
                     at_pool.Dpx, at_pool.Dpy, at_pool.Spx, at_pool.Spy,
                     at_pool.PoolPad, at_active.ReLUOper)
    code_block.deindent()

# extern void CNN_LinearReLU(
#         char *Name,

# 	CNN_GenControl_T *Ctrl,

#         int In_DataSize,
#         int Filter_DataSize,
#         int Bias_DataSize,
#         int Out_DataSize,

#         int In_InL3,
#         int Filter_InL3,
#         int Bias_InL3,
#         int Out_InL3,

#         int InDim,
#         int OutDim,

#         KernelOper_T LinearOper,
#         KernelOper_T ReLUOper
#         );

# pylint: disable=too-many-arguments


def gen_at_linear_relu(code_block: CodeBlock, name, in_q, out_q,
                       filt_q, bias_q, in_dim, out_dim,
                       at_linear, at_active, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                         GEN_LINEAR_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_dim.size(), out_dim.size())
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                         GEN_LINEAR_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_q.q, filt_q.q, bias_q.q, out_q.q,
                         in_dim.size(), out_dim.size())
    code_block.indent()
    code_block.write('{}, {});',
                     at_linear.LinearOper, at_active.ReLUOper)
    code_block.deindent()

# int CNN_Mat2DTranspose(
#     char *Name,
#     CNN_GenControl_T *Ctrl,
#     int InOut_DataSize,
#     int In_InL3,
#     int Out_InL3,
#     int Width,
#     int Height,
#     KernelOper_T AddMatOper
# )


def gen_at_3d_transpose(code_block: CodeBlock, name, in_q, out_q,
                        in_shape, at_transpose_params, gen_ctrl=None,
                        at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {});',
                     GEN_3D_TRANSPOSE, name, gen_ctrl, at_bits(in_q), at_bits(out_q),
                     in_q.q, out_q.q, in_shape[0], in_shape[1], in_shape[2],
                     at_transpose_params.MatPermOper)


# extern void CNN_SoftMax(
#         char *Name,
# 	CNN_GenControl_T *Ctrl,
#         int In_DataSize,
#         int Out_DataSize,
#         int In_InL3,
#         int Out_InL3,
#         int Dim,
#         KernelOper_T SoftMaxOper
#         );

# pylint: disable=too-many-arguments


def gen_at_softmax(code_block: CodeBlock, name, in_q, out_q,
                   in_dim, at_softmax, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {});',
                         GEN_SOFTMAX, name, gen_ctrl,
                         at_bits(in_q), at_bits(out_q), in_dim.size(), at_softmax.SoftMaxOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {});',
                         GEN_SOFTMAX, name, gen_ctrl,
                         at_bits(in_q), at_bits(out_q), in_q.q, out_q.q, in_dim.size(), at_softmax.SoftMaxOper)

# /** \brief CNN_GlobalPool
#  *  Generator for Global Pooling (Max or Average)
#  * 
 
#     \param    Name:           Name of the generated user kernel

#     \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

#     \param    In_DataSize:    1: byte, 2: half word, 4: word
#     \param    Out_DataSize:   1: byte, 2: half word, 4: word

#     \param    In_Q:           In fixed point format
#     \param    Out_Q:          Out fixed point format

#     \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
#     \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

#     \param    InFeat:         Number of input feature's maps
#     \param    OutFeat:        Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
#     \param    Width:          Number of columns of a given feature map
#     \param    Height:         Number of lines of a given feature map

#     \param    PoolOper:       KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL


def gen_at_globalpool(code_block: CodeBlock, name, in_q, out_q,
                      in_dim, out_dim, at_globalpool, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(out_q), in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_globalpool.GlobalPoolOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(
                             out_q), in_q.q, out_q.q, in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_globalpool.GlobalPoolOper)

# pylint: disable=too-many-arguments


def gen_at_matrixadd(code_block: CodeBlock, name, in_q1, in_q2, out_q,
                     in_dim, out_dim, at_matrixadd, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                         GEN_MATADD, name, gen_ctrl,
                         at_bits(in_q1), at_bits(in_q2), at_bits(
                             out_q), in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                         GEN_MATADD, name, gen_ctrl,
                         at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                         in_q1.q, in_q2.q, out_q.q, in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# pylint: disable=too-many-arguments


def gen_at_matrixadddyn(code_block: CodeBlock, name, in_q1, in_q2, out_q,
                        in_dim, out_dim, at_matrixadd, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                     GEN_MATADDDYN, name, gen_ctrl,
                     at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                     in_q1.q, in_q2.q, out_q.q,
                     in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# pylint: disable=too-many-arguments


def gen_at_matscale(code_block: CodeBlock, name, other_q, vector_q, scalar_q, out_q,
                    in_dim, out_dim, at_matscale, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {}, {}, {});',
                     GEN_MATSCALE, name, gen_ctrl,
                     at_bits(other_q), at_bits(vector_q), at_bits(scalar_q), at_bits(out_q),
                     at_q(other_q), at_q(vector_q), at_q(scalar_q), at_q(out_q),
                     in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[2], in_dim.shape[1], at_matscale.ScaleOper, at_matscale.ReLUOper)

# convolution followed by a pool and optional relu
# pylint: disable=too-many-branches


def gen_conv_pool_relu(name, conv_params, conv_q, pool_params, pool_q, act_params, act_q, code_block=None, at_ver=3, gen_ctrl=None):

    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    in_q = filter_q = out_q = bias_q = None
    in_dim = out_dim = None
    pad_compatibilities = []
    if conv_params is not None:
        at_conv_params = gen_conv_at_params(conv_params, conv_q, pad_compatibilities)
        in_dim = conv_params.in_dims[0]
        out_dim = conv_params.out_dims[0]
        filter_q = conv_q.weights_q
        in_q = conv_q.in_qs[0]
        out_q = conv_q.out_qs[0]
        bias_q = conv_q.biases_q
    else:
        at_conv_params = NO_CONV

    if pool_params is not None:
        at_pool_params = gen_pool_at_params(pool_params, pad_compatibilities)
        if in_dim is None:
            in_dim = pool_params.in_dims[0]
        out_dim = pool_params.out_dims[0]
        if in_q is None:
            in_q = pool_q.in_qs[0]
        out_q = pool_q.out_qs[0]
    else:
        at_pool_params = NO_POOL

    if act_params is not None:
        at_act_params = gen_active_at_params(act_params)
        if in_dim is None:
            in_dim = act_params.in_dims[0]
        if out_dim is None:
            out_dim = act_params.out_dims[0]
        if in_q is None:
            in_q = act_q.in_qs[0]
        out_q = act_q.out_qs[0]
        if at_ver < 3:
            if act_params.activation == "relu6" and out_q.q != 0:
                gen_ctrl.ReluN = 6 << out_q.q
                gen_ctrl.ReluNNoNorm = 1
        else:
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params

    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    if pad_compatibilities:
        reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                    "convolution padding is not compatible with pool padding")
        if not reduction[2]:  # default is balanced pad left
            at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
            LOG.debug("%s: generating pad control block", name)
            gen_ctrl.PadType = at_pad_ctrl

    if not gen_ctrl.is_unmodified:
        gen_ctrl.gen_ctrl_decl(code_block)

    if conv_params is None:
        if in_q.bits != out_q.bits:
            raise NotImplementedError("only homogenious operations are supported at present")
        LOG.debug("%s: pool relu inq %s outq %s control block", name, in_q, out_q)
        gen_at_pool_relu(code_block, name, in_q, out_q,
                         in_dim, out_dim, at_pool_params, at_act_params, gen_ctrl=gen_ctrl,
                         at_ver=at_ver)
    else:
        if isinstance(at_conv_params, ConvATParam):
            LOG.debug("%s: conv pool relu inq %s outq %s control block", name, in_q, out_q)
            gen_at_conv_pool_relu(code_block, name, in_q, out_q,
                                  filter_q, bias_q,
                                  in_dim, out_dim, at_conv_params, at_pool_params,
                                  at_act_params, gen_ctrl=gen_ctrl, at_ver=at_ver)
        elif isinstance(at_conv_params, GroupedConvATParam):
            LOG.debug("%s: grouped conv pool relu inq %s outq %s control block", name, in_q, out_q)
            gen_at_grouped_conv_pool_relu(code_block, name, in_q, out_q,
                                          filter_q, bias_q,
                                          in_dim, out_dim, at_conv_params, at_pool_params,
                                          at_act_params, gen_ctrl=gen_ctrl, at_ver=at_ver)
        else:
            raise ValueError('Internal error')

    return code_block


def gen_pool_relu(name, pool_params, pool_q, act_params, act_q, code_block=None, at_ver=3, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    in_q = out_q = None
    in_dim = out_dim = None
    pad_compatibilities = []

    if pool_params is not None:
        at_pool_params = gen_pool_at_params(pool_params, pad_compatibilities)
        if in_dim is None:
            in_dim = pool_params.in_dims[0]
        out_dim = pool_params.out_dims[0]
        if in_q is None:
            in_q = pool_q.in_qs[0]
        out_q = pool_q.out_qs[0]
    else:
        at_pool_params = NO_POOL

    if act_params is not None:
        at_act_params = gen_active_at_params(act_params)
        if in_dim is None:
            in_dim = act_params.in_dims[0]
        if out_dim is None:
            out_dim = act_params.out_dims[0]
        if in_q is None:
            in_q = act_q.in_qs[0]
        out_q = act_q.out_qs[0]
        if at_ver < 3:
            if act_params.activation == "relu6" and out_q.q != 0:
                gen_ctrl.ReluN = 6 << out_q.q
                gen_ctrl.ReluNNoNorm = 1
        else:
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params
    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    if pad_compatibilities:
        reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                    "convolution padding is not compatible with pool padding")
        if not reduction[2]:  # default is balanced pad left
            at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
            gen_ctrl.PadType = at_pad_ctrl

    if not gen_ctrl.is_unmodified:
        gen_ctrl.gen_ctrl_decl(code_block)

    if in_q.bits != out_q.bits:
        raise NotImplementedError("only homogenious operations are supported at present")
    if pool_params is None:
        raise NotImplementedError("activation layer on its own is not implemented at present")
    gen_at_pool_relu(code_block, name, in_q, out_q,
                     in_dim, out_dim, at_pool_params, at_act_params, gen_ctrl=gen_ctrl,
                     at_ver=at_ver)
    return code_block


# linear followed by an optional relu
# pylint: disable=too-many-branches


def gen_linear_relu(name, linear_params, linear_q, act_params, act_q, code_block=None, at_ver=3, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    assert linear_params is not None, "linear should always be included"
    at_linear_params = gen_linear_at_params(linear_params)
    in_dim = linear_params.in_dims[0]
    out_dim = linear_params.out_dims[0]
    filter_q = linear_q.weights_q
    in_q = linear_q.in_qs[0]
    out_q = linear_q.out_qs[0]
    bias_q = linear_q.biases_q

    if act_params is not None:
        at_act_params = gen_active_at_params(act_params)
        out_q = act_q.out_qs[0]
        if at_ver < 3:
            if act_params.activation == "relu6" and out_q.q != 0:
                gen_ctrl.ReluN = 6 << out_q.q
                gen_ctrl.ReluNNoNorm = 1
        else:
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params
    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    gen_at_linear_relu(code_block, name, in_q, out_q,
                       filter_q, bias_q,
                       in_dim, out_dim, at_linear_params, at_act_params,
                       at_ver=at_ver)
    return code_block


def gen_3d_transpose(name, transpose_params, transpose_q, code_block=None):
    at_transpose_params = gen_3d_transpose_at_params(transpose_params)
    in_shape = transpose_params.in_dims[0].shape
    if transpose_params.transpose_dimension == 2:
        in_shape.insert(0, 1)
    in_q = transpose_q.in_qs[0]
    out_q = transpose_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_3d_transpose(code_block, name, in_q, out_q,
                        in_shape, at_transpose_params)
    return code_block


def gen_globalpool(name, globalpool_params, globalpool_q, code_block=None, at_ver=3):
    at_globalpool_params = gen_globalpool_at_params(globalpool_params)
    in_dim = globalpool_params.in_dims[0]
    out_dim = globalpool_params.out_dims[0]
    in_q = globalpool_q.in_qs[0]
    out_q = globalpool_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_globalpool(code_block, name, in_q, out_q,
                      in_dim, out_dim, at_globalpool_params, at_ver=at_ver)
    return code_block


def gen_softmax(name, softmax_params, softmax_q, code_block=None, at_ver=3):
    at_softmax_params = gen_softmax_at_params(softmax_params)
    in_dim = softmax_params.in_dims[0]
    in_q = softmax_q.in_qs[0]
    out_q = softmax_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_softmax(code_block, name, in_q, out_q,
                   in_dim, at_softmax_params, at_ver=at_ver)
    return code_block


def gen_matrixadd(name, matrixadd_params, matrixadd_q, code_block=None, at_ver=3):
    at_matrixadd_params = gen_matrixadd_at_params(matrixadd_params)
    in_dim = matrixadd_params.in_dims[0]
    out_dim = matrixadd_params.out_dims[0]
    in_q1 = matrixadd_q.in_qs[0]
    in_q2 = matrixadd_q.in_qs[1]
    out_q = matrixadd_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_matrixadd(code_block, name, in_q1, in_q2, out_q,
                     in_dim, out_dim, at_matrixadd_params, at_ver=at_ver)
    return code_block


def gen_matscale(name, params, qrec, code_block=None):
    at_matscale_params = gen_matscale_at_params(params)
    in_dim = params.in_dims[0]
    out_dim = params.out_dims[0]
    assert in_dim.shape[0] == out_dim.shape[0]
    if params.fusion_type == "vec_scalar":
        otherq = qrec.in_qs[0]
        vectorq = qrec.in_qs[1]
        scalarq = qrec.in_qs[2]
    elif params.fusion_type == "vector":
        otherq = qrec.in_qs[1]
        vectorq = qrec.in_qs[2]
        scalarq = None
    elif params.fusion_type == "scalar":
        otherq = qrec.in_qs[0]
        vectorq = None
        scalarq = qrec.in_qs[1]
    else:
        raise NotImplementedError("unknown fusion type %s" % params.fusion_type)

    if code_block is None:
        code_block = CodeBlock()

    gen_at_matscale(code_block, name, otherq, vectorq, scalarq, qrec.out_qs[0],
                    in_dim, out_dim, at_matscale_params)
    return code_block


def gen_matrixadddyn(name, matrixadd_params, matrixadd_q, code_block=None, at_ver=3):
    at_matrixadd_params = gen_matrixadddyn_at_params(matrixadd_params)
    in_dim = matrixadd_params.in_dims[0]
    out_dim = matrixadd_params.out_dims[0]
    in_q1 = matrixadd_q.in_qs[0]
    in_q2 = matrixadd_q.in_qs[1]
    out_q = matrixadd_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_matrixadddyn(code_block, name, in_q1, in_q2, out_q, in_dim,
                        out_dim, at_matrixadd_params)
    return code_block
