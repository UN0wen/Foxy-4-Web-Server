#!/bin/bash

ERROR=0

#test server using integration.conf

printf "listen https 8000;\nlisten http 8008;\nhostname \"localhost:8000\";\nlocation \"/static/\" StaticHandler {\n\troot \"./data/\";\n} \n location \"/echo/\" EchoHandler{\n}" >integration.conf


SERVER_EXECUTABLE=$1
$SERVER_EXECUTABLE integration.conf &

#sleep after starting command
sleep 1

###############
# HTTPS Tests #
###############

#basic curl test
if  curl -k -s https://localhost:8000/echo| \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" > /dev/null;
then
	echo "Basic HTTPS echo curl test success."
else
	echo "Basic HTTPS echo curl test fail."
	ERROR=1
fi

#curl post test
if  curl -k -s -d val=0 https://localhost:8000/echo | \
	tr "\n\r" " "| \
	grep "POST /echo HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" | \
	grep "Content-Length:" | \
	grep "Content-Type:" | \
	grep "val=0" > /dev/null; 
then
	echo "Curl HTTPS post test success."
else
	echo "Curl HTTPS post test fail."
	ERROR=1
fi

# basic bad command test
if 	echo "test" | \
	ncat --ssl -w 1 localhost 8000 | \
	tr "\n\r" " "| \
	grep "HTTP/1.1 400 Bad Request" | \
	grep "Content-Length: 89" | \
	grep "Content-Type: text/html" | \
	grep "<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>" > /dev/null;
then
	echo "HTTPS Bad request test success."
else 
	echo "HTTPS Bad request test fail."
	ERROR=1
fi

##############
# HTTP Tests #
##############

#basic curl test
if  curl -k -s -L localhost:8008/echo -H 'Connection: close' | \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.0" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:";
then
	echo "Basic HTTP echo curl test success."
else
	echo "Basic HTTP echo curl test fail."
	ERROR=1
fi

# basic bad command test
if 	echo "test" | \
	nc -w 1 localhost 8008 | \
	tr "\n\r" " "| \
	grep "HTTP/1.1 400 Bad Request" | \
	grep "Content-Length: 89" | \
	grep "Content-Type: text/html" | \
	grep "<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>" > /dev/null;
then
	echo "HTTP Bad request test success."
else 
	echo "HTTP Bad request test fail."
	ERROR=1
fi

#multithread tests

#connection that stays open
printf "GET / HTTP/1.1\r\n" | ncat --ssl localhost 8000&

sleep 0.5

#repeat basic curl test, should stall if no multithread
if  timeout 1 curl -k -s https://localhost:8000/echo -H 'Connection: close'| \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.1" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:" > /dev/null;
then
	echo "Basic HTTPS echo curl test success."
else
	echo "Basic HTTPS echo curl test fail."
	ERROR=1
fi

#kill the incomplete ncat
pkill ncat

#connection that stays open
printf "GET / HTTP/1.1\r\n" | nc localhost 8008&

sleep 0.5

#basic curl test
if  curl -k -s -L localhost:8008/echo -H 'Connection: close' | \
	tr "\n\r" " "| \
	grep "GET /echo HTTP/1.0" | \
	grep "Host:" | \
	grep "User-Agent:" | \
	grep "Accept:";
then
	echo "Basic HTTP echo curl test success."
else
	echo "Basic HTTP echo curl test fail."
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

#proxy test
printf "listen 8080;\nhostname \"localhost:8000\";\nlocation \"/static/\" StaticHandler {\n\troot \"./data/\";\n} \nlocation \"/echo/\" EchoHandler{\n} \nlocation \"/prox/\" ProxyHandler {\nhost \"localhost\";\nport 8000;\n} \n " >prox_integration.conf
printf "listen 8000;\nhostname \"localhost:8000\";\nlocation \"/static/\" StaticHandler {\n\troot \"./data/\";\n} \nlocation \"/echo/\" EchoHandler{\n}" >integration.conf
$SERVER_EXECUTABLE integration.conf &
SERVER_PID=$!
$SERVER_EXECUTABLE prox_integration.conf &
SERVER_PID1=$! 

sleep 1

curl -k https://localhost:8080/prox/echo/ -i -o proxy_output.txt -s  -H 'Connection: close'
curl -k https://localhost:8000/echo/ -i -o echo_output.txt -s  -H 'Connection: close'

# kill -9 $SERVER_PID

diff --strip-trailing-cr <(head -n 3 $"echo_output.txt") <(head -n 3 $"proxy_output.txt")
if [[ $? -eq 0 ]]; then
    echo "SUCCESS"
    #kill -9 $SERVER_PID1
else
    echo "FAIL"
    #kill -9 $SERVER_PID1
    ERROR=1
fi

pkill server

# printf "HTTP/1.1 200 OK \
# Content-Length: 83 \
# Content-Type: text/plain \
# \
# GET /echo/ HTTP/1.1 \
# Accept: */* \
# Host: localhost:8080 \
# User-Agent: curl/7.58.0 \
# " > ans

# if  curl localhost:8080/echo/ -i -o test_output -s | \
# 	tr "\n\r" " "| \
# 	grep "Content-Type:"  > /dev/null;
# then
# 	echo "	Proxy curl test success."
# 	cat test_output > output.txt
# else
# 	echo "	Proxy echo curl test fail."
# 	cat test_output > output.txt
# 	ERROR=1
# fi

#kill -9 $SERVER_PID

if [ $ERROR -eq 0 ]
then 
	echo "All tests finished and passed."
	exit 0
else 
	echo "All tests finished with some failed."
	exit 1
fi


