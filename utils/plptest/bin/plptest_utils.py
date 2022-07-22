
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
import plptest_bench
import psutil

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


class Testplan_feature(object):

    def __init__(self, name):
        self.name = name
        self.nb_passed = 0
        self.nb_tests = 0
        self.tests = []

    def testcase_result(self, testcase, status, test_name):
        self.nb_tests += 1
        if status:
            self.nb_passed += 1

        self.tests.append([testcase.description, status, test_name])

        return (True, '')

    def dump(self, x, category, indent):
        if self.nb_tests > 0:
            ratio = float(self.nb_passed) / self.nb_tests
        else:
            ratio = 0.0

        x.add_row(['%s%s' % (indent, self.name), self.nb_passed, self.nb_tests, ratio, '' ])

        for test in self.tests:
            x.add_row(['%s%s' % (indent + '    ', test[2]), 1 if test[1] else 0, 1, float(1 if test[1] else 0), test[0]])

class Testplan_category(object):

    def __init__(self, name, full_name):
        self.name = name
        self.features = {}
        self.nb_passed = 0
        self.nb_tests = 0
        self.categories = {}
        self.full_name = full_name

    def add_category(self, name):
        name_list = name.split(':')

        if self.categories.get(name_list[0]) is None:
            self.categories[name_list[0]] = Testplan_category(name_list[0], '%s:%s' % (self.full_name, name_list[0]))

        if len(name_list) != 1:
            self.categories[name_list[0]].add_category(':'.join(name_list[1:]))

        return self.categories[name_list[0]]

    def add_feature(self, name):
        name_list = name.split(':')
        if len(name_list) == 1:
            self.features[name] = Testplan_feature(name)
        else:
            self.categories.get(name_list[0]).add_feature(':'.join(name_list[1:]))

    def testcase_result(self, testcase, status, name, test_name):
        self.nb_tests += 1
        if status:
            self.nb_passed += 1

        if name is not None:
            name_list = name.split(':')
            if len(name_list) == 1:
                subname = None
            else:
                subname = '/'.join(name_list[1:])

            if self.categories.get(name_list[0]) is None:
                return (False, "Unknown testplan category (category: %s)\n" % name_list[0])

            return self.categories.get(name_list[0]).testcase_result(testcase, status, subname, test_name)

        else:
            for feature in testcase.features:
                if self.features.get(feature) is None:
                    return (False, "Unknown testplan feature (category: %s, features: %s)\n" % (testcase.category, ' '.join(testcase.features)))

                (error, msg) = self.features.get(feature).testcase_result(testcase, status, test_name)
                if not error:
                    return (error, msg)

            return (True, '')

    def dump(self, x, indent):
        if self.nb_tests > 0:
            ratio = float(self.nb_passed) / self.nb_tests
        else:
            ratio = 0.0
        x.add_row(['%s%s' % (indent, self.name), self.nb_passed, self.nb_tests, ratio, '' ])

        for name, feature in self.features.items():
            feature.dump(x, self.full_name, indent + '    ')

        for name, category in self.categories.items():
            category.dump(x, indent + '    ')




class Testplan(object):

    def __init__(self, name):
        self.name = name
        self.categories = {}

    def testcase_result(self, testcase, status, test_name):

        name = testcase.category
        name_list = name.split(':')
        if len(name_list) == 1:
            subname = None
        else:
            subname = '/'.join(name_list[1:])

        if self.categories.get(name_list[0]) is None:
            return (False, "Unknown testplan category (category: %s)\n" % name_list[0])

        return self.categories.get(name_list[0]).testcase_result(testcase, status, subname, test_name)

    def add_category(self, name):

        name_list = name.split(':')

        if self.categories.get(name_list[0]) is None:
            self.categories[name_list[0]] = Testplan_category(name_list[0], name_list[0])

        if len(name_list) != 1:
            return self.categories[name_list[0]].add_category(':'.join(name_list[1:]))

        return self.categories[name_list[0]]

    def dump(self):
        x = PrettyTable(['Test', 'Passed', 'Total', 'Ratio passed', 'Description'])
        x.align = "l"
        for name, category in self.categories.items():
            category.dump(x, '')

        print (x)



