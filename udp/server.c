#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

int main(int argc,const char* argv[])
{
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	if(sock < 0)
	{
		perror("socket");
		return 1;
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]); 
	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		return 2;
	}
	char buf[1024];
	struct sockaddr_in peer;
	int len = sizeof(peer);
	while(1)
	{
	   size_t s = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
	   if(s > 0)
	   {
		   buf[s] = 0;
		   printf("client# %s\n",buf);
	   }
	   else if(s == 0)
	   {
		   close(sock);
		   break;
	   }
	   else
	   {
		   perror("recvfrom");
		   break;
	   }
	}
	return 0;
}
