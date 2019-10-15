#! /bin/bash

if [[ $# -lt 3 ]]
then
    echo "Needs 3 args !"
    echo "regmap.sh xls_file ip_name filename"
    exit -1
fi

XLS="$1"
NAME="$2"
HEADER="$(basename "$3")"
DEST="$( cd "$(dirname "$3")" ; pwd -P )"

./regmap --input-xls $XLS --name $NAME --header $HEADER $@
mv $HEADER $DEST > 2 >& /dev/null
