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

import logging

from typing import Optional, Sequence, Mapping

import numpy as np

from utils.graph import Graph
from utils.node_id import NodeId
from graph.types import ConvFusionParameters, ActivationFusion, InputParameters, ConstantInputParameters, Parameters
from quantization.quantization_record_base import QuantizationRecordBase
from quantization.kernels.kernel_switch import KernelSwitchBase, DefaultKernelSwitch
from quantization.float32.float_kernet_set import Float32KernelSet
from quantization.symmetric.symmetric_kernet_set import SymmetricKernelSet
from execution.quantization_mode import QuantizationMode
from execution.execution_progress import ExecutionProgress

LOG = logging.getLogger('nntool.'+__name__)


class GraphExecuter():
    def __init__(self,
                 G: Graph,
                 qrecs: Optional[Mapping[NodeId, QuantizationRecordBase]] = None,
                 kernel_switch: Optional[KernelSwitchBase] = None,
                 quantized_kernel_switch: Optional[KernelSwitchBase] = None):
        self._G = G
        self._qrecs = qrecs
        self._kernel_switch = DefaultKernelSwitch(
            Float32KernelSet()) if kernel_switch is None else kernel_switch
        self._quantized_kernel_switch = DefaultKernelSwitch(
            SymmetricKernelSet()) if quantized_kernel_switch is None else quantized_kernel_switch

    def collect_outputs(self, saved_outputs, node):
        # collect outputs from previous nodes
        # InputNode is already set above
        if isinstance(node, InputParameters):
            output = None
        else:
            output = [None]*len(node.in_dims)
            for edge in self._G.in_edges(node.name):
                output[edge.to_idx] = saved_outputs[edge.from_node][edge.from_idx]
        return output

    @staticmethod
    def save_output(saved_outputs, node, outputs):
        saved_outputs[node] = outputs

    def execute_qnoq_iterator(self, in_tensors, step_idx_limit=None, silent=False, yield_fusions=True):

        if not silent:
            LOG.info("execute quantization comparison")
            ExecutionProgress.start()
        saved_outputs = {}
        for step_idx, step in enumerate(self._G.graph_state.steps):

            if step_idx_limit is not None and step_idx > step_idx_limit:
                break

            node = step['node']

            if not silent:
                ExecutionProgress.progress(step_idx, node.name)

            output = self.collect_outputs(saved_outputs, node)
            nid = NodeId(node, None)
            qrec = self._qrecs[nid]

            if isinstance(node, (ConvFusionParameters, ActivationFusion)):
                for fusion_node in node.contained_nodes():
                    fnid = NodeId(node, fusion_node)
                    fqrec = self._qrecs[fnid]

                    qoutput = []
                    for val_idx, val in enumerate(output):
                        qoutput.append(fqrec.in_qs[val_idx].quantize(val))

                    details = {}
                    output = self._kernel_switch.execute(fusion_node, output,
                                                         fqrec if self._G.has_quantized_parameters else None,
                                                         details=details)
                    qdetails = {}
                    qoutput = self._quantized_kernel_switch.execute(
                        fusion_node, qoutput, fqrec, details=qdetails)
                    qoutput = [fqrec.out_qs[i].dequantize(out) for i, out in enumerate(qoutput)]
                    if yield_fusions:
                        yield step_idx, node, output, details, qoutput, qdetails, fusion_node
            else:
                if isinstance(node, (InputParameters, ConstantInputParameters)):
                    details = {}
                    output = self._kernel_switch.execute(node, in_tensors,
                                                         qrec if self._G.has_quantized_parameters else None,
                                                         details=details)
                    qdetails = {}
                    qoutput = self._quantized_kernel_switch.execute(
                        node, in_tensors, qrec, details=qdetails)
                else:
                    qoutput = []
                    for val_idx, val in enumerate(output):
                        qoutput.append(qrec.in_qs[val_idx].quantize(val))
                    details = {}
                    output = self._kernel_switch.execute(node, output,
                                                         qrec if self._G.has_quantized_parameters else None,
                                                         details=details)
                    qdetails = {}
                    qoutput = self._quantized_kernel_switch.execute(
                        node, qoutput, qrec, details=qdetails)

                qoutput = [qrec.out_qs[i].dequantize(out) for i, out in enumerate(qoutput)]

            yield step_idx, node, output, details, qoutput, qdetails, None
            self.save_output(saved_outputs, node, output)

        if not silent:
            ExecutionProgress.end()

    def execute_iterator(self,
                         in_tensors: Sequence[np.ndarray],
                         step_idx_limit: Optional[int] = None,
                         start_node: Optional[Parameters] = None,
                         qmode: Optional[QuantizationMode] = None,
                         yield_fusions=True,
                         yield_details=True,
                         only_yield_step=False,
                         record_inputs: Optional[Mapping] = None,
                         silent=False):
        if qmode is None:
            qmode = QuantizationMode.none()

        saved_outputs = {}

        if not silent:
            LOG.info("execute uncached: quantization mode %s", qmode)
            ExecutionProgress.start()
        for step_idx, step in enumerate(self._G.graph_state.steps):

            if step_idx_limit is not None and step_idx > step_idx_limit:
                break

            node = step['node']

            if start_node and start_node != node:
                continue

            # collect outputs from previous nodes
            # InputNode is already set above
            output_tensors = self.collect_outputs(saved_outputs, node)

            if not silent:
                ExecutionProgress.progress(step_idx, node.name)
            nid = NodeId(node, None)
            if record_inputs is not None:
                if output_tensors is None:
                    record_inputs[nid] = output_tensors
                else:
                    record_inputs[nid] = [np.copy(output_tensor)
                                          for output_tensor in output_tensors]

            qrec = self._qrecs[nid] if self._qrecs is not None else None
            if qmode.get_quantized(node, step_idx):
                switch = self._quantized_kernel_switch
                if qmode.is_step and output_tensors:
                    output_tensors = [qrec.in_qs[i].quantize(
                        output_tensor) for i, output_tensor in enumerate(output_tensors)]
            else:
                switch = self._kernel_switch

            details = {} if yield_details and (
                not only_yield_step or step_idx == step_idx_limit) else None
            if isinstance(node, (ConvFusionParameters, ActivationFusion)):
                for fusion_node in node.contained_nodes():
                    fnid = NodeId(node, fusion_node)
                    fqrec = None if not qrec else self._qrecs[fnid]
                    if record_inputs is not None:
                        record_inputs[nid] = [np.copy(output_tensor)
                                              for output_tensor in output_tensors]
                    details = {} if yield_fusions and yield_details else None
                    output_tensors = switch.execute(fusion_node, output_tensors, fqrec, details)
                    if yield_fusions:
                        if qmode.dequantize:
                            qoutput_tensors = [fqrec.out_qs[i].dequantize(output_tensor)
                                               for i, output_tensor
                                               in enumerate(output_tensors)]
                            yield step_idx, node, fusion_node, qoutput_tensors, details
                        elif qmode.is_float_q_deq:
                            qoutput_tensors = [fqrec.out_qs[i].dequantize(fqrec.out_qs[i].quantize(output_tensor))
                                               for i, output_tensor
                                               in enumerate(output_tensors)]
                            yield step_idx, node, fusion_node, qoutput_tensors, details
                        else:
                            yield step_idx, node, fusion_node, output_tensors, details
            elif isinstance(node, InputParameters):
                output_tensors = switch.execute(node, in_tensors, qrec, details)
            else:
                output_tensors = switch.execute(node, output_tensors, qrec, details)

            if qmode.dequantize:
                qoutput_tensors = [qrec.out_qs[i].dequantize(
                    output_tensor) for i, output_tensor in enumerate(output_tensors)]
                if not only_yield_step or step_idx == step_idx_limit:
                    yield step_idx, node, None, qoutput_tensors, details
                if qmode.is_step and qmode.get_quantized(node, step_idx):
                    output_tensors = qoutput_tensors
            elif qmode.is_float_q_deq:
                if qmode.is_step and qmode.get_quantized(node, step_idx):
                    output_tensors = [qrec.out_qs[i].dequantize(
                        output_tensor) for i, output_tensor in enumerate(output_tensors)]
                qoutput_tensors = [qrec.out_qs[i].dequantize(qrec.out_qs[i].quantize(
                    output_tensor)) for i, output_tensor in enumerate(output_tensors)]
                if not only_yield_step or step_idx == step_idx_limit:
                    yield step_idx, node, None, qoutput_tensors, details
            else:
                if qmode.is_step and qmode.get_quantized(node, step_idx):
                    output_tensors = [qrec.out_qs[i].dequantize(
                        output_tensor) for i, output_tensor in enumerate(output_tensors)]
                if not only_yield_step or step_idx == step_idx_limit:
                    yield step_idx, node, None, output_tensors, details

            self.save_output(saved_outputs, node, output_tensors)

        if not silent:
            ExecutionProgress.end()

    def execute_qnoq(self,
                     in_tensors: Sequence[np.ndarray],
                     step_idx_limit=None,
                     all_details=None,
                     yield_fusions=False,
                     silent=False):
        outputs = []
        if yield_fusions:
            fusion_outputs = []
            if all_details is not None:
                fusion_details = []
        for _, _, _, _, qoutput, qdetails, fnode in self.execute_qnoq_iterator(in_tensors,
                                                                               step_idx_limit=step_idx_limit,
                                                                               silent=silent):
            if yield_fusions:
                if fnode:
                    fusion_outputs.append([output_tensor.copy()
                                           for output_tensor in qoutput])
                    if all_details is not None:
                        fusion_details.append(qdetails)
                else:
                    outputs.append({
                        'outputs': outputs.append([output_tensor.copy() for output_tensor in qoutput]),
                        'fusion_outputs': fusion_outputs.copy(),
                    })
                    fusion_outputs.clear()
                    if all_details is not None:
                        all_details.append({
                            'details': qdetails,
                            'fusion_details': fusion_details.copy()
                        })
                        fusion_details.clear()
            elif fnode is None:
                outputs.append([output_tensor.copy() for output_tensor in qoutput])
                if all_details is not None:
                    all_details.append(qdetails)
        return outputs

    def execute(self,
                in_tensors: Sequence[np.ndarray],
                step_idx_limit=None,
                only_yield_step=False,
                qmode: QuantizationMode = None,
                all_details=None,
                yield_fusions=False,
                silent=False):

        if qmode is None:
            qmode = QuantizationMode.none()

        if qmode.is_step_all:
            iterator = [(qoutput, qdetails, fnode)
                        for _, _, _, _, qoutput, qdetails, fnode
                        in self.execute_qnoq_iterator(in_tensors,
                                                      yield_fusions=yield_fusions,
                                                      step_idx_limit=step_idx_limit,
                                                      silent=silent)]
        else:
            iterator = [(output_tensors, details, fnode)
                        for _, _, fnode, output_tensors, details
                        in self.execute_iterator(in_tensors, step_idx_limit=step_idx_limit,
                                                 qmode=qmode,
                                                 yield_fusions=yield_fusions,
                                                 only_yield_step=only_yield_step,
                                                 yield_details=all_details is not None,
                                                 silent=silent)]

        outputs = []
        if yield_fusions:
            fusion_outputs = []
            if all_details is not None:
                fusion_details = []

        for output_tensors, details, fnode in iterator:
            if yield_fusions:
                if fnode:
                    fusion_outputs.append([output_tensor.copy()
                                           for output_tensor in output_tensors])
                    if all_details is not None:
                        fusion_details.append(details)
                else:
                    outputs.append({
                        'outputs': outputs.append([output_tensor.copy() for output_tensor in output_tensors]),
                        'fusion_outputs': fusion_outputs.copy(),
                    })
                    fusion_outputs.clear()
                    if all_details is not None:
                        all_details.append({
                            'details': details,
                            'fusion_details': fusion_details.copy()
                        })
                        fusion_details.clear()
            else:
                outputs.append([output_tensor.copy() for output_tensor in output_tensors])
                if all_details is not None:
                    all_details.append(details)
        return outputs
