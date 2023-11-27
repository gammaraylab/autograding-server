# Autograding-server

Auto grding server for DECS project CS744. github repo: https://github.com/gammaraylab/autograding-server

## Usage

There are three versions of this autogader, the usage is as follows:

#### Ver1: single threaded autograder

`gcc -o server server.c`
`./server <port-no>`
`gcc -o client client.c`
`./client <hostname> <post-no> <test-file-path>`

#### Ver2: single threaded autograder with load tester

`gcc -o server server.c`
`./server <port-no>`
`gcc -o client client.c`
`./loadtest.sh <numClients> <loopNum> <sleepTimeSeconds> <port num>`

#### Ver3: Multithreaded autograder with load tester

`gcc -o server server.c -lpthread`
`./server <port-no>`
`gcc -o client client.c`
`./loadtest.sh <numClients> <loopNum> <sleepTimeSeconds> <port num>`

## Work
I have not done the load testing of the version 3 and above. version 1,2 and 3 are working properly.
