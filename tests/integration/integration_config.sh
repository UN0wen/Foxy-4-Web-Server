#!/bin/bash

ERROR=0

#test server using integration.conf

printf "server {\n\tlisten   8000;\n" >integration.conf
SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

pid=$!
while ps | grep "${pid}"
do
    echo ""
done
wait $pid
status=$?
if [ $status -eq 0 ]
then
	echo "	Invalid config test success."
else
    ERROR=1
fi
#cleanup
pkill server
rm integration.conf

printf "server {\n\tlisten   800;\n}" >integration.conf
SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

pid=$!
while ps | grep "${pid}"
do
    echo ""
done
wait $pid
status=$?
if [ $status -eq 0 ]
then
	echo "	Invalid config test success."
else
    ERROR=1
fi
#cleanup
pkill server
rm integration.conf

if [ $ERROR -eq 0 ]
then 
	echo "All tests finished and passed."
	exit 0
else 
	echo "All tests finished with some failed."
	exit 1
fi
