#define _WIN32_WINNT 0x501
#include "server.h"
int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("usage:./server ip port\n");
		exit(0);
	}
	int sock_listen = 0,sock_control = 0,port = 0,pid = 0;
	port = atoi(argv[2]);//获取到端口号
	//创建监听套接字
	if((sock_listen = socket_create(port,argv[1])) < 0)
	{
		perror("sock create");
		exit(1);
	}
	//接收来自用户的请求
	while(1)
	{
		//得到控制套接字，处理控制信息
		if((sock_control = socket_accept(sock_listen)) < 0)
			break;
		//fork出子进程，让子进程去处理客户端的请求，子进程不能监听信息，父进程不能处理请求
		if((pid = fork()) < 0)
			perror("fork:");
		else if(pid == 0)//子进程的处理
		{
			close(sock_listen);
			server_process(sock_control);
			close(sock_control);
			exit(0);
		}
		close(sock_control);
	}

	close(sock_listen);

	return 0;
}
//处理客户端的请求
void server_process(int sock_control)
{
	int sock_data;
	char cmd[5];
	char arg[MAXSIZE];
	//给客户端发送应答码
	send_response(sock_control,220);
	//获取用户的登录信息
	if(server_login(sock_control) == 1)
		send_response(sock_control,230);
	else
	{
		send_response(sock_control,430);
		exit(0);
	}
	//处理客户端的请求
	while(1)
	{
		//接收命令，并解析
		int rc = server_recv_cmd(sock_control,cmd,arg);
		if(rc < 0 || rc == 221)//quit
			break;
		if(rc == 200)
		{
			//创建和客户端的连接
			if((sock_data = server_start_data_conn(sock_control)) < 0)
			{
				close(sock_control);
				exit(1);
			}
			//执行命令
			if(strcmp(cmd,"LIST") == 0)
				server_list(sock_data,sock_control);
			else if(strcmp(cmd,"RETR") == 0)
				server_retr(sock_data,sock_control,arg);
			close(sock_control);
		}
	}
}
//用户登录
int server_login(int sock_control)
{
	char buf[MAXSIZE];
	char user[MAXSIZE];
	char pass[MAXSIZE];
	memset(buf,0,sizeof(buf));
	memset(user,0,sizeof(user));
	memset(pass,0,sizeof(pass));
	//获取用户名
	if(recv_data(sock_control,buf,sizeof(buf)) == -1)
	{
		perror("recv usename");
		exit(1);
	}
	//用户名放到user中
	int i = 5;
	int n = 0;
	while(buf[i])
		user[n++] = buf[i++];
	//用户名正确，通知用户输入密码
	send_response(sock_control,331);
	memset(buf,0,MAXSIZE);
	//获取密码
	if(recv_data(sock_control,buf,sizeof(buf)) == -1)
	{
		perror("recv pass");
		exit(1);
	}
	//拷贝密码到pass中
	i = 5;
	n = 0;
	while(buf[i])
		pass[n++] = buf[i++];
	return server_check_user(user,pass);
}
//接收客户端的响应并回应
int server_recv_cmd(int sock_control,char* cmd,char* arg)
{
	int rc = 200;
	char buf[MAXSIZE];//存储客户端发来的命令

	memset(cmd,0,5);
	memset(arg,0,MAXSIZE);
	memset(buf,0,MAXSIZE);

	if(recv_data(sock_control,buf,sizeof(buf)) == -1)//接收客户端发来的命令
	{
		perror("recv");
		return -1;
	}
	//解析客户发来的命令
	strncpy(cmd,buf,4);
	char* tmp = buf + 5;
	strcpy(arg,tmp);

	if(strcmp(cmd,"QUIT") == 0)
		rc = 221;
	else if(strcmp(cmd,"USER") == 0 ||strcmp(cmd,"PASS") == 0 || strcmp(cmd,"LIST") == 0 || strcmp(cmd,"RETR") == 0)
		rc = 200;
	else
		rc = 502;
	//响应客户
	send_response(sock_control,rc);
	return rc;
}
//服务器到客户的数据连接
int server_start_data_conn(int sock_control)
{
	char buf[1024];
	int wait,sock_data;
	if(recv(sock_control,&wait,sizeof(wait),0) < 0)
	{
		perror("recv");
		return -1;
	}
	struct sockaddr_in client_addr;
	size_t len = sizeof(client_addr);
	getpeername(sock_control,(struct sockaddr*)&client_addr,&len);
	inet_ntop(AF_INET,&client_addr.sin_addr,buf,sizeof(buf));
	
	if((sock_data = socket_connect(CLIENT_PORT_ID,buf)) < 0)
		return -1;
	return sock_data;
}
//处理客户发的list请求
int server_list(int sock_data,int sock_control)
{
	char data[MAXSIZE];
	size_t num_read;
	FILE* fd;

	int rs = system("ls | tail -n+2 > tmp.txt");
	if(rs < 0)
	{
		exit(1);
	}
	fd = fopen("tmp.txt","r");
	if(fd == NULL)
		exit(1);

	//定位到文件的开始处
	fseek(fd,SEEK_SET,0);

	send_response(sock_control,1);
	memset(data,0,MAXSIZE);
	//发送tmp.txt中的数据
	while((num_read = fread(data,1,MAXSIZE,fd)) > 0)
	{
		//发送数据给对端
		if(send(sock_data,data,num_read,0) < 0)
			perror("send");

		memset(data,0,MAXSIZE);
	}
	fclose(fd);
	send_response(sock_control,226);
	return 0;
}

void server_retr(int sock_data,int sock_control,char* filename)
{
	char data[MAXSIZE];
	FILE* fd = NULL;
	size_t num_read;
	fd = fopen(filename,"r");
	if(fd == NULL)
		send_response(sock_control,550);//读取文件失败，发送错误码
	else
	{
		send_response(sock_control,150);
		while((num_read = fread(data,1,MAXSIZE,fd)) > 0)
		{
			if(send(sock_data,data,num_read,0) < 0)
				perror("send");
		}
		//num_read <= 0
		perror("fread");
		send_response(sock_control,226);
		fclose(fd);
	}
}
int server_check_user(char* user,char* pass)
{
	char username[MAXSIZE];
	char password[MAXSIZE];
	char buf[MAXSIZE];
	char* line;
	FILE* fd;
	char* pch;
	size_t num_read;
	int len = 0;
	int auth = 0;

	fd = fopen(".auth","r");
	if(fd == NULL)
	{
		perror("fopen:");
		exit(1);
	}
	while((num_read = getline(&line,&len,fd)) != -1)
	{
		memset(buf,0,MAXSIZE);
		strcpy(buf,line);

		pch = strtok(buf," ");
		strcpy(username,pch);
		if(pch != NULL)
		{
			pch = strtok(NULL," ");
			strcpy(password,pch);
		}
		//去掉密码中的换行
		trimstr(password,(int)strlen(password));
		if(strcmp(user,username) == 0 && strcmp(pass,password) == 0)
		{
			auth = 1;
			break;
		}
	}
	fclose(fd);
	free(line);
	return auth;
}
