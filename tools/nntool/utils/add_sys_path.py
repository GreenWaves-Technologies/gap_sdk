import sys
import os


def add_sys_path(new_path):

    if not os.path.exists(new_path):
        return False

    new_path = os.path.abspath(new_path)
    for x in sys.path:
        x = os.path.abspath(x)
        if new_path in (x, x + os.sep):
            return True
    sys.path.append(new_path)
    return True
