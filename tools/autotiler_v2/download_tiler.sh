#!/bin/bash

echo ${TILER_LIB} | wget --no-use-server-timestamps --base=`cat $TILER_URL` --input-file=- -O lib/libtile.a
if [ $? != 0 ]; then
	rm $TILER_URL
	rm -f lib/libtile.a
	exit 1
fi
