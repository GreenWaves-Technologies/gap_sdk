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
from bfloat16 import bfloat16
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.node_id import NodeId

LOG = logging.getLogger('nntool.' + __name__)

FLOAT_DTYPES = {
    'float16': np.float16,
    'float32': np.float32,
    'bfloat16': bfloat16,
}

POW2_DTYPES = {
    'int16': np.int16,
    'int8': np.int8,
}


class TuneError(Exception):
    pass

# set dtype
# set bits
# set scheme
# out or in and out
# out forces out
# in forces all outs above
# forces are stored with quantization


def tune_options(G, nodes, new_options):
    if G.quantization:
        options = G.quantization.options
    else:
        options = {}
    for node in nodes:
        nid = NodeId(node)
        if nid in options:
            options[nid].update(new_options)
        else:
            options[nid] = new_options.copy()

    quantizer = NewQuantizer(G)
    quantizer.options = options
    quantizer.quantize()
    G.add_dimensions()


def tune_float(G, nodes, float_type):
    new_options = {'scheme': 'FLOAT', 'float_type': float_type}
    return tune_options(G, nodes, new_options)

def tune_scaled(G, nodes):
    new_options = {'scheme': 'SQ8'}
    return tune_options(G, nodes, new_options)

def tune_pow2(G, nodes, pow2_type):
    new_options = {'scheme': 'POW2', 'bits': 16 if pow2_type == 'int16' else 8}
    return tune_options(G, nodes, new_options)
