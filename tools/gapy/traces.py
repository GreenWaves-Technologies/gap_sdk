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
		