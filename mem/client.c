#include"comm.h"
int main()
{
	int shmid = GetShm();
	//挂接
	char* addr = ShmAt(shmid);
	int i = 0;
	while(1)
	{
		addr[i++] = '1';
		addr[i] = 0;
		i %= (SIZE - 1);
	}
	//去关联
	ShmDt(addr);
	return 0;
}
