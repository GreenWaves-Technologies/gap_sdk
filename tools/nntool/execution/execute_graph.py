# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
from typing import Optional, Sequence, Set, Mapping

from utils.graph import Node
from utils.node_id import NodeId
from graph.types import FusionParameters, InputParameters
from quantization.quantization_record import QuantizationRecord

from .executer import Executer

from .quantization_mode import QuantizationMode

LOG = logging.getLogger('nntool.'+__name__)

def __collect_outputs(saved_outputs, node, G):
    # collect outputs from previous nodes
    # InputNode is already set above
    if isinstance(node, InputParameters):
        output = None
    else:
        output = [None]*len(node.in_dims)
        for edge in G.in_edges(node.name):
            output[edge.to_idx] = saved_outputs[edge.from_node][edge.from_idx]
    return output

def __save_output(saved_outputs, node, outputs):
    saved_outputs[node] = outputs

def __quantize_input(qrec, vals):
    for val_idx, val in enumerate(vals):
        vals[val_idx] = qrec.in_qs[val_idx].quantize(val)

class ExecutionProgress(object):
    __instance = None
    def __new__(cls):
        if ExecutionProgress.__instance is None:
            ExecutionProgress.__instance = object.__new__(cls)
        return ExecutionProgress.__instance

    def __init__(self):
        if not hasattr(self, 'listeners'):
            self.listeners = []

    @classmethod
    def progress(cls, step_idx, name, is_cached=False):
        inst = cls()
        for func in inst.listeners:
            func(step_idx, name, is_cached)

    @classmethod
    def start(cls):
        inst = cls()
        for func in inst.listeners:
            func(None, "start")

    @classmethod
    def end(cls):
        inst = cls()
        for func in inst.listeners:
            func(None, "end")

    @classmethod
    def listen(cls, func):
        inst = cls()
        inst.listeners.append(func)

    @classmethod
    def unlisten(cls, func):
        inst = cls()
        inst.listeners.remove(func)

def execute_triangle_iterator(G,
                              in_tensors: Sequence,
                              qrecs,
                              value_cache,
                              include_nodes: Optional[Set[Node]] = None):

    LOG.info("execute triangle")

    cache_entry = value_cache.get_outputs(G, in_tensors[0])

    for step_idx, step in enumerate(G.graph_state.steps):
        node = step['node']

        if include_nodes and node not in include_nodes:
            continue

        # collect outputs from previous nodes
        # InputNode is already set above
        if isinstance(node, InputParameters):
            inputs = [in_tensors]
        else:
            inputs = [None]*len(node.in_dims)
            for edge in G.in_edges(node.name):
                inputs[edge.to_idx] = edge.from_node.value

            in_values = len(inputs[0])
            fixed_inputs = []
            for in_values_idx in range(in_values):
                fixed_inputs.append([None]*len(node.in_dims))
            for in_values_idx in range(in_values):
                fixed_input = fixed_inputs[in_values_idx]
                for input_idx in range(len(node.in_dims)):
                    if inputs[input_idx]:
                        fixed_input[input_idx] = inputs[input_idx][in_values_idx]
            inputs = fixed_inputs
            # # regroup inputs [[a1, a2], [b1, b2], ...] to [[a1, b1], [a2, b2], ...]
            # assert all(len(inputs[idx]) == len(inputs[0]) for idx in range(1, len(inputs)))
            # # pylint: disable=unsubscriptable-object
            # inputs = [[inp[idx] for inp in inputs] for idx in range(len(inputs[0]))]

        outputs = []
        qrec = qrecs[NodeId(node, None)]
        for inp in inputs:
            ExecutionProgress.progress(step_idx, node.name)

            if isinstance(node, FusionParameters):
                for fusion_node in node.contained_nodes():
                    fqrec = qrecs[NodeId(node, fusion_node)]
                    outputs.append(Executer.execute(fusion_node, inp, qrec=fqrec))
            else:
                outputs.append(Executer.execute(node, inp, qrec=qrec)[0])

        report_outputs = [qrec.out_qs[i](out) for i, out in enumerate(outputs)]
        report_outputs.append(cache_entry[step_idx][0])
        yield step_idx, node, report_outputs
        outputs.append(qrec.out_qs[0].quantize(cache_entry[step_idx][0]))
        node.value = outputs

