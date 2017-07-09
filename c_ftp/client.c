#define _WIN32_WINNT 0x501
#include"client.h"
int sock_control;
//接收服务器的响应
int read_reply()
{
	int ret = 0;
	if(recv(sock_control,&ret,sizeof(ret),0) < 0)
	{
		perror("recv from server :");
		return -1;
	}
	return ntohl(ret);
}

//打印响应信息
void print_reply(int ret)
{
	switch (ret)
	{
		case 220:
			printf("220,server ready!\n");
			break;
		case 221:
			printf("221,bye!\n");
			break;
		case 226:
			printf("226,close data connection!\n");
			break;
		case 550:
			printf("550,request error!\n");
			break;
	}
}
void client_login()
{
	struct command cmd;
	char user[256];
	memset(user,0,256);

	//获取用户名
	printf("userName:");
	fflush(stdout);
	read_input(user,256);//读取用户名并写到user数组中

	//发送用户名到服务器
	strcpy(cmd.code,"USER");
	strcpy(cmd.arg,user);
	client_send_cmd(&cmd);

	//等待服务器给的应答码
	int wait; 
	recv(sock_control,&wait,sizeof(wait),0);

	//获取密码
	char* pass = getpass("password:");
	//发送密码到服务器
	strcpy(cmd.code,"PASS");
	strcpy(cmd.arg,pass);
	client_send_cmd(&cmd);

	int retcode = read_reply();
	switch(retcode)
	{
		case 430:
			printf("invalid username\n");
			exit(0);
			break;
		case 230:
			printf("success login\n");
			break;
		default:
			printf("error\n");
			exit(1);
			break;
	}
}

//发送数据到服务器
int client_send_cmd(struct command* cmd)
{
	char buf[MAXSIZE];
	int s = 0;
	sprintf(buf,"%s %s",cmd->code,cmd->arg);

	s = send(sock_control,buf,strlen(buf),0);
	if(s < 0)
	{
		perror("send:");
		return -1;
	}
}
//解析用户输入的命令行到command结构体
int client_read_command(char* buf,int size,struct command* cmd)
{
	memset(cmd->code,0,sizeof(cmd->code));
	memset(cmd->arg,0,sizeof(cmd->arg));
	printf("ftclient>");
	fflush(stdout);
	read_input(buf,size);//等待用户输入命令
	char* arg = NULL;
	arg = strtok(buf," ");
	arg = strtok(NULL," ");//code部分存储在buf中，arg存储在arg中。
	if(arg != NULL)
		strncpy(cmd->arg,arg,strlen(arg));
	if(strcmp(buf,"list") == 0)
		strcpy(cmd->code,"LIST");
	else if(strcmp(buf,"get") == 0)
		strcpy(cmd->code,"RETR");
	else if(strcmp(buf,"quit") == 0)
		strcpy(cmd->code,"QUIT");
	else
		return -1;

	memset(buf,0,256);
	strcpy(buf,cmd->code);//将code存储在buf的开头

	if(arg != NULL)
	{
		strcat(buf," ");
		strncat(buf,cmd->arg,strlen(cmd->arg));
	}
	return 0;
}
int client_list(int sock_data,int sock_control)
{
	size_t s;
	char buf[MAXSIZE];
	int tmp = 0;
	if(recv(sock_control,&tmp,sizeof(tmp),0) < 0)//等待服务器的启动信息
	{
		printf("reading message from server error\n");
		return -1;
	}
	memset(buf,0,sizeof(buf));
	//接收服务器传来的数据
	while((s = recv(sock_data,buf,MAXSIZE,0)) > 0)
	{
		printf("%s",buf);
		memset(buf,0,sizeof(buf));
	}
	if(s < 0)
		perror("recv:");
	if(recv(sock_control,&tmp,sizeof(tmp),0) < 0)//等待服务器的完成信息
	{
		perror("recv:");
		return -1;
	}
	return 0;
}
int client_get(int data_sock,int sock_control,char* arg)
{
	char data[MAXSIZE];
	int size;
	FILE* fd = fopen(arg,"w");
	//将服务器传来的数据写进去fd中
	while((size = recv(data_sock,data,MAXSIZE,0)) > 0)
		fwrite(data,1,size,fd);
	if(size < 0)
		perror("recv:");
	fclose(fd);
	return 0;
}
//客户端打开数据连接
int client_open_conn(int sock_control,const char* ip)
{
	int sock_listen = socket_create(CLIENT_PORT_ID,ip);
	//发送确认
	int ack = 1;
	if(send(sock_control,&ack,sizeof(ack),0) < 0)
	{
		perror("send:");
		return -1;
	}
	int sock_con = socket_accept(sock_listen);
	close(sock_listen);
//	printf("%d",sock_con);
	return sock_con;
}
int main(int argc,char* argv[])
{
	int data_sock,retcode,s;
	char buf[MAXSIZE];
	struct command cmd;
//	struct addrinfo hints,*res,*rp;
	if(argc != 3)
	{
		printf("usage:./client server_ip port\n");
		exit(0);
	}

	sock_control = socket_connect(atoi(argv[2]),argv[1]);
	
	//连接成功
	printf("connect to host %s success",argv[1]);
	print_reply(read_reply());

	//获取用户的名字和密码
	client_login();

	while(1)
	{
		//读取用户输入的命令
		if(client_read_command(buf,sizeof(buf),&cmd) < 0)
		{
			printf("read command error\n");
			continue;
		}
		if(send(sock_control,buf,strlen(buf),0) < 0)
		{
			printf("send data error\n");
			close(sock_control);
			exit(1);
		}
		//
		retcode = read_reply();
		if(retcode == 221)
		{
			print_reply(221);
			break;
		}
		else if(retcode == 502)
		{
			printf("%d invalid command\n",retcode);
		}
		else
		{
			//200
			if((data_sock = client_open_conn(sock_control,argv[1])) < 0)
			{
				perror("open sock connect");
				exit(1);
			}
			if(strcmp(cmd.code,"list") == 0)
				client_list(data_sock,sock_control);
			else if(strcmp(cmd.code,"RETR") ==0)
			{
				if(read_reply() == 550)//请求失败
				{
					print_reply(550);
					close(data_sock);
					continue;
				}
				client_get(data_sock,sock_control,cmd.arg);
				print_reply(read_reply());
			}
			close(data_sock);
		}
	}
	close(sock_control);
}