class TestCommon(object):
    def __init__(self, runner, name, path, parent, user, is_testset=False):
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
        self.skip = None
        self.exclude = None
        self.user = user
        self.is_testset = is_testset

    def get_skip(self):
        if self.skip is not None:
            return self.skip

        if self.parent is not None:
            return self.parent.get_skip()

        return None

    def get_exclude(self):
        if self.exclude is not None:
            return self.exclude

        if self.parent is not None:
            return self.parent.get_exclude()

        return None

    def checkConfig(self, config):

        if not self.is_testset and len(self.runner.tags) != 0 and self.user is not None:
            self.isActive = len(set(self.runner.tags).intersection(self.user.tags)) != 0
            if not self.isActive:
                return

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

    def getFromName(self, name):

        if name == self.getFullName():
            return [self]

        tests = []
        for child in self.childs:
            tests += child.getFromName(name)

        return tests

    def getNbTests(self, config):
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

    def __init__(self, runner, name, path, parent=None, user=None):
        super(Testset, self).__init__(runner, name, path, parent, user, is_testset=True)
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

    def skip_tests(self, tests):

        name = self.getFullName()

        for test in tests:
            if name == test:
                self.skip = "Skipped from command line"

        for child in self.childs:
            child.skip_tests(tests)

    def score(self, table=None, file=None):
        error = False
        score = 0.0
        nb_score = 0
        for child in self.childs:
            (child_error, child_score, child_nb_score) = child.score(table=table, file=file)

            error = error or child_error

            if child_nb_score == 0:
                continue

            score += child_score
            nb_score += child_nb_score

        if nb_score > 0:
            score = score / nb_score

            plot = plptest_bench.Jenkins_plot(self.getFullName() + '.score.csv')
            plot.append('score', str(score))
            plot.gen()

            return (error, score, 1)

        else:

            return (error, None, 0)

    def run(self, config):
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

    def __init__(self, runner, name, path, parent=None, user=None):
        super(Test, self).__init__(runner, name, path, parent, user)
        self.childs = []
        self.commands = []
        self.dir = None
        self.timeout = -1
        self.checkers = []
        self.params = []
        self.deps = []
        self.testcase = None

        if parent is not None:
            parent.regChild(self)

    def skip_tests(self, tests):
        name = self.getFullName()

        for test in tests:
            if name == test:
                self.skip = "Skipped from command line"

    def set_testcase(self, testcase):
        self.testcase = testcase

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

        if self.runner.bench_csv_file is None:
            return (False, None, 0)

        error = False

        total_score = 0

        if len(self.scores) != 0:

            is_first = True
            nb_score = 0

            #print (self.getFullName())

            for score in self.scores:

                if self.runner.bench_csv_file.get(score.name) is None:
                    value = 0
                    score_value = 0.0
                else:
                    value, desc = self.runner.bench_csv_file.get(score.name)
                    value = float(value)

                    try:
                        score_value = eval(score.score)
                    except:
                        score_value = 0.0

                    name = self.getFullName() if is_first else ""
                    is_first = False

                    if table is not None:
                        table.add_row([name, score.name, desc, value, score_value])

                #print ('\t%s\t%f' % (score.name, score_value))

                if score.checker is not None:
                    score.value = value
                    score.score = score_value
                    error = error or score.checker(score)

                total_score += score_value
                nb_score += 1

            if nb_score > 0:
                total_score = total_score / nb_score

            plot = plptest_bench.Jenkins_plot(self.getFullName() + '.score.csv')
            plot.append('score', str(total_score))
            plot.gen()


            return (error, total_score, 1)

        else:

            return (error, None, 0)

    def get_testrun(self, config):
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
        self.skip = None
        self.exclude = None
        self.reachedMaxOutputSize = False
        self.closed = False
        self.id = runner.get_test_id()
        self.skip = self.test.skip
        self.exclude = self.test.exclude

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

        try:
            process = psutil.Process(pid=self.pid)
    
            for children in process.children(recursive=True):
                os.kill(children.pid, signal.SIGKILL)
        except:
            pass

    def terminate(self):
        if self.callback is not None:
            self.callback(*self.kargs, **self.kwargs)


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

        if self.test.testcase is not None:
            status, msg = self.runner.testcase_result(self.test.testcase, self.status, self.test.name)
            if not status:
                self.appendOutput(msg)
                self.status = False

        if self.runner.safe_stdout:
            print (self.log)

        self.terminate()

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
        return int(cycles)
        # nb_cores = 1
        # platform = self.config.get('platform')
        # convert_func = self.config.get('%s/cycles_to_seconds' %
        #                                platform)

        # if convert_func is not None:
        #     timeout = eval(convert_func)
        # else:
        #     timeout = int(cycles)

        # return timeout

    def runCommand(self):

        if self.timeout_call_id is not None:
            self.timeout_call_id.cancel()
            self.timeout_call_id = None

        if len(self.commands) == 0:
            self.status = True
            self.handle_cmd_end()
            return

        cmd = self.commands.pop(0)

        if type(cmd) == p.Shell:

            cmdDict = {'config': self.config}

#             if self.config.get('flag') is not None:
#                 pass
# #               cmdDict['flags'] = ' '.join(runConfig.get('flag'))
#             else:
            
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
            testEnv['PULP_CURRENT_CONFIG'] = self.config

            testEnv['PLPTEST_RUN_ID'] = str(self.id)
            testEnv['PLPTEST_PATH'] = self.getExecPath()
            testEnv['PLPTEST_NAME'] = self.test.getFullName()
            testEnv['PLPTEST_CONFIG'] = self.config

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

        elif type(cmd) == p.Check_score:
            self.appendOutput('Executing score checker\n')

            (error, score, nb_scores) = self.test.score()

            self.status = not error

            self.handle_cmd_end()
            
    def run(self, reactor, callback=None, commands=None, exclude_commands=None, dry_run=False, *kargs, **kwargs):

        self.dry_run = dry_run
        self.callback = callback
        self.kargs = kargs
        self.kwargs = kwargs
        self.reactor = reactor
        self.startTime = datetime.now()

        if self.skip is not None or self.exclude is not None:

            self.duration = 0
            self.terminate()

        else:

            self.commands = []

            for test_command in self.test.commands:

                if (commands is None or test_command.name in commands) and (exclude_commands is None or not test_command.name in exclude_commands):
                    self.commands.append(test_command)

            self.appendOutput('Running: ' + self.test.getFullName() + ' / ' +
                            self.config + '\n')

            self.timeout_call_id = None


            self.reactor.callLater(0, self.runCommand)
