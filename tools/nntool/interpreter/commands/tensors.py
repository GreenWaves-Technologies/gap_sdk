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

import numpy as np
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser

from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import print_comparison
from utils.gap_tensor_file import read_gap_tensors, write_gap_tensor
from utils.stats_funcs import qsnr
from utils.at_tensor_loader import at_map_tensors, at_tensor_loader

LOG = logging.getLogger('nntool.'+__name__)


class TensorsCommand(NNToolShellBase):
    # TENSORS_COMMAND
    parser_tensors = Cmd2ArgumentParser()
    parser_tensors.add_argument('-c', '--channel',
                                nargs=(1, 2),
                                type=int,
                                help='channel to compare')
    parser_tensors.add_argument('-s', '--step',
                                type=int,
                                help='step to compare')
    parser_outexclu = parser_tensors.add_mutually_exclusive_group()
    parser_outexclu.add_argument('-Q', '--compare_qsnr',
                                 action='store_true',
                                 help='compare two tensors QSNR')
    parser_outexclu.add_argument('-E', '--compare_error',
                                 action='store_true',
                                 help='compare two tensors error (first - second)')
    parser_tensors.add_argument('-n', '--name',
                                type=str,
                                choices_method=lambda x: x.tensor_store_names,
                                help='name to use for the tensor in the tensor store')
    parser_tensors.add_argument('--write_numpy',
                                type=str,
                                completer_method=Cmd.path_complete,
                                help='write a tensor in npy format. you must select a step. ' +
                                'the output of this step is written. specify a single tensor with ' +
                                'the -t option.')
    parser_tensors.add_argument('-m', '--make_filename',
                                type=str,
                                completer_method=Cmd.path_complete,
                                help='write a makefile including the dimensions of the tensor written ' +
                                'and the dimensions of the input to the node that produced it.')
    parser_texclu1 = parser_tensors.add_mutually_exclusive_group()
    parser_texclu1.add_argument('-W', '--weights',
                                action='store_true',
                                help='compare weights')
    parser_texclu1.add_argument('-B', '--biases',
                                action='store_true',
                                help='compare biases')
    parser_texclu2 = parser_tensors.add_mutually_exclusive_group()
    parser_texclu2.add_argument('-t', '--tensors',
                                nargs=(1, 2),
                                type=str,
                                choices_method=lambda x: x.tensor_store_names,
                                help='compare two tensors')
    parser_texclu2.add_argument('-g', '--gap_load',
                                completer_method=Cmd.path_complete,
                                help='load tensors dumped by autotiler code. ' +
                                'Supply the filename and' +
                                ' an optional tensor store name. If none is given' +
                                ' the filename will be used.')
    parser_texclu2.add_argument('-X', '--clear',
                                action='store_true',
                                help='clears the tensor store')

    @with_argparser(parser_tensors)
    def do_tensors(self, args):
        """
Load and manipulate tensors. If no option is supplied the saved tensors will be listed.
All the tensors in the store are available in dictionary 'tensors' in the python console
accessed by the command 'py'. Tensors can be displayed side by side or the average absolute
error or QSNR displayed. If a step is selected then the error by channel will be displayed."""
        if args.clear:
            self.pfeedback('tensor store cleared')
            self.tensor_store.clear()
            return
        if args.gap_load:
            store_name = args.gap_load if not args.name else args.name
            self.tensor_store[store_name] = at_map_tensors(self.G, at_tensor_loader(args.gap_load))
            return
        if args.tensors:
            if len(args.tensors) == 1:
                tensor_name = args.tensors[0]
                tensors = self.tensor_store.get(tensor_name)
                if tensors is None:
                    self.perror("{} not in store".format(tensor_name))
                    return
                if args.step is None:
                    self.perror("you must select a step")
                    return
                if args.step >= len(tensors):
                    self.perror("{} doesn't have that step".format(tensor_name))
                    return
                if tensors[args.step] is None:
                    self.perror("{} doesn't have this tensor for that step".format(tensor_name))
                    return
                tensor = tensors[args.step]

                if args.weights:
                    tensor = tensor[1]
                elif args.biases:
                    tensor = tensor[2]
                else:
                    tensor = tensor[0]
                if args.write_numpy:
                    np.save(args.write_numpy, tensor)
                else:
                    self.perror("not sure what to do with this single tensor")
                return

            compare = args.tensors
            tensors = [None]*2
            for i in range(2):
                tensors[i] = self.tensor_store.get(compare[i])
                if tensors[i] is None:
                    self.perror("{} not in store".format(compare[i]))
                    return
                if args.weights:
                    tensors[i] = [t[1] for t in tensors[i]]
                elif args.biases:
                    tensors[i] = [t[2] for t in tensors[i]]
                else:
                    tensors[i] = [t[0] for t in tensors[i]]

            if args.step is not None:
                for i in range(2):
                    if args.step >= len(tensors[i]):
                        self.perror("{} doesn't have that step".format(compare[i]))
                        return
                    if tensors[i][args.step] is None:
                        self.perror(
                            "{} doesn't have this tensor for that step".format(compare[i]))
                        return
                    tensors[i] = [tensors[i][args.step]]

            if args.channel is not None:
                for i in range(2):
                    for j, tensor in enumerate(tensors[i]):
                        if len(tensor.shape) <= len(args.channel):
                            self.perror("selected too many channels for this tensor")
                        for c in args.channel:
                            tensor = tensor[c]
                        tensors[i][j] = tensor

            if args.compare_qsnr or args.compare_error:
                if args.compare_qsnr:
                    def func(x, y):
                        if x is not None and y is not None:
                            return qsnr(x.astype(np.float), y.astype(np.float))
                        return float('nan')
                else:
                    def func(x, y):
                        if x is not None and y is not None:
                            return np.abs(x - y)
                        return float('nan')

                if args.step is not None:
                    print("error for step %s" % args.step)
                    if args.channel is not None:
                        print("error for dimensions [%s]" %
                              (",".join([str(chan) for chan in args.channel])))
#pylint: disable=unsubscriptable-object
                    out = [func(tensors[0][0][i], tensors[1][0][i])
                           for i in range(len(tensors[0][0]))]
                else:
                    out = [func(t1, t2)
                           for t1, t2 in zip(*tensors)]
                for idx, val in enumerate(out):
                    if idx % 10 == 0:
                        print("\n{:03d} {:03d}:  ".format(idx, idx+9), end='')
                    print('{}{}'.format(val, "" if (idx + 1) % 10 == 0 else ", "), end='')
                print()
            else:
                self.ppaged("\n".join(print_comparison(tensors)))
            return

        for idx, k in enumerate(self.tensor_store):
            print("{:3d}) {}".format(idx, k))
