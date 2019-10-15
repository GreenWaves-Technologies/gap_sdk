#
# Copyright (c) 2013 Germain Haugou
#
# This file is part of the gvsoc software
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os.path
import ast

class Platform(object):

    def __init__(self, config, js_config):
        self.commands = []
        self.modules = []
        self.config = config
        self.js_config = js_config

    def get_json(self):
        return self.js_config

    def addParser(self, parser):
        self.modules.append(parser)

    def addCommand(self, name, helpStr):
        self.commands.append([name, helpStr])

    def getCommands(self):
        print('Available commands:')
        for command in self.commands:
            print('  %-10s %s' % (command[0], command[1]))
            
    def handleCommands(self):

        for module in self.modules:
            module.parse(self.config)

        self.config.checkOptions()

        if self.config.getOption('showHelp'):
            self.config.getParser().print_help()
        elif self.config.getOption('showCommands'):
            self.getCommands()
        else:
            commands = self.config.getArgs().command
            if len(commands) == 0: commands = ['run']
            for command in commands:
                if self.execCommand(command) != 0: return 1
            return 0


    def execCommand(self, cmd):
        if hasattr(self, cmd):
            return getattr(self, cmd)()
        else:
            print("Unknown command: " + cmd)
            return -1
