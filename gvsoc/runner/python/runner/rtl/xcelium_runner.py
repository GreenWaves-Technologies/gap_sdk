# 
# Copyright (c) 2017 GreenWaves Technologies SAS
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

#
# Authors: Florent Rotenberg, GWT (florent.rotenberg@greenwaves-technologies.com)
#

from plp_platform import *
import runner.plp_flash_stimuli as plp_flash_stimuli
import time
import sys
import subprocess
import shlex
import io
import platform
import runner.stim_utils
from shutil import copyfile
import runner.rtl.common_runner as runner


GWT_NETWORK="greenwaves-technologies.com"
ETH_NETWORK="ee.ethz.ch"
UNIBO_NETWORK="eees.dei.unibo.it"

# This class is the default runner for all chips but can be overloaded
# for specific chips in order to change a bit the behavior.
class Runner(runner.Runner):



    def __init__(self, config, tree):

        super(Runner, self).__init__(config, tree)
        

    def __get_rtl_path(self):

        if self.rtl_path is None:

            vsim_chip = self.get_json().get_child_str('**/runner/vsim_chip')
            if vsim_chip is None:
                vsim_chip = self.get_json().get('**/pulp_chip_family').get()

            chip_path_name = 'PULP_RTL_%s' % vsim_chip.upper()
            chip_path = os.environ.get(chip_path_name)
            vsim_path = os.environ.get('XCSIM_PATH')

            if vsim_path is not None:
                path_name = chip_path_name
                self.rtl_path = vsim_path
            elif chip_path is not None:
                path_name = 'XCSIM_PATH'
                self.rtl_path = chip_path
            else:
                raise Exception("WARNING: no RTL install specified, neither %s nor XCSIM_PATH is defined:" % (chip_path_name))


            if not os.path.exists(self.rtl_path):
                raise Exception("ERROR: %s=%s path does not exist" % (path_name, self.rtl_path))

            os.environ['XCSIM_PATH'] = self.rtl_path
            os.environ['PULP_PATH'] = self.rtl_path
            os.environ['TB_PATH']   = self.rtl_path


            self.__create_symlink(self.rtl_path, 'boot')
            self.__create_symlink(self.rtl_path, 'cds.lib')
            self.__create_symlink(self.rtl_path, 'hdl.var')
            self.__create_symlink(self.rtl_path, 'tcl_files')
            self.__create_symlink(self.rtl_path, 'waves')
            self.__create_symlink(self.rtl_path, 'xcsim_libs')
            self.__create_symlink(self.rtl_path, 'min_access.txt')
            self.__create_symlink(self.rtl_path, 'models')

        return self.rtl_path


    def __get_sim_cmd(self):


        simulator = self.get_json().get_child_str('**/runner/simulator')
        if simulator == 'xcelium':
            # vsim_script = self.get_json().get_child_str('**/vsim/script')
            # tcl_args = self.get_json().get('**/vsim/tcl_args').get_dict()
            # xmsim_args = self.get_json().get('**/vsim/args').get_dict()
            xmelab_args = self.get_json().get('**/vsim/tcl_args').get_dict()
            xmsim_args = self.get_json().get('**/vsim/args').get_dict()
            gui = self.get_json().get_child_str('**/vsim/gui')

            # recordwlf = self.get_json().get_child_str('**/vsim/recordwlf')
            # vsimdofile = self.get_json().get_child_str('**/vsim/dofile')
            # enablecov = self.get_json().get_child_str('**/vsim/enablecov')
            # vopt_args = self.get_json().get_child_str('**/vsim/vopt_args')

            if not self.get_json().get('**/runner/boot_from_flash').get():
                xmelab_args.append('-defparam tb.tb_test_i.LOAD_L2=JTAG')

            autorun = self.get_json().get('**/debug_bridge/autorun')
            if self.get_json().get('**/runner/use_external_tb').get() or \
              autorun is not None and autorun.get():
                xmelab_args.append('-defparam tb.tb_test_i.ENABLE_EXTERNAL_DRIVER=1')

            if self.get_json().get('**/runner/boot_from_flash').get():
              if self.get_json().get('**/runner/flash_type').get() == 'spi':
                xmelab_args.append('-defparam tb.tb_test_i.SPI_FLASH_LOAD_MEM=1')
              elif self.get_json().get('**/runner/flash_type').get() == 'hyper':
                xmelab_args.append('-defparam tb.tb_test_i.HYPER_FLASH_LOAD_MEM=1')
                if self.get_json().get_child_str('**/chip/name') == 'vega':
                    xmelab_args.append('-defparam tb.tb_test_i.LOAD_L2=HYPER_DEV')
                    xmsim_args.append('+VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH')

            if self.get_json().get('**/use_tb_comps').get():
                xmelab_args.append('-defparam CONFIG_FILE=rtl_config.json')

            if self.get_json().get('**/efuse') is not None:
                xmsim_args.append('+preload_file=efuse_preload.data') #+debug=1  Add that to get debug messages from efuse

            # if gui:
            #     self.set_env('VOPT_ACC_ENA', 'YES')

            # if recordwlf:
            #     self.set_env('RECORD_WLF', 'YES')
            
            # if vsimdofile:
            #     self.set_env('RECORD_WLF', 'YES')
            #     tcl_args.append('-do %s/waves/%s' % (self.__get_rtl_path(), vsimdofile))

            # if enablecov:
            #     self.set_env('VSIM_COV', 'YES')                
            
            xmelab_args.append('-64bit \
                               -licqueue \
                               -timescale 1ns/1ps \
                               -mccodegen \
                               -perfstat \
                               -update \
                               -nxmbind \
                               -nowarn STRINT:CUDEFB \
                               -disable_sem2009 \
                               -gateloopwarn \
                               -show_forces \
                               -dpiheader %s/../tb/tb_driver/dpiheader.h' % (self.__get_rtl_path()))

                               # -always_trigger \
                               # -default_delay_mode distributed \
                               # -no_tchk_msg \
                               # -noassert \


            xmsim_args.append('-64bit \
                               -licqueue \
                               -update \
                               -perfstat \
                               -messages \
                               -xceligen on \
                               -assert_logging_error_off \
                               +GAP_PATH=%s/../../' % (self.__get_rtl_path()))
            xmsim_args.append('-sv_lib %s/install/ws/lib/libpulpdpi' % (os.environ.get('PULP_SDK_HOME')))
           
            if gui:
                xmelab_args.append('-access +rwc +fsmdebug \
                                    -createdebugdb')
                xmsim_args.append('-gui')
            else:
                xmelab_args.append('-afile min_access.txt')
                xmsim_args.append('-input %s/tcl_files/%s' % (self.__get_rtl_path(), "run_and_exit.tcl"))

            if platform.node()[-len(ETH_NETWORK):] == ETH_NETWORK or platform.node()[-len(UNIBO_NETWORK):] == UNIBO_NETWORK:
                cds_xmelab = 'cds_ius-18.09.005 xmelab'
                cds_xmsim = 'cds_ius-18.09.005 xmsim'
            else:
                cds_xmelab = 'xmelab'
                cds_xmsim = 'xmsim'

            xmelab_cmd = "%s tb_lib.tb %s" % (cds_xmelab, ' '.join(xmelab_args))
            xmsim_cmd = "%s tb %s" % (cds_xmsim, ' '.join(xmsim_args))
            cmd = "%s; %s" % (xmelab_cmd, xmsim_cmd)
            return cmd

        else:
            raise Exception('Unknown RTL simulator: ' + simulator)
