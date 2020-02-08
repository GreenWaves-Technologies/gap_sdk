# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# pylint: disable=line-too-long

import logging

from graph.dim import PadDim

from .code_block import CodeBlock
from .kernel_parameters import (NO_ACTIVATION, NO_CONV, NO_POOL,
                                ActivationATParam, ConvATParam,
                                GroupedConvATParam, LinearATParam,
                                MatrixAddATParam, PoolATParam, SoftMaxATParam,
                                TwoDTransposeATParam, GenCtrl)

LOG = logging.getLogger("nntool." + __name__)

GEN_CONV_POOL_RELU = "CNN_ConvolutionPoolReLU"
GEN_GROUPED_CONV_POOL_RELU = "CNN_GroupedConvolutionPoolReLU"
GEN_POOL_RELU = "CNN_PoolReLU"
GEN_LINEAR_RELU = "CNN_LinearReLU"
GEN_SOFTMAX = "CNN_SoftMax"
GEN_MATADD = "CNN_MatAdd"
GEN_MATADDDYN = "CNN_MatAddDynAdjust"
GEN_2D_TRANSPOSE = "CNN_Mat2DTranspose"


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
    # 1: byte, 2: half word, 4: word
    if qtype.bits == 8:
        return 1
    if qtype.bits == 16:
        return 2
    if qtype.bits == 32:
        return 4
    raise NotImplementedError("unsupported number of bits")


def at_bits_and_q(qtype):
    return "{}, {}".format(at_bits(qtype), qtype.q)


def gen_active_at_params(params):
    if params.activation == "relu":
        aop = "KOP_RELU"
    elif params.activation == "relu6":
        aop = "KOP_RELUN"
    elif params.activation == "sigmoid":
        aop = "KOP_HSIGMOID"
    else:
        raise NotImplementedError()
    return ActivationATParam(
        ReLUOper=aop
    )


def gen_linear_at_params(_):
    return LinearATParam(
        LinearOper="KOP_LINEAR"
    )


def gen_softmax_at_params(_):
    return SoftMaxATParam(
        SoftMaxOper="KOP_SOFTMAX"
    )


def gen_matrixadd_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD"
    )


def gen_matrixadddyn_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD_DYNADJUST"
    )


