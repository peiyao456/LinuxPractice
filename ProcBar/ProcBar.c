/*************************************************************************
    > File Name: ProcBar.c
    > Author: peiyao
    > Mail: wpy199508@163.com 
    > Created Time: Sat 31 Dec 2016 10:36:11 PM CST
 ************************************************************************/

#include"ProcBar.h"
void ProcBar()
{
	int rate = 0;
	char str[102]={0};
	const char* sta = "-\\|/";
	while(rate<=100)
	{
		str[rate] = '=';
		printf("[%-100s],%d%%,[%c]\r",str,rate,sta[rate%4]);
		rate++;
		fflush(stdout);
		usleep(100000);
	}
	printf("\n");
}
