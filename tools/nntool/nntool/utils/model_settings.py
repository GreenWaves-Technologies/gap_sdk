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


def model_settings(
    default_input_home_location: str='AT_MEM_L2',
    default_input_exec_location: str='AT_MEM_L2',
    default_output_home_location: str='AT_MEM_L2',
    default_output_exec_location: str='AT_MEM_L2',
    default_global_home_location: str='AT_MEM_L3_HFLASH',
    default_global_exec_location: str='AT_MEM_UNDEF',
    default_local_location: str='AT_MEM_UNDEF',
    l2_ram_ext_managed: bool=True,
    l3_ram_ext_managed: bool=False,
    l3_flash_ext_managed: bool=False,
    include_project_header: bool=False,
    tensor_directory: str='.',
    model_directory: str='.',
    model_file: str='model.c',
    basic_kernel_source_file: str='Expression_Kernels.c',
    basic_kernel_header_file: str='Expression_Kernels.h',
    at_ver: int=3,
    l3_ram_device: str='AT_MEM_L3_HRAM',
    l3_flash_device: str='AT_MEM_L3_HFLASH',
    AT_force_relu: bool=True,
    l1_size: int=64000,
    l2_size: int=300000,
    l3_size: int=8000000,
    l3_flash_mb: int=64,
    fc_freq: int=250000000,
    cl_freq: int=175000000,
    cluster_stack_size: int=4096,
    cluster_slave_stack_size: int=1024,
    cluster_num_cores: int=8,
    anonymise: bool=False,
    graph_monitor_cycles: bool=False,
    graph_monitor_cvar_name: str='AT_GraphPerf',
    graph_produce_node_names: bool=False,
    graph_produce_node_cvar_name: str='AT_GraphNodeNames',
    graph_produce_operinfos: bool=False,
    graph_produce_operinfos_cvar_name: str='AT_GraphOperInfosNames',
    graph_reorder_constant_in: bool=True,
    graph_trace_exec: bool=False,
    graph_noinline_node: bool=False,
    graph_pref_l3_exec: str='AT_MEM_L3_HRAM',
    graph_const_exec_from_flash: bool=False,
    graph_pref_l3_home: str='AT_MEM_L3_HFLASH',
    graph_dump_tensor: int=0,
    graph_checksum: int=0,
    graph_dump_one_node: str=None,
    graph_arg2struct: bool=False,
    graph_size_opt: int=0,
    graph_warm_construct: int=0,
    graph_group_weights: bool=False,
    graph_async_fork: bool=False
):
    """ Create execute on target model settings

    Utility method to create execute on target settings settings dictionary.

    Args:
        default_input_home_location (str):
            default home location for inputs for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L2'.
        default_input_exec_location (str):
            default exec location for inputs for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L2'.
        default_output_home_location (str):
            default home location for outputs for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L2'.
        default_output_exec_location (str):
            default exec location for outputs for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L2'.
        default_global_home_location (str):
            default home location for globals for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L3_HFLASH'.
        default_global_exec_location (str):
            default exec location for globals for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_UNDEF'.
        default_local_location (str):
            default location for locals for code generation.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_UNDEF'.
        l2_ram_ext_managed (bool):
            Externally manage L2 RAM.

            Choices are: True, False.

            Default is True.
        l3_ram_ext_managed (bool):
            Externally manage L3 RAM.

            Choices are: True, False.

            Default is False.
        l3_flash_ext_managed (bool):
            Externally manage L3 flash.

            Choices are: True, False.

            Default is False.
        include_project_header (bool):
            Include a header file called "GraphName.h" in generated code.

            Choices are: True, False.

            Default is False.
        tensor_directory (str):
            directory to dump tensors to.

            Default is '.'.
        model_directory (str):
            directory to dump model to.

            Default is '.'.
        model_file (str):
            filename for model.

            Default is 'model.c'.
        basic_kernel_source_file (str):
            filename for generated basic kernels.

            Default is 'Expression_Kernels.c'.
        basic_kernel_header_file (str):
            filename for generated basic kernel headers.

            Default is 'Expression_Kernels.h'.
        at_ver (int):
            AutoTiler version.

            Default is 3.
        l3_ram_device (str):
            L3 RAM device.

            Choices are: 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM'.

            Default is 'AT_MEM_L3_HRAM'.
        l3_flash_device (str):
            L3 FLASH device.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH'.

            Default is 'AT_MEM_L3_HFLASH'.
        AT_force_relu (bool):
            Replace reluN with relu in the AT model.

            Default is True.
        l1_size (int):
            Amount of L1 memory to use in target (including cluster stack).

            Default is 64000.
        l2_size (int):
            Amount of L2 memory to use in target.

            Default is 300000.
        l3_size (int):
            Amount of L3 memory to use in target.

            Default is 8000000.
        l3_flash_mb (int):
            Amount of FLASH L3 memory MB to use in target.

            Default is 64.
        fc_freq (int):
            Fabric Controller frequency in Hz.

            Default is 250000000.
        cl_freq (int):
            Cluster frequency in Hz.

            Default is 175000000.
        cluster_stack_size (int):
            Size of stack for cluster master core.

            Default is 4096.
        cluster_slave_stack_size (int):
            Size of stack for slave core.

            Default is 1024.
        cluster_num_cores (int):
            Number of cores in cluster.

            Default is 8.
        anonymise (bool):
            Try to anonymise names.

            Default is False.
        graph_monitor_cycles (bool):
            Enable automatic cycle capture for each node of the graph.

            Choices are: True, False.

            Default is False.
        graph_monitor_cvar_name (str):
            When GRAPH_MONITOR_CYCLES is enabled, set the name of the C var array to receive results.

            Default is 'AT_GraphPerf'.
        graph_produce_node_names (bool):
            Enable production of an array containing the name of each graph node.

            Choices are: True, False.

            Default is False.
        graph_produce_node_cvar_name (str):
            When GRAPH_PRODUCE_NODE_NAMES is enamed, set the name of the C array receiving the names as strings.

            Default is 'AT_GraphNodeNames'.
        graph_produce_operinfos (bool):
            Enable production of number of operations/macs for each layer.

            Choices are: True, False.

            Default is False.
        graph_produce_operinfos_cvar_name (str):
            When GRAPH_PRODUCE_OPERINFOS is enabled, set the name of the C array receiving mac infos for each node.

            Default is 'AT_GraphOperInfosNames'.
        graph_reorder_constant_in (bool):
            Enable reordering of constant inputs in order to transform 2D accesses into 1D accesses.

            Choices are: True, False.

            Default is True.
        graph_trace_exec (bool):
            Enable trace of activity.

            Choices are: True, False.

            Default is False.
        graph_noinline_node (bool):
            If on, all user kernel function are marked as noinline.

            Choices are: True, False.

            Default is False.
        graph_pref_l3_exec (str):
            In case a symbol must be allocated in L3 for execution this is the prefered memory.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L3_HRAM'.
        graph_const_exec_from_flash (bool):
            If on, constant symbols executes from home location.

            Choices are: True, False.

            Default is False.
        graph_pref_l3_home (str):
            L3 flash type preferrer for for constant symbols.

            Choices are: 'AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH', 'AT_MEM_L3_DEFAULTFLASH', 'AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM', 'AT_MEM_L3_DEFAULTRAM', 'AT_MEM_L2'.

            Default is 'AT_MEM_L3_HFLASH'.
        graph_dump_tensor (int):
            Trace selected tensors arguments at inference time, either all nodes or selected node.

            Choices are: 0, 1.

            Default is 0.
        graph_checksum (int):
            Generate the checksum functions call in the C code and print the checksum values.

            Choices are: 0, 1.

            Default is 0.
        graph_dump_one_node (str):
            Trace one specific graph node.

            Default is None.
        graph_arg2struct (bool):
            Kernel C arguments are promoted to struct.

            Choices are: True, False.

            Default is False.
        graph_size_opt (int):
            0: Default make opt, 1: O2 for all layers, Os for xtor,dxtor,runner, 2: Os for layers and  xtor,dxtor,runner.

            Choices are: 0, 1, 2.

            Default is 0.
        graph_warm_construct (int):
            Generate construct/destruct functions with the Warm option to only 1. allocate/deallocate L1 buffer, 2. separate alloc/dealloc of L2 dynamic memory.

            Choices are: 0, 1, 2.

            Default is 0.
        graph_group_weights (bool):
            Group together KerArg of the same type/size in the context of the single layer. For example weights and biases in RNN.

            Choices are: True, False.

            Default is False.
        graph_async_fork (bool):
            ONLY AVAILABLE IN GAP9: Generate user kernel code with async fork option.

            Choices are: True, False.

            Default is False.
    """
    retval = {}
    if default_input_home_location != 'AT_MEM_L2':
        retval["default_input_home_location"] = default_input_home_location
    if default_input_exec_location != 'AT_MEM_L2':
        retval["default_input_exec_location"] = default_input_exec_location
    if default_output_home_location != 'AT_MEM_L2':
        retval["default_output_home_location"] = default_output_home_location
    if default_output_exec_location != 'AT_MEM_L2':
        retval["default_output_exec_location"] = default_output_exec_location
    if default_global_home_location != 'AT_MEM_L3_HFLASH':
        retval["default_global_home_location"] = default_global_home_location
    if default_global_exec_location != 'AT_MEM_UNDEF':
        retval["default_global_exec_location"] = default_global_exec_location
    if default_local_location != 'AT_MEM_UNDEF':
        retval["default_local_location"] = default_local_location
    if not l2_ram_ext_managed:
        retval["l2_ram_ext_managed"] = l2_ram_ext_managed
    if l3_ram_ext_managed:
        retval["l3_ram_ext_managed"] = l3_ram_ext_managed
    if l3_flash_ext_managed:
        retval["l3_flash_ext_managed"] = l3_flash_ext_managed
    if include_project_header:
        retval["include_project_header"] = include_project_header
    if tensor_directory != '.':
        retval["tensor_directory"] = tensor_directory
    if model_directory != '.':
        retval["model_directory"] = model_directory
    if model_file != 'model.c':
        retval["model_file"] = model_file
    if basic_kernel_source_file != 'Expression_Kernels.c':
        retval["basic_kernel_source_file"] = basic_kernel_source_file
    if basic_kernel_header_file != 'Expression_Kernels.h':
        retval["basic_kernel_header_file"] = basic_kernel_header_file
    if at_ver != 3:
        retval["at_ver"] = at_ver
    if l3_ram_device != 'AT_MEM_L3_HRAM':
        retval["l3_ram_device"] = l3_ram_device
    if l3_flash_device != 'AT_MEM_L3_HFLASH':
        retval["l3_flash_device"] = l3_flash_device
    if not AT_force_relu:
        retval["AT_force_relu"] = AT_force_relu
    if l1_size != 64000:
        retval["l1_size"] = l1_size
    if l2_size != 300000:
        retval["l2_size"] = l2_size
    if l3_size != 8000000:
        retval["l3_size"] = l3_size
    if l3_flash_mb != 64:
        retval["l3_flash_mb"] = l3_flash_mb
    if fc_freq != 250000000:
        retval["fc_freq"] = fc_freq
    if cl_freq != 175000000:
        retval["cl_freq"] = cl_freq
    if cluster_stack_size != 4096:
        retval["cluster_stack_size"] = cluster_stack_size
    if cluster_slave_stack_size != 1024:
        retval["cluster_slave_stack_size"] = cluster_slave_stack_size
    if cluster_num_cores != 8:
        retval["cluster_num_cores"] = cluster_num_cores
    if anonymise:
        retval["anonymise"] = anonymise
    if graph_monitor_cycles:
        retval["graph_monitor_cycles"] = graph_monitor_cycles
    if graph_monitor_cvar_name != 'AT_GraphPerf':
        retval["graph_monitor_cvar_name"] = graph_monitor_cvar_name
    if graph_produce_node_names:
        retval["graph_produce_node_names"] = graph_produce_node_names
    if graph_produce_node_cvar_name != 'AT_GraphNodeNames':
        retval["graph_produce_node_cvar_name"] = graph_produce_node_cvar_name
    if graph_produce_operinfos:
        retval["graph_produce_operinfos"] = graph_produce_operinfos
    if graph_produce_operinfos_cvar_name != 'AT_GraphOperInfosNames':
        retval["graph_produce_operinfos_cvar_name"] = graph_produce_operinfos_cvar_name
    if not graph_reorder_constant_in:
        retval["graph_reorder_constant_in"] = graph_reorder_constant_in
    if graph_trace_exec:
        retval["graph_trace_exec"] = graph_trace_exec
    if graph_noinline_node:
        retval["graph_noinline_node"] = graph_noinline_node
    if graph_pref_l3_exec != 'AT_MEM_L3_HRAM':
        retval["graph_pref_l3_exec"] = graph_pref_l3_exec
    if graph_const_exec_from_flash:
        retval["graph_const_exec_from_flash"] = graph_const_exec_from_flash
    if graph_pref_l3_home != 'AT_MEM_L3_HFLASH':
        retval["graph_pref_l3_home"] = graph_pref_l3_home
    if graph_dump_tensor != 0:
        retval["graph_dump_tensor"] = graph_dump_tensor
    if graph_checksum != 0:
        retval["graph_checksum"] = graph_checksum
    if graph_dump_one_node is not None:
        retval["graph_dump_one_node"] = graph_dump_one_node
    if graph_arg2struct:
        retval["graph_arg2struct"] = graph_arg2struct
    if graph_size_opt != 0:
        retval["graph_size_opt"] = graph_size_opt
    if graph_warm_construct != 0:
        retval["graph_warm_construct"] = graph_warm_construct
    if graph_group_weights:
        retval["graph_group_weights"] = graph_group_weights
    if graph_async_fork:
        retval["graph_async_fork"] = graph_async_fork
    return retval
