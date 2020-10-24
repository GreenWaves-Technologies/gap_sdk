#!/usr/bin/env bash
set -e

output_file=benchmark_results.txt

echo "" > $output_file
if [ "$#" -ne 1 ]
then
  echo "Usage: ./benchmark.sh <directory of the makefile of your project>"
  exit 1
fi

for ((freq = 40000 ; freq <= 1000000000 ; freq *= 2 ))
do
  echo "Current run @ " $freq
  echo "Run @ " $freq >>$output_file
  /usr/bin/time -v bash -c "$(pwd)/run_profiler.sh $1 $freq \
    | grep ' Got last packet' -m 1" 1>/dev/null 2>>$output_file

  echo -e "\n\n" >> $output_file
done

echo "Done! All results have been saved in " $output_file
