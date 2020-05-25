#!/bin/bash

ERROR=0

#test server using integration.conf

printf "listen 8000;\nlocation \"/static/\" StaticHandler {\n\troot \"./data/\";\n} \n location \"/echo/\" EchoHandler{\n}" >integration.conf


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
	echo "	Basic echo curl test fail."
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
	echo "	Curl post test fail."
	ERROR=1
fi

# basic bad command test
if 	echo "test" | \
	nc -w 1 localhost 8000 | \
	tr "\n\r" " "| \
	grep "HTTP/1.1 400 Bad Request" | \
	grep "Content-Length: 89" | \
	grep "Content-Type: text/html" | \
	grep "<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>" > /dev/null;
then
	echo "Bad request test success."
else 
	echo "Bad request test fail."
	ERROR=1
fi

#multithread test

#connection that stays open
printf printf "GET / HTTP/1.1\r\n" | nc localhost 8000&

sleep 0.5

#repeat basic curl test, should stall if no multithread
if  timeout 1 curl -s localhost:8000/echo | \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" > /dev/null;
then
	echo "	Basic echo curl test success."
else
	echo "	Basic echo curl test fail."
	ERROR=1
fi

#kill the incomplete nc
pkill nc

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


