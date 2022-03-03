#
# Copyright (C) 2019 GreenWaves Technologies
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

import runner.default_runner
import os
import errors
import argparse
import json_tools as js
from elftools.elf.elffile import ELFFile


def appendArgs(parser: argparse.ArgumentParser, runnerConfig: js.config) -> None:
    """
    Append specific module arguments.

    """




class Runner(runner.default_runner.Runner):

    def __init__(self, args, config, system):
        super(Runner, self).__init__(args, config, system)


    def flash(self):
        for flash in self.get_flashs():
            if flash.get_bool('content/flash'):

                image = flash.get_str('content/image')

                if os.environ.get('GAP_USE_PLPBRIDGE') is not None:
                    cmd = 'plpbridge --chip=%s --verbose 10 --cable=%s --flash-image=%s flash wait' % (os.environ.get('TARGET_NAME'), os.environ.get("PLPBRIDGE_CABLE"), image)

                else:
                    if os.environ.get('GAPY_OPENOCD_CABLE') is not None:
                        self.config.set('openocd/cable', os.environ.get('GAPY_OPENOCD_CABLE'))

                    openocd = self.config.get_str("openocd/path")
                    cable = self.config.get_str('openocd/cable')
                    script = self.config.get_str('openocd/script')
                    image_size = os.path.getsize(image)
                    gap_tools = os.environ.get('GAP_OPENOCD_TOOLS')

                    wsl    = self.config.get_str('runner/wsl')
                    if wsl is None:
                        wsl_image = image
                    else:
                        path_header = '\\"//wsl$/' + wsl
                        path_footer = '\\"'
                        wsl_image = path_header + image + path_footer
                        script = os.environ.get('OPENOCD_CHIP_TARGET')

                    if self.config.get_str('**/chip_family') == 'gap':

                        if flash.get_str('datasheet/type') == 'spi':
                            flasher_script = 'gap_flash_raw_spi'
                        else:
                            flasher_script = 'gap_flash_raw_hyper'

                        cmd = '%s -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -c "script %s; script %s; script tcl/flash_image.tcl; script tcl/jtag_boot.tcl; %s %s %d %s; exit;"' % (openocd, cable, script, flasher_script, image, image_size, gap_tools)

                    elif self.config.get_str('**/chip/name') == 'vega':

                        cmd = '%s -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -f "%s" -f "%s" -f "%s/tcl/flash_image.tcl" -c "vega_flash_raw_hyper %s %d %s; exit;"' % (openocd, cable, script, gap_tools, wsl_image, image_size, gap_tools)

                    else:

                        if flash.get_str('datasheet/type') == 'mram':

                            flasher_binary = gap_tools + '/gap_bins/gap_flasher-gap9_evk-mram.elf'
                            sector_size = 0x2000

                        else:

                            if os.environ.get('BOARD_NAME') == 'gap9_evk':
                                if self.config.get_str('runner/platform') == 'fpga':
                                    flasher_binary = gap_tools + '/gap_bins/gap_flasher-gap9_evk-fpga.elf'
                                else:
                                    flasher_binary = gap_tools + '/gap_bins/gap_flasher-gap9_evk.elf'
                                sector_size = 0x1000
                            else:
                                if self.config.get_str('runner/platform') == 'fpga':
                                    flasher_binary = gap_tools + '/gap_bins/gap_flasher-gapuino9.elf'
                                    sector_size = 0x40000
                                else:
                                    # This is for the variant of socketed gap9mod with atxp032
                                    flasher_binary = gap_tools + '/gap_bins/gap_flasher-gap9_v2.elf'
                                    sector_size = 0x1000

                        cmd = '%s -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -f "%s" -f "%s" -f "%s/tcl/flash_image.tcl" -c "gap9_flash_raw %s %d %s 0x%x; exit;"' % (openocd, cable, script, gap_tools, wsl_image, image_size, flasher_binary, sector_size)

                print ('Flashing image with command:')
                print (cmd)
                
                if os.system(cmd):
                    return -1
        
        return 0


    def exec(self):

        if os.environ.get('GAPY_OPENOCD_CABLE') is not None:
            self.config.set('openocd/cable', os.environ.get('GAPY_OPENOCD_CABLE'))

        openocd = self.config.get_str("openocd/path")
        cable = self.config.get_str('openocd/cable')
        script = self.config.get_str('openocd/script')
        binary = self.config.get_str('runner/boot-loader')

        wsl    = self.config.get_str('runner/wsl')
        if wsl is None:
            wsl_bin = binary
        else:
            path_header = '\\"//wsl$/' + wsl
            path_footer = '\\"'
            wsl_bin = path_header + binary + path_footer
            script = os.environ.get('OPENOCD_CHIP_TARGET') 

        with open(binary, 'rb') as file:
            elffile = ELFFile(file)
            entry = elffile.header['e_entry']

        if self.config.get_bool("openocd/remote/enabled"):

            url = self.config.get_str('openocd/remote/url')
            path = self.config.get_str('openocd/remote/path')

            cmd = 'ssh %s "mkdir -p %s"' % (url, path)
            print ('Creating remote path with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            cmd = 'scp %s %s:%s/test' % (binary, url, path)
            print ('Copying binary with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            cmd = 'ssh -t %s "%s -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -c \\"script %s; script %s; load_and_start_binary %s/test 0x%x\\""' % (url, openocd, cable, script, path, entry)
            print ('Launching execution with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            return 0

        else:

            chip_family = self.config.get_str('**/chip_family')

            if chip_family == 'vega' and os.environ.get('GAPY_USE_OPENOCD') is None:
                cmd = 'plpbridge --chip=vega --verbose 10 --cable=ftdi --binary %s reset load ioloop reqloop start wait' % (binary)

            elif os.environ.get('GAP_USE_PLPBRIDGE') is not None:
                cmd = 'plpbridge --chip=%s --verbose 10 --cable=%s --binary %s reset load ioloop reqloop start wait' % (os.environ.get('TARGET_NAME'), os.environ.get("PLPBRIDGE_CABLE"), binary)


            else:
                platform = self.config.get_str('runner/platform')
                if chip_family == 'vega' or chip_family == 'gap9_v2':
                    cmd = '%s -d0 -c "gdb_port disabled; telnet_port disabled; tcl_port disabled" -f "%s" -f "%s" -c "load_and_start_binary %s 0x%x"' % (openocd, cable, script, wsl_bin, entry)
                else:
                    cmd = "%s -d0 -c 'gdb_port disabled; telnet_port disabled; tcl_port disabled' -f %s -f %s -f tcl/jtag_boot_entry.tcl -c 'gap8_jtag_load_binary_and_start \"%s\" elf 0x%x'" % (openocd, cable, script, wsl_bin, entry)

            os.chdir(self.config.get_str('gapy/work_dir'))

            print ('Launching execution with command:')
            print (cmd)

            if os.system(cmd) != 0:
                return -1

            return 0
