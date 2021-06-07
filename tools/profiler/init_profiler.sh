##!/bin/bash
set -e

fifo_name="all.bin"
exe_name="${exe_name:-GCC_RISCV/test}"
build_dir="${build_dir:-BUILD}"

if [ "$#" -gt 2 ]
then
  echo "Error: you must provide at most : "\
      " the directory containing the makefile of your project and the profiling"\
      " frequency"
  exit 1
fi

#default profiling frequency: 1 MHz
frequency=1000000
if [ "$#" -eq 2 ]
then
  frequency=$2
fi

if ! [ -d $1 ]
then
  if [ $(basename $1) == "Makefile" ]
  then
    set -- $(dirname $1)
  else
    echo "Error: directory '"$1"' does not exist"
    exit 1
  fi
fi

# WARNING DO NOT use -j option of make
# for some very well written makefiles in sdk/examples it does not work

make -C $1 all platform=gvsoc
if ! [ -d $1/$build_dir ]
then
 echo "Error: wrong build dir specified "
 exit 1
fi

full_exe_name=$(readlink --canonicalize \
    $(find $1/$build_dir -name $(basename $exe_name) | grep -m 1 $exe_name))
if [ $full_exe_name = "" ]
then
  echo "Error: cannot find executable file after build" \
    "Make sure variable $exe_name is set correctly"
  exit 1
fi

echo " *********************** exe name"
echo $full_exe_name
full_fifo_name=$(readlink --canonicalize $(dirname $full_exe_name)/$fifo_name)
echo " *********************** fifo name"
echo $full_fifo_name
if ! [ -e $full_fifo_name ]
then
  mkfifo $full_fifo_name
else#
  rm $full_fifo_name
  mkfifo $full_fifo_name
fi



make -C $1 run platform=gvsoc runner_args="--vcd --event=.*@$fifo_name \
  --event-format=raw" \
 || (echo "Build or run failed, did you install and source the sdk correctly?" \
      && exit 1) &

#echo $full_fifo_name
#echo $full_exe_name $frequency
#(cd $(dirname $0) && gui/build/profiler $full_fifo_name $full_exe_name $frequency)
