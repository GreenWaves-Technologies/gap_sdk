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


from abc import ABC, abstractmethod

from nntool.generation.gen_utils import at_bits
from nntool.generation.code_block import CodeBlock


class AutotilerKernel(ABC):
    KINCLUDES = None
    KSOURCES = None
    KDEFINES = None
    GINCLUDES = None
    GSOURCES = None
    GDEFINES = None

    @abstractmethod
    def code(self, code_block=None):
        pass

    @staticmethod
    def kernel_includes(*val):
        return AutotilerKernel.property_register("KINCLUDES", val)

    @staticmethod
    def kernel_include_paths(*val):
        return AutotilerKernel.property_register("KPATHS", val)

    @staticmethod
    def kernel_sources(*val):
        return AutotilerKernel.property_register("KSOURCES", val)

    @staticmethod
    def kernel_defines(*val):
        return AutotilerKernel.property_register("KDEFINES", val)

    @staticmethod
    def gen_includes(*val):
        return AutotilerKernel.property_register("GINCLUDES", val)

    @staticmethod
    def gen_include_paths(*val):
        return AutotilerKernel.property_register("GPATHS", val)

    @staticmethod
    def gen_sources(*val):
        return AutotilerKernel.property_register("GSOURCES", val)

    @staticmethod
    def gen_defines(*val):
        return AutotilerKernel.property_register("GDEFINES", val)

    @staticmethod
    def property_register(name, value):
        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

class NewAutoTilerKernel(AutotilerKernel):
    CALL_TEMPLATE = None

    def __init__(self, attrs, extra_attrs, gen_ctrl=None) -> None:
        super().__init__()
        self._attrs = attrs
        self._extra_attrs = extra_attrs
        self._gen_ctrl = gen_ctrl

    @property
    def attrs(self):
        return self._attrs

    def get_template(self):
        return self.CALL_TEMPLATE

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        if self._gen_ctrl and not self._gen_ctrl.is_unmodified:
            self._gen_ctrl.gen_ctrl_decl(code_block)

        global_vars = {
            'at_bits': at_bits
        }
        local_vars = {}
        if self._gen_ctrl is None:
            local_vars['gen_ctrl'] = "0"
        else:
            local_vars['gen_ctrl'] = self._gen_ctrl.ctrl_name

        local_vars.update(self._attrs)
        local_vars.update(self._extra_attrs)

        #pylint: disable=eval-used
        try:
            call_str = eval("f'''"+self.get_template() +
                            "'''", global_vars, local_vars)
        except Exception as exc:
            raise ValueError(f'exception processing template {self.get_template()} '
                             f'in handler {self.__class__.__name__} {exc}') from exc

        code_block.write_lines(call_str)
        return code_block

    def __hash__(self) -> int:
        return self._attrs.__hash__()

    def __eq__(self, o: object) -> bool:
        return self.attrs == o.attrs


kernel_includes = AutotilerKernel.kernel_includes
kernel_sources = AutotilerKernel.kernel_sources
kernel_defines = AutotilerKernel.kernel_defines
gen_includes = AutotilerKernel.gen_includes
gen_sources = AutotilerKernel.gen_sources
gen_defines = AutotilerKernel.gen_defines
kernel_include_paths = AutotilerKernel.kernel_include_paths
gen_include_paths = AutotilerKernel.gen_include_paths
