#ifndef __COMM_H__
#define __COMM_H__

#include<stdio.h>
#include<sys/shm.h>
#include<sys/types.h>

#define PATHNAME "."
#define PROJID 0x5000
#define SIZE 4096*1

int CreateShm();
int GetShm();
void* ShmAt(int shmid);
int ShmDt(void* addr);
int DestoryShm(int shmid);
#endif
