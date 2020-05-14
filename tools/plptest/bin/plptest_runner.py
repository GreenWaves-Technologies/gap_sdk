#!/usr/bin/env python3

#
# Copyright (C) 2018 ETH Zurich and University of Bologna
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
# Authors: Germain Haugou (germain.haugou@gmail.com)
#

import configparser
import collections
import socket
import pickle
import logging
import os
from twisted.internet import protocol, reactor, endpoints
from plptest_utils import *
import plpobjects
import imp
import plptest_condor
try:
  import psutil
except:
  pass

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

if os.environ.get('PYTHON_LOG') != None:
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')


def getOptionValue(value):
    return os.path.expandvars(value)
            

def parse_testset(top_testset, testset, topParent, runner, path, rootdir):
  if testset.parent != None: parent = testset.struct
  else: parent = topParent

  testset.struct = Testset(runner, testset.name, path, parent)
  if top_testset is not None:
    top_testset.append(testset.struct)

  testset.struct.set_restrict(testset.restrict)
  testset.struct.set_parallel(testset.parallel)


  for file in testset.files:
    if file.find('.ini') != -1:
      IniParser(runner, os.path.join(rootdir, file)).parse(testset.struct)
    else:
      CfgParser(runner, os.path.join(rootdir, file)).parse(testset.struct)

  for sub_testset in testset.testsets:
    parse_testset(None, sub_testset, testset.struct, runner, path, rootdir)

  for sub_test in testset.tests:
    parse_test(None, sub_test, testset.struct, runner, path)


  return parent


def parse_test(top_testset, test, topParent, runner, path):
  if test.parent != None: parent = test.struct
  else: parent = topParent

  logging.debug("Adding test (name: %s)" % (test.name))
  test.struct = Test(runner, test.name, path, parent)
  if top_testset is not None:
    top_testset.append(test.struct)

  for cmd in test.commands:
    test.struct.addCommand(cmd)

  if test.path != None: test.struct.setDir(test.path)

  test.struct.set_restrict(test.restrict)
  test.struct.scores = test.scores

  for tag in test.tags:
    test.struct.addTag(tag)

  test.struct.setTimeout(int(test.timeout))

  for param in test.params:
    test.struct.addParam(param)


class CfgParser(object):

  def __init__(self, runner, file):
    self.file = file
    self.runner = runner
    self.path = os.path.dirname(self.file)
    self.config = {}

  def parse(self, topParent=None):

    logging.debug("Starting parsing file (path: %s)" % (self.file))

    try:
      module = imp.load_source('test', self.file)
    except:
      raise Exception(bcolors.FAIL + 'Unable to open test configuration file: ' + self.file + bcolors.ENDC)

    try:
      self.config.update(module.TestConfig)
    except:
      raise Exception(bcolors.FAIL + 'Project configuration must define the TestConfig variable: ' + self.file + bcolors.ENDC)

    top_testset = []
    result = top_testset

    testsets = self.config.get('testsets')

    if topParent is None and (testsets is None or len(testsets) > 1):
      topParent = Testset(self.runner, 'top', 'top')
      result = [topParent]


    if testsets != None:
      for testset in testsets:
        parent = parse_testset(top_testset, testset, topParent, self.runner, self.path, os.path.dirname(self.file))

    files = self.config.get('files')
    if files is not None:
        for file in files:
            file_path = os.path.join(os.path.dirname(self.file), file)
            if file.find('.ini') != -1:
                testset = IniParser(self.runner, file_path)
            else:
                testset = CfgParser(self.runner, file_path)
            testset.parse(top_testset.struct)

    tests = self.config.get('tests')
    top_tests = []
    if tests != None:
      for test in tests:
        parse_test(top_testset, test, topParent, self.runner, self.path)

    return result


