
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

import os
from datetime import datetime
import re
from twisted.internet import protocol
import signal
import shlex
import plptest as p
import re
from prettytable import PrettyTable
import math

class Command(object):

    def __init__(self, cmd):
        self.name = cmd


class GetTests(Command):

    def __init__(self):
        super(GetTests, self).__init__("get tests")


class RunTests(Command):

    def __init__(self, tests=None, configs=None):
        super(RunTests, self).__init__("run tests")
        self.tests = tests
        self.configs = configs


class TestRunning(Command):

    def __init__(self, test, config):
        super(TestRunning, self).__init__("test running")
        self.test = test
        self.config = config

class TestRunResult(Command):

    def __init__(self, test, config, status, log):
        super(TestRunResult, self).__init__("test run result")
        self.status = status
        self.test = test
        self.config = config
        self.log = log


class TestCommon(object):
    def __init__(self, runner, name, path, parent):
        self.name = name
        self.parent = parent
        self.runner = runner
        self.status = {}
        self.path = path
        # A test is inactive by default meaning there is no active
        # configuration for which it must be executed. It will become active
        # when at least he is active for one config
        self.isActive = False

        # This will be populated later on when configurations are added
        self.activeForConfig = {}
        self.addedConfigs = []
        self.restrict = None

    def checkConfig(self, config):
        try:
            self.activeForConfig[config.__str__()] = \
                self.restrict is None or \
                eval(self.restrict)
        except:
            self.activeForConfig[config.__str__()] = False
        for child in self.childs:
            child.checkConfig(config)
        if self.getNbTests(config) != 0:
            self.isActive = True

    def addConfig(self, config):
        self.addedConfigs.append(config)
        self.checkConfig(config)

    def isActive(self):
        return self.isActive

    def isActiveForConfig(self, config):
        if self.parent is not None and \
                not self.parent.isActiveForConfig(config):
            return False
        return self.activeForConfig.get(config.__str__())

    def getFullName(self):
        if self.parent is None:
            return self.name
        return '%s:%s' % (self.parent.getFullName(), self.name)

    def get(self, fullName):
        nameList = fullName.split(':')
        name = nameList[0]
        if name != self.name:
            return None
        if len(nameList) == 1:
            return self
        childName = nameList[1]
        for child in self.childs:
            if child.name == childName:
                return child.get(':'.join(nameList[1:]))
        return None

    def getFromRegExp(self, regExp):

        if re.compile(regExp).search(self.getFullName()):
            return [self]

        tests = []
        for child in self.childs:
            tests += child.getFromRegExp(regExp)

        return tests

    def getNbTests(self, config):
        if not self.isActiveForConfig(config):
            return 0
        if len(self.childs) == 0:
            return 1
        else:
            nbTests = 0
            for child in self.childs:
                nbTests += child.getNbTests(config)
            return nbTests

    def getNbSuccess(self, config):
        if len(self.childs) == 0:
            return 1 if self.status.get(config) else 0
        else:
            nbTests = 0
            for child in self.childs:
                nbTests += child.getNbSuccess(config)
            return nbTests

    def walk_cb(self, cb, down=True, *args, **kwargs):
        cb(self, *args, **kwargs)
        if down:
            for child in self.childs:
                child.walk_cb(cb, down, *args, **kwargs)
        else:
            if self.parent is not None:
                self.parent.walk_cb(cb, down, *args, **kwargs)

    def set_restrict(self, restrict):
        self.restrict = restrict

    def getMaxTestNameLen(self):
        selfLen = len(self.getFullName())
        if len(self.childs) == 0:
            return selfLen
        else:
            for test in self.childs:
                childLen = test.getMaxTestNameLen()
                if childLen > selfLen:
                    selfLen = childLen
            return selfLen


class Testset(TestCommon):

    def __init__(self, runner, name, path, parent=None):
        super(Testset, self).__init__(runner, name, path, parent)
        if parent is not None:
            parent.regChild(self)
        self.childs = []
        self.parallel = True

    def set_parallel(self, parallel):
        self.parallel = parallel

    def regChild(self, child):
        self.childs.append(child)

    def add_dep(self, child, config):
        child.run(config)

    def show(self):
        for child in self.childs:
            child.show()

    def score(self, table=None, file=None):
        score = 0.0
        nb_score = 0
        for child in self.childs:
            (child_score, child_nb_score) = child.score(table=table, file=file)
            score += child_score
            nb_score += child_nb_score

        if nb_score > 0:
            score = score / nb_score

        return (score, 1)

    def run(self, config):
        if not self.isActiveForConfig(config):
            return

        if self.parallel:
            for child in self.childs:
                child.run(config)
        else:
            if len(self.childs) > 0:
                prev_child = self.childs[0]
                prev_child.run(config)

                for child in self.childs[1:]:
                    prev_child.add_dep(child, config)
                    prev_child = child



