# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import os
import pickle
import tempfile
import typing
import zipfile
from io import StringIO
from typing import Mapping, Tuple, Union

from nntool.generation.code_block import StreamCodeBlock
from nntool.graph.types import (ConstantInputNode, ExpressionFusionNode,
                         FusionInputNode, FusionNodeBase, InputNNNodeBase,
                         InputNode)
from nntool.quantization.multiplicative.scaling_qtypes import (MultMulBiasScaleQType,
                                                        ScalingQType)
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.quantization_set import QuantizationSet

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph
    from nntool.utils.graph import GraphView


def write_headers(G: 'NNGraph', has_tensors: bool, code_block: StreamCodeBlock):
    code_block.comment(f'{G.name} graph exported from NNTool')
    code_block.nl().nl()
    if has_tensors:
        code_block.write('import pickle')
        code_block.write('import os')
    code_block.write(
        'from numpy import (array, float16, float32, float64, int64, int32, int16,')
    code_block.write('                   int8, uint64, uint32, uint16, uint8)')
    code_block.write('from bfloat16 import bfloat16')
    code_block.write(
        'from nntool.api import (NNGraph, Dim, FcFilterDim, Conv2DFilterDim, ScaleDim,')
    code_block.write(
        '                        StrideDim, PadDim, DilationDim, Dim2D, PoolFilterDim)')
    code_block.write('from nntool.api.types import *')
    if G.quantization:
        code_block.write(
            'from nntool.api.quantization import QType, QRec, QSet, MultMulBiasScaleQType')
    code_block.write('from nntool.api.utils import GraphView')
    code_block.nl().nl()


def write_graph_function_header(has_tensors: bool, has_quantization: bool, graphpath: str, code_block: StreamCodeBlock):
    if has_tensors:
        if graphpath:
            code_block.write('def create_graph(tensors=None):')
        else:
            code_block.write('def create_graph(tensors):')
    else:
        code_block.write('def create_graph():')
    code_block.indent()

    code_block.write('subgraphs = {}')
    code_block.write('nodes = {}')
    if has_tensors and graphpath:
        graph_base = os.path.basename(graphpath)
        if graph_base.endswith('.zip'):
            graph_base = graph_base[:-len('.zip')]
        code_block.write('if tensors is None:').indent()
        code_block.write(
            f'tensors = pickle.loads(__loader__.get_data("{graph_base}/tensors.pickle"))').deindent()


def write_graph(
        G: Union['GraphView', 'NNGraph'],
        code_block: StreamCodeBlock,
        tensors: dict,
        store_to: str,
        nodes_key=None):
    if nodes_key:
        nodes = f'nodes[\'{nodes_key}\']'
        code_block.write(f'{nodes} = {{}}')
    else:
        nodes = 'nodes'

    code_block.write(f'{store_to} = {G!r}')
    inputs = sorted([node for node in G.inputs() if isinstance(node, (InputNode))], key=lambda node: node.index)
    inputs.extend(sorted([node for node in G.inputs() if isinstance(node, (FusionInputNode))], key=lambda node: node.name))
    inputs.extend(node for node in G.inputs()
                  if isinstance(node, ConstantInputNode))
    for node in G.topological_sort(inputs):
        inputs = []
        for edge in G.indexed_in_edges(node):
            if len(G.indexed_out_edges(edge.from_node)) > 1:
                # index needs to contain output number if outputs of connected node
                # are greater than 1
                inputs.append(
                    f"{nodes}['{edge.from_node.name}'][{edge.from_idx}]")
            else:
                inputs.append(f"{nodes}['{edge.from_node.name}']")
        code_block.write(f"{nodes}['{node.name}'] = ", nl="")
        node.write_python_description(
            code_block, indent=False, tensors=tensors)
        if isinstance(node, (InputNNNodeBase, FusionInputNode)):
            code_block.write(f"({store_to})", indent=False)
        else:
            # call needs to pass number of outputs if greater than 1
            num_outputs = len(G.indexed_out_edges(node))
            num_outputs = '' if num_outputs <= 1 else f', num_outputs={num_outputs}'
            code_block.write(
                f"({','.join(inputs)}{num_outputs})", indent=False)


def make_zip_module(tensors: dict, graph_code: str, graphpath: str):
    with tempfile.TemporaryDirectory() as temp_dir:
        graph_base = os.path.basename(graphpath)
        if graph_base.endswith('.zip'):
            graph_base = graph_base[:-len('.zip')]
        graph_loader_path = os.path.join(temp_dir, graph_base, 'graph.py')
        os.mkdir(os.path.join(temp_dir, graph_base))
        with open(graph_loader_path, 'wt') as file_stream:
            file_stream.write(graph_code)
        with open(os.path.join(temp_dir, graph_base, '__init__.py'), 'wt') as file_stream:
            file_stream.write(f"""
from .graph import create_graph

{graph_base} = create_graph()
__all__ = ['{graph_base}']
""")
        with open(os.path.join(temp_dir, '__init__.py'), 'wt') as file_stream:
            file_stream.write('')
        if tensors:
            tensor_dump_path = os.path.join(temp_dir, graph_base, 'tensors.pickle')
            with open(tensor_dump_path, 'wb') as file_stream:
                pickle.dump(tensors, file_stream)
        if not graphpath.endswith('.zip'):
            graphpath = f'{graphpath}.zip'
        with zipfile.PyZipFile(graphpath, mode="w") as zip_pkg:
            zip_pkg.writepy(os.path.join(temp_dir, '__init__.py'))
            zip_pkg.writepy(os.path.join(temp_dir, graph_base))
            if tensors:
                zip_pkg.write(tensor_dump_path, f'{graph_base}/tensors.pickle')


