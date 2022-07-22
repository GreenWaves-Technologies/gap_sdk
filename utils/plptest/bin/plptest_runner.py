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
import sys
from twisted.internet import protocol, reactor, endpoints
from plptest_utils import *
import plpobjects
import imp
import plptest_condor
import plptest
import sys
try:
  import psutil
except:
  pass

test_runner = None

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def sigint_handler(sig, frame):
    if test_runner is not None:
        test_runner.close_runner()
        test_runner.stop()


if os.environ.get('PYTHON_LOG') != None:
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')


def getOptionValue(value):
    return os.path.expandvars(value)


def parse_testset(top_testset, testset, topParent, runner, path, rootdir):
  if testset.parent != None: parent = testset.struct
  else: parent = topParent

  testset.struct = Testset(runner, testset.name, path, parent, testset)
  if top_testset is not None:
    top_testset.append(testset.struct)

  testset.struct.set_restrict(testset.restrict)
  testset.struct.set_parallel(testset.parallel)
  testset.struct.skip = testset.skip_message
  testset.struct.exclude = testset.exclude_message

  for file in testset.files:
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
  test.struct = Test(runner, test.name, path, parent, test)
  if top_testset is not None:
    top_testset.append(test.struct)

  for cmd in test.commands:
    test.struct.addCommand(cmd)

  if test.path != None: test.struct.setDir(test.path)

  test.struct.set_restrict(test.restrict)
  test.struct.set_testcase(test.testcase)
  test.struct.scores = test.scores
  test.struct.skip = test.skip_message
  test.struct.exclude = test.exclude_message
  if test.skip_message is None and topParent is not None:
    test.struct.skip = topParent.get_skip()
  if test.exclude_message is None and topParent is not None:
    test.struct.exclude = topParent.get_exclude()

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
      module = imp.load_source(self.file, self.file)
    except:
      raise Exception(bcolors.FAIL + 'Unable to open test configuration file: ' + self.file + bcolors.ENDC)

    if module.__dict__.get('get_tests') is None:

      try:
        self.config.update(module.TestConfig)
      except:
          raise Exception(bcolors.FAIL + 'Project configuration must define the TestConfig variable: ' + self.file + bcolors.ENDC)

    else:
        testconfig = plptest.Testconfig(self.runner)
        module.get_tests(testconfig)
        self.config.update(testconfig.gen())

    top_testset = []
    result = top_testset

    testsets = self.config.get('testsets')

    if self.config.get('testplan') is not None:
      self.runner.add_testplan(self.config.get('testplan'))

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
            testset = CfgParser(self.runner, file_path)
            testset.parse(top_testset.struct)

    tests = self.config.get('tests')
    top_tests = []
    if tests != None:
      for test in tests:
        parse_test(top_testset, test, topParent, self.runner, self.path)

    return result



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
        bench_csv_file=None, bench_regexp=None, commands=None, dry_run=False,
        exclude_commands=None, properties=[], tags=[], job_id=None, nb_jobs=None,
        flags=[], skip_tests=None):

        global test_runner

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
        self.exclude_commands = exclude_commands
        self.cpu_load_checker_call_id = None
        self.dry_run = dry_run
        self.testplan = None
        self.tags = tags
        self.job_id = job_id
        self.nb_jobs = nb_jobs
        self.flags = flags
        self.skip_tests = skip_tests

        test_runner = self

        if average_load is not None:
          self.average_load = average_load * 100

        if worker_pool == 'condor':
            self.worker_pool = plptest_condor.Condor_pool()

        self.properties = {}
        for prop in properties:
          name, value = prop.split('=')
          self.properties[name] = value

        if self.properties.get('os') is None and os.environ.get('PMSIS_OS') is not None:
          self.properties['os'] = os.environ.get('PMSIS_OS')

        if self.properties.get('platform') is None and os.environ.get('PMSIS_PLATFORM') is not None:
          self.properties['platform'] = os.environ.get('PMSIS_PLATFORM')

        

    def get_property(self, name):
      return self.properties.get(name)

    def testcase_result(self, testcase, status, test_name):
        if self.testplan is not None:
            return self.testplan.testcase_result(testcase, status, test_name)
        return (True, '')

    def add_testplan(self, user_testplan):
        self.testplan = Testplan(user_testplan.name)
        for user_category in user_testplan.categories:
            category = self.testplan.add_category(user_category.name)
            for user_feature in user_category.features:
                category.add_feature(user_feature)


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

      if self.skip_tests is not None:
        for test in self.tests:
          test.skip_tests(self.skip_tests)

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

      signal.signal(signal.SIGINT, sigint_handler)

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

      if testrun.exclude is not None:
        testStr = bcolors.HEADER + 'EXCLUDE '.ljust(8) + bcolors.ENDC
      elif testrun.skip is not None:
        testStr = bcolors.WARNING + 'SKIP '.ljust(8) + bcolors.ENDC
      elif testrun.status:
        testStr = bcolors.OKGREEN + 'OK '.ljust(8) + bcolors.ENDC
      else:
        testStr = bcolors.FAIL + 'KO '.ljust(8) + bcolors.ENDC
      print (testStr + bcolors.BOLD + testrun.test.getFullName().ljust(self.maxTestNameLen + 5) + bcolors.ENDC + ' %s' % (testrun.config))
      sys.stdout.flush()

      test = self.plpobjects.getTest(testrun.test.getFullName())
      testResult = plpobjects.TestRun(self.plpobjects, test, testrun.status, testrun.duration,
          testrun.config, testrun.log, build=self.build, skip=testrun.skip, exclude=testrun.exclude
      )

      if testrun in self.runnings:
        self.runnings.remove(testrun)

      runResult = TestRunResult(testrun.test.getFullName(), testrun.config, testrun.status, testrun.log)

      if self.uiServer is not None and self.uiServer.handler is not None:
        self.uiServer.handler.transport.write(pickle.dumps(runResult))

      testrun.check_deps()

      self.check_pending_tests()

      self.check_completion()

    def check_completion(self):
      if not self.enqueue_all:
        return

      if self.runCompletionCallback != None and len(self.pendings) == 0 and len(self.runnings) == 0:
        if self.cpu_load_checker_call_id is not None:
          self.cpu_load_checker_call_id.cancel()
          self.cpu_load_checker_call_id = None
        self.runCompletionCallback(*self.runCompletionArgs, **self.runCompletionKwargs)

    def run(self, testrun):

      self.runnings.append(testrun)

      if self.uiServer is not None and self.uiServer.handler is not None:
        self.uiServer.handler.transport.write(pickle.dumps(TestRunning(testrun.test.getFullName(), testrun.config)))

      if testrun.skip is None and testrun.exclude is None:
        print (bcolors.OKBLUE + 'START'.ljust(8) + bcolors.ENDC + bcolors.BOLD + testrun.test.getFullName().ljust(self.maxTestNameLen + 5) + bcolors.ENDC + ' %s' % (testrun.config))
        sys.stdout.flush()

      testrun.run(reactor, self.testEnd, self.commands, self.exclude_commands, self.dry_run, testrun)

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
      self.pendings.append(testrun)

    def cpu_load_check(self):
      self.cpu_load_checker_call_id = reactor.callLater(1, self.cpu_load_check)

      self.check_pending_tests()

    def dump_testplan(self):
      if self.testplan is not None:
        self.testplan.dump()

    def runTests(self, tests, tests_re, callback=None, *args, **kwargs):

      self.runCompletionCallback = callback
      self.runCompletionArgs = args
      self.runCompletionKwargs = kwargs
      self.enqueue_all = False

      for config in self.configs:
        if tests is None and tests_re is None:
          for test in self.tests:
            test.run(config)
        else:
          if tests_re is not None:
            for testRegExp in tests_re:
              for test2 in self.tests:
                for test in test2.getFromRegExp(testRegExp):
                  test.run(config)

          if tests is not None:
            for name in tests:
              for test2 in self.tests:
                for test in test2.getFromName(name):
                  test.run(config)

      self.enqueue_all = True

      if self.nb_jobs > 1:
          self.pendings = self.pendings[self.job_id::self.nb_jobs]

      self.cpu_load_check()

      self.check_completion()

