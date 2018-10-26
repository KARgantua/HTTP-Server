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

#define SIZE sizeof(struct sockaddr_in)
#define BUFSIZE 1024

int sockfd_connect;

int main(void)
{
	int sockfd_listen;
	int filefd;
	struct stat finfo;
	char buf[BUFSIZE];
	char fread[BUFSIZE] = {"NULL"};
	ssize_t nread;
	char res_header[512] = {"NULL"}, req_header[512] = {"NULL"};
	char *method, *path, *protocol;
	char localpath[1000] = {"NULL"};
	
	//ipv4, 5000번 포트, 사용가능 네트워크
	struct sockaddr_in server;

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
		if((sockfd_connect = accept(sockfd_listen, NULL, NULL)) == -1) {
			printf("fail to call accept()\n");
			continue;
		}
		printf("accepted\n--------------------------------------------------------\n");
		recv(sockfd_connect, buf, BUFSIZE, 0);
		printf("%s",buf);
		method = strtok(buf," ");
		path = strtok(NULL, " ");
		protocol = strtok(strtok(NULL, " "), "\r\n");

		
		if(!strcmp("GET", method)) 
		{
			//fd = open("index.html", O_RDONLY);
			if(!(strcmp("/",path)))
				filefd = open("index.html", O_RDONLY);
			else {
				sprintf(localpath, ".%s", path);
				printf("%s\n",localpath);
				filefd = open(localpath, O_RDONLY);
			}

			
			if(filefd == -1) {
				sprintf(req_header,"HTTP/1.1 404 NotFound");
				send(sockfd_connect, req_header, strlen(req_header) + 1, 0);
			}
			else
			{
				fstat(filefd, &finfo);
				sprintf(res_header, "%s 200 OK\r\nContent-type: text/html\r\nContent-Length: %ld\r\n\r\n",protocol ,finfo.st_size);
				send(sockfd_connect, res_header, strlen(res_header) + 1, 0);
				while((nread = read(filefd, fread, BUFSIZE)) > 0) {
					printf("%s", fread);
					send(sockfd_connect, fread, BUFSIZE, 0);  //사진 보낼 때 문제 생김
				}
			}
			
		}
		else if(!strcmp("POST", method))
			printf("POST\n");
		//close(sockfd_connect);
	}
	
	return 0;
}
