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
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	dest.sin_addr.s_addr = inet_addr(argv[1]); 
	char buf[1024];
	int len = sizeof(dest);
	while(1)
	{
		printf("please input#");
		fflush(stdout);
	    ssize_t _s = read(0,buf,sizeof(buf)-1);
	    if(_s > 0)
	    {
	        buf[_s-1] = 0;
	    }
	    size_t s = sendto(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&dest,len);
		if(s < 0)
		{
			perror("sendto");
			break;
		}
		else
		{
			continue;
		}
	}
	return 0;
}
