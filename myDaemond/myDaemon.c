#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
void myDaemond()
{
	umask(0);
	pid_t id = fork();
	if(id < 0)
	{
		return ;
	}
	else if(id  > 0)
	{
		exit(0);
	}
	int ret = setsid();
	if(ret < 0)
		return;
	if(fork() > 0)
	{
		exit(0);
	}
	ret = chdir("/");
	if(ret < 0)
		return;
	close(0);
	close(1);
	close(2);

	signal(SIGCHLD,SIG_IGN);
}
int main()
{
	myDaemond();
	while(1)
	{
		sleep(1);
	}
	return 0;
}
