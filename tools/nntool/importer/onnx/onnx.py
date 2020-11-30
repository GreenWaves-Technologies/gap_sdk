
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

import onnx
from onnx import defs, numpy_helper
from onnx.helper import make_opsetid

from graph.constant_store import ConstantStore
from graph.dim import Dim
from graph.nngraph import NNGraph
from graph.types import ConstantInputParameters
from graph.types.base import NNEdge

from ..common.provisional_dim import ProvisionalDim
from ..importer_base import ImporterBase
from .common.handler_helper import get_all_backend_handlers
from .common.onnx_node import OnnxNode

# pylint: disable=E1101


class OnnxImporter(ImporterBase):
    def __init__(self, *args, **kwargs) -> None:
        super(OnnxImporter, self).__init__(*args, **kwargs)
        self._name_cache = None

    def create_graph(self, filename, opts):
        model = onnx.load(filename)
        self._name_cache = {}
        if model.ir_version < 3:
            opset_import = [make_opsetid(defs.ONNX_DOMAIN, 1)]
        else:
            opset_import = model.opset_import
        G = NNGraph(filename=filename,
                    name=opts.get('name'),
                    constant_store=ConstantStore())
        return self._import_onnx_model(G, model.graph, opset_import, opts)

    def _import_onnx_model(self, G, graph, opset, opts):
        handlers = self._get_handlers(opset)
        all_nodes = {}
        constants = self._get_initializers(graph.initializer)
        all_nodes.update(constants)
        inputs = self._get_input_nodes(G, graph.input, constants)
        all_nodes.update(inputs)
        outputs = self._get_output_nodes(G, graph.output)
        self._import_nodes(G, graph, handlers, all_nodes, outputs, opts)
        return G

    @staticmethod
    def _validate_name(name):
        return name
        # return name.replace(":", "_nntool_") + "_" + get_unique_suffix() if ":" in name else name

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

    def _get_initializers(self, initializer):
        return {
            init.name: (
                ConstantInputParameters(
                    self._validate_name(init.name),
                    dims=Dim.unnamed(init.dims or [1]),
                    value=self._get_numpy_array(init)),
                0,
                Dim.unnamed(init.dims)
            )
            for init in initializer}

    def _get_input_nodes(self, G, inputs, constants):
        prov_dims = {
            idx: ProvisionalDim.from_onnx_shape(input.type.tensor_type.shape, check_for_batch=0)
            for idx, input in enumerate(inputs) if input.name not in constants
        }
        return {
            input.name: (
                G.add_input(Dim.unnamed(prov_dims[idx].known_shape)),
                0,
                prov_dims[idx]
            )
            for idx, input in enumerate(inputs) if input.name not in constants
        }

    def _get_output_nodes(self, G, outputs):
        return {
            output.name: (
                G.add_output(),
                0,
                ProvisionalDim.from_onnx_shape(output.type.tensor_type.shape)
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

    def _import_nodes(self, G, graph, handlers, all_nodes, outputs, opts):
        used_tensors = set(all_nodes.keys()) | set(outputs.keys()) | set.union(*(set(node.input)
                                                                                 for node in graph.node))
        used_tensors.discard('')
        vars_dict = {}
        for node in graph.node:
            handler = handlers[node.domain].get(
                node.op_type, None) if node.domain in handlers else None
            if not handler:
                raise ValueError("no handler found for %s" % node.op_type)

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
