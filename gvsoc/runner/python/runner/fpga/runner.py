# -*- coding: utf-8 -*-
import argparse
from plp_platform import *

import os
import os.path
import time
import plptree

def execCmd(cmd):
    print ('Executing command: ' + cmd)
    return os.system(cmd)

class Runner(Platform):

    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)
        
        parser = config.getParser()
        
        parser.add_argument("--board", dest="board", default="genesys2", help="Specify gvsoc trace")

        [args, otherArgs] = parser.parse_known_args()
   
        self.args = args

        self.addCommand('run', 'Run execution')
        self.addCommand('prepare', 'Prepare binary')


    def reset(self):
        return 0


    def prepare(self):
        return 0

    def run(self):
        return os.system('%s/python/runner/fpga/pulpissimo/elf_run_%s.sh %s'  % (os.environ['INSTALL_DIR'], self.args.board, self.get_json().get('**/runner/binaries').get_dict()[0]))
