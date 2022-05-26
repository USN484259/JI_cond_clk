#!/bin/bash
#rtcwake -m mem -t $(date +%s -d ‘tomorrow 05:55’)
#nohup rtcwake -m mem -s 60 > /dev/null &

if [ "$EUID" -eq 0 ];
then
	echo "root"
else
	echo "not root"
fi

echo $USER

rtcwake -m mem -t $(date +%s -d "$1 $2")