# pylint: disable=too-many-locals, too-many-branches
def execute_cached_comparison_iterator(G,
                                       in_tensors: Sequence,
                                       cache_entry: Sequence,
                                       qrecs,
                                       limit: Optional[int] = None,
                                       start_node=None):
    '''Yields outputs from cache and calculated quantized output values
    simultaniously'''
    LOG.info("execute comparison")

    saved_outputs = {}
    ExecutionProgress.start()
    for step_idx, step in enumerate(G.graph_state.steps):

        if limit is not None and step_idx > limit:
            break

        node = step['node']

        if start_node and start_node != node:
            continue

        # collect outputs from previous nodes
        # InputNode is already set above
        output = __collect_outputs(saved_outputs, node, G)

        ExecutionProgress.progress(step_idx, node.name)
        if isinstance(node, FusionParameters):
            fusion_outputs = cache_entry[step_idx]
            for idx, fusion_node in enumerate(node.contained_nodes()):
                qoutput = []
                qrec = qrecs[node, fusion_node]
                for val_idx, val in enumerate(output):
                    qoutput.append(qrec.in_qs[val_idx].quantize(val))
                qoutput = Executer.execute(fusion_node, qoutput, qrec=qrec)
                output = fusion_outputs[idx]
                yield step_idx, node, output, qoutput, True
        else:
            # The first quantization occurs in the InputNode execute
            qrec = qrecs[node, None]
            if isinstance(node, InputParameters):
                qoutput = Executer.execute(node, in_tensors, qrec=qrec)
            else:
                qoutput = []
                for val_idx, val in enumerate(output):
                    qoutput.append(qrec.in_qs[val_idx].quantize(val))
                qoutput = Executer.execute(node, qoutput, qrec=qrec)
            output = cache_entry[step_idx]
            yield step_idx, node, output, qoutput, True

        node.value = output
    ExecutionProgress.end()

# pylint: disable=too-many-locals, too-many-branches
def execute_cached_iterator(G,
                            cache_entry: Sequence,
                            limit: Optional[int] = None,
                            qrecs: Mapping[NodeId, QuantizationRecord] = False,
                            qmode: QuantizationMode = None,
                            start_node=None):
    if qmode is None:
        qmode = QuantizationMode.none()
    LOG.info("execute cached: quantization mode %s", qmode)
    has_quantized = False
    saved_outputs = {}
    ExecutionProgress.start()
    for step_idx, step in enumerate(G.graph_state.steps):

        if limit is not None and step_idx > limit:
            break

        node = step['node']

        if start_node and start_node != node:
            continue

        # collect outputs from previous nodes
        # InputNode is already set above
        output = __collect_outputs(saved_outputs, node, G)

        key = NodeId(node, None)
        if qmode.is_step and qmode.get_quantized(node, step_idx) and output:
            qrec = qrecs[key]
            __quantize_input(qrec, output)
            has_quantized = True
        else:
            qrec = None

        if has_quantized:
            ExecutionProgress.progress(step_idx, node.name)
            if isinstance(node, FusionParameters):
                for fusion_node in node.contained_nodes():
                    fqrec = None if not qrec else qrecs[NodeId(node, fusion_node)]
                    output, details = Executer.execute(fusion_node, output, qrec=fqrec)
                    yield step_idx, step, node, output, fusion_node.op_name, fusion_node, {}
            else:
                output, details = Executer.execute(node, output, qrec=qrec)
        else:
            ExecutionProgress.progress(step_idx, node.name, is_cached=True)
            if isinstance(node, FusionParameters):
                fusion_outputs = cache_entry[step_idx]
                for fusion_idx, fusion_node in enumerate(node.contained_nodes()):
                    output = fusion_outputs[fusion_idx]
                    yield step_idx, step, node, output, fusion_node.op_name, fusion_node, {}
            else:
                output = cache_entry[step_idx]
                details = {}

        yield step_idx, step, node, output, None, None, details

        if qmode.is_step and qmode.get_quantized(node, step_idx):
            qrec = qrecs[key]
            output = [qrec.out_qs[i].dequantize(out) for i, out in enumerate(output)]

        __save_output(saved_outputs, node, output)
    ExecutionProgress.end()

