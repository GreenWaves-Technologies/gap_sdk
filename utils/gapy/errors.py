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

#
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

import common


class FatalError(RuntimeError):
	"""
	Wrapper class for runtime errors that aren't caused by internal bugs.
	"""
	
	def __init__(self, message):
		RuntimeError.__init__(self, message)
	
	@staticmethod
	def WithResult(message, result):
		"""
		Return a fatal error object that appends the hex values of
		'result' as a string formatted argument.
		"""
		message += " (result was %s)" % common.hexify(result)
		return FatalError(message)


class InputError(RuntimeError):
	def __init__(self, e):
		super(InputError, self).__init__(e)


class NotSupportedError(FatalError):
	def __init__(self, gap, function_name):
		FatalError.__init__(self, "Function %s is not supported for %s." % (function_name, gap.CHIP_NAME))
