#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
int StartUp(int port,const char* ip)
{
	int ListenSock = socket(AF_INET,SOCK_STREAM,0);
	if(ListenSock < 0)
	{
		perror("socket");
		exit(1);
	}
	int opt = 1;
	setsockopt(ListenSock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);
	if(bind(ListenSock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		exit(2);
	}
	if(listen(ListenSock,5) < 0)
	{
		perror("listen");
		exit(3);
	}
	return ListenSock;
}

int main(int argc,const char* argv[])
{
	if(argc != 3)
	{
		printf("input error\n");
		return 1;
	}
	int len;
	int listenSock = StartUp(atoi(argv[2]),argv[1]);
	struct sockaddr_in client;
	while(1)
	{
		int sock = accept(listenSock,(struct sockaddr*)&client,&len);//获取客户机的信息
		if(sock < 0)
		{
			perror("accept");
			continue;
		}
		
		printf("get a client,ip is %s,port is %d\n",inet_ntoa(client.sin_addr),\
					ntohs(client.sin_port));
		int id = fork();
		if(id > 0)
		{
			close(sock);
			while(waitpid(-1,NULL,WNOHANG) > 0);
			continue;
		}
		else
		{
			close(listenSock);
			if(fork() > 0)
			{
				exit(0);
			}
			char buf[1024];
			while(1)
			{
				ssize_t s = read(sock,buf,sizeof(buf)-1);//服务器进行读数据
				if(s > 0)
				{
					buf[s] = 0;
					printf("client# %s\n",buf);
				}
				else
				{
					//数据已经读完了，客户端不发送数据了
					printf("client is quit!\n");
					break;
				}
			}
			close(sock);
		//	exit(4);
			break;
		}
	}
	return 0;
}
