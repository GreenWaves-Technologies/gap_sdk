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

import numpy as np


def update_ema(ema, value, decay):
    ema = value * decay + (1 - decay) * ema
    return ema

def update_ranges(range_out, tensor_min, tensor_max, ema_decay=None):
    if ema_decay is not None and all([range_out['min'] != float('inf'), range_out['max'] != float('-inf')]):
        range_out['min'] = update_ema(
            range_out['min'], tensor_min, ema_decay)
        range_out['max'] = update_ema(
            range_out['max'], tensor_max, ema_decay)
    else:
        range_out['min'] = min(range_out['min'], tensor_min)
        range_out['max'] = max(range_out['max'], tensor_max)

def collect_stat(stat: dict, name, details, details_name=None, ema_decay=None):
    range_stat = stat.setdefault(name, {'min': float('inf'), 'max': float('-inf')})
    postfix = "" if details_name is None else f'_{details_name}'
    tensors = tuple(details[f'{key}{postfix}'] for key in ('min', 'max'))
    update_ranges(range_stat, *tensors, ema_decay=ema_decay)
