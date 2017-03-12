#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
int StartUp(int port,const char* ip)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		exit(2);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}
	if(listen(sock,5) < 0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}
void* thread_hander(void* arg)
{
	int sock = *((int*)arg);
	char buf[1024];
	while(1)
	{
		ssize_t _s = read(sock,buf,sizeof(buf)-1);
		if(_s > 0)
		{
			buf[_s-1] = 0;
			printf("client say#%s\n",buf);
			if(write(sock,buf,sizeof(buf)-1)<0)
			{
				break;
			}
		}
		else if(_s == 0)
		{
			printf("client is quit!\n");
			break;
		}
		else
		{
			perror("read");
			break;
		}
	}
	close(sock);
}
int main(int argc,const char* argv[])
{
	if(argc != 3)
	{
		printf("input  error\n");
		return 1;
	}
	int listenSock = StartUp(atoi(argv[2]),argv[1]);
	struct sockaddr_in client;
	int len = 0;
	while(1)
	{
		int sock = accept(listenSock,(struct sockaddr*)&client,&len);
		if(sock < 0)
		{
			perror("accept");
			return 5;
		}
		printf("get a client!ip is %s,port is %d\n",inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));
		pthread_t tid;
		int ret = pthread_create(&tid,NULL,thread_hander,&sock);
		if(ret < 0)
		{
			perror("pthread_create");
			return 6;
		}
		pthread_detach(tid);
	}
	return 0;
}
