#include"comm.h"
static int CommonShm(int flag)
{
	int _k = ftok(PATHNAME,PROJID);
	if(_k < 0)
	{
		perror("ftok");
		return -1;
	}
	int shmid = shmget(_k,SIZE,flag);
	if(shmid < 0)
	{
		perror("shmget");
		return -2;
	}
	return shmid;
}

int CreateShm()
{
	int flag = IPC_CREAT|IPC_EXCL|0666;
	return CommonShm(flag);
}

int GetShm()
{
	return CommonShm(0);
}

void* ShmAt(int shmid)
{
	void* ret = shmat(shmid,NULL,0);
	if( ret == (void*)-1) 
	{
		perror("shmat");
	}
	return ret;
}
int ShmDt(void* addr)
{
	if(shmdt(addr) < 0)
	{
		perror("shmdt");
		return -1;
	}
	return 0;
}

int DestoryShm(int shmid)
{
	if(shmctl(shmid,IPC_RMID,NULL) < 0 )
	{
		perror("shmctl");
		return -1;
	}
	return 0;
}
