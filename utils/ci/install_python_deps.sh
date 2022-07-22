#!/usr/bin/env bash

set -euo pipefail

# install all Python dependencies required by build, tools, tests and examples

pip install -r requirements.txt
pip install -r nn_menu/requirements.txt
pip install -r tools/nntool/requirements.txt
pip install -r utils/gapy/requirements.txt
pip install -r utils/plptest/requirements.txt
pip install -r doc/requirements.txt

