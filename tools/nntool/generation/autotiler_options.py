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

from .memory_device_info import MemoryDeviceInfos, AT_L3_FLASH_DEVICES, AT_L3_RAM_DEVICES

#pylint: disable=line-too-long

AUTO_TILER_OPTIONS = [
    {
        'name': 'KERNEL_BUFFER_PROMOTE', 'type': 'kernel', 'var_type': bool, 'choices': [True, False],
        'descr': 'When all user kernel arguments can fit into given L1 memory promote them to buffer', 'default': True},
    {
        'name': 'KERNEL_PARTIAL_BUFFER_PROMOTE', 'type': 'kernel', 'var_type': bool, 'choices': [True, False],
        'descr': 'When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer', 'default': True},
    {
        'name': 'KERNEL_NOSOLUTION_ERROR', 'type': 'kernel', 'var_type': bool, 'choices': [True, False],
        'descr': 'Report an error when no tiling solution is found', 'default': True},
    {
        'name': 'GRAPH_MONITOR_CYCLES', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'Enable automatic cycle capture for each node of the graph', 'default': False},
    {
        'name': 'GRAPH_MONITOR_CVAR_NAME', 'type': 'graph', 'var_type': str, 'choices': None,
        'descr': 'When monitor cycles is on name of the C var array to receive results', 'default': 'AT_GraphPerf'},
    {
        'name': 'GRAPH_PRODUCE_NODE_NAMES', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'Enable production of an array containing the name of each graph node', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_NODE_CVAR_NAME', 'type': 'graph', 'var_type': str, 'choices': None,
        'descr': 'When producing node names is on name of the C array receiving the names as strings', 'default': 'AT_GraphNodeNames'},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'Enable production of number of macs for each layer', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS_CVAR_NAME', 'type': 'graph', 'var_type': str, 'choices': None,
        'descr': 'When Number of oper Infos is on name of the C array receiving mac infos for each node', 'default': 'AT_GraphOperInfosNames'},
    {
        'name': 'GRAPH_REORDER_CONSTANT_IN', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses', 'default': True},
    {
        'name': 'GRAPH_TRACE_EXEC', 'type': 'graph', 'var_type': bool, 'descr': 'Enable trace of activity', 'default': False, 'choices': [True, False]},
    {
        'name': 'GRAPH_NOINLINE_NODE', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'If on, all user kernel function are marked as noinline', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_EXEC', 'type': 'graph', 'var_type': str, 'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', ),
        'descr': 'In case a symbol must be allocated in L3 for execution this is the prefered memory', 'default': 'AT_MEM_L3_HRAM'},
    {
        'name': 'GRAPH_CONST_EXEC_FROM_FLASH', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'If on, constant symbols executes from home location', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_HOME', 'type': 'graph', 'var_type': str, 'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', ),
        'descr': 'For constant symbols which L3 flash prefered memory', 'default': 'AT_MEM_L3_HFLASH'
    },
    {
        'name': 'GRAPH_DUMP_TENSOR', 'type': 'graph', 'var_type': int, 'choices': (0, 1),
        'descr': 'Trace selected tensors arguments at inference time, either all nodes or selected node', 'default': 0
    },
    {
        'name': 'GRAPH_DUMP_ONE_NODE', 'type': 'graph', 'var_type': str, 'choices': None,
        'descr': 'Trace one specific graph node', 'default': None
    },
    {
        'name': 'GRAPH_ARG2STRUCT', 'type': 'graph', 'var_type': bool, 'choices': [True, False],
        'descr': 'Kernel C arguments are promoted to struct', 'default': False
    },
    {
        'name': 'GRAPH_SIZE_OPT', 'type': 'graph', 'var_type': int, 'choices': (0, 1, 2),
        'descr': '0: Default make opt, 1: O2 for all layers, Os for xtor,dxtor,runner, 2: Os for layers and  xtor,dxtor,runner', 'default': 0
    },
]

DEFAULT_GEN_OPTS = {
    'default_input_home_location': 'AT_MEM_L2',
    'default_input_exec_location': 'AT_MEM_L2',
    'default_output_home_location': 'AT_MEM_L2',
    'default_output_exec_location': 'AT_MEM_L2',
    'default_global_home_location': 'AT_MEM_L3_HFLASH',
    'default_global_exec_location': 'AT_MEM_UNDEF',
    'default_local_location': 'AT_MEM_UNDEF',
    'l2_ram_ext_managed': True,
    'l3_ram_ext_managed': False,
    'l3_flash_ext_managed': False,
    'generate_checksums': False,
    'include_project_header': False,
    'checksum_file': '',
    'tensor_directory': '.',
    'model_directory': '.',
    'model_file': 'model.c',
    'at_ver': 3,
    'memory_devices': MemoryDeviceInfos.default(),
    'l3_ram_device': 'AT_MEM_L3_HRAM',
    'l3_flash_device': 'AT_MEM_L3_HFLASH',
    'AT_force_relu': True
}

DEFAULT_GEN_OPTS.update({(elem['name'].lower()): elem['default'] for elem in AUTO_TILER_OPTIONS})

DEFAULT_GEN_OPTS_DESCRIPTIONS = {
    'default_input_home_location': {'type': str, 'descr': 'default home location for inputs for code generation',
                                    'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_input_exec_location': {'type': str, 'descr': 'default exec location for inputs for code generation',
                                    'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_output_home_location': {'type': str, 'descr': 'default home location for outputs for code generation',
                                     'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_output_exec_location': {'type': str, 'descr': 'default exec location for outputs for code generation',
                                     'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_global_home_location': {'type': str, 'descr': 'default home location for globals for code generation',
                                     'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_global_exec_location': {'type': str, 'descr': 'default exec location for globals for code generation',
                                     'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'default_local_location': {'type': str, 'descr': 'default location for locals for code generation',
                               'choices': AT_L3_FLASH_DEVICES + AT_L3_RAM_DEVICES + ('AT_MEM_L2', )},
    'l2_ram_ext_managed': {'type': bool, 'descr': 'Externally manage L2 RAM', 'choices': [True, False]},
    'l3_ram_ext_managed': {'type': bool, 'descr': 'Externally manage L3 RAM', 'choices': [True, False]},
    'l3_flash_ext_managed': {'type': bool, 'descr': 'Externally manage L3 flash', 'choices': [True, False]},
    'include_project_header': {'type': bool, 'descr': 'Include a header file called "GraphName.h" in generated code', 'choices': [True, False]},
    'tensor_directory': {'type': str, 'descr': 'directory to dump tensors to'},
    'model_directory': {'type': str, 'descr': 'directory to dump model to'},
    'model_file': {'type': str, 'descr': 'filename for model'},
    'at_ver': {'type': int, 'descr': 'AutoTiler version'},
    'l3_ram_device': {'type': str, 'descr': 'L3 RAM device', 'choices': AT_L3_RAM_DEVICES},
    'l3_flash_device': {'type': str, 'descr': 'L3 FLASH device', 'choices': AT_L3_FLASH_DEVICES},
    'AT_force_relu': {'type': bool, 'descr': 'Replace reluN with relu in the AT model'}
}

DEFAULT_GEN_OPTS_DESCRIPTIONS.update(
    {elem['name'].lower(): {'type': elem['var_type'], 'descr': elem['descr'], 'choices': elem['choices']} for elem in AUTO_TILER_OPTIONS})
