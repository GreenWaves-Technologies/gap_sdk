# -*- coding: utf-8 -*-
import argparse
from plp_platform import *

import os
import os.path
import time
import runner.plp_flash_stimuli as plp_flash_stimuli

def execCmd(cmd):
    print ('Executing command: ' + cmd)
    return os.system(cmd)

class Runner(Platform):

    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)
        
        parser = config.getParser()

        parser.add_argument("--pulp-core-archi", dest="pulpCoreArchi",
                            help='specify the core architecture to be simulated', default="or1k")
        parser.add_argument("--pulp-archi", dest="pulpArchi",
                            help='specify the PULP architecture to be simulated', default="mia")
        parser.add_argument("--load", dest="load",
                            help='specify the way the binary is loaded', default=None)
        parser.add_argument("--avr-load", dest="avrLoader",
                            help='use AVR loader', action="store_true", default=False)
        parser.add_argument("--use-dpi", dest="dpi",
                            help='use AVR loader', action="store_true", default=False)
        
        parser.add_argument("--devices", dest="devices", default=[], action="append",
                            help='specify platform devices')
        
        [args, otherArgs] = parser.parse_known_args()
   
        self.addCommand('run', 'Run execution')
        self.addCommand('prepare', 'Prepare binary')
        self.addCommand('copy', 'Copy binary')


    def reset(self):
        return 0


    def copy(self):
        return 0

    def header(self):
        binary = self.get_json().get('**/runner/binaries').get_dict()[0]
        if execCmd("objcopy --srec-len 1 --output-target=srec %s %s.s19" % (binary, os.path.basename(binary))) != 0: return -1
        if execCmd("s19toheader.py %s.s19 " % (os.path.basename(binary))) != 0: return -1
        return 0

    def get_flash_preload_file(self):
        return os.path.join(os.getcwd(), 'stimuli/flash.bin')

    def prepare(self):
        comps = []
        comps_conf = self.get_json().get('**/flash/fs/files')
        if comps_conf is not None:
            comps = comps_conf.get_dict()

        if len(comps) != 0 is not None or (self.get_json().get_child_bool('**/runner/boot_from_flash') and self.get_json().get_child_str('**/runner/boot-mode') == 'dev_hyper'):

            encrypted = self.get_json().get_child_str('**/efuse/encrypted')
            aes_key = self.get_json().get_child_str('**/efuse/aes_key')
            aes_iv = self.get_json().get_child_str('**/efuse/aes_iv')

            if plp_flash_stimuli.genFlashImage(
                raw_stim=self.get_flash_preload_file(),
                bootBinary=self.get_json().get('**/runner/binaries').get_elem(0).get(),
                comps=comps,
                verbose=self.get_json().get('**/runner/verbose').get(),
                archi=self.get_json().get('**/pulp_chip_family').get(),
                flashType=self.get_json().get('**/runner/flash_type').get(),
                encrypt=encrypted, aesKey=aes_key, aesIv=aes_iv):
                return -1

            if self.flash():
                return -1

        return 0

    def flash(self):
        chip_name = self.get_json().get_child_str('**/chip/name')

        if execCmd('plpbridge --cable=ftdi@digilent --chip=%s flash_erase_chip --flasher-init' % chip_name):
            return -1
        if execCmd('plpbridge --cable=ftdi@digilent --chip=%s flash_write --addr=0 --file=%s --flasher-init' % (chip_name, self.get_flash_preload_file())):
            return -1

        return 0

    def run(self):

        binary = self.config.getOption('binary')[0]
        if binary is None:
            raise Exception("No binary specified")

        if binary.find(':') != -1:
            binary, mask = binary.split(':')
        else:
            mask = "1"

        if self.config.getOption('load') == 'flasher':
            flashOpt = '-f %s' % (os.path.join(os.environ.get('PULP_SDK_HOME'), 'install/%s/bin/flash_programmer' % (self.config.getOption('pulpCoreArchi'))))
        else:
            flashOpt = ''

        if self.get_json().get_child_str('**/chip/name') not in ['honey', 'mia']:

            if self.get_json().get('**/gdb/active').get_bool():
                commands_name = '**/debug_bridge/gdb_commands'
            else:
                commands_name = '**/debug_bridge/commands'

            commands = " ".join(self.get_json().get_child_str(commands_name).split(','))


            chip_name = self.get_json().get_child_str('**/chip/name')

            if chip_name in ['gap', 'gap_rev1']:

                config = self.config.getOption('config_file')
                if config is not None:
                    config = '--config-path %s' % config
                else:
                    config = '--config %s' % self.config.getOption('config_name')

                bridge_opt = '%s --cable %s' % (config, self.get_json().get_child_str('**/debug_bridge/cable/type'))

                if self.get_json().get_child_str('**/runner/boot-mode') == 'dev_hyper':
                    return execCmd('plpbridge %s --boot-mode=jtag_hyper load' % (bridge_opt))
                else:
                    return execCmd('plpbridge %s --boot-mode=jtag --binary=%s %s' % (bridge_opt, binary, commands))
            elif self.get_json().get_child_str('**/chip/name') in ['wolfe']:
                return execCmd('plpbridge --cable=ftdi --boot-mode=jtag --binary=%s --chip=wolfe %s' % (binary, commands))
            elif self.get_json().get_child_str('**/chip/name') in ['vivosoc3']:
                return execCmd('plpbridge --cable=ftdi --binary=%s --chip=vivosoc3 %s' % (binary, commands))
            else:
                return execCmd('plpbridge --binary=%s --config=%s %s' % (binary, self.config.getOption('config_file'), commands))

        elif self.config.getOption('pulpArchi') == 'pulp3':
            return execCmd("debug_bridge -c ft2232 -b %s --binary %s --load --late-reset --loop --printf --start %s %s" % (self.config.getOption('pulpArchi').replace('-riscv', ''), binary, mask, flashOpt))
        else:
            if self.config.getOption('avrLoader'):
              return execCmd("debug_bridge -c vivo -b vivosoc2 --binary %s --load --loop --printf --start %s %s" % (binary, mask, flashOpt))
            else:
              return execCmd("debug_bridge -c ft2232 -b %s --reset --binary %s --load --loop --printf --start %s %s" % (self.config.getOption('pulpArchi').replace('-riscv', ''), binary, mask, flashOpt))
          
#        else:
#          if self.reset() != 0: return -1
#          if binary != None:
#            if self.copy() != 0:
#              return -1
#
#            return execCmd("vivo-boot --start --poll")
