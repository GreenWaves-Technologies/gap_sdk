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

import tempfile

from expressions.symbolic.symbol import Constant, Variable
from graph.nngraph import NNGraph
from graph.types import ExpressionFusionParameters, FusionBase, Parameters
from graph.types.fusions import FusionInputParameters, FusionOutputParameters
from graphviz import Digraph, nohtml
from quantization.qtype import QType
from utils.node_id import NodeId


def str_shape(shape):
    if shape is None:
        return ""
    return "x".join(str(dim) for dim in shape)


def make_ports(x, in_ports, out_ports):
    return (
        [f'{x.name}:{port}' for port in in_ports] if in_ports is not None else None,
        [f'{x.name}:{port}' for port in out_ports] if out_ports is not None else None
    )


class DrawGraphReporter():
    def __init__(self):
        self._name_cache = None

    @staticmethod
    def dim_or_error(dims, idx):
        if dims is None or len(dims) <= idx:
            return "not set", True
        return f'({dims[idx]})', False

    @staticmethod
    def get_trans(node, dir_name):
        return hasattr(node, f'transpose_{dir_name}') and getattr(node, f'transpose_{dir_name}')

    @staticmethod
    def get_ports(all_ports, edge):
        if edge.from_node in all_ports:
            out_port = all_ports[edge.from_node][1][edge.from_idx]
        else:
            out_port = None
        if edge.to_node in all_ports:
            in_port = all_ports[edge.to_node][0][edge.to_idx]
        else:
            in_port = None
        return out_port, in_port

    @staticmethod
    def add_edges(node, dir_name, num, names):
        trans = DrawGraphReporter.get_trans(node, dir_name)
        if num > 1 or trans:
            if trans:
                trans = [f' {t}' if t is not None else '' for t in trans]
            else:
                trans = [''] * num
            label = 'i' if dir_name == 'in' else 'o'
            ports = [f'{node.name}:{label}{idx}' for idx in range(num)]
            edges = "|".join(
                [f'<{label}{idx}> {idx}{trans[idx]}' for idx in range(num)])
            names.append(f'{{{edges}}}')
        else:
            ports = [f'{node.name}:name'] * num
        return ports

    @staticmethod
    def insert_tag(idx, tag, names):
        if idx >= len(names):
            raise ValueError()
        name = names[idx]
        if isinstance(name, str):
            names[idx] = f'{tag} {name}'
            return
        name = name[0]
        names[idx] = [f'{tag} {name}'] + names[idx][1::]
        return

    @staticmethod
    def get_label(node, anon):
        if hasattr(node, 'graph_label'):
            return node.graph_anon_label if anon else node.graph_label
        return [node.name]

    @staticmethod
    def build_nodebox(node, ports, num_in, num_out, anon=False):
        trans_in = DrawGraphReporter.get_trans(node, 'in')
        trans_out = DrawGraphReporter.get_trans(node, 'out')
        names = []
        if num_in > 1 or trans_in:
            if trans_in:
                trans = [f' {t}' if t is not None else '' for t in trans_in]
            else:
                trans = [''] * num_in
            ports[0] = [f'{node.name}:in{idx}' for idx in range(num_in)]
            edges = [
                f'<in{idx}> {idx if num_in > 1 else ""}{trans[idx] if idx < len(trans) else ""}' for idx in range(num_in)]
            names.append(edges)
            names.extend(DrawGraphReporter.get_label(node, anon))
        else:
            ports[0] = [f'{node.name}:name']
            names.extend(DrawGraphReporter.get_label(node, anon))
            DrawGraphReporter.insert_tag(0, f'<name>', names)
        if num_out > 1 or trans_out:
            if trans_out:
                trans = [f' {t}' if t is not None else '' for t in trans_out]
            else:
                trans = [''] * num_out
            ports[1] = [f'{node.name}:o{idx}' for idx in range(num_out)]
            edges = [
                f'<o{idx}> {idx if num_out > 1 else ""}{trans[idx] if idx < len(trans) else ""}' for idx in range(num_out)]
            names.append(edges)
        else:
            if len(names) == 1:
                ports[1] = [f'{node.name}:name']
            else:
                ports[1] = [f'{node.name}:bottom']
                DrawGraphReporter.insert_tag(-1, '<bottom>', names)
        names = [name if isinstance(
            name, str) else f'{{{"|".join(name)}}}' for name in names]
        names = f'{{{"|".join(name for name in names)}}}'
        return names

    def init_name_cache(self):
        self._name_cache = {}

    def reset_name_cache(self):
        self._name_cache = None

    def get_next(self, name_type):
        next_idx = self._name_cache.setdefault(name_type, 0)
        next_id = f'{name_type}{next_idx}'
        self._name_cache[name_type] = next_idx + 1
        return next_id

    def out_label(self, G, edge, qrecs, parent=None, to_node=True, from_node=True):
        node = edge.from_node
        idx = edge.from_idx
        if qrecs:
            nid = NodeId(node) if parent is None else NodeId(
                parent, fnode=node)
            if from_node:
                qrec = qrecs.get(nid)
                if qrec is None:
                    qtype = 'no qrec'
                else:
                    qtype = qrec.out_qs[idx] if qrec.out_qs and idx < len(
                        qrec.out_qs) else None
                    if not qtype:
                        qtype = 'no qtype'
            else:
                qtype = "n/a"
            if to_node:
                nid = NodeId(edge.to_node) if parent is None else NodeId(
                    parent, fnode=edge.to_node)
                to_qrec = qrecs.get(nid)
                if to_qrec is None:
                    return f'{qtype}/no qrec', True
                to_qtype = to_qrec.in_qs[edge.to_idx] if to_qrec.in_qs and edge.to_idx < len(
                    to_qrec.in_qs) else None
                if not to_qtype:
                    return f'{qtype}/no qtype', True
            else:
                to_qtype = "n/a"
            if not to_node:
                assert from_node
                return str(qtype), False
            if not from_node:
                return str(to_qtype), False
            if not to_qtype.quantization_equal(qtype):
                return f'{qtype}/{to_qtype}', True
            return str(qtype), False
        else:
            if node.out_dims:
                return f'({node.out_dims[idx]})', False
            return 'not set', True

    def in_label(self, G, edge, qrecs, parent=None, to_node=True, from_node=True):
        node = edge.to_node
        idx = edge.to_idx
        if qrecs:
            if to_node:
                nid = NodeId(node) if parent is None else NodeId(
                    parent, fnode=node)
                qrec = qrecs.get(nid)
                if qrec is None:
                    qtype = 'no qrec'
                else:
                    qtype = qrec.in_qs[idx] if qrec.in_qs and idx < len(
                        qrec.in_qs) else None
                    if qtype is None:
                        return 'no qtype', True
            else:
                qtype = "n/a"
            if from_node:
                nid = NodeId(edge.from_node) if parent is None else NodeId(
                    parent, fnode=edge.from_node)
                from_qrec = qrecs.get(nid)
                if from_qrec is None:
                    return f'no qrec/{qtype}', True
                from_qtype = from_qrec.out_qs[edge.from_idx] if from_qrec.out_qs and edge.from_idx < len(
                    from_qrec.out_qs) else None
                if not from_qtype:
                    return f'no qtype/{qtype}', True
            else:
                from_qtype = "n/a"
            if not from_node:
                assert to_node
                return str(qtype), False
            if not to_node:
                return str(from_qtype), False
            if not from_qtype.quantization_equal(qtype):
                return f'{from_qtype}/{qtype}', True
            return str(qtype), False
        elif isinstance(node, Parameters):
            if node.in_dims:
                return self.dim_or_error(node.in_dims, idx)
            return 'not set', True
        return '', False


    def report_graph(self, G: NNGraph, dot, all_ports, fake_idx, nodes=None, all_dims=False,
                     anonymise=False, expressions=False, qrecs=None, fusions=False, parent=None):
        if nodes is None:
            nodes = set(G.nodes())
        for node in G.topological_sort():
            if node not in nodes:
                continue
            if isinstance(node, (FusionInputParameters)):
                continue
            if expressions and isinstance(node, ExpressionFusionParameters):
                all_ports[node] = self.report_expression(
                    dot, G, node, anonymise=anonymise, report_quantized=expressions == "quantized")
            elif fusions and isinstance(node, FusionBase) and node.quantize_internals:
                all_ports[node] = self.report_fusion(
                    dot, G, node, all_ports, fake_idx, all_dims=all_dims,
                    anonymise=anonymise, expressions=expressions, qrecs=qrecs)

            else:
                num_in_edges = len(G.indexed_in_edges(node.name))
                num_out_edges = len(G.indexed_out_edges(node.name))
                ports = all_ports.setdefault(node, [None] * 2)
                if not isinstance(node, FusionOutputParameters):
                    names = self.build_nodebox(
                        node, ports, num_in_edges, num_out_edges, anon=anonymise)
                    if not isinstance(node, Parameters):
                        dot.node(node.name, nohtml(names),
                                 shape='record', color='black')
                    else:
                        dot.node(node.name, nohtml(names), shape='record',
                                 xlabel=f"{node.step_idx}" if parent is None else "",
                                 color="blue" if node.is_not_generated else "black")
            for edge in G.in_edges(node.name):
                if edge.from_node not in nodes:
                    if not all_dims:
                        continue

                out_port, in_port = self.get_ports(all_ports, edge)
                if edge.from_node in nodes:
                    from_node_id = self.get_from_id(all_ports, edge, out_port)
                    to_node_id = self.get_to_id(all_ports, edge, in_port)
                    edge_label, edge_error = self.in_label(
                        G, edge, qrecs, parent=parent,
                        from_node=not isinstance(
                            edge.from_node, FusionInputParameters),
                        to_node=not isinstance(edge.to_node, FusionOutputParameters))
                    dot.edge(
                        from_node_id,
                        to_node_id,
                        xlabel=edge_label,
                        color="red" if edge_error else "black")
                else:
                    fake_name = f'fake_{fake_idx}'
                    fake_idx += 1
                    dot.node(fake_name, shape='point', fillcolor='black')
                    to_node_id = self.get_to_id(all_ports, edge, in_port)
                    edge_label, edge_error = self.in_label(
                        G, edge, qrecs, parent=parent)
                    dot.edge(
                        fake_name,
                        to_node_id,
                        xlabel=edge_label,
                        color="red" if edge_error else "black")
            if not all_dims:
                continue
            for edge_group in G.indexed_out_edges(node.name):
                if any(edge.to_node in nodes for edge in edge_group):
                    continue
                edge = edge_group[0]
                out_port, _ = self.get_ports(all_ports, edge)
                fake_name = f'fake_{fake_idx}'
                fake_idx += 1
                dot.node(fake_name, shape='plaintext',
                         label=' ', fillcolor='black')
                from_node_id = self.get_from_id(all_ports, edge, out_port)
                edge_label, edge_error = self.out_label(
                    G, edge, qrecs, parent=parent,
                    from_node=not isinstance(
                        edge.from_node, FusionInputParameters),
                    to_node=not isinstance(edge.to_node, FusionOutputParameters))
                dot.edge(
                    from_node_id,
                    fake_name,
                    xlabel=edge_label,
                    color="red" if edge_error else "black")

    def report(self, G: NNGraph, nodes=None, graph_format='PDF', all_dims=False,
               filename=None, view=True, anonymise=False, expressions=False, quant_labels=False, fusions=False):

        if quant_labels:
            if G.quantization is None:
                raise ValueError("graph is not quantized")
            qrecs = G.quantization
        else:
            qrecs = None
        self.init_name_cache()
        all_ports = {}
        graph_name = G.name if hasattr(G, 'name') else 'graph'
        dot = Digraph(comment=graph_name, format=graph_format, node_attr={
                      'height': '.1'}, edge_attr={'fontsize': '10.0'})
        fake_idx = 0
        self.report_graph(G, dot, all_ports, fake_idx, nodes=nodes, all_dims=all_dims,
                          anonymise=anonymise, expressions=expressions, qrecs=qrecs, fusions=fusions)

        # dot = dot.unflatten(stagger=2)
        if filename:
            dot.render(filename, cleanup=True)
        if view:
            filename = tempfile.mktemp('.gv')
            dot.view(filename, cleanup=True, quiet=True)
        self.reset_name_cache()

    @staticmethod
    def get_from_id(subgraphs, edge, out_port):
        if edge.from_node in subgraphs:
            from_node_id = subgraphs[edge.from_node][1][edge.from_idx]
        else:
            from_node_id = f'{edge.from_node.name}:{out_port}'
        return from_node_id

    @staticmethod
    def get_to_id(subgraphs, edge, in_port):
        if edge.to_node in subgraphs:
            to_node_id = subgraphs[edge.to_node][0][edge.to_idx]
        else:
            to_node_id = f'{edge.to_node.name}:{in_port}'
        return to_node_id

    def report_expression(self, dot: Digraph, G: NNGraph,
                          node: ExpressionFusionParameters, report_quantized=False, anonymise=False):
        if report_quantized:
            nid = NodeId(node)
            if not G.quantization or nid not in G.quantization:
                raise ValueError(
                    f'requested drawing quantized expression {node.name} but it is not quantized')
            func_col = G.quantization[nid].cache['qfunc_col']
        else:
            func_col = node.func_col
        intermediates = {}
        with dot.subgraph(name=f'cluster{node.name}',
                          graph_attr={
                              'style': 'dashed',
                              'label': f"{node.step_idx}",
                              'labelloc': 't',
                              'labeljust': 'l'},
                          node_attr={'style': 'solid(dashed)'}) as sub:
            for var, func in func_col:
                node_id, shape = self.report_symbol(
                    sub, func, intermediates, anonymise=anonymise)
                var_name = self.get_next('Var') if anonymise else var.name
                if node_id == var.name:
                    intermediates[node_id] = shape
                else:
                    dot.node(var.name, nohtml(var_name),
                             shape='plaintext', fontsize='10.0')
                    sub.edge(
                        node_id, var.name, xlabel=f'{str_shape(shape)}', color="red" if shape is None else "black")

        return [node.input_symbols, node.output_symbols]

    def report_fusion(self, dot: Digraph, G: NNGraph,
                      node: FusionBase, all_ports, fake_idx, nodes=None, all_dims=False,
                      anonymise=False, expressions=False, qrecs=None):

        inputs_by_idx = sorted(node.subgraph.nodes(
            node_classes=FusionInputParameters), key=lambda x: x.idx)
        outputs_by_idx = sorted(node.subgraph.nodes(
            node_classes=FusionOutputParameters), key=lambda x: x.idx)

        input_symbols = []
        output_symbols = []
        for input_node in inputs_by_idx:
            dot.node(input_node.name, nohtml(input_node.name),
                     shape='plaintext', fontsize='10.0')
            all_ports[input_node] = [[input_node.name], [input_node.name]]
            input_symbols.append(input_node.name)
        for output_node in outputs_by_idx:
            dot.node(output_node.name, nohtml(output_node.name),
                     shape='plaintext', fontsize='10.0')
            all_ports[output_node] = [[output_node.name], [output_node.name]]
            output_symbols.append(output_node.name)
        with dot.subgraph(name=f'cluster{node.name}',
                          graph_attr={
                              'style': 'dashed',
                              'label': f"{node.step_idx}",
                              'labelloc': 't',
                              'labeljust': 'l'},
                          node_attr={'style': 'solid(dashed)'}) as sub:
            self.report_graph(node.subgraph, sub, all_ports, fake_idx, all_dims=all_dims,
                              anonymise=anonymise, expressions=expressions, qrecs=qrecs, parent=node)
        return [input_symbols, output_symbols]

    def report_symbol(self, dot, symbol, intermediates, anonymise=False):
        if isinstance(symbol, Variable):
            if symbol.name in intermediates:
                return symbol.name, intermediates[symbol.name]
            var_name = self.get_next('Var') if anonymise else symbol.name
            dot.node(symbol.name, nohtml(var_name),
                     shape='plaintext', fontsize='10.0')
            return symbol.name, symbol.shape
        if isinstance(symbol, Constant):
            const_name = self.get_next('Const')
            dot.node(const_name, 'Const' if anonymise else str(
                symbol.value[0]), shape='oval', fontsize='10.0')
            return const_name, symbol.shape
        ids_and_shapes = [self.report_symbol(dot, sym, intermediates, anonymise=anonymise)
                          for sym in symbol.contents]
        func_label = self.get_next(
            'Op') if anonymise else symbol.__class__.__name__
        dot.node(symbol.name, nohtml(func_label), shape='record')
        for child_id, shape in ids_and_shapes:
            dot.edge(child_id, symbol.name,
                     xlabel=f'{str_shape(shape)}', color="red" if shape is None else "black")
        try:
            symbol_shape = symbol.shape
        except ValueError:
            symbol_shape = None
        return symbol.name, symbol_shape
