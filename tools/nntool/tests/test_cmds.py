import tempfile
from interpreter.nntool_shell import NNToolShell

def test_cmd1():
    with tempfile.TemporaryDirectory() as temp_directory:
        shell = NNToolShell({}, temp_directory, startup_script="tests/cmd_scripts/test1.txt")
        shell.cmdloop()
