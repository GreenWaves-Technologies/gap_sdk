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

import os

import traces
import json_tools as js


class OpenOCD(object):
    prg = 'openocd'
    
    def __init__(self, cable, chip, scriptsTCL = []):
        self.scripts = []
        self.scripts.append(cable)
        self.scripts.append(chip)
        if scriptsTCL:
            self.scripts.extend(scriptsTCL)
    
    def run(self, openocdCommand = []):
        command = self.prg + ' -c "'
        for scr in self.scripts:
            command += 'script %s; ' % scr
        
        for cmd in openocdCommand:
            command += '%s; ' % cmd
        command += '"'
        traces.info(command)
        rc = os.system(command)
        if rc != 0:
            traces.critical('OpenOCD return with an error: %d' % rc)
            exit(-1)
