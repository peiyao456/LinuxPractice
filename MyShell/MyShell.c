#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
int main()
{
	while(1)
	{
		printf("[MyShell@localhost MyShell]&:");
		fflush(stdout);
		//读取字符串
		char buf[1024];
		int s = read(0,buf,1024);
		if(s > 0)//有读取到字符
		{
			int i = 0;
			for( i = 0; i < s; ++i)
			{
				if(buf[i] == '\b' && i >= 1)
				{
				//	printf("debug:%d\n",i);
					int j = 0;
					for( j = i+1; j < s; ++j)
					{
						buf[j-2] = buf[j];
					}
					buf[s-2] = 0;
					i-=1;
				}
				else if(buf[i] == '\b' && i == 0)
				{
				//	printf("debug:%d\n",i);
					int j = 0;
					for( j = 1; j < s; ++j)
					{
						buf[j-1] = buf[j];
					}
					buf[s-1] = 0;
				//	i-=1;
				}
				else
				{
					continue;
				}
			}
			buf[s] = 0;
		}
		else
		{
			continue;
		}
		//将读取到的字符串分成多个字符串
		char* start = buf;
		int i =1;
		char* MyArgv[10] = {0};
		MyArgv[0] = start;
		while(*start)
		{
			if(isspace(*start))
			{
				*start = 0;
				start++;
				MyArgv[i++] = start;
			}
			else
			{
				++start;
			}
		}
		MyArgv[i-1] = NULL;
		
		//打印一下字符串信息
		int m = 0;
		for(m = 0; m <i-1;++m)
		{
			printf("debug:%s\n",MyArgv[m]);
		}
		//fork新的进程
		int  id = fork();
		if(id == 0)
		{
			//child,执行替换操作
			execvp(MyArgv[0],MyArgv);
			perror("error");
			exit(1);
		}
		else
		{
			printf("father\n");
			wait(NULL);
		}
	}
	return 0;
}
