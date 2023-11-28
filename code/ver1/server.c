#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#define maxBufferSize 4096

char buffer[maxBufferSize];

void error(char *msg){
	perror(msg);
	exit(1);
}

void comp_exe(int socketFD, int errFD){
  int n=-1;
  sprintf(buffer,"gcc -o temp temp.c 2>err.txt");
  if(system(buffer)!=0){
    n=read(errFD,buffer,maxBufferSize);
    write(socketFD,buffer,n);
    write(socketFD,"compilation failed\n",19);
    system(buffer);
    return;
  }

  sprintf(buffer,"chmod +x temp");
  if(system(buffer)!=0){
    n=read(errFD,buffer,maxBufferSize);
    write(socketFD,buffer,n);
    write(socketFD,"execution permission denied\n",28);
    system(buffer);
    return;
  }
  bzero(buffer, maxBufferSize);
  sprintf(buffer,"./temp 1>err.txt");
  n=system(buffer);
  printf("\n");
  if(n!=0){
    write(socketFD,"execution error\n",16);
  }
  bzero(buffer, maxBufferSize);
  n=read(errFD,buffer,maxBufferSize);
  write(socketFD,buffer,n);
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

  sprintf(buffer,"rm -f err.txt temp temp.c");
  system(buffer);
}

void *start_function(void *sockfd){
	int newsockfd = *(int *) sockfd;
	int filesize=read(newsockfd,buffer,maxBufferSize);  // size of incoming file
  if(filesize<0){
    error("error while reading the size of file");
  }

  filesize=atoi(buffer);

  struct timeval end;
  sprintf(buffer,"temp.c");
  int tempFD=open(buffer,O_CREAT|O_RDWR,0644); //creating a temp file to store the uploaded file
  sprintf(buffer,"err.txt");
  int errFD=open(buffer,O_CREAT|O_RDWR,0644); //creating a temp file to store the uploaded file
  

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
  comp_exe(newsockfd,errFD);
  close(tempFD);
  close(newsockfd);
  
}
int main(int argc, char const *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if(argc<2){
		fprintf(stderr, "error, no port provided\n");
		exit(1);
	}
	sockfd= socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
		error("Error opening socket");
	bzero((char *)&serv_addr,sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family =AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if(bind(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
		error("Error on binding");
	listen(sockfd,5);
	clilen=sizeof(cli_addr);

	while(1){
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    start_function(&newsockfd);
  }

	return 0;
}