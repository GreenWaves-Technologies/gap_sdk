#!/bin/bash

echo ${SHUTTERLESS_LIB} | wget --no-use-server-timestamps --base=`cat $SHUTTERLESS_LIB_URL` --input-file=- -O gap8_project/shutterless/libshutterless.a
if [ $? != 0 ]; then
	rm $SHUTTERLESS_LIB_URL
	rm -f gap8_project/shutterless/lib*
	exit 1
fi
