#define TRANS_PORT 8010
#include <stdio.h>

void ls_serv(int sockfd,char *path);

void pwd_serv(int sockfd);

void get_serv(int sourcefd,char *path);

void put_serv(int sourcefd,char *path);

void cd_serv(int socokfd,char *path);

void ls_cli(int sockfd);

void pwd_cli(int sockfd);

void get_cli();
void put_cli();

void cd_cli(int sockfd);

void cmdstr(char *str,char **str1,char **str2);
void readpawd(char **p,FILE *fp);
void set_null(char **p,int n);

extern int serv_port[1000];

int login_serv(int sockfd,FILE *fp,char **p);

typedef struct t_args{
  int skfd;
  char *path;
  
}T_args;

void *thread_get_serv(void *arg);

void *thread_put_serv(void *arg);

#define MAX_EVENTS 20  



