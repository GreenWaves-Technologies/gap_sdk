#
# Copyright (C) 2020 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import gsystree as st
import os.path

class Iss(st.Component):
    """
    Riscv instruction set simulator

    Attributes
    ----------
    vp_component : str
        The path to the GVSOC model
    isa : str, optional
        A string describing the list of ISA groups that the ISS should simulate (default: "rv32imfc").
    misa : int, optional
        The initial value of the MISA CSR (default: 0).
    first_external_pcer : int, optional
        The index of the first PCER which is retrieved externally (default: 0).
    riscv_dbg_unit : bool, optional
        True if a riscv debug unit should be included, False otherwise (default: False).
    debug_binaries : list, optional
        A list of path to riscv binaries debug info which can be used to get debug symbols for the assembly trace (default: []).
    binaries : list, optional
        A list of path to riscv binaries (default: []).
    debug_handler : int, optional
        The address where the core should jump when switching to debug mode (default: 0).
    power_models : dict, optional
        A dictionnay describing all the power models used to estimate power consumption in the ISS (default: {})
    power_models_file : file, optional
        A path to a file describing all the power models used to estimate power consumption in the ISS (default: None)
    cluster_id : int, optional
        The cluster ID of the core simulated by the ISS (default: 0).
    core_id : int, optional
        The core ID of the core simulated by the ISS (default: 0).
    fetch_enable : bool, optional
        True if the ISS should start executing instructins immediately, False if it will start after the fetch_enable signal
        starts it (default: False).
    boot_addr : int, optional
        Address of the first instruction (default: 0)
    
    """

    def __init__(self,
            parent,
            name,
            vp_component: str,
            isa: str='rv32imfc',
            misa: int=0,
            first_external_pcer: int=0,
            riscv_dbg_unit: bool=False,
            debug_binaries: list=[],
            binaries: list=[],
            debug_handler: int=0,
            power_models: dict={},
            power_models_file: str=None,
            cluster_id: int=0,
            core_id: int=0,
            fetch_enable: bool=False,
            boot_addr: int=0):

        super(Iss, self).__init__(parent, name)

        if power_models_file is not None:
            power_models = self.load_property_file(power_models_file)

        self.add_properties({
            'vp_component': vp_component,
            'isa': isa,
            'misa': misa,
            'first_external_pcer': first_external_pcer,
            'riscv_dbg_unit': riscv_dbg_unit,
            'debug_binaries': debug_binaries,
            'binaries': binaries,
            'debug_handler': debug_handler,
            'power_models': power_models,
            'cluster_id': cluster_id,
            'core_id': core_id,
            'fetch_enable': fetch_enable,
            'boot_addr': boot_addr,
        })


    def gen_gtkw(self, tree, comp_traces):

        if tree.get_view() == 'overview':
            map_file = tree.new_map_file(self, 'core_state')
            map_file.add_value(1, 'CadetBlue', 'ACTIVE')

            tree.add_trace(self, self.name, 'state', '[7:0]', map_file=map_file, tag='overview')

        else:
            tree.add_trace(self, 'pc', 'pc', '[31:0]', tag='pc')
            tree.add_trace(self, 'asm', 'asm', tag='asm')
            tree.add_trace(self, 'func', 'func', tag='debug')
            tree.add_trace(self, 'inline_func', 'inline_func', tag='debug')
            tree.add_trace(self, 'file', 'file', tag='debug')
            tree.add_trace(self, 'line', 'line', '[31:0]', tag='debug')

            tree.begin_group('events')
            tree.add_trace(self, 'cycles', 'pcer_cycles', tag='core_events')
            tree.add_trace(self, 'instr', 'pcer_instr', tag='core_events')
            tree.add_trace(self, 'ld_stall', 'pcer_ld_stall', tag='core_events')
            tree.add_trace(self, 'jmp_stall', 'pcer_jmp_stall', tag='core_events')
            tree.add_trace(self, 'imiss', 'pcer_imiss', tag='core_events')
            tree.add_trace(self, 'ld', 'pcer_ld', tag='core_events')
            tree.add_trace(self, 'st', 'pcer_st', tag='core_events')
            tree.add_trace(self, 'jump', 'pcer_jump', tag='core_events')
            tree.add_trace(self, 'branch', 'pcer_branch', tag='core_events')
            tree.add_trace(self, 'taken_branch', 'pcer_taken_branch', tag='core_events')
            tree.add_trace(self, 'rvc', 'pcer_rvc', tag='core_events')
            tree.add_trace(self, 'ld_ext', 'pcer_ld_ext', tag='core_events')
            tree.add_trace(self, 'st_ext', 'pcer_st_ext', tag='core_events')
            tree.add_trace(self, 'ld_ext_cycles', 'pcer_ld_ext_cycles', tag='core_events')
            tree.add_trace(self, 'st_ext_cycles', 'pcer_st_ext_cycles', tag='core_events')
            tree.add_trace(self, 'tcdm_cont', 'pcer_tcdm_cont', tag='core_events')
            tree.add_trace(self, 'misaligned', 'pcer_misaligned', tag='core_events')
            tree.add_trace(self, 'insn_cont', 'pcer_insn_cont', tag='core_events')
            tree.end_group('events')



    def gen_gtkw_conf(self, tree, traces):
        if tree.get_view() == 'overview':
            self.vcd_group(self, skip=True)
        else:
            self.vcd_group(self, skip=False)
