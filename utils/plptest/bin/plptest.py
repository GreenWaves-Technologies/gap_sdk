
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

  def __init__(self, name, files=[], tests=[], testsets=[], parent=None, restrict=None, tags=[], description=None, parallel=True, skip=None):
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

    def __init__(self, name, commands=[], timeout=-1, parent=None, path=None, restrict=None, tags=[], params=[], description=None, scores=[], skip=None, testcase=None):
        self.name = name
        self.commands = commands
        self.timeout = timeout
        self.parent = parent
        self.path = path
        self.restrict = restrict
        self.tags = tags
        self.params = params
        self.description = description
        self.scores = scores
        self.skip = skip
        self.testcase = testcase

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
