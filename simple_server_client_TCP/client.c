
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char **argv)
{
int n,sockfd,port;
char buffer[256];
struct sockaddr_in server_addr;
port=atoi(argv[1]);
if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
{
        fprintf(stderr,"cant create a socket\n");
        exit(1);
}

server_addr.sin_family=AF_INET;
server_addr.sin_port=htons(port);
server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

 if(connect(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        fprintf(stderr,"Can't connect\n");
        exit(1);
    }
    printf("Enter message\n");
    scanf("%s",buffer);
    write(sockfd,buffer,sizeof(buffer));
    printf("%s\n",buffer);
    close(sockfd);
}

