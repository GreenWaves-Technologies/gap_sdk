
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

  def __init__(self, name, files=[], tests=[], testsets=[], parent=None, restrict=None, tags=[], description=None, parallel=True):
    self.name = name
    self.files = files
    self.parent = parent
    self.restrict = restrict
    self.tags = tags
    self.description = description
    self.parallel = parallel
    self.tests = tests
    self.testsets = testsets



class Test(object):

    def __init__(self, name, commands=[], timeout=-1, parent=None, path=None, restrict=None, tags=[], params=[], description=None, scores=[]):
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


class Score(object):

  def __init__(self, name=None, avg=None, score=None):
    self.avg = avg
    self.name = name
    self.score = score

