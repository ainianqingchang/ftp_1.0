#include "unp.h"
#include <stdio.h>
#include "ftp.h"
void overtime();

int login(int sockfd);


int main(int argc,char **argv){

  struct sockaddr_in servaddr,cliaddr;

  char command[MAXLINE];
  char command_fix[MAXLINE];
  char **cmd=(char **)malloc(sizeof(char *));
  char **path=(char **)malloc(sizeof(char *));
  socklen_t clilen;
  int sockfd;
  int m;
  int pass;
 
  
  if(argc!=2)
    err_sys("useage:command<ip address>");
    
  bzero(command, MAXLINE);
  bzero(command_fix, MAXLINE);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  sockfd = Socket(AF_INET,SOCK_STREAM, 0);
  
  signal(SIGALRM, overtime);
  clilen=(socklen_t)(sizeof(servaddr));
  alarm(10);
  if((m=connect(sockfd, (SA *)&servaddr, clilen))<0)
    {
      err_sys("connect error!");
      return -1;
    }
  else
    alarm(0);

  pass=login(sockfd);
  if(pass==1){
    printf("=======welcome to ftp server======\n");
    printf("=======user the command as:======\n");
    printf("\n\n\t(1)\"ls\"  shows the files\n");
    printf("\t(2)\"pwd\"  show work dir\n ");
    printf("\t(3)\"get\"  send the file to the server\n");
    printf("\t(4)\"put\"  down the file form the server\n");
    printf("\t(5)\"cd\"   change the workdir\n");
    printf("\t(6)\"bye\"   exit the ftp server\n");
    printf("=========================================\n\n");
    
  for(;;)
    {
      printf("ftp>"); fflush(stdout);
      readline(STDIN_FILENO, command, MAXLINE);
      strncpy(command_fix, command, strlen(command)-1);
      write(sockfd, command_fix, strlen(command_fix));
      if(strcmp(command_fix, "bye")==0) break;
      else if(strcmp(command_fix, "ls")==0||strncasecmp(command_fix, "ls ", 3)==0)
	ls_cli(sockfd);
      else if(strcmp(command_fix, "pwd")==0||strncasecmp(command_fix, "pwd ",4)==0)
	pwd_cli(sockfd);
      else if(strncasecmp(command_fix, "get ", 4)==0)
	{
	  cmdstr(command_fix, cmd, path);
	  if(*path==NULL)
	    write(STDOUT_FILENO, "1", 1);
	  get_cli(sockfd,*path);
	}
      else if(strncasecmp(command_fix, "put ", 4)==0){
	cmdstr(command_fix,cmd,path);
	put_cli(sockfd,*path);
      }
	
      else if(strncasecmp(command_fix, "cd ", 3)==0)
	cd_cli(sockfd);
      else
	{
	  char string[100];
	  sprintf(string, "command invalid.\n");
	  write(STDOUT_FILENO, string, strlen(string));
	}
      bzero(command,MAXLINE);
      bzero(command_fix, MAXLINE);
    }
  }
}

void overtime()
{
  return;
}

int login(int sockfd)
{

  int pass=0;
  char user[100],passwd[100];
  char logn[MAXLINE];
  char ch[2];
  bzero(logn, MAXLINE);
  sprintf(logn, "login");
  send(sockfd,logn,strlen(logn),0);
  for(int i=0;i<3;i++){
    bzero(user, 100);
    bzero(passwd, 100);
    printf("user name:"); fflush(stdout);
    scanf("%s",user);
    printf("password:");  fflush(stdout);
    scanf("%s",passwd);
    sprintf(logn, "%s %s\n",user,passwd);
    send(sockfd,logn ,strlen(logn) ,0 );
    alarm(5);
    recv(sockfd, ch,2,0);
    alarm(0);
    if(strncasecmp(ch,"OK",2)==0)
      {
	pass=1;
	break;
      }
    else if(strncasecmp(ch, "NO", 2)==0)
      {
	printf("login incorect..\n"); fflush(stdout);
      }
  }
  if(pass==0)
    printf("you have try 3 times..."); fflush(stdout);
  return pass;
}



















