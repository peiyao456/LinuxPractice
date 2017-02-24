#include<stdio.h>
#include<signal.h>
void AlarmRun(int signo)
{
}
int mySleep(int nsec)
{
	struct sigaction act,oact;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	act.sa_handler = AlarmRun;
	sigaction(SIGALRM,&act,&oact);
	alarm(nsec);
	pause();
	int ret = alarm(0);
	sigaction(SIGALRM,&oact,NULL);//恢复到之前的状态
	return ret;
}
int main()
{
	while(1)
	{
		if(mySleep(1)==0)
			printf("1 second\n");
		else
			break;
	}
	return 0;
}
