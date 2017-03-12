#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
int main(int argc, const char* argv[])
{
	if(argc != 3)
	{
		printf("input error\n");
		return 1;
	}
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);
	int ret = connect(sock,(struct sockaddr*)&server,sizeof(server));
	if(connect < 0)
	{
		perror("connect");
		return 3;
	}
	char buf[1024];
	while(1)
	{
		printf("send#");
		fflush(stdout);
		ssize_t _s = read(0,buf,sizeof(buf)-1);
		if(_s > 0)
		{
			buf[_s - 1] = 0;
			if(write(sock,buf,sizeof(buf)-1) < 0)
			{
				break;
			}
			ssize_t s = read(sock,buf,sizeof(buf)-1);
			if(s > 0)
			{
				buf[s] = 0;
				printf("server echo#%s\n",buf);
			}
		}
		else 
		{
			perror("read");
			return 4;
		}
	}
	return 0;
}
