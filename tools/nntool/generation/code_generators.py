# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

# pylint: disable=line-too-long
from graph.dim import PadDim
from .code_block import CodeBlock
from .kernel_parameters import ConvATParam, PoolATParam, ActivationATParam,\
                               LinearATParam, SoftMaxATParam, get_default_gen_ctrl,\
                               GroupedConvATParam, NO_ACTIVATION, NO_CONV, NO_POOL

GEN_CONV_POOL_RELU = "CNN_ConvolutionPoolReLU"
GEN_GROUPED_CONV_POOL_RELU = "CNN_GroupedConvolutionPoolReLU"
GEN_POOL_RELU = "CNN_PoolReLU"
GEN_LINEAR_RELU = "CNN_LinearReLU"
GEN_SOFTMAX = "CNN_SoftMax"

# TCArgInfo("int *__restrict__",         "JumboOut",       ARG_SCOPE_ARG,    ARG_DIR_OUT,     ARG_LOC_L2,      ARG_LOC_L2)

def gen_output_decl(eparams, in_q, home_location, exec_location, code_block):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_ARG, ARG_DIR_OUT, {2}, {3}, 0)'\
        .format(in_q.ctype, eparams.name, home_location, exec_location))

# TCArgInfo("short int *__restrict__",   "JumboIn",        ARG_SCOPE_ARG,    ARG_DIR_IN,      ARG_LOC_L2,      ARG_LOC_L2),

def gen_input_decl(eparams, out_q, home_location, exec_location, code_block):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_ARG, ARG_DIR_IN, {2}, {3}, 0)'\
        .format(out_q.ctype, eparams.name, home_location, exec_location))

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

def gen_const_info(fname, qtype, fmt="fixed"):
    fmt = FMT_TYPES[fmt]
    return 'ConstInfo("{0}", {1}, 1, {2}, {3})'.format(fname, fmt, qtype.bits, qtype.q)

def gen_global_decl(name, qtype, home_location, exec_location, code_block, const_info=None):
    if const_info is None:
        const_info = "0"
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, {2}, {3}, {4})'\
        .format(qtype.ctype, name, home_location, exec_location, const_info))

def gen_local_decl(eparams, qtype, location, code_block):
    code_block.write('TCArgInfo("{0} *__restrict__", "{1}", ARG_SCOPE_LOCAL, ARG_DIR_INOUT, {2}, {2}, 0)'\
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
    code_block.write('AddNode("{0}", Bindings({1}, {2}));'\
        .format(name, len(binding_list), ", ".join(binding_list)))

def gen_at_func_bindings(name, func_name, where, binding_list, code_block):
    code_block.write('AddCallToNode("{0}", {1}, "{2}", Bindings({3}, {4}));'\
        .format(name, where, func_name, len(binding_list), ", ".join(binding_list)))

def gen_conv_at_params(params, conv_q, pad_compatibilities, do_dp=False):
    if params.is_depthwise_conv():
        assert params.multiplier == 1, "Multiplier not supported"
        assert not do_dp, "No DP output for DW convolution"
        cop = "KOP_CONV_DW"
    elif params.is_grouped_conv():
        if conv_q.calc_q == conv_q.acc_q and\
           conv_q.acc_q.bits // 2 == conv_q.out_qs[0].bits:
            cop = "KOP_CONV_DP"
        else:
            cop = "KOP_CONV"
        return GroupedConvATParam(
            ConvOper=cop,
            GroupIn = params.groups,
            GroupOut = params.multiplier,
            Fcx=params.filter.w,
            Fcy=params.filter.h,
            Dcx=params.dilation.w,
            Dcy=params.dilation.h,
            Scx=params.stride.w,
            Scy=params.stride.h,
            ConvPad=params.has_at_zero_pad() and 1 or 0
        )
    else:
        # Generate DP operator
        if conv_q.calc_q == conv_q.acc_q and\
           conv_q.acc_q.bits // 2 == conv_q.out_qs[0].bits:
            cop = "KOP_CONV_DP"
        else:
            cop = "KOP_CONV"

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

def gen_active_at_params(params):
    if params.activation == "relu":
        aop = "KOP_RELU"
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
        gen_ctrl = "&{}".format(gen_ctrl)
 
    code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {},',
                     GEN_POOL_RELU, name, gen_ctrl, in_size, out_size,
                     in_dim.c, out_dim.c, in_dim.w, in_dim.h)
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
        gen_ctrl = "&{}".format(gen_ctrl)
 
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
        gen_ctrl = "&{}".format(gen_ctrl)
 
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
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                     GEN_LINEAR_RELU, name, gen_ctrl,
                     in_size, filt_size, bias_size, out_size,
                     in_dim.size(), out_dim.size())
    code_block.indent()
    code_block.write('{}, {});',
                     at_linear.LinearOper, at_active.ReLUOper)
    code_block.deindent()

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

# convolution followed by a pool and optional relu
# pylint: disable=too-many-branches
def gen_conv_pool_relu(name, conv_params, conv_q, pool_params, pool_q, act_params, act_q, code_block=None):
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
    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    if not pad_compatibilities:
        at_pad_ctrl = -1
    else:
        reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,\
            "convolution padding is not compatible with pool padding")
        if reduction[2]: # default is balanced pad left
            at_pad_ctrl = -1
        else:
            at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)

    gen_ctrl = None
    if at_pad_ctrl != -1:
        gen_ctrl = 'gen_ctrl_' + name
        code_block.write('CNN_GenControl_T {} = {{-1, -1, -1, -1, {}, -1, -1, -1}};', gen_ctrl, at_pad_ctrl)

    if conv_params is None:
        if in_q.bits != out_q.bits:
            raise NotImplementedError("only homogenious operations are supported at present")
        gen_at_pool_relu(code_block, name, at_bits(in_q), at_bits(out_q),
                         in_dim, out_dim, at_pool_params, at_act_params, gen_ctrl=gen_ctrl)
    else:
        if isinstance(at_conv_params, ConvATParam):
            gen_at_conv_pool_relu(code_block, name, at_bits(in_q), at_bits(out_q),
                                  at_bits(filter_q), at_bits(bias_q),
                                  in_dim, out_dim, at_conv_params, at_pool_params,
                                  at_act_params, gen_ctrl=gen_ctrl)
        elif isinstance(at_conv_params, GroupedConvATParam):
            gen_at_grouped_conv_pool_relu(code_block, name, at_bits(in_q), at_bits(out_q),
                                          at_bits(filter_q), at_bits(bias_q),
                                          in_dim, out_dim, at_conv_params, at_pool_params,
                                          at_act_params, gen_ctrl=gen_ctrl)
        else:
            raise ValueError('Internal error')

    return code_block

# convolution followed by a pool and optional relu
# pylint: disable=too-many-branches
def gen_linear_relu(name, linear_params, linear_q, act_params, act_q, code_block=None):
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
    else:
        at_act_params = NO_ACTIVATION

    if code_block is None:
        code_block = CodeBlock()

    gen_at_linear_relu(code_block, name, at_bits(in_q), at_bits(out_q),
                       at_bits(filter_q), at_bits(bias_q),
                       in_dim, out_dim, at_linear_params, at_act_params)
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
