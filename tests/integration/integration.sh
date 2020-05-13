#!/bin/bash

ERROR=0

#test server using integration.conf

printf "listen 8000; \nlocation \"/static/\" StaticHandler {\n\troot \"./data/\";\n} \n location \"/echo\" EchoHandler{\n}" >integration.conf

SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

#sleep after starting command
sleep 1

#basic curl test
if  curl -s localhost:8000/echo | \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" > /dev/null;
then
	echo "	Basic echo curl test success."
else
	ERROR=1
fi

#curl post test
if  curl -s -d val=0 localhost:8000/echo | \
	tr "\n\r" " "| \
	grep "POST /echo HTTP/1.1" | \
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

# basic bad command test
if 	echo "test" | \
	nc -w 1 localhost 8000 | \
	tr "\n\r" " "| \
	grep "HTTP/1.1 400 Bad Request" | \
	grep "Content-Length: 5" | \
	grep "content-type: text/plain" | \
	grep "test" > /dev/null;
then
	echo "Bad request test success."
else 
	ERROR=1
fi

#cleanup
pkill server
rm integration.conf

#invalid config file with no port number
printf "" >invalid.conf

$SERVER_EXECUTABLE invalid.conf

if [ $? -eq 1 ]
then
	echo "Invalid config file test success."
else
	ERROR=1
	pkill server
	echo "Invalid config file test failed."
fi

#missing arguments 
$SERVER_EXECUTABLE

if [ $? -eq 1 ]
then
	echo "Missing arguments test success."
else
	ERROR=1
	pkill server
	echo "Missing arguments test failed."
fi

if [ $ERROR -eq 0 ]
then 
	echo "All tests finished and passed."
	exit 0
else 
	echo "All tests finished with some failed."
	exit 1
fi