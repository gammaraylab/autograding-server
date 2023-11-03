#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <netdb.h>
#include<sys/stat.h>

#include<unistd.h>
#include<fcntl.h>

#define maxBufferSize 4096

void error(char *msg,	int ext) {
  perror(msg);
  if(ext!=0)
  	exit(ext);
}

int main(int argc, char const *argv[])
{
	int sockfd, portno, n,usedLen=0,responses=0;
	int filefd;
	int loops=atoi(argv[3]);
	int sleepTime=atoi(argv[4]);
	float serverResponseTime=0.0;
	float avgServerResponseTime=0.0;
	int idx=-1;			//separating the host name from <serverIP:port>
 	long int file_size;

	struct sockaddr_in serv_addr;
	struct hostent *server;
	struct stat st;	 // for getting file size
	struct timeval start, end;

	char buffer[maxBufferSize];
	bzero(buffer,maxBufferSize);

	if (argc < 4) {
	fprintf(stderr, "./submit <serverIP:port> <sourceCodeFileTobeGraded> <loopNum> <sleepTimeSeconds>\n");
	exit(0);
	}
 	
 	bzero(buffer, maxBufferSize);
 	while(argv[1][++idx]!=':')
 		buffer[idx]=argv[1][idx];
	server = gethostbyname(buffer);		// host name 
	portno = atoi(argv[1]+idx+1);			//separating the port number from <serverIP:port>
	/* fill in server address in sockaddr_in datastructure */
	if (server == NULL) {
	fprintf(stderr, "ERROR, no such host\n");
	exit(0);
	}
	
	if(access(argv[2],F_OK|R_OK)!=0)
		error("Error opening file",1);

 	stat(argv[2],&st);
	file_size=st.st_size; //size of input file in bytes
	int packetSize=file_size%maxBufferSize;
	
	while(loops--){
		gettimeofday(&start, NULL);
 		filefd=open(argv[2],O_RDONLY) ;
		if(filefd ==-1)
			error("Error opening file",1);
		/* create socket, get sockfd handle */
	 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0){
			// error("ERROR opening socket",0);
			continue;
		}

		bzero((char *)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
		    server->h_length);
		serv_addr.sin_port = htons(portno);

		/* connect to server */

		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
			// error("ERROR connecting",0);
			continue;
		}
		bzero(buffer, maxBufferSize);
		usedLen=sprintf(buffer,"%d",packetSize);
		write(sockfd,buffer,usedLen);	//sending the file size to the server
		sleep(1);
		bzero(buffer,maxBufferSize);
		read(filefd,buffer,packetSize);	// reading from file into buffer

		/* send user message to server */
		n = write(sockfd, buffer, packetSize);
		if (n != packetSize){
		  // error("ERROR writing to socket",0);
		  continue;
		}
		/* read reply from server */
		sleep(1);
		bzero(buffer, maxBufferSize);
	  n = read(sockfd, buffer, maxBufferSize);
	  if (n < 0){
	    // error("ERROR reading from socket",0);
	    continue;
	  }
 		// printf("Server response: %s\n", buffer); //response from the server
	  responses++;
    gettimeofday(&end, NULL);
	  serverResponseTime=end.tv_sec - start.tv_sec-2; // subtracting '2' since we introduced sleep
	  serverResponseTime+=(double)(end.tv_usec - start.tv_usec)/1000000;
	  avgServerResponseTime+=serverResponseTime;
		close(sockfd);	//closing the socket
  	close(filefd);
	  sleep(sleepTime);
	}
	
	printf("%f ",((double)avgServerResponseTime/atoi(argv[3])));
	printf("%f ",avgServerResponseTime);
	printf("%d\n",responses);
 	
	return 0;
} 