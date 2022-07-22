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

import json
import math

import texttable
from cmd2 import Cmd, with_argparser
from nntool.interpreter.commands.validation import get_validator, validation_options
from nntool.interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase)
from nntool.interpreter.shell_utils import input_options
from nntool.quantization.compression.auto_compress import AutoCompress, FileDataLoader
from nntool.quantization.compression.compress import CompressionError

from nntool.graph.types import ConstantInputNode


class CompressCommand(NNToolShellBase):

    # COMPRESS COMMAND
    parser_compress = NNToolArguementParser()
    parser_compress.add_argument(
        'step',
        nargs='?',
        help='constant input to compress. ' + NODE_SELECTOR_HELP,
        completer_method=NNToolShellBase.node_step_or_name_completer(allow_comma=True))
    parser_compress.add_argument(
        '--no_sparse', action="store_true",
        help='Do not check for sparsity'
    )
    parser_compress.add_argument(
        '--force_sparse', action="store_true",
        help='Force these layers to use sparse bit (adds an extra bin)'
    )
    parser_compress.add_argument(
        '--threshold', type=float,
        help='set values val>x>-val to 0 before clustering'
    )
    parser_compress_sub = parser_compress.add_subparsers(
        title='compress subcommands', help='compression strategy for the selected layers')
    parser_compress_bits = parser_compress_sub.add_parser(
        'bits',
        help='compress using a lookup index of a fixed number of bits')
    parser_compress_bits.add_argument(
        'num_bits', choices=list(str(v) for v in range(2, 8)),
        help='number of bits to use for lookup table indexes'
    )

    parser_compress_min_qnsr = parser_compress_sub.add_parser(
        'min_qsnr',
        help='compress keeping the value QSNR above a minimum value')

    parser_compress_min_qnsr.add_argument(
        'qsnr', type=int,
        help='QSNR to keep above'
    )

    parser_compress_auto = parser_compress_sub.add_parser(
        'auto',
        help='compress to a number of bits automatically using validation results')

    parser_compress_auto.add_argument(
        '--finetune', action="store_true",
        help='Experimental layer finetuning'
    )

    validation_options(parser_compress_auto)
    input_options(parser_compress_auto)

    parser_compress_clear = parser_compress_sub.add_parser(
        'clear',
        help='clear compression on these nodes')

    parser_compress_off = parser_compress_sub.add_parser(
        'off',
        help='disable compression on these nodes')

    parser_compress_on = parser_compress_sub.add_parser(
        'on',
        help='enable compression on these nodes')

    parser_compress_save = parser_compress_sub.add_parser(
        'save',
        help='save compression settings to a file in json format')

    parser_compress_save.add_argument(
        'file',
        completer_method=Cmd.path_complete,
        help='file to save to'
    )

    parser_compress_load = parser_compress_sub.add_parser(
        'load',
        help='load compression settings from a file in json format')

    parser_compress_load.add_argument(
        'file',
        completer_method=Cmd.path_complete,
        help='file to load from'
    )

    parser_compress_bits.set_defaults(operation='bits')
    parser_compress_min_qnsr.set_defaults(operation='min_qsnr')
    parser_compress_auto.set_defaults(operation='auto')
    parser_compress_clear.set_defaults(operation='clear')
    parser_compress_off.set_defaults(operation='off')
    parser_compress_on.set_defaults(operation='on')
    parser_compress_save.set_defaults(operation='save')
    parser_compress_load.set_defaults(operation='load')

    @with_argparser(parser_compress)
    def do_compress(self, args):
        """
Compress graph constants for GAP9 compression engine. Compress with no arguments will list current compression settings.
The compressed size in the results table includes the size of the codebook.

In bits mode the amount of bits used for table indexes is specified directly. It should be from 2 to 8.
In min_qsnr mode the tensors will be compressed with the number of bits necessary to stay above the given QSNR value. A
value of around 30 is a good starting point.

The auto mode uses the validation engine to explore possible tensor compression parameters. THe command options are the 
same as the validate command. The first part of the process tries to find the lowest QSNR that can be selected for
compression of all viable parameters with no bad validation results.

The threshold argument can be used to clip values to zero before compression.
"""
        self._check_graph()
        if not args.step:
            if hasattr(args, 'operation'):
                nodes = self.G.nodes(node_classes=ConstantInputNode)
            else:
                self.compress_make_table(self.G.nodes(
                    node_classes=ConstantInputNode))
                return
        else:
            nodes, _ = self.get_node_step_or_name(
                args.step, classes=ConstantInputNode, allow_comma=True)

        if not nodes:
            return

        if not args.operation:
            # No subcommand was provided, so call help
            self.do_help('compress')
            return

        if args.operation == "clear":
            for node in nodes:
                node.use_compressed = False
                node.compressed_value = None
                self.pfeedback(f'clear compression on {node.name}')
            return
        if args.operation == "on":
            for node in nodes:
                if node.compressed_value:
                    node.use_compressed = True
                    self.pfeedback(f'enable compression on {node.name}')
                else:
                    self.perror(
                        f"can't enable compression on {node.name} - not set")
            return
        if args.operation == "off":
            for node in nodes:
                node.use_compressed = False
                self.pfeedback(f'disable compression on {node.name}')
            return
        if args.operation == "save":
            save_map = {}
            for node in nodes:
                if node.compressed_value:
                    comp_val = node.compressed_value
                    save_map[node.name] = {
                        'bits': comp_val.bits,
                        'threshold': comp_val.threshold,
                        'sparse': bool(comp_val.sparse),
                    }
            with open(args.file, 'w') as fp:
                fp.write(json.dumps(save_map))
            return

        report_nodes = []
        if args.operation == "load":
            with open(args.file, 'r') as fp:
                save_map = json.load(fp)

            for node in nodes:
                if node.name in save_map:
                    self.pfeedback(f"Loading parameters for {node.name}")
                    try:
                        params = save_map[node.name]
                        node.compress_value(
                            bits=params['bits'],
                            threshold=params['threshold'],
                            allow_sparse=params['sparse'],
                            force_sparse=params['sparse'],
                        )
                        comp_val = node.compressed_value
                        node.use_compressed = True
                    except CompressionError as ex:
                        self.pfeedback(f'unable to compress {node.name} - {ex}')
                        comp_val = None
                    if comp_val:
                        report_nodes.append(node)
            return
        elif args.operation == "auto":
            if args.quantize:
                self._check_quantized()
            input_args = self._get_input_args(args)
            autocompress = AutoCompress(
                self.G,
                FileDataLoader(args.input_files, self.G.num_inputs, input_args=input_args),
                get_validator(args))

            def progress(msg, newline):
                print(msg, end='\n' if newline else '', flush=True)

            try:
                autocompress.tune_all(nodes, progress, quantize=args.quantize)
                if args.finetune:
                    report_nodes = [node for node in nodes if node.use_compressed]
                    self.compress_make_table(report_nodes)
                    autocompress.finetune(nodes, progress, quantize=args.quantize)
            except (KeyboardInterrupt, SystemExit):
                pass
            report_nodes = [node for node in nodes if node.use_compressed]
        else:
            for node in nodes:
                self.pfeedback(f"Evaluating {node.name}")

                nid = node.name
                if self.G.quantization and nid in self.G.quantization:
                    qtype = self.G.quantization[nid].out_qs[0]
                else:
                    qtype = None

                try:
                    kwargs = {
                        'qtype': qtype,
                        'threshold': args.threshold,
                        'allow_sparse': not args.no_sparse,
                        'force_sparse': args.force_sparse
                    }
                    if args.operation == "bits":
                        kwargs['bits'] = int(args.num_bits)
                    elif args.operation == "min_qsnr":
                        kwargs['min_qsnr'] = args.qsnr
                    elif args.operation == "auto":
                        pass
                    else:
                        raise ValueError('strange operation')
                    node.compress_value(**kwargs)
                    comp_val = node.compressed_value
                    node.use_compressed = True
                except CompressionError as ex:
                    self.pfeedback(f'unable to compress {node.name} - {ex}')
                    comp_val = None

                if comp_val:
                    report_nodes.append(node)

        self.compress_make_table(report_nodes)

    def compress_make_table(self, nodes):
        comp_size = 0
        node_size = 0
        comp_report = [["Step", "Name", "Shape", "Orig Size\n(Bytes)",
                        "Compressed Size\n(Bytes)", "%age orig", "Original\nBits/val", "Compressed\nBits/val", "Sparse", "Enabled"]]
        for node in nodes:
            if not node.compressed_value:
                continue
            comp_val = node.compressed_value
            node_size += comp_val.uncompressed_size
            comp_size += comp_val.size
            comp_report.append([
                node.step_idx, node.name, node.value.shape, comp_val.uncompressed_size,
                comp_val.size, (100 * comp_val.size)//comp_val.uncompressed_size,
                comp_val.uncompressed_bits, 
                comp_val.bits, "Yes" if comp_val.sparse else "No", "Yes" if node.use_compressed else "No"
            ])
        if comp_size == 0:
            self.pfeedback("no constants compressed")
            return
        comp_report.append([
            "", "Total", "", node_size, comp_size, (100 *
                                                comp_size)//node_size, "", "", "", ""
        ])
        table = texttable.Texttable()
        table.set_cols_align(['l', 'l', 'l', 'r', 'r', 'r', 'r', 'r', 'c', 'c'])
        table.set_max_width(200)
        table.add_rows(comp_report)
        self.pfeedback("Compression report\n")
        self.pfeedback(table.draw()+'\n')
