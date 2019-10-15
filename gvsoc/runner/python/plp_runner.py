#
# Copyright (c) 2015 Germain Haugou
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

import argparse
import os
import sys
import re
import imp
import json_tools as js
import pulp_config as plpconf

class Property(object):
    def __init__(self, path, className, compName, propertyName, value):
        self.path = path
        self.className = className
        self.compName = compName
        self.pathRegexp = re.compile(path)
        self.classNameRegexp = re.compile(className)
        self.propertyName = propertyName
        self.value = value
        self.compNameRegexp = re.compile(compName)

    def __str__(self):
        return "%s: %s: %s: %s: %s" % (self.path, self.className, self.compName, self.propertyName, self.value)

    def check(self, path, className, compName):
        # In case we match the component path, its class and its component name, we set the property as being
        # an argument to overload the arguments with the same name
        # Note that components with no name always match the name
        return self.pathRegexp.search(path) and self.classNameRegexp.search(className) and (compName == None or self.compNameRegexp.search(compName))

class Option(object):

    def __init__(self, name, comp, config):
        self.name = name
        self.config = config
        self.comp = comp

    def bindToProperty(self, propertyName, path='.*', className='.*', compName='.*'):
        prop = self.config.setProperty(propertyName=propertyName, path=path, compName=compName, value=self.config.args.__dict__[self.name])
        self.comp.applyProperty(prop)
        

class Config(object):

    def __init__(self, parser):
        self.props = []
        self.parser = parser

    def setProperty(self, propertyName, value=None, path='.*', className='.*', compName='.*'): 
        prop = Property(path=path, className=className, compName=compName, propertyName=propertyName, value=value)
        self.props.append(prop)
        return prop

    def getProperties(self):
        return self.props

    def dumpProperties(self):
        for prop in self.props:
            print(str(prop))

    def getProperty(self, propertyName, path='.*', className='.*', compName='.*'):
        for prop in self.props:
            if prop.propertyName == propertyName and prop.check(path, className, compName):
                return prop
        return None

    def checkOptions(self):
        self.parser.parse_args()        

    def getParser(self):
        return self.parser

    def getArgs(self):
        self.args = self.parser.parse_known_args()[0]
        return self.args

    def addOption(self, *args, **kwargs):
        if 'comp' in kwargs:
            comp = kwargs['comp']
            del kwargs['comp']
        else:
            comp = None
        self.parser.add_argument(*args, **kwargs)
        self.args = self.parser.parse_known_args()[0]
        return Option(name=kwargs['dest'], config=self, comp=comp)

    def getOption(self, name):
        self.args = self.parser.parse_known_args()[0]
        if name not in self.args.__dict__: return None
        return self.args.__dict__[name]

class Runner(object):
    def __init__(self):
        parser = argparse.ArgumentParser(description='Launch a test on the specified architecture.', add_help=False)
        self.config = Config(parser)
        self.platforms = {}
        self.modules = []
        self.parser = parser

    def addModule(self, module):
        self.modules.append(module)

    def addPlatform(self, name, platform):
        self.platforms[name] = platform

    def run(self):
        self.config.addOption("--help", dest="showHelp", action="store_true", help="show this help message and exit")

        self.config.addOption("--platform", dest="platform", default=None, choices=list(self.platforms.keys()), help="specify the platform. Default: %(default)s.")
    
        self.config.addOption("--binary", dest="binary", default=[], action="append",
                            help='specify the binary to be loaded')

        self.config.addOption("--dir", dest="dir", default=os.getcwd(),
                         help="specify the test directory containing the binaries and from which the execution must be launched.", metavar="PATH")
        
        self.config.addOption("--src-dir", dest="src_dir", default=os.getcwd(),
                         help="specify the test source directory", metavar="PATH")
        
        self.config.addOption("--commands", dest="showCommands", action="store_true", default=False,
                         help="show the available commands.")
    
        self.parser.add_argument('command', metavar='commands', type=str, nargs='*',
                            help='a command to be executed')

        self.parser.add_argument("--chip", dest="chip", default=None, help="Specify target chip")

        self.config.addOption("--dev", dest="dev", action="store_true", default=False,
                         help="activate development mode (more advanced options)")

        self.config.addOption("--no-warnings", dest="warnings", action="store_false", default=True,
                         help='deactivate warnings')

        self.config.addOption("--config", dest="config_name", default=None, help='specify the system configuration name')

        self.config.addOption("--reentrant", dest="reentrant", action="store_true", help='This script was called was pulp-run')

        self.config.addOption("--config-file", dest="config_file", default=None, help='specify the system configuration file')

        self.config.addOption("--config-user", dest="config_user", default=[], action="append", help='specify the user configuration file')

        self.config.addOption("--config-opt", dest="configOpt", default=[], action="append", help='specify configuration option (DEPRECATED)')

        self.config.addOption("--property", dest="properties", default=[], action="append", help='specify configuration property')

        if self.config.getOption('dev'):
            self.config.addOption("--py-stack", dest="pyStack", action="store_true", default=False,
                            help="activate Python tracestack")


        testPath = os.path.abspath(self.config.getOption('dir'))
        try:
            os.makedirs(testPath)
        except:
            pass
        os.chdir(testPath)



        config_path = self.config.getOption('config_file')
        config_name = self.config.getOption('config_name')

        if config_path is None:

            if config_name is None:
                raise Exception('A config name or a config file must be specified')

            config_path = os.path.join(
                os.path.dirname(os.path.dirname(sys.argv[0])),
                'configs', 'config', '%s.json' % config_name
            )


        config = plpconf.get_config(config_path, ini_configs=self.config.getOption('config_user'), ini_configs_dict={'srcdir': self.config.getOption('src_dir')}, config_opts=self.config.getOption('configOpt'), properties=self.config.getOption('properties'), interpret=True)

        self.pyStack = config.get_child_bool('**/runner/py-stack')


        platform = self.config.getOption('platform')
        if platform is not None:
            config.set('platform', platform)

        if self.config.getOption('binary') is not None:
            for binary in self.config.getOption('binary'):
                config.get('**/runner').set('binaries', binary)


        platform_name = config.get('**/platform').get()

        try:


            file, path, descr = imp.find_module(self.platforms[platform_name], None)
            module = imp.load_module('module', file, path, descr)

            

            platform = module.Runner(self.config, config)

            for module in self.modules:
                platform.addParser(module(self.config, config))

            retval = platform.handleCommands()
            return retval
        except Exception as e:
            raise
            if self.pyStack:
                raise
            else:
                print(e)
                return -1