class IniParser(object):

  def __init__(self, runner, file):
    self.file = file
    self.runner = runner
    self.path = os.path.dirname(self.file)

  def getOptions(self, section):
    result = []
    fullDict = {}
    #fullDict = dict(list(runConfig.getAll().items()) + list(self.userConf.items()))
    #fullDict['config'] = runConfig.getDeprecatedString()
    #if runConfig.get('flag') != None:
    #  fullDict['flags'] = ' '.join(runConfig.get('flag'))
    #else:
    #  fullDict['flags'] = ''
    for option in self.parser.options(section):
      result.append([option, getOptionValue(self.parser.get(section, option))])
        #, vars=fullDict))])
    return result

  def parse(self, topParent=None):

    logging.debug("Starting parsing file (path: %s)" % (self.file))

    config = configparser.SafeConfigParser(dict_type=collections.OrderedDict)
    self.parser = config
    config.optionxform = str
    openedPaths = config.read(self.file)
    
    if len(openedPaths) == 0:
        logging.warning("Didn't manage to open file: %s" % (self.file))
	        
    testsets = {}
    topTestset = None

    for section in config.sections():

      parent = None

      sectionList = section.split(':')
      if len(sectionList) < 2: raise Exception("Invalid section, must contains t least 2 items: [type:name]")
      sectionType = sectionList[0]
      if len(sectionList) > 2:
        parentName = ':'.join(sectionList[1:len(sectionList)-1])
        parent = testsets.get(parentName)
      if parent == None: parent = topParent
      name = sectionList[len(sectionList)-1]

      if sectionType == 'testset':

        testset = Testset(self.runner, name, self.path, parent)
        testsets[name] = testset
        if topTestset == None: topTestset = testset

        for item in config.items(section):
          if item[0] == 'files':
            pass
            files = getOptionValue(item[1]).split()
            for childFile in files:
              if childFile.find('.ini') != -1:
                IniParser(self.runner, os.path.join(os.path.dirname(self.file), childFile)).parse(testset)
              else:
                CfgParser(self.runner, os.path.join(os.path.dirname(self.file), childFile)).parse(testset)
          if item[0] == 'configs':
            for conf in getOptionValue(item[1]).split():
              testset.addConfigConstraint(pulpconfig.Configuration(useRegExp=True, name=conf))
            #print (bcolors.FAIL + 'Caught an error while parsing test description file: ' + file + bcolors.ENDC)
            #raise
          else:
            pass
            #testset.addUserConfig(item[0], getOptionValue(item[1]))

      elif sectionType == 'run':
        # Deprecated type, just here for compatibility
        pass

      elif sectionType == 'test':
        test = Test(self.runner, name, self.path, parent)
        if topTestset == None: topTestset = test

        for option, value in config.items(section, raw=True):
          if option.find('command.') == 0:
            test.addCommand([option.split('.')[1], value])
          elif option == 'dir':
            test.setDir(value)
          elif option == 'configs':
            for conf in value.split():
              test.addConfigConstraint(conf)
          elif option == 'tags':
            for tag in value.split():
              test.addTag(tag)
          elif option == 'timeout':
            test.setTimeout(int(value))
          elif option == 'check':
            for checker in value.split():
              test.addChecker(checker)
          elif option == 'parameters':
            for param in value.split():
              test.addParam(param)
          elif option.find('probe') == 0:
            pass
            #probeName = option.split('[')[1].split(']')[0]
            #if probes.get(probeName) == None:
            #    probes[probeName] = Probe(probeName, testsuiteName, moduleName, testName)
            #probeOption = option.split('[')[1].split(']')[1].split('.')[1]
            #probes[probeName].setProp(probeOption, value)
          else:
            raise BaseException("Unknown item %s in test %s" % (option, self.name))

      else:
        raise Exception("Invalid section type: " + sectionType)

    return topTestset    






class UiHandler(protocol.Protocol):
    def __init__(self, configs, tests):
      self.tests = tests
      self.configs = configs
      self.data = b""

    def dataReceived(self, data):

      self.data += data

      try:
        cmd = pickle.loads(self.data)

      except:
        return

      if cmd.name == 'get tests':
        cmd.tests = self.tests
        cmd.configs = self.configs
        self.transport.write(pickle.dumps(cmd))
      elif cmd.name == 'run tests':
        for config in cmd.configs:
          for test_name in cmd.tests:
            for test in self.tests:
              test_obj = test.get(test_name)
              if test_obj is not None:
                test_obj.run(config)

      self.data = b""

class UiServer(protocol.Factory):

    def __init__(self, configs, tests):
      self.tests = tests
      self.configs = configs
      self.handler = None

    def buildProtocol(self, addr):
      self.handler = UiHandler(self.configs, self.tests)
      return self.handler


