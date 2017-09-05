#include "ftp.h"
#include <dirent.h>
#include "unp.h"
#include <stdio.h>

int serv_port[1000];

void overtime1(){
  
  char *temp="transport port connect overtime!";
  write(STDOUT_FILENO, temp, strlen(temp));
  return;
}


void ls_cli(int sockfd){
  // ls 命令客户端  通过传入的套接字 接受来自服务器的返回数据 显示到标准输出。
  char temp[MAXLINE];
  bzero(temp,MAXLINE);
  char teph[MAXLINE];
  char *ch="\t";
  char *newline="\n";
  while(readline(sockfd, temp, MAXLINE)>0){
    if(strcmp(temp, "LSEND\n")==0)
     	break;
      
    else
      {
	strncpy(teph, temp,strlen(temp)-1);
	//printf("%9s",teph);
	//printf("the lengh is:%d",strlen(temp)-1);
	printf("%s \t",teph);
	fflush(stdout);
	bzero(temp, MAXLINE);
	bzero(teph, MAXLINE);
      }
  }
  printf("\n"); fflush(stdout);
  
}

void pwd_cli(int sockfd)
{
  // pwd客户端  接受来自 服务器的返回数据 并且把他们显示在客户端的标准输出。
  char *print="PWDEND\n";
  char temp[MAXLINE];
  int n;
  bzero(temp, MAXLINE);
  while((n=readline(sockfd, temp, MAXLINE)>0)){
    if(strcmp(temp, "PWDEND\n")==0)
      break;
    write(STDOUT_FILENO, temp, strlen(temp));
    bzero(temp,MAXLINE);
  }
}

void get_cli(int sourcefd,char *path){
  // get指令的客户  打开一个本地文件 传送到服务器的当前的文件目录里面。
  struct sockaddr_in servaddr,cliaddr,taddr;
  int fd,listenfd,sockfd;
  socklen_t clilen,tlen;
  char temp[MAXLINE];
  int m,n;
  int port;
  signal(SIGALRM, overtime1);
  
  bzero(temp, MAXLINE);
  
  bzero(&servaddr, sizeof(servaddr));
  alarm(5);
  recv(sourcefd, &port, sizeof(int), 0);
  alarm(0);
  getpeername(sourcefd, (SA *)&taddr, &tlen);

 
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=taddr.sin_addr.s_addr;
  servaddr.sin_port=htons(port);

  
  sockfd=socket(AF_INET, SOCK_STREAM, 0);
  alarm(18);
  if((m=connect(sockfd, (SA *)&servaddr, sizeof(servaddr)))<0)
    {
      perror("server transport port error");
      return;
    }
  else
    alarm(0);


  fd=open(path, O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
 
   
  while((n=recv(sockfd, temp,MAXLINE, 0))>0)
    {
      write(fd, temp, n);
      bzero(temp, MAXLINE);
    }
 
  
  if(n==0) ;

  else
    perror("receive file error:");
   
  close(sockfd);
  close(fd);
   
}


void put_cli(int sourcefd,char *path){
  // 客户端传送到服务器处理函数
  struct sockaddr_in servaddr,cliaddr,taddr;
  int fd,listenfd,sockfd;
  socklen_t clilen,tlen;
  char temp[MAXLINE];
  int m,n;
  int port;
  signal(SIGALRM, overtime1);
  
  bzero(temp, MAXLINE);
  bzero(&servaddr, sizeof(servaddr));
  alarm(5);
  recv(sourcefd, &port, sizeof(int), 0);
  alarm(0);
  getpeername(sourcefd, (SA *)&taddr, &tlen);

  
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=taddr.sin_addr.s_addr;
  servaddr.sin_port=htons(port);

  
  sockfd=socket(AF_INET, SOCK_STREAM, 0);
 
  alarm(18);
  if((m=connect(sockfd, (SA *)&servaddr, sizeof(servaddr)))<0) //连接套接字 错误处理。
    {
      perror("server transport port error");
      return;
    }
  else
    alarm(0);
  if((fd=open(path, O_RDONLY))<0)
    {
      perror("open error:");
      return;
    }
  char ch[2];
  alarm(5);
  recv(sourcefd,ch, 2, 0);
  alarm(0);
  if(strncasecmp(ch,"NO" , 2)==0) {
    alarm(5);
    readline(sockfd, temp, MAXLINE);
    alarm(0);
    printf("error form service:%s",temp); fflush(stdout);
    goto done; 
  }

 
  bzero(temp, MAXLINE); 
  while((n=read(fd, temp, MAXLINE))>0)
    {
      send(sockfd, temp, n, 0);
      bzero(temp, MAXLINE);
    }
  if(n==0);
  else
    perror("file send error.");
     
 done:
  close(sockfd);
  close(fd);
}
void cd_cli(int sockfd)
{

  // cd 命令客户端 如果服务器 返回cd 出错信息 把它们送到标准输出。
  char temp[MAXLINE];
  char print[MAXLINE];
  int n;
  bzero(temp, MAXLINE);
  bzero(print, MAXLINE);
  while((n=readline(sockfd,temp,MAXLINE))>0)
    {
      if(strcmp(temp,"CDEND\n")==0)
	break;
      else
	write(STDOUT_FILENO, temp, strlen(temp));
      
      bzero(temp, MAXLINE);
    }
}