def gen_2d_transpose_at_params(params):
    size = params.transpose_size
    return TwoDTransposeATParam(
        TwoDTransposeOper="KOP_MAT2DTRANSPOSE",
        Width=size[0],
        Height=size[1]
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


def gen_at_pool_relu(code_block, name, in_size, out_size, in_dim,
                     out_dim, at_pool, at_active, gen_ctrl=None):
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

    code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {},',
                     GEN_POOL_RELU, name, gen_ctrl, in_size, out_size,
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


def gen_at_conv_pool_relu(code_block: CodeBlock, name, in_size, out_size,
                          filt_size, bias_size, in_dim, out_dim,
                          at_conv, at_pool, at_active, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                     GEN_CONV_POOL_RELU, name, gen_ctrl,
                     in_size, filt_size, bias_size, out_size,
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


def gen_at_grouped_conv_pool_relu(code_block: CodeBlock, name, in_size, out_size,
                                  filt_size, bias_size, in_dim, out_dim,
                                  at_conv, at_pool, at_active, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                     GEN_GROUPED_CONV_POOL_RELU, name, gen_ctrl,
                     at_conv.GroupIn, at_conv.GroupOut,
                     in_size, filt_size, bias_size, out_size,
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


def gen_at_linear_relu(code_block: CodeBlock, name, in_size, out_size,
                       filt_size, bias_size, in_dim, out_dim,
                       at_linear, at_active, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                     GEN_LINEAR_RELU, name, gen_ctrl,
                     in_size, filt_size, bias_size, out_size,
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


def gen_at_2d_transpose(code_block: CodeBlock, name, in_size, out_size,
                        in_dim, at_transpose_params, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, 1, 1, {}, {}, {});',
                     GEN_2D_TRANSPOSE, name, gen_ctrl, in_size,
                     at_transpose_params.Width, at_transpose_params.Height,
                     at_transpose_params.TwoDTransposeOper)


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


def gen_at_softmax(code_block: CodeBlock, name, in_size, out_size,
                   in_dim, at_softmax, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {});',
                     GEN_SOFTMAX, name, gen_ctrl,
                     in_size, out_size, in_dim.size(), at_softmax.SoftMaxOper)

# pylint: disable=too-many-arguments


def gen_at_matrixadd(code_block: CodeBlock, name, in_size1, in_size2, out_size,
                     in_dim, out_dim, at_matrixadd, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                     GEN_MATADD, name, gen_ctrl,
                     in_size1, in_size2, out_size, in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# pylint: disable=too-many-arguments


def gen_at_matrixadddyn(code_block: CodeBlock, name, in_size1, in_size2, out_size,
                        inq1, inq2, outq, in_dim, out_dim, at_matrixadd, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {}, {}, {}, {});',
                     GEN_MATADDDYN, name, gen_ctrl,
                     in_size1, in_size2, out_size,
                     inq1, inq2, outq,
                     in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# convolution followed by a pool and optional relu
# pylint: disable=too-many-branches


def gen_conv_pool_relu(name, conv_params, conv_q, pool_params, pool_q, act_params, act_q, code_block=None, at_ver=2, gen_ctrl=None):

    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    if at_ver < 3:
        fsize = at_bits
    else:
        fsize = at_bits_and_q
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
        if act_params.activation == "relu6" and out_q.q != 0:
            gen_ctrl.ReluN = 6 << out_q.q
            gen_ctrl.ReluNNoNorm = 1
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
        gen_at_pool_relu(code_block, name, fsize(in_q), fsize(out_q),
                         in_dim, out_dim, at_pool_params, at_act_params, gen_ctrl=gen_ctrl)
    else:
        if isinstance(at_conv_params, ConvATParam):
            LOG.debug("%s: conv pool relu inq %s outq %s control block", name, in_q, out_q)
            gen_at_conv_pool_relu(code_block, name, fsize(in_q), fsize(out_q),
                                  fsize(filter_q), fsize(bias_q),
                                  in_dim, out_dim, at_conv_params, at_pool_params,
                                  at_act_params, gen_ctrl=gen_ctrl)
        elif isinstance(at_conv_params, GroupedConvATParam):
            LOG.debug("%s: grouped conv pool relu inq %s outq %s control block", name, in_q, out_q)
            gen_at_grouped_conv_pool_relu(code_block, name, fsize(in_q), fsize(out_q),
                                          fsize(filter_q), fsize(bias_q),
                                          in_dim, out_dim, at_conv_params, at_pool_params,
                                          at_act_params, gen_ctrl=gen_ctrl)
        else:
            raise ValueError('Internal error')

    return code_block


def gen_pool_relu(name, pool_params, pool_q, act_params, act_q, code_block=None, at_ver=2, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    if at_ver < 3:
        fsize = at_bits
    else:
        fsize = at_bits_and_q
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
        if act_params.activation == "relu6" and out_q.q != 0:
            gen_ctrl.ReluN = 6 << out_q.q
            gen_ctrl.ReluNNoNorm = 1
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
    gen_at_pool_relu(code_block, name, fsize(in_q), fsize(out_q),
                     in_dim, out_dim, at_pool_params, at_act_params, gen_ctrl=gen_ctrl)
    return code_block


# linear followed by an optional relu
# pylint: disable=too-many-branches


def gen_linear_relu(name, linear_params, linear_q, act_params, act_q, code_block=None, at_ver=2, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = GenCtrl(None, cname=name)
    else:
        gen_ctrl.cname = name

    if at_ver < 3:
        fsize = at_bits
    else:
        fsize = at_bits_and_q
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
        if act_params.activation == "relu6" and out_q.q != 0:
            gen_ctrl.ReluN = 6 << out_q.q
            gen_ctrl.ReluNNoNorm = 1
    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    gen_at_linear_relu(code_block, name, fsize(in_q), fsize(out_q),
                       fsize(filter_q), fsize(bias_q),
                       in_dim, out_dim, at_linear_params, at_act_params)
    return code_block


def gen_2d_transpose(name, transpose_params, transpose_q, code_block=None):
    at_transpose_params = gen_2d_transpose_at_params(transpose_params)
    in_dim = transpose_params.in_dims[0]
    in_q = transpose_q.in_qs[0]
    out_q = transpose_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_2d_transpose(code_block, name, at_bits(in_q), at_bits(out_q),
                        in_dim, at_transpose_params)
    return code_block


def gen_softmax(name, softmax_params, softmax_q, code_block=None):
    at_softmax_params = gen_softmax_at_params(softmax_params)
    in_dim = softmax_params.in_dims[0]
    in_q = softmax_q.in_qs[0]
    out_q = softmax_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_softmax(code_block, name, at_bits(in_q), at_bits(out_q),
                   in_dim, at_softmax_params)
    return code_block


def gen_matrixadd(name, matrixadd_params, matrixadd_q, code_block=None):
    at_matrixadd_params = gen_matrixadd_at_params(matrixadd_params)
    in_dim = matrixadd_params.in_dims[0]
    out_dim = matrixadd_params.out_dims[0]
    in_q1 = matrixadd_q.in_qs[0]
    in_q2 = matrixadd_q.in_qs[1]
    out_q = matrixadd_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_matrixadd(code_block, name, at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                     in_dim, out_dim, at_matrixadd_params)
    return code_block


def gen_matrixadddyn(name, matrixadd_params, matrixadd_q, code_block=None):
    at_matrixadd_params = gen_matrixadddyn_at_params(matrixadd_params)
    in_dim = matrixadd_params.in_dims[0]
    out_dim = matrixadd_params.out_dims[0]
    in_q1 = matrixadd_q.in_qs[0]
    in_q2 = matrixadd_q.in_qs[1]
    out_q = matrixadd_q.out_qs[0]

    if code_block is None:
        code_block = CodeBlock()

    gen_at_matrixadddyn(code_block, name, at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                        in_q1.q, in_q2.q, out_q.q, in_dim, out_dim, at_matrixadd_params)
    return code_block
