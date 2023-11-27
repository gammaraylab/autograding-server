#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

#define maxBufferSize 4096

void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char const *argv[])
{
	
	int sockfd, portno, n;
	int filefd;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[maxBufferSize];
	if (argc < 3) {
	fprintf(stderr, "usage %s hostname port location/of/your/file.c\n", argv[0]);
	exit(0);
	}

	if(access(argv[3],F_OK|R_OK)!=0){
		error("Error opening file");
	}
	filefd=open(argv[3],O_RDONLY);
	
	if(filefd==-1){
		error("Error opening file");
	}

	struct stat st;
 	stat(argv[3],&st);
 	long int file_size=st.st_size; //size of input file in bytes

	/* create socket, get sockfd handle */


	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	error("ERROR opening socket");

	/* fill in server address in sockaddr_in datastructure */

	server = gethostbyname(argv[1]);
	if (server == NULL) {
	fprintf(stderr, "ERROR, no such host\n");
	exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
	    server->h_length);
	serv_addr.sin_port = htons(portno);

	/* connect to server */

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	error("ERROR connecting");

	
	int packetSize=file_size%maxBufferSize;
	bzero(buffer, sizeof(int));
	sprintf(buffer,"%d",packetSize);
	write(sockfd,buffer,sizeof(int));
	sleep(1);
	read(filefd,buffer,packetSize);	// reading from file into buffer

	/* send user message to server */
	n = write(sockfd, buffer, packetSize);
	if (n != packetSize)
	  error("ERROR writing to socket");
	sleep(1);
	bzero(buffer, packetSize);
	/* read reply from server */
  n = read(sockfd, buffer, maxBufferSize);
  if (n < 0)
    error("ERROR reading from socket");
  printf("Server response: %s\n", buffer);
  sleep(1);
	close(sockfd);	//closing the socket
	/* code */
 	
	return 0;
}