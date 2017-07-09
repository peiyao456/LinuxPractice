#ifndef __SERVER_H__
#define __SERVER_H__

#include "comm.h"
void server_process(int sock_control);
int server_login(int sock_control);
int server_check_user(char* user,char* pass);
int server_start_data_conn(int sock_control);
int server_list(int sock_data,int sock_control);
void server_retr(int sock_data,int sock_coontrol,char* filename);
int server_recv_cmd(int sock_control,char* cmd,char* arg);

#endif