class Test(TestCommon):

    def __init__(self, runner, name, path, parent=None):
        super(Test, self).__init__(runner, name, path, parent)
        self.childs = []
        self.commands = []
        self.tags = []
        self.dir = None
        self.timeout = -1
        self.checkers = []
        self.params = []
        self.deps = []

        if parent is not None:
            parent.regChild(self)

    def getPath(self):
        path = self.path
        if self.dir is not None:
            path = os.path.join(path, self.dir)
        return path

    def add_dep(self, child, config):
        testrun = child.get_testrun(config)
        if testrun is not None:
            self.deps.append(testrun)

    def show(self):
        print (self.name)

    def score(self, table=None, file=None):

        if len(self.scores) != 0:

            is_first = True
            nb_score = 0
            total_score = 0

            #print (self.getFullName())

            for score in self.scores:

                if self.runner.bench_csv_file.get(score.name) is None:
                    raise Exception("Unknown benchmark item: " + score.name)

                value, desc = self.runner.bench_csv_file.get(score.name)
                value = float(value)

                score_value = eval(score.score)

                name = self.getFullName() if is_first else ""
                is_first = False

                table.add_row([name, score.name, desc, value, score_value])

                #print ('\t%s\t%f' % (score.name, score_value))

                total_score += score_value
                nb_score += 1

            if nb_score > 0:
                total_score = total_score / nb_score

        return (total_score, 1)

    def get_testrun(self, config):
        if not self.isActiveForConfig(config):
            return None
        return TestRun(self.runner, self, config)

    def check_deps(self):
        for dep in self.deps:
            self.runner.enqueueTestRun(dep)

    def run(self, config):
        testrun = self.get_testrun(config)
        if testrun is not None:
            self.runner.enqueueTestRun(testrun)

    def addCommand(self, command):
        self.commands.append(command)

    def addParam(self, param):
        self.params.append(param)

    def addTag(self, tag):
        self.tags.append(tag)

    def addChecker(self, checker):
        self.checkers.append(checker)

    def setDir(self, path):
        self.dir = path

    def setTimeout(self, timeout):
        self.timeout = timeout


class CheckerProcess(protocol.ProcessProtocol):

    def outReceived(self, data):
        print(data.decode('utf-8'))

    def dataReceived(self, data):
        print(data.decode('utf-8'))


class TestRun(protocol.ProcessProtocol):

    def __init__(self, runner, test, config):
        self.runner = runner
        self.test = test
        self.config = config
        self.log = ''
        self.outputLen = 0
        self.status = False
        self.reachedMaxOutputSize = False
        self.closed = False
        self.id = runner.get_test_id()

    def check_deps(self):
        self.test.check_deps()

    def appendOutput(self, data):
        if self.reachedMaxOutputSize:
            return

        if self.runner.stdout and not self.runner.safe_stdout:
            print (data, end = '', flush=True)

        dataLen = len(data)

        if self.runner.maxOutputLen == -1 or \
                self.outputLen + dataLen <= self.runner.maxOutputLen:
            self.log += data
            self.outputLen += dataLen
        else:
            self.log += data[0:self.runner.maxOutputLen - self.outputLen]
            self.outputLen = self.runner.maxOutputLen
            msg = '\nReached maximum test output length of %s bytes\n' %\
                self.runner.maxOutputLen
            if self.runner.stdout and not self.runner.safe_stdout:
                print (msg)
                self.log += msg
            self.reachedMaxOutputSize = True
            self.close(kill=True)

    def outReceived(self, data):
        try:
            self.appendOutput(data.decode('utf-8'))
        except:
            pass

    def dataReceived(self, data):
        try:
            self.appendOutput(data.decode('utf-8'))
        except:
            pass

    def getExecPath(self):
        return self.test.getPath()

    def close(self, kill=False):
        if self.closed:
            return True

        os.killpg(self.pid, signal.SIGKILL)

    def handleEnd(self):
        self.closed = True
        endTime = datetime.now()
        duration = endTime - self.startTime
        self.duration = \
            (duration.microseconds +
                (duration.seconds + duration.days * 24 * 3600) * 10**6) / 10**6

        if self.runner.bench_csv_file is not None:
            pattern = re.compile(self.runner.bench_regexp)
            for line in self.log.splitlines():
                bench = pattern.match(line)
                if bench is not None:
                    key, value = bench.group(1).split('=')
                    self.runner.bench_csv_file[key] = [value, bench.group(2)]

        if self.runner.safe_stdout:
            print (self.log)

        if self.callback is not None:
            self.callback(*self.kargs, **self.kwargs)

    def handle_cmd_end(self):
        if not self.status or len(self.commands) == 0:
            self.handleEnd()
        else:
            self.runCommand()

    def processEnded(self, reason):
        if reason.value.exitCode is None:
            self.appendOutput('Process was killed\n')
            self.status = False
        else:
            self.appendOutput('Reached EOF with exit status ' +
                              str(reason.value.exitCode) + '\n')
            self.status = reason.value.exitCode == 0
        self.handle_cmd_end()

    def reachedTimeout(self):

        if not self.closed:
            self.appendOutput('Reached timeout of %s seconds\n' %
                              self.timeout)
        self.close(kill=True)

    def timeoutToTime(self, cycles):
        nb_cores = 1
        platform = self.config.get('platform')
        convert_func = self.config.get('%s/cycles_to_seconds' %
                                       platform)

        if convert_func is not None:
            timeout = eval(convert_func)
        else:
            timeout = int(cycles)

        return timeout

    def runCommand(self):

        if self.timeout_call_id is not None:
            self.timeout_call_id.cancel()

        if len(self.commands) == 0:
            self.status = True
            self.handle_cmd_end()
            return

        cmd = self.commands.pop(0)

        if type(cmd) == p.Shell:

            cmdDict = {'config': self.config}

            if self.config.get('flag') is not None:
                pass
