import os
import SCons.Util
import re


def get_files(directory, include=None, exclude=None):
  result = []

  if include is not None:
    include = re.compile(include)

  if exclude is not None:
    exclude = re.compile(exclude)

  for root, dirs, files in os.walk(directory):
      for name in files:
        path = os.path.join(root, name)
        if (include is None or include.search(path)) and (exclude is None or not exclude.search(path)):
          result.append(path)

  return result


files = []
files += get_files('configs', include='.*\.json$')
files += get_files('configs', include='.*\.rst$')
files += get_files('python', include='.*\.py$')
files += get_files('bin', exclude='__pycache__')

install_dir = os.environ.get('INSTALL_DIR')

targets = []

for file in files:
  targets.append(InstallAs(os.path.join(install_dir, file), source = [file]))

Default(targets)