class TestRunner(object):

    def __init__(
        self, nbThreads=1, server=False, stdout=False, 
        maxOutputLen=-1, maxTimeout=-1, worker_pool=None,
        db=False, pobjs=None, build=None, average_load=None, safe_stdout=False, home=None,
        bench_csv_file=None, bench_regexp=None, commands=None, dry_run=False):
        self.nb_runs = 0
        self.tests = []
        self.server = server
        self.pendings = []
        self.runnings = []
        self.nbThreads = nbThreads
        self.stdout = stdout
        self.safe_stdout = safe_stdout
        self.maxOutputLen = maxOutputLen
        self.configs = []
        self.plpobjects = pobjs
        if self.plpobjects is None:
          self.plpobjects = plpobjects.PulpObjects(db_import=db)
        self.runCompletionCallback = None
        self.maxTestNameLen = 0
        self.maxTimeout = maxTimeout
        self.worker_pool = None
        self.build = build
        self.home = home
        self.average_load = None
        self.bench_regexp = bench_regexp
        self.bench_csv_file = bench_csv_file
        self.commands = commands
        self.cpu_load_checker_call_id = None
        self.dry_run = dry_run

        if average_load is not None:
          self.average_load = average_load * 100

        if worker_pool == 'condor':
            self.worker_pool = plptest_condor.Condor_pool()

    def get_test_id(self):
        test_id = self.nb_runs
        self.nb_runs += 1
        return test_id

    def get_worker_pool(self):
        return self.worker_pool

    def __getstate__(self):
        return None

    def __setstate__(self, state):
        pass

    def stop(self):
        if reactor.running:
            reactor.stop()

    def addTestset(self, testset):
        if testset.find('.ini') != -1:
            self.tests += IniParser(self, testset).parse()
        else:
            self.tests += CfgParser(self, testset).parse()

        for test in self.tests:
            len = test.getMaxTestNameLen()
            if len > self.maxTestNameLen:
                self.maxTestNameLen = len

    def addConfig(self, config):
      self.configs.append(config)

    def close_runner(self):
      self.pendings = []
      for run in self.runnings:
        run.close()
      self.runnings = []

    def command_done(self):
      reactor.callLater(1, self.command_callback)

    def start(self, callback=None, *args, **kwargs):

      self.command_callback = callback

      for config in self.configs:
        for test in self.tests:
          test.addConfig(config)

      if self.server:
        self.uiServer = UiServer(self.configs, self.tests)
        endpoints.serverFromString(reactor, "tcp:38497").listen(self.uiServer)
      else:
        self.uiServer = None

      if callback != None:
        reactor.callWhenRunning(callback, *args, **kwargs)
        
      if not reactor.running:
        reactor.run()
        self.close_runner()
      else:
        reactor.addSystemEventTrigger('after', 'shutdown', self.close_runner)

    def check_pending_tests(self):
      if len(self.pendings) > 0 and self.check_cpu_load():
        testrun = self.pendings.pop()
        self.run(testrun)

    def testEnd(self, testrun):

      if testrun.status: testStr = bcolors.OKGREEN + 'OK: '.ljust(6) + bcolors.ENDC
      else: testStr = bcolors.FAIL + 'KO: '.ljust(6) + bcolors.ENDC
      print (testStr + bcolors.BOLD + testrun.test.getFullName().ljust(self.maxTestNameLen + 5) + bcolors.ENDC + ' %s' % (testrun.config))


      test = self.plpobjects.getTest(testrun.test.getFullName())
      testResult = plpobjects.TestRun(self.plpobjects, test, testrun.status, testrun.duration, testrun.config, testrun.log, build=self.build)

      if testrun in self.runnings:
        self.runnings.remove(testrun)

      runResult = TestRunResult(testrun.test.getFullName(), testrun.config, testrun.status, testrun.log)

      if self.uiServer is not None and self.uiServer.handler is not None:
        self.uiServer.handler.transport.write(pickle.dumps(runResult))

      testrun.check_deps()

      self.check_pending_tests()

      self.check_completion()

    def check_completion(self):
      if self.runCompletionCallback != None and len(self.pendings) == 0 and len(self.runnings) == 0:
        if self.cpu_load_checker_call_id is not None:
          self.cpu_load_checker_call_id.cancel()
          self.cpu_load_checker_call_id = None
        self.runCompletionCallback(*self.runCompletionArgs, **self.runCompletionKwargs)

    def run(self, testrun):

      self.runnings.append(testrun)

      if self.uiServer is not None and self.uiServer.handler is not None:
        self.uiServer.handler.transport.write(pickle.dumps(TestRunning(testrun.test.getFullName(), testrun.config)))
      
      print (bcolors.OKBLUE + 'START'.ljust(6) + bcolors.ENDC + bcolors.BOLD + testrun.test.getFullName().ljust(self.maxTestNameLen + 5) + bcolors.ENDC + ' %s' % (testrun.config))

      testrun.run(reactor, self.testEnd, self.commands, self.dry_run, testrun)

    def check_cpu_load(self):
      if len(self.runnings) >= self.nbThreads:
        return False

      if self.average_load is None:
        return True

      try:
        load = psutil.cpu_percent(interval=0.1)
      except:
        return True
      return load < self.average_load

    def enqueueTestRun(self, testrun):
      if not self.check_cpu_load():
        self.pendings.append(testrun)
        return

      self.run(testrun)

    def cpu_load_check(self):
      self.cpu_load_checker_call_id = reactor.callLater(1, self.cpu_load_check)

      self.check_pending_tests()


    def runTests(self, tests, callback=None, *args, **kwargs):

      self.cpu_load_checker_call_id = reactor.callLater(1, self.cpu_load_check)

      self.runCompletionCallback = callback
      self.runCompletionArgs = args
      self.runCompletionKwargs = kwargs

      for config in self.configs:
        if tests == None:
          for test in self.tests:
            test.run(config)
        else:
          for testRegExp in tests:
            for test2 in self.tests:
              for test in test2.getFromRegExp(testRegExp):
                test.run(config)

      self.check_completion()

