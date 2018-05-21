#!/bin/sh

echo ""
echo "---------------------------------GVSOC_SIMULATION------------------------------------"
echo ""
plpflags gen  --output-dir=`pwd`/BUILD/GAP8/GCC_RISCV --makefile=`pwd`/BUILD/GAP8/GCC_RISCV/config.mk --app=test --out-config=`pwd`/BUILD/GAP8/GCC_RISCV/config.json

plpflags genlink  --output-dir=`pwd`/BUILD/GAP8/GCC_RISCV  --app=test

pulp-run --platform=gvsoc --dir=`pwd`/BUILD/GAP8/GCC_RISCV --config-file=`pwd`/BUILD/GAP8/GCC_RISCV/config.json --pdb-no-break --load-binary=test:-1 --boot-binary=$PULP_SDK_HOME/install/bin/boot-gap prepare

pulp-run --platform=gvsoc --dir=`pwd`/BUILD/GAP8/GCC_RISCV --config-file=`pwd`/BUILD/GAP8/GCC_RISCV/config.json --pdb-no-break --load-binary=test:-1 --boot-binary=$PULP_SDK_HOME/install/bin/boot-gap

# If you want to save trace add this
#--iss-trace
