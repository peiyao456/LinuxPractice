#ifndef __COMM_H__
#define __COMM_H__

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<unistd.h>


#define PATHNAME "."
#define PROJID 0x5000

union semun
{
	int val;
	struct semid_ds*buf;
	unsigned short* array;
	struct seminfo* __buf;
};
int CreateSemSet(int nums);
int GetSemSet();
int InitSem();
int P(int semid,int which);
int V(int semid,int which);
int DestorySemSet(int semid);
#endif
