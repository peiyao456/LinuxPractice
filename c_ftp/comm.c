#include "comm.h"
int socket_create(int port,const char* ip)
{
	int sockfd;
	int yes = 1;
	struct sockaddr_in sock_addr;
	//创建套接字
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket");
		return -1;
	}
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = inet_addr(ip);
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) == -1)
	{
		close(sockfd);
		error("setsockopt");
		return -1;
	}
	if(bind(sockfd,(struct sockaddr*)&sock_addr,sizeof(sock_addr)) < 0)
	{
		close(sockfd);
		perror("bind");
		return -1;
	}
	if(listen(sockfd,5) < 0)
	{
		close(sockfd);
		perror("listen");
		return -1;
	}
	return sockfd;
}
//服务器接收客户端的连接请求
int socket_accept(int sock_listen)
{
	int sockfd;
	struct sockaddr_in client;
	size_t len = sizeof(client);
	sockfd = accept(sock_listen,(struct sockaddr*)&client,&len);

	if(sockfd < 0)
	{
		perror("accept");
		return -1;
	}
	return sockfd;
}
//客户端连接远程主机
int socket_connect(int port,char* host)
{
	int sockfd;
	struct sockaddr_in peer;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket");
		return -1;
	}
	memset(&peer,0,sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_port = htons(port);
	peer.sin_addr.s_addr = inet_addr(host);

	if(connect(sockfd,(struct sockaddr*)&peer,sizeof(peer)) < 0)
	{
		perror("connect");
		return -1;
	}
	return sockfd;
}
//接收数据
int recv_data(int sockfd,char* buf,int size)
{
	size_t read_nums;
	memset(buf,0,size);
	read_nums = recv(sockfd,buf,size,0);
	if(read_nums< 0)
	{
		perror("recv");
		return -1;
	}
	return read_nums;
}
//去掉字符串中的回车换行
void trimstr(char* str, int size)
{
	int i = 0;
	for(i = 0; i < size; ++i)
	{
		if(isspace(str[i]))
			str[i] = 0;
		if(str[i] == '\n')
			str[i] = 0;
	}
}

int send_response(int sockfd,int rc)
{
	int conv = htonl(rc);
	if(send(sockfd,&conv,sizeof(conv),0) < 0)
	{
		perror("send");
		return -1;
	}
	return 0;
}
//从命令行读取输入
void read_input(char* buf, int size)
{
	char* s = NULL;
	memset(buf,0,size);
	if(fgets(buf,size,stdin) != NULL)
	{
		s = strchr(buf,'\n');
		if(s)
			*s = 0;
	}
}