# pylint: disable=too-many-locals
def execute_uncached_iterator(G,
                              in_tensors,
                              limit=None,
                              qrecs: Mapping[NodeId, QuantizationRecord] = False,
                              qmode: QuantizationMode = None,
                              start_node=None):
    if qmode is None:
        qmode = QuantizationMode.none()
    LOG.info("execute uncached: quantization mode %s", qmode)
    saved_outputs = {}
    ExecutionProgress.start()
    for step_idx, step in enumerate(G.graph_state.steps):

        if limit is not None and step_idx > limit:
            break

        node = step['node']

        if start_node and start_node != node:
            continue

        # collect outputs from previous nodes
        # InputNode is already set above
        output = __collect_outputs(saved_outputs, node, G)

        ExecutionProgress.progress(step_idx, node.name)

        if qmode.get_quantized(node, step_idx):
            qrec = qrecs[NodeId(node, None)]
            if qmode.is_step and output:
                __quantize_input(qrec, output)
        else:
            qrec = None

        if isinstance(node, FusionParameters):
            for fusion_node in node.contained_nodes():
                fqrec = None if not qrec else qrecs[NodeId(node, fusion_node)]
                output, details = Executer.execute(fusion_node, output, qrec=fqrec)
                yield step_idx, step, node, output, fusion_node.op_name, fusion_node, details
        elif isinstance(node, InputParameters):
            output, details = Executer.execute(node, in_tensors, qrec=qrec)
        else:
            output, details = Executer.execute(node, output, qrec=qrec)

        yield step_idx, step, node, output, None, None, details

        if qmode.is_step and qmode.get_quantized(node, step_idx):
            qrec = qrecs[NodeId(node, None)]
            output = [qrec.out_qs[i].dequantize(out) for i, out in enumerate(output)]

        __save_output(saved_outputs, node, output)
    ExecutionProgress.end()

# pylint: disable=too-many-arguments
def execute_iterator(G,
                     in_tensors,
                     limit=None,
                     qrecs: Mapping[NodeId, QuantizationRecord] = False,
                     qmode: QuantizationMode = None,
                     value_cache=None,
                     disable_cache=False):
    if qmode is None:
        qmode = QuantizationMode.none()
    value_cache = G.value_cache if value_cache is None and not disable_cache else value_cache
    if value_cache is not None and (qmode.is_none or qmode.is_step) and len(in_tensors) == 1:
        cached_outputs = value_cache.get_outputs(G, in_tensors[0])
        return execute_cached_iterator(G, cached_outputs, limit, qrecs, qmode)
    return execute_uncached_iterator(G, in_tensors, limit, qrecs, qmode)

# pylint: disable=too-many-arguments
def execute_comparison_iterator(G,
                                in_tensor,
                                qrecs: Mapping[NodeId, QuantizationRecord] = False,
                                limit=None,
                                value_cache=None):
    value_cache = G.value_cache if value_cache is None else value_cache
    assert value_cache
    cached_outputs = value_cache.get_outputs(G, in_tensor)
    return execute_cached_comparison_iterator(G, [in_tensor], cached_outputs, limit, qrecs)

# pylint: disable=too-many-arguments
def execute(G,
            in_tensors,
            limit=None,
            output_fn=None,
            qrecs: Mapping[NodeId, QuantizationRecord] = False,
            qmode: QuantizationMode = None,
            value_cache=None,
            dequantize=True,
            all_details=None):
    if qmode is None:
        qmode = QuantizationMode.none()
    outputs = []
    for _, step, node, output, fusion_op_name, fusion_node, details in\
        execute_iterator(G, in_tensors, limit, qrecs,
                         qmode, value_cache):
        if fusion_op_name is not None:
            if output_fn is not None:
                output_fn(step, node, output, fusion_op_name, fusion_node)
        else:
            if output_fn is not None:
                output_fn(step, node, output)
            if qmode.is_all and dequantize:
                qrec = qrecs[NodeId(node, None)]
                outputs.append([qrec.out_qs[i].dequantize(out) for i, out in enumerate(output)])
            else:
                outputs.append([out.copy() for out in output])
            if all_details is not None:
                all_details.append(details)
    return outputs
