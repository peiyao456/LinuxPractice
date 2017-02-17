#include"comm.h"
int main()
{
	int shmid = CreateShm();
	//挂接到共享内存上
	char* addr = (char*) ShmAt(shmid);
	while(1)
	{
		printf("%s\n",addr);
	}
	//去关联
	ShmDt(addr);
	DestoryShm(shmid);
	return 0;
}
