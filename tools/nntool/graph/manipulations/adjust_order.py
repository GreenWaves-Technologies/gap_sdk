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

#pylint: disable=wildcard-import
from .adjusts import *
from .adjust_base import AdjusterBase
from .dimensions import add_dimensions
from .eliminate_transposes import eliminate_transposes

LOG = logging.getLogger("nntool." + __name__)


def adjust_order(G, reshape_weights=True, postprocess=True, debug_function=None):
    opts = {'reshape_weights': reshape_weights}
    selector = AdjusterBase.get_all_handlers(opts)
    LOG.info("adding transposes to correct tensor order for AT kernels")
    for node in [n for n in G.nodes() if n.__class__ in selector]:
        adjuster = selector[node.__class__]
        adjuster.adjust(G, node)
    add_dimensions(G)
    if debug_function:
        debug_function(G)
    if postprocess:
        eliminate_transposes(G, debug_function=debug_function)
        add_dimensions(G)
