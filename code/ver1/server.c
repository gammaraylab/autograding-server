#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

void error(char *msg){
	perror(msg);
	exit(1);
}

void *start_function(void *sockfd){
	int newsockfd = *(int *) sockfd;
	char buffer[256];
	bzero(buffer,256);
	int d=10;
	int n;
	while(d--){
		n= read(newsockfd,buffer,255);
		if (n<0)
			error("Error reading from the socket");

		printf("message received %s",buffer);
		n=write(newsockfd,"i read your message",19);

		if(n<0)
			error("Error while writing to socket");
	}
	close(newsockfd);
	// pthread_exit(NULL);
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

	// while(1){
		newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr, &clilen);
		if(newsockfd<0)
			error("Error on accept");
		start_function(&newsockfd);  //for sinlge theaded version
	// 	pthread_t thread;
	// 	if(pthread_create(&thread,NULL, start_function,&newsockfd) !=0)
	// 		printf("Failed to create Thread\n");
	// }

	return 0;
}