#!/usr/bin/env bash

# Copyright (C) 2018 TU Kaiserslautern
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

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)"
DEST=${HOME}
SYSC=systemc-2.3.1a
SYSCTARBALL=$SYSC.tar.gz
BUILDDIR=objdir

getnumprocs() {
	local __retvar=$1;
	local __nprocs=$(cat /proc/cpuinfo | grep processor | wc -l)
	if [[ "$__retvar" ]]; then
		eval $__retvar="'$__nprocs'"
	else
		echo "$__nprocs"
	fi
}

# Create the destination folder and download the code into it
mkdir -p $DEST
cd $DEST
wget -nc http://accellera.org/images/downloads/standards/systemc/$SYSCTARBALL
tar -xaf $SYSCTARBALL
rm $SYSCTARBALL
# Add files to a git repository to keep track of local changes
cd $SYSC
git init
git add .
git commit -m "Adding files to repository"
# Patch to suppress some deprecated warnings
patch -p1 < ${DIR}/systemc-2.3.1a.patch
# Build the library
mkdir $BUILDDIR
cd $BUILDDIR
# Patch needed for building systemc-2.3.1a
if [[ $SYSC == 'systemc-2.3.1a' ]]; then
	printf "Patching $SYSC\n"
	SEDFILE="../src/systemc.h"
	sed -i "s@using\ std::gets;@//using\ std::gets;@g" $SEDFILE
fi
export SYSTEMC_HOME=$DEST/$SYSC
../configure $CFGOPTS
getnumprocs NP
NJ=`expr $NP - 1`
make -j$NJ
make check -j$NJ
make install
# Add env. variables to ~/.bashrc
echo "# SystemC home" >> ~/.bashrc
echo "export SYSTEMC_HOME=$DEST/$SYSC" >> ~/.bashrc
echo "# Configure gvsoc virtual platform to use systemC" >> ~/.bashrc
echo "export VP_USE_SYSTEMC=1" >> ~/.bashrc
# Reload configuration file
. ~/.bashrc
echo "Done."
