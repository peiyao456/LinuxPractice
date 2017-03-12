#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>

int main(int argc,const char* argv[])
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		return 1;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);
	if(connect(sock,(struct sockaddr*)&server,sizeof(server)) < 0)
	{
		perror("connect");
		return 2;
	}
	char buf[1024];
	while(1)
	{
		printf("send# ");
		fflush(stdout);
		//从标准输入读数据，读到buf中，然后从buf写到管道
		ssize_t s = read(0,buf,sizeof(buf)-1);
		if(s < 0)
		{			
			perror("read");
			break;
		}		
		buf[s-1] = 0;
		write(sock,buf,s);
	}
	close(sock);
	return 0;
}
