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
from abc import ABC, abstractmethod

from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecordBase
from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationBase

LOG = logging.getLogger("nntool." + __name__)

GENERATION_PHASES = [
    "inputs",
    "outputs",
    "globals",
    "bindings",
    "kernels"
]

QREC_POW2 = "qrec_pow2"
QREC_MULT8 = "qrec_mult8"
QREC_MULT32 = "qrec_mult32"


class GeneratorMatcher(ABC):
    @abstractmethod
    def match(self, param, qrec):
        pass


class OldMatcher(GeneratorMatcher):
    def __init__(self, params_matched, qrecs_matched):
        self._params_matched = params_matched
        self._qrecs_matched = qrecs_matched

    @staticmethod
    def match_qrec(qrec_types, qrec):
        if qrec_types is None:
            return True
        if isinstance(qrec, SymmetricQuantizationBase):
            return any([qrec_type == "qrec_pow2" for qrec_type in qrec_types])
        if isinstance(qrec, MultQuantizationRecordBase):
            return any([qrec_type == "qrec_mult8" for qrec_type in qrec_types])
        return False

    def match(self, param, qrec):
        return isinstance(param, self._params_matched) and self.match_qrec(self._qrecs_matched, qrec)

    def __str__(self):
        return "OldMatcher: nodes:({}) qrecs:({})".format(",".join(param.__name__ for param in self._params_matched),
                                                          "All" if self._qrecs_matched is None
                                                          else ",".join(self._qrecs_matched))


class RegisteredGeneratorsMixin():
    Generators = {phase: [] for phase in GENERATION_PHASES}

    def execute_phase(self, phase_name, param, qrec, *args, break_on_true=False, **kwargs):
        gens = self.Generators[phase_name]
        res = False
        for gen in gens:
            if gen['matcher'].match(param, qrec):
                LOG.debug("gen phase %s: matched function %s",
                          phase_name,
                          gen['func'].__name__)
                this_res = gen['func'](self, param, qrec, *args, **kwargs)
                res = this_res or res
                if res and break_on_true:
                    break
        return res

def generation_match(phase_name, matcher):
    def phase_func(func):
        gens = RegisteredGeneratorsMixin.Generators[phase_name]
        gens.append({'matcher': matcher, 'func': func})
        LOG.debug("registering function %s with matcher %s",
                  func.__name__, matcher)
        return func
    return phase_func

def generation_function(phase_name, params, qrec_types=None):
    def phase_func(func):
        gens = RegisteredGeneratorsMixin.Generators[phase_name]
        matcher = OldMatcher(params, qrec_types)
        gens.append({'matcher': matcher, 'func': func})
        LOG.debug("registering function %s with matcher %s",
                  func.__name__, matcher)
        return func
    return phase_func
