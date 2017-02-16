#include"comm.h"
int main()
{
	int semid = CreateSemSet(1);
	InitSem(semid,0);
//	printf("semid:%d\n",semid);
	int id = fork();
	if(id == 0)
	{
		//child
		while(1)
		{
				
			P(semid,0);
			printf("1");
			fflush(stdout);
			printf("1");
			fflush(stdout);
			V(semid,0);
			sleep(1);
		}
	}
		else
		{
			//father
			while(1)
			{
				P(semid,0);
				printf("2");
				fflush(stdout);
				printf("2");
				fflush(stdout);
				V(semid,0);
			sleep(1);
			}
		}
//	sleep(10);
	DestorySemSet(semid);
	return 0;
}
