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

dir="gem5.TnT"
repo_url="https://github.com/tukl-msd/gem5.TnT.git"
if [[ ! -d ${dir} ]]; then
	# Clone gem5.TnT repository
	git clone ${repo_url}
fi
cd $dir
# Install known gem5 dependencies
sudo ./dep_install.sh
# Get gem5 essential repositories
./get_essential_repos.sh
# Build gem5
./build_gem5.sh
cd -
