
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
import runner.plp_flash_stimuli_vivo3 as plp_flash_stimuli_vivo3
import time
import sys
import subprocess
import shlex
import io
import runner.stim_utils
from shutil import copyfile


# This class is the default runner for all chips but can be overloaded
# for specific chips in order to change a bit the behavior.
class Runner(Platform):
    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)
        
        self.params = {}
        self.args = {}

        parser = config.getParser()

        parser.add_argument("--gui", dest="gui",
                            action="store_true", help='Open the GUI')
                        
        self.config.addOption("--simulator", dest="simulator", default=None, help='specify the rtl simulator to be used: Mentor vsim or Cadence xcelium')

        parser.add_argument("--vsim-recordwlf", dest="vsim_recordwlf",
                            action="store_true", help='Record vsim Wlf Waveform file')
                        
        parser.add_argument("--vsim-enablecov", dest="vsim_enablecov",
                            action="store_true", help='Enable vsim coverage analysis')
                        
        parser.add_argument("--boot-from-flash", dest="boot_from_flash",
                            action="store_true", help='boot from flash')

        parser.add_argument("--vsim-dofile", dest="vsim_dofile", default=None, help="Specify vsim do file")

        parser.add_argument("--vsim-model", dest="vsim_model", default="sverilog", help="Specify platform model used")
                        
        [args, otherArgs] = parser.parse_known_args()

        self.addCommand('run', 'Run execution on simulator')
        self.addCommand('prepare', 'Prepare binary for simulation (SLM files, etc)')


        # Overwrite JSON configuration with specific options
        if args.gui:
            js_config.get('**/vsim').set('gui', True)

        if args.vsim_recordwlf:
            js_config.get('**/vsim').set('recordwlf', True)

        if args.vsim_enablecov:
            js_config.get('**/vsim').set('enablecov', True)

        if args.vsim_dofile is not None:
            js_config.get('**/vsim').set('dofile', args.vsim_dofile)

        js_config.get('**/vsim').set('vsim_model', args.vsim_model)


        binary = self.config.getOption('binary')
        if binary is not None:
            js_config.get('**/runner').set('binary', binary)

        if args.boot_from_flash:
            js_config.get('**/runner').set('boot_from_flash', True)

        self.stim = runner.stim_utils.stim(verbose=self.get_json().get('**/runner/verbose').get())

        for binary in self.get_json().get('**/runner/binaries').get_dict():
            self.stim.add_binary(binary)

        self.env = {}
        self.rtl_path = None


    def set_param(self, name, value):
        self.params[name] = value

    def get_params(self):
        return self.params

    def set_arg(self, name, value):
        self.args[name] = value

    def get_args(self):
        return self.args

    def get_stim(self):
        return self.stim

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

    def __check_env(self):
        if self.get_json().get_child_str('**/runner/boot-mode').find('rom') != -1:
            if self.get_json().get_child_str('**/vsim/boot-mode') in ['jtag']:
                self.get_json().get('**/runner').set('boot_from_flash', False)
            else:
                self.get_json().get('**/runner').set('boot_from_flash', True)

        if self.get_json().get('**/runner/peripherals') is not None:
            self.get_json().get('**/runner').set('use_tb_comps', True)

        self.__check_debug_bridge()

        with open('rtl_config.json', 'w') as file:
            file.write(self.get_json().dump_to_string())

        if not os.path.exists('stdout'):
            os.makedirs('stdout')


    def prepare(self):

        self.__check_env()

        if self.get_json().get_child_str('**/pulp_chip_family').find('vivosoc3') != -1:

            # Boot from flash, we need to generate the flash image
            # containing the application binary.
            # This will generate SLM files used by the RTL platform
            # to preload the flash.
            comps = []
            fs = self.get_json().get('**/fs')
            if fs is not None:
                comps_conf = self.get_json().get('**/flash/fs/files')
                if comps_conf is not None:
                    comps = comps_conf.get_dict()

            encrypted = self.get_json().get_child_str('**/efuse/encrypted')
            aes_key = self.get_json().get_child_str('**/efuse/aes_key')
            aes_iv = self.get_json().get_child_str('**/efuse/aes_iv')

            if plp_flash_stimuli_vivo3.genFlashImage(
                slmStim=self.get_json().get('**/runner/flash_slm_file').get(),
                rawStim=self.get_json().get('**/runner/flash_raw_file').get(),
                binDescr=self.get_json().get('**/runner/flash_binDescr_file').get(),
                sysDescr=self.get_json().get('**/runner/flash_sysDescr_file').get(),
                bootBinary=self.get_json().get('**/runner/binaries').get_elem(0).get(),
                comps=comps,
                verbose=self.get_json().get('**/runner/verbose').get(),
                archi=self.get_json().get('**/pulp_chip_family').get(),
                flashType=self.get_json().get('**/runner/flash_type').get(),
                encrypt=encrypted, aesKey=aes_key, aesIv=aes_iv):
                return -1
    
            if not self.get_json().get('**/runner/boot_from_flash').get():

                self.stim.gen_stim_slm_64('vectors/stim.txt')


        else:

            if self.get_json().get('**/runner/boot_from_flash').get():

                # Boot from flash, we need to generate the flash image
                # containing the application binary.
                # This will generate SLM files used by the RTL platform
                # to preload the flash.
                comps = []
                fs = self.get_json().get('**/fs')
                if fs is not None:
                    comps_conf = self.get_json().get('**/flash/fs/files')
                    if comps_conf is not None:
                        comps = comps_conf.get_dict()

                encrypted = self.get_json().get_child_str('**/efuse/encrypted')
                aes_key = self.get_json().get_child_str('**/efuse/aes_key')
                aes_iv = self.get_json().get_child_str('**/efuse/aes_iv')

                if plp_flash_stimuli.genFlashImage(
                    slmStim=self.get_json().get('**/runner/flash_slm_file').get(),
                    bootBinary=self.get_json().get('**/runner/binaries').get_elem(0).get(),
                    comps=comps,
                    verbose=self.get_json().get('**/runner/verbose').get(),
                    archi=self.get_json().get('**/pulp_chip_family').get(),
                    flashType=self.get_json().get('**/runner/flash_type').get(),
                    encrypt=encrypted, aesKey=aes_key, aesIv=aes_iv):
                    return -1

            else:

                self.stim.gen_stim_slm_64('vectors/stim.txt')


        if self.get_json().get('**/efuse') is not None:
            efuse = runner.stim_utils.Efuse(self.get_json(), verbose=self.get_json().get('**/runner/verbose').get())
            efuse.gen_stim_txt('efuse_preload.data')

        return 0


    def __check_debug_bridge(self):

        gdb = self.get_json().get('**/gdb/active')
        autorun = self.get_json().get('**/debug_bridge/autorun')

        bridge_active = False

        self.get_json().set('**/debug_bridge/cable/type', 'jtag-proxy')

        if gdb is not None and gdb.get_bool() or autorun is not None and autorun.get_bool():
            bridge_active = True
            self.get_json().get('**/jtag_proxy').set('active', True)
            self.get_json().get('**/runner').set('use_tb_comps', True)
            self.get_json().get('**/runner').set('use_external_tb', True)


        if bridge_active:
            # Increase the access timeout to not get errors as the RTL platform
            # is really slow
            self.get_json().get('**/debug_bridge/cable').set('access_timeout_us', 10000000)



    def run(self):

        self.__check_env()



        cmd = self.__get_sim_cmd()

        autorun = self.get_json().get('**/debug_bridge/autorun')
        if autorun is not None and autorun.get():
            print ('Setting simulator env')
            print (self.env)
            os.environ.update(self.env)

            print ('Launching simulator with command:')
            print (cmd)
            vsim = subprocess.Popen(shlex.split(cmd),stderr=subprocess.PIPE, bufsize=1)
            port = None
            for line in io.TextIOWrapper(vsim.stderr, encoding="utf-8"):
                sys.stderr.write(line)
                string = 'Proxy listening on port '
                pos = line.find(string)
                if pos != -1:
                    port = line[pos + len(string):]
                    break

            if port is None:
                return -1

            options = self.get_json().get_child_str('**/debug_bridge/options')
            if options is None:
                options  = ''

            bridge_cmd = 'plpbridge --config-path=%s/rtl_config.json --verbose=10 --port=%s %s' % (os.getcwd(), port, options)
            print ('Launching bridge with command:')
            print (bridge_cmd)
            time.sleep(10)
            bridge = subprocess.Popen(shlex.split(bridge_cmd))
            
            retval = bridge.wait()
            vsim.terminate()

            return retval

        else:
            for key, value in self.env.items():
                cmd = 'export %s="%s" && ' % (key, value) + cmd

            print ('Launching simulator with command:')
            print (cmd)

            if os.system(cmd) != 0: 
                print ('Simulator reported an error, leaving')
                return -1

        

        return 0


    def __create_symlink(self, rtl_path, name):
        if os.path.islink(name):
          os.remove(name)

        os.symlink(os.path.join(rtl_path, name), name)



    def set_env(self, key, value):
        self.env[key] = value
