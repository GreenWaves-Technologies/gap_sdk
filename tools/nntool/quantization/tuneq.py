# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from graph.types import FusionParameters
from quantization.qtype import QType
from utils.node_id import NodeId
from utils.stats_funcs import STATS_BITS


class TuneError(Exception):
    pass

def get_qtype(qparam1, qparam2):
    try:
        bits_idx = STATS_BITS.index(qparam1 + qparam2)
    except ValueError:
        raise TuneError("bit width is not valid")
    return QType(STATS_BITS[bits_idx], qparam2, True)

def tuneq(G, qrecs, step_num, param, qparam1, qparam2):
    step = G.graph_state.steps[step_num]
    node = step['node']
    if param == 'dp':
        if isinstance(node, FusionParameters):
            for filt in node.contained_filters():
                qrec = qrecs[NodeId(node, filt)]
                qrec.acc_q = qrec.calc_q.clone()
        else:
            qrec = qrecs[NodeId(node, None)]
            if not hasattr(qrec, 'calc_q') or not hasattr(qrec, 'acc_q'):
                raise TuneError("you cannot set double precision on this operation")
            qrec.acc_q = qrec.calc_q.clone()
    elif param in ['+', '-']:
        pass
        # TODO - Implement
        # Increase or decrease precision of layer and set input to next layers at that precision
        # handle propagation of too high Q for next layers
    else:
        qrec = qrecs[NodeId(node, None)]
        if not hasattr(qrec, param + '_q'):
            raise TuneError("parameter " + param + " not found")
        setattr(qrec, param + '_q', get_qtype(qparam1, qparam2))
        # TODO - validate this more
