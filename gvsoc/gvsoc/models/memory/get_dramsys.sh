#!/usr/bin/env bash

# Copyright (C) 2019 TU Kaiserslautern
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

# Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)

dir="dram.sys"
repo_url="https://git.eit.uni-kl.de/ems/astdm/dram.sys.git"
if [[ ! -d ${dir} ]]; then
	# Clone dram.sys repository and its submodules
	git clone --recursive ${repo_url}
fi
cd $dir/utils
# Install known dependencies
# Notes:
#  - Script supports Debian/Ubuntu
#  - Script uses sudo
./install_deb.sh
# Get qwt-6.1 source code and build lib
./getqwt.sh
cd -
