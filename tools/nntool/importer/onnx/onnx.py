
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

import re
from typing import Union

import onnx
from onnx import defs, numpy_helper, shape_inference
from onnx.helper import make_opsetid

import numpy as np
from graph.dim import Dim
from graph.matches.matchers.duplicate_constants import MatchDuplicateConstants
from graph.nngraph import NNGraph
from graph.types import ConstantInputParameters
from graph.types.base import NNEdge
from importer.common.clean_dangling_nodes import clean_dangling_nodes
from importer.common.propagate_qrecs import propagate_qrecs
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantization_set import QuantizationSet
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.node_id import NodeId

from ..common.provisional_dim import ProvisionalDim
from ..importer_base import ImporterBase
from .common import logger
from .common.handler_helper import get_all_backend_handlers
from .common.onnx_node import OnnxNode

# pylint: disable=E1101


class OnnxImporter(ImporterBase):
    def __init__(self, *args, **kwargs) -> None:
        super(OnnxImporter, self).__init__(*args, **kwargs)
        self._name_cache = None
        self._handlers = None

    def create_graph(self, filename, opts) -> NNGraph:
        opts = self.get_opts(opts)
        model = onnx.load(filename)

        # onnx.checker.check_model(model)
        try:
            model = shape_inference.infer_shapes(model)
        except RuntimeError as ex:
            msg = "\n".join(f">   {line}" for line in str(
                ex).split("\n") if line)
            logger.warning('shape inference failed on onnx graph. '
                           f'This may not affect import.\nONNX runtime error was:\n{msg}')

        self._name_cache = {}
        if model.ir_version < 3:
            opset_import = [make_opsetid(defs.ONNX_DOMAIN, 1)]
        else:
            opset_import = model.opset_import
        G = NNGraph(filename=filename,
                    name=opts.get('name'))
        G, qrecs = self._import_onnx_model(G, model.graph, opset_import, opts)
        G.add_dimensions(quiet=True)
        if qrecs:
            propagate_qrecs(G, qrecs)
            qset = QuantizationSet()
            qset.update(qrecs)
            qset.scheme_priority = ['SQ8']
            qset.schemes_present = {'SQ8'}
            G.quantization = qset
            try:
                quantizer = NewQuantizer(G)
                quantizer.quantize()
            except ValueError as ex:
                logger.warning(
                    f'unable to import quantization from FakeQuantize nodes correctly - {ex}')

        clean_dangling_nodes(G)
        MatchDuplicateConstants().match(G)
        return G

    def _update_qrecs(self, G, qrecs, all_nodes, ranges_dict):
        for node, idx, _, qtype in all_nodes.values():
            if qtype is None and node.name not in ranges_dict.keys():
                continue
            if node.name not in G:
                continue
            nid = NodeId(node)
            qrec = qrecs.get(nid)
            if not qrec:
                in_qs = [None] * G.num_in_edges(node)
                out_qs = [None] * len(G.indexed_out_edges(node))
                qrec = QRec.scaled(in_qs=in_qs, out_qs=out_qs)
                qrecs[nid] = qrec

            if node.name in ranges_dict.keys():
                out_min, out_max = ranges_dict[node.name]["range"]
                dtype = ranges_dict[node.name].get("dtype", np.int8)
                bits = ranges_dict[node.name].get("n_bits", 8)
                channel = ranges_dict[node.name].get("per_channel", None)
                qtype = QType.from_min_max_sq(
                    out_min, out_max, dtype=dtype, bits=bits, quantized_dimension=channel)
            qrec.out_qs[idx] = qtype

    def _import_onnx_model(self, G, graph, opset, opts):
        self._handlers = self._get_handlers(opset)
        all_nodes = {}
        constants = self._get_initializers(G, graph.initializer)
        all_nodes.update(constants)
        inputs = self._get_input_nodes(G, graph.input, constants,
                                       batch_hint=opts.get('batch_hint', None),
                                       substitutions=opts.get(
                                           'substitutions', None),
                                       fixed_inputs=opts.get(
                                           'fixed_inputs', {}),
                                       input_shapes=opts.get('input_shapes', {}))
        all_nodes.update(inputs)
        qrecs = {}
        outputs = self._get_output_nodes(
            G, graph.output, substitutions=opts.get('substitutions', None))
        shapes = {elem.name: elem.type for elem in graph.value_info}
        self._import_nodes(
            G, graph, self._handlers, all_nodes, outputs,
            opts=opts, qrecs=qrecs, shapes=shapes)
        self._update_qrecs(G, qrecs, all_nodes, opts.get('ranges_dict', {}))
        return G, qrecs

    def import_subgraph(self, G, graph, opts, all_nodes=None):
        if all_nodes is None:
            all_nodes = {}
        constants = self._get_initializers(G, graph.initializer)
        all_nodes.update(constants)
        inputs = self._get_input_nodes(G, graph.input, constants,
                                       batch_hint=opts.get('batch_hint', None),
                                       substitutions=opts.get(
                                           'substitutions', None),
                                       set_name=True)
        all_nodes.update(inputs)
        qrecs = {}
        outputs = self._get_output_nodes(
            G, graph.output, substitutions=opts.get('substitutions', None), set_name=True)
        self._import_nodes(
            G, graph, self._handlers, all_nodes, outputs,
            opts=opts, qrecs=qrecs)
        self._update_qrecs(G, qrecs, all_nodes, {})
        return G, qrecs

    @staticmethod
    def _validate_name(name):
        return re.sub(r'\W|^(?=\d)', '_', name)

    @staticmethod
    def _get_dim_from_shape(onnx_shape):
        return Dim.unnamed([d.dim_value for d in onnx_shape.dim
                            if (d.dim_value > 0 and d.dim_param == "")])

    def _get_handlers(self, opset):
        opset = opset or [make_opsetid(
            defs.ONNX_DOMAIN, defs.onnx_opset_version())]
        opset_dict = dict([(o.domain, o.version) for o in opset])
        return get_all_backend_handlers(opset_dict)

    @staticmethod
    def _get_numpy_array(val):
        val = numpy_helper.to_array(val)
        if val.shape == ():
            val = val.reshape([1])
        return val

    def _get_initializers(self, G, initializer):
        return {
            init.name: (
                ConstantInputParameters(
                    f'constant_{self._validate_name(init.name)}',
                    dims=Dim.unnamed(init.dims or [1]),
                    value=self._get_numpy_array(init),
                    imported_dtype=self.get_onnx_tensor_dtype(init)),
                0,
                ProvisionalDim(init.dims),
                None
            )
            for init in initializer}

    @staticmethod
    def get_onnx_tensor_dtype(onnx_tensor: Union[onnx.ValueInfoProto, onnx.TensorProto]) -> np.dtype:
        if isinstance(onnx_tensor, onnx.TensorProto):
            onnx_type = onnx_tensor.data_type
        else:
            onnx_type = onnx_tensor.type.tensor_type.elem_type
        if onnx_type in onnx.mapping.TENSOR_TYPE_TO_NP_TYPE:
            return onnx.mapping.TENSOR_TYPE_TO_NP_TYPE[onnx_type]
        return None

    def _get_input_nodes(self, G, inputs, constants, batch_hint=None,
                         substitutions=None, set_name=False, fixed_inputs=None, input_shapes=None):
        if fixed_inputs is None:
            fixed_inputs = {}
        if input_shapes is None:
            input_shapes = {}
        prov_dims = {}
        res = {}
        for inp_idx, inp in enumerate(inputs):
            if inp.name in fixed_inputs:
                val = fixed_inputs[inp.name]
                res[inp.name] = (
                    ConstantInputParameters(
                        f'constant_{self._validate_name(inp.name)}',
                        dims=Dim.unnamed(val.shape),
                        value=val,
                        imported_dtype=val.dtype),
                    0,
                    ProvisionalDim(val.shape),
                    None
                )
                continue

            tshape = ProvisionalDim.from_onnx_shape(
                inp.type.tensor_type.shape, check_for_batch=0, substitutions=substitutions).shape
            if inp.name in input_shapes:
                if len(tshape) != len(input_shapes[inp.name]):
                    raise ValueError(f'attempt to override shape for {inp.name} failed. Input shape is {tshape}. '
                                     f'Override shape is {input_shapes[inp.name]}')
                tshape = [t1 if t2 is None else t2 for t1,
                          t2 in zip(tshape, input_shapes[inp.name])]
            prov_dims[inp_idx] = ProvisionalDim(tshape)

        if batch_hint is None and any(len(pshape.shape) >= 4 and pshape.shape[0] != 1 for pshape in prov_dims.values()):
            logger.warning(
                "unable to determine batch dimension. if the graph fails to import properly set it to 1 or a variable.")
            batch_hint = 0

        if batch_hint is not None:
            prov_dims = {idx: dim.eliminate_dimension(batch_hint)
                         for idx, dim in prov_dims.items()}

        hints = {
            idx: (['c', 'h', 'w'] if (len(dim.shape) == 4 and
                                      (dim.shape[1] == 1 or dim.shape[1] == 3))
                  else None)
            for idx, dim in prov_dims.items()
        }
        res.update({
            inp.name: (
                G.add_input(Dim.unnamed(prov_dims[idx].known_shape).apply_naming_hints(hints[idx]),
                            in_dims_hint=[hints[idx]] if hints[idx] else None,
                            out_dims_hint=[hints[idx]] if hints[idx] else None,
                            imported_dtype=self.get_onnx_tensor_dtype(inp),
                            name=inp.name if set_name else None),
                0,
                prov_dims[idx],
                None
            )
            for idx, inp in enumerate(inputs) if inp.name not in constants and inp.name not in fixed_inputs
        })
        return res

    def _get_output_nodes(self, G, outputs, substitutions=None, set_name=False):
        return {
            output.name: (
                G.add_output(name=output.name if set_name else None),
                0,
                ProvisionalDim.from_onnx_shape(
                    output.type.tensor_type.shape, substitutions=substitutions),
                None
            )
            for output in outputs}

    def _node_name(self, node):
        name = hasattr(node, 'name') and getattr(node, 'name')
        if name:
            return self._validate_name(name)
        if len(node.output) > 1:
            count = self._name_cache.get(node.op_type)
            count = count + 1 if count is not None else 0
            self._name_cache[node.op_type] = count
            return "%s_%s" % (node.op_type, count)
        return "%s_%s" % (node.op_type, self._validate_name(node.output[0]))

    # Non functional node sorting to work around networks with bad node order
    # Shouldn't be necessary
    # def _walk_nodes(self, in_name, nodes_by_input, seen):
    #     seen.add(in_name)
    #     nodes = []
    #     if in_name not in nodes_by_input:
    #         return nodes
    #     for node in nodes_by_input[in_name]:
    #         if not all(inp in seen for inp in node.input):
    #             continue
    #         nodes.append(node)
    #         for out_name in node.output:
    #             nodes.extend(self._walk_nodes(out_name, nodes_by_input, seen))
    #     return nodes

    # def _sort_nodes(self, nodes, used_tensors):
    #     nodes_by_output = {output: node for node in nodes for output in node.output}
    #     nodes_by_input = {}
    #     for node in nodes:
    #         for inpname in node.input:
    #             in_nodes = nodes_by_input.setdefault(inpname, [])
    #             in_nodes.append(node)
    #     out_names = set(nodes_by_output.keys())
    #     start_names = (used_tensors|set(nodes_by_input.keys())) - out_names
    #     nodes = []
    #     seen = set()
    #     for in_name in start_names:
    #         nodes.extend(self._walk_nodes(in_name, nodes_by_input, seen))
    #     return nodes

    def _import_nodes(self, G, graph, handlers, all_nodes, outputs, **kwargs):
        used_tensors = set(all_nodes.keys()) | set(outputs.keys()) | set.union(*(set(node.input)
                                                                                 for node in graph.node))
        used_tensors.discard('')
        vars_dict = {}
        banned_inputs = set()
        for node in graph.node:
            if any(inp in banned_inputs for inp in node.input):
                logger.warning(
                    f'{node.name} has an input from a node that was not imported - skipping')
                banned_inputs.update(node.output)
                continue
            handler = handlers[node.domain].get(
                node.op_type, None) if node.domain in handlers else None
            if not handler or (handler.CONSTANT_ONLY and
                               not all(isinstance(all_nodes[inp_name][0], ConstantInputParameters)
                                       for inp_name in node.input)):
                handler = handlers['__extensions'].get(node.op_type, None)
                if not handler:
                    logger.warning(
                        f'{node.name} uses ONNX operator "{node.op_type}" domain '
                        f'"{node.domain}" which is not currently supported')
                    logger.warning(
                        f"{node.name} - creating outputs at this node's inputs")
                    for inp_name in node.input:
                        if inp_name not in all_nodes:
                            continue
                        producer = all_nodes[inp_name]
                        if isinstance(producer[0], ConstantInputParameters):
                            continue
                        output = G.add_output(name=G.unique_name(
                            f'Input_{inp_name}_to_{node.name}'))
                        G.add_edge(NNEdge(from_node=producer[0],
                                          to_node=output,
                                          from_idx=producer[1]))
                    banned_inputs.update(node.output)
                    continue
            onode = OnnxNode(node)
            inputs = [all_nodes[inp] if inp else None for inp in onode.input]
            if inputs:
                x = inputs[0]
                x_shape = x[2].shape
                name = hasattr(node, 'name') and getattr(node, 'name')
                x=0
            params = handler.handle(onode, all_nodes=all_nodes, vars_dict=vars_dict,
                                    G=G, valid_name=self._node_name(node),
                                    used_tensors=used_tensors, importer=self, **kwargs)
            if params is None:
                continue
            # some handlers set the meta information
            if 'onnx_output' not in params.meta:
                params.meta['onnx_output'] = list(node.output)
            for out_name in node.output:
                output = outputs.get(out_name)
                if not output:
                    continue
                # extra this from all nodes since some handlers add multiple nodes
                producer = all_nodes[out_name]
                G.add_edge(NNEdge(from_node=producer[0],
                                  to_node=output[0],
                                  from_idx=producer[1],
                                  to_idx=output[1]))
        banned_outputs = banned_inputs.intersection(set(outputs))
        for output_name in banned_outputs:
            output_node = outputs[output_name][0]
            logger.warning(f'removing unused output {output_node.name}')
            G.remove(output_node)
