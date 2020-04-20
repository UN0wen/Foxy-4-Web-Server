#!/bin/bash

ERROR=0

#test server using integration.conf

printf "server {\n\tlisten   8000;\n}" >integration.conf

SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

#sleep after starting command
sleep 1

#basic curl test
if  curl -s localhost:8000 | \
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

#curl post test
if  curl -s -d val=0 localhost:8000 | \
	tr "\n\r" " "| \
	grep "POST / HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" | \
	grep "Content-Length:" | \
	grep "Content-Type:" | \
	grep "val=0" > /dev/null; 
then
	echo "	Curl post test success."
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