#               cmdDict['flags'] = ' '.join(runConfig.get('flag'))
            else:
                cmdDict['flags'] = ''

            cmd = cmd.cmd % cmdDict

            post_cmd_args = os.environ.get('PLPTEST_POST_CMD_ARGS')
            if post_cmd_args is not None:
                cmd += post_cmd_args

            if self.test.timeout != -1:
                timeout = int(self.timeoutToTime(self.test.timeout)) * 2
                if self.runner.maxTimeout != -1:
                    timeout = min(int(self.runner.maxTimeout), timeout)

                self.timeout = timeout

                self.timeout_call_id = self.reactor.callLater(timeout, self.reachedTimeout)
                self.appendOutput("Using timeout: %s seconds\n" % timeout)
#           if args.stdoutDump: logfile = sys.stdout
#           if envFile != None: cmd = '. %s && %s' % (envFile, cmd)
            self.appendOutput('Launching test with command: ' + cmd + '\n')

            if self.dry_run:
                self.status = True
                self.handle_cmd_end()
                return


            testEnv = os.environ.copy()
            testEnv['PULP_CURRENT_CONFIG'] = self.config.get_name()

            testEnv['PLPTEST_RUN_ID'] = str(self.id)
            testEnv['PLPTEST_PATH'] = self.getExecPath()
            testEnv['PLPTEST_NAME'] = self.test.getFullName()
            testEnv['PLPTEST_CONFIG'] = self.config.get_name()

            install_dir = self.runner.home
            if install_dir is None:
                install_dir = os.environ.get('GAP_SDK_HOME')
                if install_dir is not None:
                    cmd_exec = install_dir + '/install/workstation/bin/plptest_checker'
                else:
                    install_dir = os.environ.get('PULP_SDK_HOME') + \
                    '/install/ws'
                    cmd_exec = install_dir + '/bin/plptest_checker'
            else:
                cmd_exec = install_dir + '/plptest_checker'

            cmd_args = [cmd_exec] + shlex.split(cmd)

            if self.runner.get_worker_pool() is not None:
                cmd = self.runner.get_worker_pool().get_cmd(' '.join(cmd_args))
                cmd_args = shlex.split(cmd)
                cmd_exec = cmd_args[0]
                testEnv.update(self.runner.get_worker_pool().get_env())

            self.p = self.reactor.spawnProcess(
                self, executable=cmd_exec, args=cmd_args,
                path=self.getExecPath(),
                usePTY=True, env=testEnv
            )

            self.gid = os.getpgid(self.p.pid)
            self.pid = self.p.pid

        elif type(cmd) == p.Check:
            self.appendOutput('Executing checker\n')
            
            if self.dry_run:
                self.status = True
                self.handle_cmd_end()
                return

            cwd = os.getcwd()
            os.chdir(self.getExecPath())
            try:
                self.status, checker_output = cmd.checker(self.config, self.log, *cmd.kargs, **cmd.kwargs)
                if checker_output: self.appendOutput(checker_output)
            except Exception as e:
                self.appendOutput(str(e))
                self.status = False
            os.chdir(cwd)
            if not self.status:
                self.appendOutput('Checker returned wrong status\n')
            self.handle_cmd_end()

    def run(self, reactor, callback=None, commands=None, dry_run=False, *kargs, **kwargs):

        if commands is None:
            self.commands = self.test.commands.copy()
        else:
            self.commands = []
            for test_command in self.test.commands:
                if test_command.name in commands:
                    self.commands.append(test_command)

        self.dry_run = dry_run
        self.callback = callback
        self.kargs = kargs
        self.kwargs = kwargs
        self.reactor = reactor
        self.startTime = datetime.now()

        self.appendOutput('Running: ' + self.test.getFullName() + ' / ' +
                          self.config.get_config_name() + '\n')

        self.timeout_call_id = None


        self.reactor.callLater(0, self.runCommand)
