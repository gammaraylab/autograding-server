# Autograding-server

Auto grding server for DECS project CS744. github repo: https://github.com/gammaraylab/autograding-server
gammaraylab/flutter_app
## Usage

There are two versions of this autogader, the usage is as follows:

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
