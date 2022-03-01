
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


class Testset(object):

    def __init__(self, name, files=None, tests=None, testsets=None, parent=None, restrict=None, tags=None, description=None, parallel=True, skip=None):
        if testsets is None:
            testsets = []
        if tests is None:
            tests = []
        if files is None:
            files = []
        if tags is None:
            tags = []

        self.name = name
        self.files = files
        self.parent = parent
        self.restrict = restrict
        self.tags = tags
        self.description = description
        self.parallel = parallel
        self.tests = tests
        self.testsets = testsets
        self.skip = skip



class Test(object):

    def __init__(self, name, commands=None, timeout=-1, parent=None, path=None, restrict=None, tags=None, params=None, description=None, scores=None, skip=None, testcase=None):

        if tags is None:
            tags = []
        if params is None:
            params = []
        if scores is None:
            scores = []
        if commands is None:
            commands = []

        self.name = name
        self.commands = commands
        self.timeout = timeout
        self.parent = parent
        self.path = path
        self.restrict = restrict
        self.tags = tags.copy()
        self.params = params
        self.description = description
        self.scores = scores
        self.skip = skip
        self.testcase = testcase

    def add_tags(self, tags):
        self.tags += tags

    def skip_test(self, message):
        self.skip = message

    def add_testcase(self, testcase):
        self.testcase = testcase

class Sdk_test(Test):

    def __init__(self, name, flags='', commands=None, timeout=1000000, parent=None, path=None, restrict=None, tags=None, params=None, description=None, scores=None, skip=None, testcase=None, checker=None, gen=None, check=None):

        if params is None:
            params = []
        if scores is None:
            scores = []
        if commands is None:
            commands = []

        if len(commands) == 0:

          build_dir = name.replace(':', '_')

          commands = [
            Shell('clean', 'make clean %s build_dir_ext=_%s' % (flags, build_dir)),
          ]

          if gen is not None:
            commands.append(Shell('gen', 'make %s %s build_dir_ext=_%s' % (gen, flags, build_dir)))

          commands += [
            Shell('build', 'make build image %s build_dir_ext=_%s' % (flags, build_dir)),
            Shell('run',   'make flash_noforce run %s build_dir_ext=_%s' % (flags, build_dir))
          ]

          if check is not None:
            commands.append(Shell('check', 'make %s %s build_dir_ext=_%s' % (check, flags, build_dir)))

          if checker is not None:
            commands.append(Check('check', checker))

        super(Sdk_test, self).__init__(name=name, commands=commands, timeout=timeout, parent=parent, path=path, restrict=restrict, tags=tags,params=params, description=description, scores=scores, skip=skip, testcase=testcase)


class Shell(object):

  def __init__(self, name, cmd):
    self.name = name
    self.cmd = cmd


class Check(object):

  def __init__(self, name, checker, *kargs, **kwargs):
    self.name = name
    self.checker = checker
    self.kargs = kargs
    self.kwargs = kwargs

class Check_score(object):

  def __init__(self, name):
    self.name = name


class Score(object):

  def __init__(self, name=None, avg=None, score=None, checker=None):
    self.avg = avg
    self.name = name
    self.score = score
    self.checker = checker


class Testcase(object):

  def __init__(self, description=None, category=None, features=[]):
    self.description = description
    self.category = category
    self.features = features


class Testplan_category(object):
  def __init__(self, name):
    self.name = name
    self.features = []

  def add_feature(self, name):
    self.features.append(name)


class Testplan(object):

  def __init__(self, name):
    self.name = name
    self.categories = []

  def add_category(self, name):
    category = Testplan_category(name)
    self.categories.append(category)
    return category

class Testconfig(object):

  def __init__(self, runner):
    self.config = {}
    self.config['tests'] = []
    self.config['testsets'] = []
    self.runner = runner
    self.tests = {}
    self.testsets = {}

  def add_test(self, test):
    self.config['tests'].append(test)
    self.tests[test.name] = test

  def add_testset(self, testset):
    self.config['testsets'].append(testset)
    self.testsets[testset.name] = testset

  def gen(self):
    return self.config

  def get(self, name):
    return self.runner.get_property(name)

  def get_test(self, name):
    test = self.tests.get(name)
    if test is None:
        return Test('')
    else:
        return test

  def get_tests(self):
    return self.config['tests'] 

  def add_tag(self, tag, tests):
      for name in tests:
          self.get_test(name).add_tags([tag])
