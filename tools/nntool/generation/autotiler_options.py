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

from .memory_device_info import MemoryDeviceInfos

#pylint: disable=line-too-long

AUTO_TILER_OPTIONS = [
    {
        'name': 'KERNEL_BUFFER_PROMOTE', 'type': 'kernel', 'var_type': bool,
        'descr': 'When all user kernel arguments can fit into given L1 memory promote them to buffer', 'default': True},
    {
        'name': 'KERNEL_PARTIAL_BUFFER_PROMOTE', 'type': 'kernel', 'var_type': bool,
        'descr': 'When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer', 'default': True},
    {
        'name': 'KERNEL_NOSOLUTION_ERROR', 'type': 'kernel', 'var_type': bool,
        'descr': 'Report an error when no tiling solution is found', 'default': True},
    {
        'name': 'GRAPH_MONITOR_CYCLES', 'type': 'graph', 'var_type': bool,
        'descr': 'Enable automatic cycle capture for each node of the graph', 'default': False},
    {
        'name': 'GRAPH_MONITOR_CVAR_NAME', 'type': 'graph', 'var_type': str,
        'descr': 'When monitor cycles is on name of the C var array to receive results', 'default': 'AT_GraphPerf'},
    {
        'name': 'GRAPH_PRODUCE_NODE_NAMES', 'type': 'graph', 'var_type': bool,
        'descr': 'Enable production of an array containing the name of each graph node', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_NODE_CVAR_NAME', 'type': 'graph', 'var_type': str,
        'descr': 'When producing node names is on name of the C array receiving the names as strings', 'default': 'AT_GraphNodeNames'},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS', 'type': 'graph', 'var_type': bool,
        'descr': 'Enable production of number of macs for each layer', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS_CVAR_NAME', 'type': 'graph', 'var_type': str,
        'descr': 'When Number of oper Infos is on name of the C array receiving mac infos for each node', 'default': 'AT_GraphOperInfosNames'},
    {
        'name': 'GRAPH_REORDER_CONSTANT_IN', 'type': 'graph', 'var_type': bool,
        'descr': 'Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses', 'default': True},
    {
        'name': 'GRAPH_TRACE_EXEC', 'type': 'graph', 'var_type': bool, 'descr': 'Enable trace of activity', 'default': False},
    {
        'name': 'GRAPH_NOINLINE_NODE', 'type': 'graph', 'var_type': bool,
        'descr': 'If on, all user kernel function are marked as noinline', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_EXEC', 'type': 'graph', 'var_type': str,
        'descr': 'In case a symbol must be allocated in L3 for execution this is the prefered memory', 'default': 'AT_MEM_L3_HRAM'},
    {
        'name': 'GRAPH_CONST_EXEC_FROM_FLASH', 'type': 'graph', 'var_type': bool,
        'descr': 'If on, constant symbols executes from home location', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_HOME', 'type': 'graph', 'var_type': str,
        'descr': 'For constant symbols which L3 flash prefered memory', 'default': 'AT_MEM_L3_HFLASH'
    },
    {
        'name': 'GRAPH_DUMP_TENSOR', 'type': 'graph', 'var_type': int,
        'descr': 'Trace selected tensors arguments at inference time, either all nodes or selected node', 'default': 0
    },
    {
        'name': 'GRAPH_DUMP_ONE_NODE', 'type': 'graph', 'var_type': str,
        'descr': 'Trace one specific graph node', 'default': None
    }
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
    'memory_devices': MemoryDeviceInfos.default()
}

DEFAULT_GEN_OPTS.update({(elem['name'].lower()): elem['default'] for elem in AUTO_TILER_OPTIONS})

DEFAULT_GEN_OPTS_DESCRIPTIONS = {
    'default_input_home_location': {'type': str, 'descr': 'default home location for inputs for code generation'},
    'default_input_exec_location': {'type': str, 'descr': 'default exec location for inputs for code generation'},
    'default_output_home_location': {'type': str, 'descr': 'default home location for outputs for code generation'},
    'default_output_exec_location': {'type': str, 'descr': 'default exec location for outputs for code generation'},
    'default_global_home_location': {'type': str, 'descr': 'default home location for globals for code generation'},
    'default_global_exec_location': {'type': str, 'descr': 'default exec location for globals for code generation'},
    'default_local_location': {'type': str, 'descr': 'default location for locals for code generation'},
    'l2_ram_ext_managed': {'type': bool, 'descr': 'Externally manage L2 RAM'},
    'l3_ram_ext_managed': {'type': bool, 'descr': 'Externally manage L3 RAM'},
    'l3_flash_ext_managed': {'type': bool, 'descr': 'Externally manage L3 flash'},
    'include_project_header': {'type': bool, 'descr': 'Include a header file called "GraphName.h" in generated code'},
    'tensor_directory': {'type': str, 'descr': 'directory to dump tensors to'},
    'model_directory': {'type': str, 'descr': 'directory to dump model to'},
    'model_file': {'type': str, 'descr': 'filename for model'},
    'at_ver': {'type': int, 'descr': 'AutoTiler version'},
}

DEFAULT_GEN_OPTS_DESCRIPTIONS.update(
    {elem['name'].lower(): {'type': elem['var_type'], 'descr': elem['descr']} for elem in AUTO_TILER_OPTIONS})
