#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import sys

verbose = False
def newLine():
	info("")

def critical(msg):
	""" Print critical message to stderr """
	sys.stderr.write(msg)
	sys.stderr.write('\n')

def info(msg):
	infoWithoutNewLine(msg + '\n')

def infoWithoutNewLine(msg):
	if verbose:
		sys.stdout.write(msg)
		