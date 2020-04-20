#!/bin/bash

ERROR=0

#test server using integration.conf

printf "server {\n\tlisten 8010;\n\tserver_name foo.com;\n}" >integration.conf

SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

#sleep after starting command
sleep 1

#basic curl test
if  curl -s localhost:8010 | \
	tr "\n\r" " "| \
	grep "GET / HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" > /dev/null;
then
	echo "	Basic curl test success."
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
