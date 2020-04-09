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
