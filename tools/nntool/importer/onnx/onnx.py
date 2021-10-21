
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

from graph.constant_store import ConstantStore
from graph.dim import Dim
from graph.matches.matchers.duplicate_constants import MatchDuplicateConstants
from graph.nngraph import NNGraph
from graph.types import ConstantInputParameters
from graph.types.base import NNEdge
from importer.common.clean_dangling_nodes import clean_dangling_nodes

import onnx
from onnx import defs, numpy_helper, shape_inference
from onnx.helper import make_opsetid

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

    def create_graph(self, filename, opts):
        opts = self.get_opts(opts)
        model = onnx.load(filename)
        # onnx.checker.check_model(model)
        # model_with_shapes = shape_inference.infer_shapes(model)

        self._name_cache = {}
        if model.ir_version < 3:
            opset_import = [make_opsetid(defs.ONNX_DOMAIN, 1)]
        else:
            opset_import = model.opset_import
        G = NNGraph(filename=filename,
                    name=opts.get('name'),
                    constant_store=ConstantStore())
        G = self._import_onnx_model(G, model.graph, opset_import, opts)
        clean_dangling_nodes(G)
        MatchDuplicateConstants().match(G)
        return G

    def _import_onnx_model(self, G, graph, opset, opts):
        handlers = self._get_handlers(opset)
        all_nodes = {}
        constants = self._get_initializers(G, graph.initializer)
        all_nodes.update(constants)
        inputs = self._get_input_nodes(G, graph.input, constants,
                                       batch_hint=opts.get('batch_hint', None),
                                       substitutions=opts.get('substitutions', None))
        all_nodes.update(inputs)
        outputs = self._get_output_nodes(G, graph.output, substitutions=opts.get('substitutions', None))
        self._import_nodes(G, graph, handlers, all_nodes, outputs, opts)
        return G

    @staticmethod
    def _validate_name(name):
        return re.sub('\W|^(?=\d)','_', name)

    @staticmethod
    def _get_dim_from_shape(onnx_shape):
        return Dim.unnamed([d.dim_value for d in onnx_shape.dim
                            if (d.dim_value > 0 and d.dim_param == "")])

    def _get_handlers(self, opset):
        opset = opset or [make_opsetid(defs.ONNX_DOMAIN, defs.onnx_opset_version())]
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
                    constant_store=G.constant_store),
                0,
                ProvisionalDim(init.dims)
            )
            for init in initializer}

    def _get_input_nodes(self, G, inputs, constants, batch_hint=None, substitutions=None):
        prov_dims = {
            idx: ProvisionalDim.from_onnx_shape(input.type.tensor_type.shape, check_for_batch=0, substitutions=substitutions)
            for idx, input in enumerate(inputs) if input.name not in constants
        }
        if batch_hint is None and any(len(pshape.shape) >= 4 and pshape.shape[0] != 1 for pshape in prov_dims.values()):
            logger.warning("unable to determine batch dimension. if the graph fails to import properly set it to 1 or a variable.")
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
        return {
            input.name: (
                G.add_input(Dim.unnamed(prov_dims[idx].known_shape).apply_naming_hints(hints[idx]),
                            in_dim_hint=[hints[idx]] if hints[idx] else None,
                            out_dim_hint=[hints[idx]] if hints[idx] else None),
                0,
                prov_dims[idx]
            )
            for idx, input in enumerate(inputs) if input.name not in constants
        }

    def _get_output_nodes(self, G, outputs, substitutions=None):
        return {
            output.name: (
                G.add_output(),
                0,
                ProvisionalDim.from_onnx_shape(output.type.tensor_type.shape, substitutions=substitutions)
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

    def _import_nodes(self, G, graph, handlers, all_nodes, outputs, opts):
        used_tensors = set(all_nodes.keys()) | set(outputs.keys()) | set.union(*(set(node.input)
                                                                                 for node in graph.node))
        used_tensors.discard('')
        vars_dict = {}
        for node in graph.node:
            handler = handlers[node.domain].get(
                node.op_type, None) if node.domain in handlers else None
            if not handler:
                handler = handlers['__extensions'].get(node.op_type, None)
                if not handler:
                    raise ValueError(f"no handler found for '{node.op_type}' domain '{node.domain}'")

            params = handler.handle(OnnxNode(node), all_nodes=all_nodes, vars_dict=vars_dict,
                                    G=G, valid_name=self._node_name(node), opts=opts,
                                    used_tensors=used_tensors)
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
