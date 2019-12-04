#
# Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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
# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
#

from plp_platform import *
import runner.plp_flash_stimuli as plp_flash_stimuli
import time
import sys
import subprocess
import shlex
import io
import runner.stim_utils
from shutil import copyfile
import runner.rtl.common_runner as runner


# This class is the default runner for all chips but can be overloaded
# for specific chips in order to change a bit the behavior.
class Runner(runner.Runner):



    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)
        

    def power(self):
        os.environ['POWER_VCD_FILE'] = os.path.join(os.getcwd(), 'cluster_domain.vcd.gz')
        os.environ['POWER_ANALYSIS_PATH'] = os.path.join(os.environ.get('PULP_SRC_PATH'), 'gf22fdx', 'power_analysis')

        if os.path.exists('cluster_domain.vcd.gz'):
            os.remove('cluster_domain.vcd.gz')

        if os.system('gzip cluster_domain.vcd') != 0:
            return -1

        if os.system(os.path.join(os.environ.get('POWER_ANALYSIS_PATH'), 'start_power_zh.csh')) != 0:
            return -1

        copyfile('reports_cluster_domain_0.8V/CLUSTER_power_breakdown.csv', 'power_report.csv')

        if os.system('power_report_extract --report=power_report.csv --dump --config=rtl_config.json --output=power_synthesis.txt') != 0:
            return -1

        return 0


    def __get_rtl_path(self):

        if self.rtl_path is None:

            vsim_path = self.get_json().get_child_str('**/vsim/platform_path_eval')
            if vsim_path is None:
                vsim_path = os.environ.get('VSIM_PATH')
            else:
                vsim_path = eval(vsim_path)

            vsim_chip = self.get_json().get_child_str('**/runner/vsim_chip')
            if vsim_chip is None:
                vsim_chip = self.get_json().get('**/pulp_chip_family').get()

            chip_path_name = 'PULP_RTL_%s' % vsim_chip.upper()
            chip_path = os.environ.get(chip_path_name)

            if vsim_path is not None:
                path_name = chip_path_name
                self.rtl_path = vsim_path
            elif chip_path is not None:
                path_name = 'VSIM_PATH'
                self.rtl_path = chip_path
            else:
                raise Exception("WARNING: no RTL install specified, neither %s nor VSIM_PATH is defined:" % (chip_path_name))


            if not os.path.exists(self.rtl_path):
                raise Exception("ERROR: %s=%s path does not exist" % (path_name, self.rtl_path))

            os.environ['VSIM_PATH'] = self.rtl_path
            os.environ['PULP_PATH'] = self.rtl_path
            os.environ['TB_PATH']   = self.rtl_path

            os.environ['VSIM_PLATFORM'] = self.rtl_path

            self.__create_symlink(self.rtl_path, 'boot')
            self.__create_symlink(self.rtl_path, 'modelsim.ini')
            self.__create_symlink(self.rtl_path, 'work')
            self.__create_symlink(self.rtl_path, 'tcl_files')
            self.__create_symlink(self.rtl_path, 'waves')
            self.__create_symlink(self.rtl_path, 'models')



        return self.rtl_path

    def __get_sim_cmd(self):


        simulator = self.get_json().get_child_str('**/runner/rtl_simulator')

        if simulator == 'vsim':

            vsim_script = self.get_json().get_child_str('**/vsim/script')
            tcl_args = self.get_json().get('**/vsim/tcl_args').get_dict()
            vsim_args = self.get_json().get('**/vsim/args').get_dict()
            gui = self.get_json().get_child_str('**/vsim/gui')

            for param, key in self.get_params().items():
                tcl_args.append('+%s=%s' % (param, key))

            for param, key in self.get_args().items():
                tcl_args.append('-g%s=%s' % (param, key))

            recordwlf = self.get_json().get_child_str('**/vsim/recordwlf')
            vsimdofile = self.get_json().get_child_str('**/vsim/dofile')
            enablecov = self.get_json().get_child_str('**/vsim/enablecov')
            enableJtagTargetSync = self.get_json().get_child_str('**/vsim/enableJtagTargetSync')
            vopt_args = self.get_json().get_child_str('**/vsim/vopt_args')
            model = self.get_json().get_child_str('**/vsim/model')
            if self.get_json().get_child_str('**/chip/name') == 'vega':
                if os.environ.get('USE_NETLIST'):
                    if os.environ.get('USE_PGPIN_NETLIST'):
                        if gui:
                            vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                            vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                            vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck;'" % (self.__get_rtl_path(), vsim_script))
                        else:
                            vsim_args.append("-c")
                            vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                            vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                            vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck; run_and_exit;'" % (self.__get_rtl_path(), vsim_script))
                    else:
                        if gui:
                            vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                            vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                            vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck;'" % (self.__get_rtl_path(), vsim_script))
                        else:
                            vsim_args.append("-c")
                            vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                            vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                            vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck; run_and_exit;'" % (self.__get_rtl_path(), vsim_script))
                else:
                    if gui:
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                        vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck;'" % (self.__get_rtl_path(), vsim_script))
                    else:
                        vsim_args.append("-c")
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/disable_tcheck_fll.do'" % (self.__get_rtl_path()))                    
                        vsim_args.append("-do 'source %s/tcl_files/%s; set_tcheck; run_and_exit;'" % (self.__get_rtl_path(), vsim_script))
            else:
                if model == 'pgpin':
                    os.environ['USE_NETLIST'] = '1'
                    os.environ['USE_PGPIN_NETLIST'] = '1'
                    if gui:
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/go_5_dec.tcl; '" % (self.__get_rtl_path()))
                        vsim_args.append("-do 'source %s/tcl_files/run.tcl; set_osi_pgpin_netlist_tcheck; '" % (self.__get_rtl_path()))
                    else:
                        vsim_args.append("-c")
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/go_5_dec.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/run.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/exec_pgpin.tcl'" % self.__get_rtl_path())

                else:
                    if gui:
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/%s; '" % (self.__get_rtl_path(), vsim_script))
                    else:
                        vsim_args.append("-c")
                        vsim_args.append("-do 'source %s/tcl_files/config/run_and_exit.tcl'" % self.__get_rtl_path())
                        vsim_args.append("-do 'source %s/tcl_files/%s; run_and_exit;'" % (self.__get_rtl_path(), vsim_script))

            if not self.get_json().get('**/runner/boot_from_flash').get():
                tcl_args.append('-gLOAD_L2=JTAG')

            if self.get_json().get_child_str('**/chip/name') == 'vivosoc3':
                tcl_args.append("-gBOOT_ADDR=32'h1C004000")

            autorun = self.get_json().get('**/debug_bridge/autorun')
            if self.get_json().get('**/runner/use_external_tb').get() or \
              autorun is not None and autorun.get():
                tcl_args.append('-gENABLE_EXTERNAL_DRIVER=1')

            if self.get_json().get('**/runner/boot_from_flash').get():
              if self.get_json().get('**/runner/flash_type').get() == 'spi':
                tcl_args.append('-gSPI_FLASH_LOAD_MEM=1')
              elif self.get_json().get('**/runner/flash_type').get() == 'hyper':
                tcl_args.append('-gHYPER_FLASH_LOAD_MEM=1')

                if self.get_json().get_child_str('**/pulp_chip_family') == 'gap':
                    tcl_args.append('+VSIM_PADMUX_CFG=TB_PADMUX_ALT3_HYPERBUS')
                    tcl_args.append('+VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH')

                if self.get_json().get_child_str('**/chip/name') == 'vega':
                    tcl_args.append('+VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH')

              if self.get_json().get_child_str('**/chip/name') == 'wolfe':
                bootsel = 1 if self.get_json().get('**/runner/flash_type').get() == 'hyper' else 0
                tcl_args.append('-gBOOTSEL=%d' % bootsel)

              if self.get_json().get_child_str('**/chip/name') in [ 'pulp', 'pulpissimo', 'arnold' ]:
                tcl_args.append('-gLOAD_L2=STANDALONE')


            if os.environ.get('QUESTA_CXX') != None:
                tcl_args.append('-dpicpppath ' + os.environ.get('QUESTA_CXX'))


            if os.environ.get('PULP_SDK_HOME') is not None:
                os.environ['PULP_SDK_DPI_ARGS'] = '-sv_lib %s/install/ws/lib/libpulpdpi' % (os.environ.get('PULP_SDK_HOME'))
            else:
                os.environ['PULP_SDK_DPI_ARGS'] = '-sv_lib %s/lib/libpulpdpi' % (os.environ.get('INSTALL_DIR'))

            if self.get_json().get('**/use_tb_comps').get():
                tcl_args.append('-gCONFIG_FILE=rtl_config.json -permit_unmatched_virtual_intf')

                tcl_args.append(os.environ['PULP_SDK_DPI_ARGS'])

            else:
                tcl_args.append('-permit_unmatched_virtual_intf')
                

            if self.get_json().get('**/efuse') is not None:
                tcl_args.append('+preload_file=efuse_preload.data') #+debug=1  Add that to get debug messages from efuse

            baudrate = self.get_json().get_child_int('**/uart/baudrate')
            if baudrate is not None:
                tcl_args.append('-gBAUDRATE=%d' % baudrate)

            if gui:
                self.set_env('VOPT_ACC_ENA', 'YES')

            if recordwlf is True:
                self.set_env('RECORD_WLF', 'YES')
            
            if vsimdofile is not None:
                self.set_env('RECORD_WLF', 'YES')
                tcl_args.append('-do %s/waves/%s' % (self.__get_rtl_path(), vsimdofile))

            if enablecov:
                self.set_env('VSIM_COV', 'YES')                
                # tcl_args.append('-coverage -coverstore $VSIM_PATH/fe/sim/cov') 
                # if vopt_args is None:
                #     vopt_args = list()
                # vopt_args.append('+cover=sbecft+pulp_chip.')
                # js_config.get('**/vsim').set('vopt_args', option)

            if enableJtagTargetSync:
                tcl_args.append('+ENABLE_JTAG_TARGET_SYNC=1')

            eoc_check_wait = self.get_json().get_child_str('**/vsim/eoc_check_wait')
            if eoc_check_wait is not None:
                tcl_args.append('+EOC_CHECK_WAIT=%s' % eoc_check_wait)

            if len(tcl_args) != 0:
                self.set_env('VSIM_RUNNER_FLAGS', ' '.join(tcl_args))
            
            if vopt_args is not None:
                if len(vopt_args) != 0:
                    self.set_env('VOPT_RUNNER_FLAGS', ' '.join(vopt_args))

            vsim_cmd = os.environ.get('VSIM_COMMAND')
            if vsim_cmd is None:
                vsim_cmd = "vsim -64"

            cmd = "%s %s" % (vsim_cmd, ' '.join(vsim_args))


            return cmd

        else:
            raise Exception('Unknown RTL simulator: ' + simulator)