def dump_scaleqtype(tensors, key: Union[Tuple, str], qtype: ScalingQType, code_block: StreamCodeBlock, indent=True, nl='\n'):
    code_block.write(f'{qtype.__class__.__name__}(', indent=indent).indent()
    if hasattr(qtype, 'pre_normalization') and qtype.pre_normalization:
        code_block.write(f'pre_normalization={qtype.pre_normalization!r},')
    for attr in ['scale']:
        val = getattr(qtype, attr)
        if val is None:
            continue
        if len(val) > 1:
            this_key = tuple(list(key) + [attr])
            tensors[this_key] = val
            code_block.write(f'{attr}=tensors[{this_key!r}],')
        else:
            code_block.write(f'{attr}={val.item()!r},')
    code_block.write(f'dtype={qtype.dtype.__name__})', nl=nl).deindent()


def dump_qrec(tensors, key: Union[Tuple, str], qrec: QRec, code_block: StreamCodeBlock):
    if isinstance(key, str):
        key = (key,)
    code_block.write('QRec(', indent=False)
    code_block.indent()
    for attr_name in ['in_qs', 'out_qs']:
        code_block.write(f'{attr_name}=[')
        code_block.indent()
        qtypes = getattr(qrec, attr_name)
        for idx, qtype in enumerate(qtypes):
            qtype.dump(tensors, key + (attr_name, idx), code_block, nl='')
            if idx < (len(qtypes) - 1):
                code_block.write(',', indent=False)
        code_block.deindent()
        code_block.nl().write('],')
    for attr_name, val in qrec.cache.items():
        code_block.write(f'{attr_name}=', nl='')
        if isinstance(val, QType):
            val.dump(tensors, key + (attr_name,), code_block, indent=False)
            code_block.write(',', indent=False)
        elif isinstance(val, MultMulBiasScaleQType):
            dump_scaleqtype(tensors, key + (attr_name,), val,
                            code_block, indent=False, nl='')
            code_block.write(',', indent=False)
        else:
            code_block.write(f'{val!r},', indent=False)
    code_block.write(f'ktype="{qrec.ktype}"')
    code_block.deindent()
    code_block.write(')')


def dump_qrecs(G, quantization: QuantizationSet, tensors: Mapping, code_block: StreamCodeBlock):
    code_block.write('def set_quantization(tensors):')
    code_block.indent()
    code_block.write('recs=QSet()')
    for node in G.nodes():
        if isinstance(node, FusionNodeBase) and node.quantize_internals:
            for fnode in node.subgraph.nodes():
                nid = (node.name, fnode.name)
                if nid not in quantization:
                    continue
                key = (node.name, fnode.name)
                code_block.write(
                    f'recs[{key}] = ', nl='')
                dump_qrec(tensors, key, quantization[nid], code_block)
        elif isinstance(node, ExpressionFusionNode):
            # Expression QRecs are not saved since the quantized expression is rebuilt
            # at the end of import from statistics to avoid having to serialize it
            continue
        nid = node.name
        key = node.name
        code_block.write(f'recs["{key}"] = ', nl='')
        dump_qrec(tensors, key, quantization[nid], code_block)
    code_block.nl()
    for k, v in quantization.node_options.items():
        v = {opt_k: opt_v for opt_k, opt_v in v.items()
                if opt_k not in "qtype_ind"} # TODO - qtype_ind to serialize as qtype
        if not v:
            continue
        code_block.write(f'recs.node_options[{k.id!r}] = {v!r}')
    for k, v in quantization.graph_options.items():
        code_block.write(f'recs.graph_options[{k!r}] = {v!r}')
    code_block.write(
        f'recs.schemes_present = {quantization.schemes_present!r}')
    code_block.write(
        f'recs.schemes_priority = {quantization.scheme_priority!r}')
    code_block.write('if "__stats__" in tensors:').indent()
    code_block.write('recs.init_stats()')
    code_block.write('for k, v in tensors["__stats__"].items():').indent()
    code_block.write('recs.stats[k] = v').deindent().deindent()
    code_block.nl()
    code_block.write('return recs')
    code_block.deindent().nl()

def dump_build_settings(build_settings: dict, code_block: StreamCodeBlock):
    code_block.write('settings = {}')
    for k, v in build_settings.items():
        code_block.write(f'settings[{k!r}] = {v!r}')
    code_block.write('G.settings = settings')

def dump_graph(G: 'NNGraph', graphpath: str = None, build_settings=None):
    graph_stream = StringIO()
    tensors = {}
    code_block = StreamCodeBlock(graph_stream, line_comment="#")
    has_tensors = G.all_constants or G.quantization
    write_headers(G, has_tensors, code_block)
    if G.quantization:
        dump_qrecs(G, G.quantization, tensors, code_block)
    write_graph_function_header(has_tensors, bool(
        G.quantization), graphpath, code_block)
    for node in G.nodes(node_classes=FusionNodeBase):
        subgraph = f'subgraphs["{node.name}"]'
        write_graph(
            node.subgraph,
            code_block,
            tensors,
            subgraph,
            nodes_key=node.name)

    write_graph(G, code_block, tensors, 'G')
    if build_settings:
        dump_build_settings(build_settings, code_block)
    if G.quantization:
        code_block.write('G.quantization = set_quantization(tensors)')
        tensors["__stats__"] = G.quantization.stats.copy()
    code_block.write('G.add_dimensions()')
    if G.quantization:
        # verify quantization. particularly rebuild quantized expressions
        code_block.write('G.quantize()')
    code_block.write('return G')
    code_block.deindent()
    code_block.nl()
    graph_stream.flush()
    graph_code = graph_stream.getvalue()
    graph_stream.close()
    if graphpath:
        make_zip_module(tensors, graph_code, graphpath)
    return graph_code, tensors
