#!/bin/bash

if [[ $# != 4 ]]; then
	echo "usage: ./loadtest.sh <numClients> <loopNum> <sleepTimeSeconds> <port num>"
	exit 
fi

numCli=$1
loops=$2
sleepTime=$3
port=$4
# if [[ ! -d testResults ]]; then
# 	mkdir testResults
# fi

gcc -o client client.c
# gcc -o server server.c -lpthread
# ./server $port &

for (( i = 0; i < $numCli; i++ )); do
	./client gammaray:$port test.c $loops $sleepTime &
	sleep 0.001
	pids[$i]=$!
done
for pid in ${pids[*]}; do
	wait $pid
done
