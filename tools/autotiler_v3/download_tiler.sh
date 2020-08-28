#!/bin/bash

if [ -z "$TILER_LICENSE_AGREED" ]
then
	echo "Please read carefully the autotiler agreement before proceeding with the download."
	read -n 1 -s -r -p "Press enter when you are ready to display the license."

	more LICENSE

	while [ true ] ; do
	read -n 1 -p "Do you agree with the license (y/n) ? " key

	echo

	if [ "$key" == "y" ]
	then
	break
	fi

	if [ "$key" == "n" ]
	then
	exit 0
	fi

	done
fi

echo ${TILER_LIB} | wget --no-use-server-timestamps --base=`cat $TILER_URL` --input-file=- -O Autotiler/LibTile.a
if [ $? != 0 ]; then
	rm $TILER_URL
	rm -f Autotiler/LibTile.a
	exit 1
fi
