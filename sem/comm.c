#include"comm.h"

static int CommSemSet(int nums,int flag)
{
	int _k = ftok(PATHNAME,PROJID);
	if(_k < 0)
	{
		perror("ftok");
		return -1;
	}
	int semid = semget(_k,nums,flag);
	if(semid < 0)
	{
		perror("semget");
		return -2;
	}
	return semid;
}

int CreateSemSet(int nums)
{
	return CommSemSet(nums,IPC_CREAT | IPC_EXCL | 0666);
}

int GetSemSet()
{	
	return CommSemSet(0,0);
}

int InitSem(int semid,int which)
{
	union semun s;
	s.val = 1;
	int _s = semctl(semid,which,SETVAL,s);
	if(_s < 0)
	{
		perror("semctl");
		return -1;
	}
	return 0;
}

static int SemOp(int semid,int which,int op)
{
	struct sembuf s;
	s.sem_num = which;
	s.sem_op = op;
	s.sem_flg = 0;
	int _s = semop(semid,&s,1);
	if(_s < 0)
	{
		perror("semop");
		return -1;
	}
	return 0;
}

int P(int semid,int which)
{
	return SemOp(semid,which,-1);
}

int V(int semid,int which)
{
	return SemOp(semid,which,1);
}

int DestorySemSet(int semid)
{
	if(semctl(semid,0,IPC_RMID) < 0)
	{
		perror("semctl");
		return -1;
	}
	return 0;
}
