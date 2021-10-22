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

from abc import ABC


class GeneratorBase(ABC):
    PARAMS = None
    KTYPES = None
    QREC_OPTIONS = None

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        pass

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        pass

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        pass

    @staticmethod
    def paramstype(*params_classes):
        return GeneratorBase.property_register("PARAMS", params_classes)

    @staticmethod
    def ktype(*ktypes):
        return GeneratorBase.property_register("KTYPES", ktypes)

    @staticmethod
    def qrec_options(**opts):
        return GeneratorBase.property_register("QREC_OPTIONS", opts)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

paramstype = GeneratorBase.paramstype
ktype = GeneratorBase.ktype
qrec_options = GeneratorBase.qrec_options
