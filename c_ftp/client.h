#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "comm.h"

int read_reply();
void print_reply();
void client_login();
int client_send_cmd(struct command* cmd);
int client_read_command(char* buf,int size,struct command* cmd);
int client_list(int data_sock,int sock_control);
int client_get(int data_sock,int sock_control,char* arg);
int client_open_conn(int sock_control,const char* ip);

#endif
