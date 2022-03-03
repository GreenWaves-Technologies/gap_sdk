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
from generation.new_generators.generator_base import GeneratorBase
from utils.subclasses import get_all_subclasses
# pylint: disable=wildcard-import,unused-wildcard-import
from .general import *
from .ne16 import *
from .float import *
from .mult8 import *

LOG = logging.getLogger("nntool." + __name__)

PHASE_METHODS = {
    'bindings': 'bindings_generator',
    'kernels': 'kernel_generator',
    'globals': 'globals_generator',
}

def test_option(cache, k, v):
    if isinstance(v, bool):
        return cache.get(k, False) == v
    elif v is None:
        return cache.get(k, None) is None
    else:
        return cache.get(k) == v


class NewGenerator():
    def __init__(self) -> None:
        self._generators = {}
        for gen_class in get_all_subclasses(GeneratorBase):
            if gen_class.PARAMS:
                for params in gen_class.PARAMS:
                    handler_list = self._generators.setdefault(params, [])
                    handlers = {}
                    handler_list.append(handlers)
                    if gen_class.KTYPES:
                        for ktype in gen_class.KTYPES:
                            handlers[ktype] = gen_class
                    else:
                        handlers[None] = gen_class
        for params, handlers in list(self._generators.items()):
            for params_subclass in get_all_subclasses(params):
                if params_subclass not in self._generators:
                    self._generators[params_subclass] = handlers


    def new_execute_phase(self, phase_name, param, qrec, *args, break_on_true=False, **kwargs):
        handler_list = self._generators.get(param.__class__, [])
        res = False
        for gen in handler_list:
            if qrec.ktype in gen:
                gen_class = gen[qrec.ktype]
            elif None in gen:
                gen_class = gen[None]
            else:
                gen_class = None
            if gen_class:
                if gen_class.QREC_OPTIONS is not None:
                    if not all(test_option(qrec.cache, k, v) for k, v in gen_class.QREC_OPTIONS.items()):
                        continue
                LOG.debug("gen phase %s: matched generator class %s",
                            phase_name,
                            gen_class.__name__)

                this_res = getattr(gen_class, PHASE_METHODS[phase_name])(self, param, qrec, *args, **kwargs)
                res = this_res or res
                if res and break_on_true:
                    break
        return res
