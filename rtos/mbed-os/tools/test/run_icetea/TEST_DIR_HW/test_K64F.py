"""
Copyright 2018 ARM Limited
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

from icetea_lib.bench import Bench


class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_K64F_only",
                       title="Test a test case which have only K64F support",
                       status="released",
                       purpose="Just for testing scripts",
                       component=[],
                       type="smoke",
                       requirements={
                           "duts": {
                               '*': {
                                   "count": 1,
                                   "type": "hardware",
                                   "allowed_platforms": ['K64F'],
                                   "application": {
                                       "name": "TEST_APPS-device-exampleapp"
                                   }
                               }
                           }
                       }
                       )

    def setup(self):
        pass

    def case(self):
        pass

    def teardown(self):
        pass
