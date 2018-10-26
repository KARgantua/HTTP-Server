#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define SIZE sizeof(struct sockaddr_in)
#define BUFSIZE 1024

int sockfd_connect;

void closeproc(int sig);

int main(void)
{
	int sockfd_listen;
	int filefd, leng;
	struct stat finfo;
	char fread[BUFSIZE];
	ssize_t nread;
	char res_header[512], req_header[512];
	char *method, *path, *protocol;
	char localpath[1000];
	int pid;
		
	//ipv4, 5000번 포트, 사용가능 네트워크
	struct sockaddr_in server;
	struct sockaddr_in client;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(5000);
	
	//연결형모델TCP 생성
	if((sockfd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	
		printf("fait to call socket()\n");
		exit(1);
	}
	printf("complete to call socket\n");

	if(bind(sockfd_listen, (struct sockaddr *)&server, SIZE) == -1) {
		printf("fail to call bind()\n");
		exit(1);
	}
	printf("complete to call bind\n");
	
	if(listen(sockfd_listen, 5) == -1) {
		printf("fail to call listen()\n");
		exit(1);
	}
	printf("listening...\n");

	while(1) {
#if 0
		if((sockfd_connect = accept(sockfd_listen, NULL, NULL)) == -1) {
			printf("fail to call accept()\n");
			continue;
		}
				
		printf("\naccepted\n--------------------------------------------------------\n");
		
		memset(req_header, 0 , sizeof(req_header));
		recv(sockfd_connect, req_header, BUFSIZE, 0);
		printf("%s",req_header);
		
		method = strtok(buf," ");
		path = strtok(NULL, " ");
		protocol = strtok(strtok(NULL, " "), "\r\n");
#else
		leng = sizeof(client);
		if((sockfd_connect = accept(sockfd_listen, (struct sockaddr *) &client, &leng)) == -1) {
			printf("fail to call accept()\n");
			continue;
		}
#endif			
		printf("\naccepted\n--------------------------------------------------------\n");
		pid = fork();


		if(pid == 0) {
			memset(req_header, 0x00, sizeof(req_header));
			read(sockfd_connect,req_header,1024);
			printf("%s", req_header);
	
			method = strtok(req_header," ");
			path = strtok(NULL, " ");
			protocol = strtok(strtok(NULL, " "), "\r\n");


		
			if(!strcmp("GET", method)) 
			{
				//fd = open("index.html", O_RDONLY);
				if(!(strcmp("/",path)))
					filefd = open("index.html", O_RDONLY);
				else {
					sprintf(localpath, ".%s", path);
					filefd = open(localpath, O_RDONLY);
				}

				memset(res_header, 0 ,sizeof(res_header));
				if(filefd == -1) {
					sprintf(req_header,"HTTP/1.1 404 NotFound");
					send(sockfd_connect, res_header, strlen(res_header), 0);
				}
				else
				{
					fstat(filefd, &finfo);
					sprintf(res_header, "%s 200 OK\r\nContent-type: text/html\r\nContent-Length: %ld\r\n\r\n",protocol ,finfo.st_size);
					send(sockfd_connect, res_header, strlen(res_header), 0);	
					while((nread = read(filefd, fread, BUFSIZE)) > 0) {
						send(sockfd_connect, fread, BUFSIZE, 0);  
					}	
				}
			
			}
			else if(!strcmp("POST", method))
				printf("POST\n");
			close(sockfd_connect);
			
			exit(1);	
		}
		else if(pid < 0) {
			printf("fail to call fork()\n");
			exit(1);
		}
	}
	return 0;
	
}
