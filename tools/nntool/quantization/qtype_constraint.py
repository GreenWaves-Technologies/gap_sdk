# Copyright (C) 2021  GreenWaves Technologies, SAS

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

from functools import partial

import numpy as np

import math

# Usage:
# qtype constraints are used to test the compatibility of an output with an input
# and to validate if a quantizer exists for a particular input and output configuration
# match float32 on first input
# @in_qs_constraint({'dtype': np.float32})
# match float32 on any input
# @in_qs_constraint(MatchAll({'dtype': np.float32}))
# match int8 on first input and float32 on the rest
# @in_qs_constraint({'dtype': np.int8}, MatchAll({'dtype': np.float32}))
# match float32 or 16 on first input
# @in_qs_constraint({'dtype':set([np.float32, np.float16])})


class MatchAll():
    def __init__(self, constraint) -> None:
        self.constraint = constraint
    
    def __repr__(self):
        return f'MatchAll({self.constraint.__repr__()})'

class ConstraintBase():
    INPUT_CONSTRAINTS = None
    OUTPUT_CONSTRAINTS = None
    OPTION_CONSTRAINT = None


    @staticmethod
    def constraint_tostring(const):
        if const is None:
            return 'None'
        return ",".join(elem.__repr__() if isinstance(elem, (MatchAll)) else elem['dtype'].__repr__() if isinstance(elem['dtype'], set) else elem['dtype'].__name__ for elem in const)

    @classmethod
    def constraints_repr(cls):
        return f"[{ConstraintBase.constraint_tostring(cls.INPUT_CONSTRAINTS)}],[{ConstraintBase.constraint_tostring(cls.OUTPUT_CONSTRAINTS)}]"


    @staticmethod
    def option_constraint(**values):
        def deco_fn(cls):
            if cls.OPTION_CONSTRAINT is None:
                constraints = {}
                setattr(cls, 'OPTION_CONSTRAINT', constraints)
            else:
                constraints = getattr(cls, 'OPTION_CONSTRAINT')

            constraints.update(values)
            return cls
        return deco_fn

    @staticmethod
    def in_qs_constraint(*values, idx=0):
        return ConstraintBase.add_qs_constraints('INPUT', *values, idx=idx)

    @staticmethod
    def out_qs_constraint(*values, idx=0):
        return ConstraintBase.add_qs_constraints('OUTPUT', *values, idx=idx)

    @staticmethod
    def add_qs_constraints(name, *values, idx=0):
        def deco_fn(idx, cls):
            key = f'{name.upper()}_CONSTRAINTS'
            if not hasattr(cls, key) or getattr(cls, key) is None:
                constraints = []
                setattr(cls, key, constraints)
            else:
                constraints = getattr(cls, key)

            constraints.extend(
                [None] * (max((idx + len(values)) - len(constraints), 0)))
            for val in values:
                constraints[idx] = val
                idx += 1
            return cls
        return partial(deco_fn, idx)


def gen_constraint(params, settings):
    constraints = []
    for name in params.INPUT_NAMES:
        for setting in settings:
            if setting == '__DEFAULT__':
                continue
            if name.endswith(setting):
                constraints.append(settings[setting])
                break
        else:
            constraints.append(settings['__DEFAULT__'])
    return constraints
