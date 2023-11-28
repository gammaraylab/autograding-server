#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

#define maxBufferSize 4096

void error(char *msg) {
  perror(msg);
  exit(1);
}

void comp_exe(int socketFD, int timestamp, int errFD){
  char buffer[maxBufferSize];

  int n=-1;
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"gcc -o temp%d temp%d.c 2>err%d.txt",timestamp,timestamp,timestamp);
  if(system(buffer)!=0){
    n=read(errFD,buffer,maxBufferSize);
    write(socketFD,buffer,n);
    write(socketFD,"compilation failed\n",19);
    system(buffer);
    return;
  }
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"chmod +x temp%d",timestamp);
  if(system(buffer)!=0){
    n=read(errFD,buffer,maxBufferSize);
    write(socketFD,buffer,n);
    write(socketFD,"execution permission denied\n",28);
    system(buffer);
    return;
  }
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"./temp%d >err%d.txt",timestamp,timestamp);
  n=system(buffer);
  if(n!=0){
    write(socketFD,"execution error\n",16);
  }
  bzero(buffer, maxBufferSize);
  n=read(errFD,buffer,maxBufferSize);
  write(socketFD,buffer,n);
  printf("debug:n=%d\ntimestamp=%d\n%s\n",n,timestamp,buffer);
  if(strcmp("1 2 3 4 5 6 7 8 9 10\n",buffer)==0){
    n = write(socketFD, "success", 7);
    if (n < 0)
      error("ERROR writing to socket");
  } 
  else{
    n = write(socketFD, "WRONG OUTPUT", 12);
    if (n < 0)
      error("ERROR writing to socket");
  }
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"rm -f err%d.txt temp%d temp%d.c",timestamp,timestamp,timestamp);
  system(buffer);
}

void *start_function(void *sockfd) {
  int newsockfd = *(int *)sockfd;
  char buffer[maxBufferSize];
  int filesize=read(newsockfd,buffer,maxBufferSize);  // size of incoming file
  if(filesize<0){
    error("error while reading the size of file");
  }
  filesize=atoi(buffer);
  struct timeval end;
  gettimeofday(&end, NULL);
  int timestamp=rand();//end.tv_usec ;
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"temp%d.c",timestamp);
  int tempFD=open(buffer,O_CREAT|O_WRONLY, 0644); //creating a temp file to store the uploaded file
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"err%d.txt",timestamp);
  int errFD=open(buffer,O_CREAT|O_WRONLY, 0644); //creating a temp file to store the uploaded file
  
  bzero(buffer, maxBufferSize);
  int n;//=read(newsockfd, buffer, maxBufferSize);   //reading from the socket
  while(1){
    n=read(newsockfd, buffer, maxBufferSize);
    write(tempFD,buffer,n);
    filesize=filesize-n;
    if (n < 0)
      error("ERROR reading from socket");
    if(filesize<1)  //checks if server recieved the full file or not
      break;
  }
  write(tempFD,"\n",1);  // one new line char to remove the warning of null char
  sleep(1);
  
  comp_exe(newsockfd,timestamp,errFD);
  close(tempFD);
  close(newsockfd);
}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  /* accept a new request, create a newsockfd */
  while(1){
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    pthread_t thread;
    if(pthread_create(&thread,NULL, start_function,&newsockfd) !=0)
      printf("Failed to create Thread\n");
  }

  return 0;
}
