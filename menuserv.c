#include "ftp.h"
#include <dirent.h>
#include "unp.h"
#include <stdio.h>


int login_serv(int sockfd,FILE *fp,char **p){

  char **tp;
 char temp[MAXLINE];
  char **pass1=(char **)malloc(sizeof(char *));
  char **pass2=(char **)malloc(sizeof(char *));
  char receive[MAXLINE];
  char **cmdstr1=(char **)malloc(sizeof(char *));
  char **cmdstr2=(char **)malloc(sizeof(char *));
  int pass=0;
  for(int j=0;j<3;j++){
	bzero(receive, MAXLINE);
	if((recv(sockfd,receive,MAXLINE,0))<=0) break;
	tp=p;
	cmdstr(receive, cmdstr1,cmdstr2);

	  while(1){
	    if (*tp!=NULL)
	      {
		bzero(temp, MAXLINE);
		sprintf(temp,"%s",*tp);
		cmdstr(temp, pass1, pass2);
		if(strcmp(*pass1,*cmdstr1)==0&&strcmp(*pass2,*cmdstr2)==0)
		  {
		    pass=1;
		    break;
		  }
		
		tp++;

	      }
	    else
	      break;
	  }
	if(pass==1){send(sockfd, "OK", 2, 0); break;}
	if(pass==0){send(sockfd, "NO", 2, 0);}
      }
      return pass;
}
      
int serv_port[1000];

void overtime1(){
  
  char *temp="transport port connect overtime!";
  write(STDOUT_FILENO, temp, strlen(temp));
  return;
}

void ls_serv(int sockfd,char *path)
{
  // 浏览文件的ls命令的服务器函数  打开path 读取目录 传递到客户端套接字上
  DIR *dp;
  struct dirent *dirp;
  struct stat statbuf;
  char temp[MAXLINE];
  char endmsg[MAXLINE];
  bzero(temp, MAXLINE);
  bzero(endmsg, MAXLINE);
  
  if((dp=opendir(path))==NULL)
    {
      sprintf(temp,"can't open %s %s\n",path,strerror(errno));
      write(sockfd, temp, strlen(temp));
    }
  else
    while((dirp=readdir(dp))!=NULL) //打开文件目录项数据。
      {
	lstat(dirp->d_name, &statbuf);
	if(S_ISDIR(statbuf.st_mode)){
	  if(strcmp(dirp->d_name, ".")==0||strcmp(dirp->d_name, "..")==0)
	    continue;
	  sprintf(temp, "%s/\n",dirp->d_name);
	  write(sockfd, temp, strlen(temp));
	}
	else
	  {
	  sprintf(temp, "%s\n",dirp->d_name);
	  write(sockfd, temp, strlen(temp));
	  }
	bzero(temp, MAXLINE);
      }
  sprintf(endmsg, "LSEND\n");
  write(sockfd, endmsg,strlen(endmsg));
  
  return;
}

void pwd_serv(int sockfd){
  // pwd命令服务端。 把getpwd()函数调用返回值写入到传入套接字。
  char temp[MAXLINE];
  char *print="PWDEND\n"; 
  bzero(temp,MAXLINE);
  char *str=getcwd(NULL, 0);
  if(strcmp(str,print)==0)
    sprintf(temp,"/%s\n",str);
  else
    sprintf(temp, "%s\n",str);
  send(sockfd, temp, strlen(temp),0);
  send(sockfd, print,strlen(print),0);
}

void get_serv(int sourcefd,char *path){
  //get命令的服务端 传入套接字和目录 打开服务器的path路径文件（打不开返回错误信息。）
  struct sockaddr_in cliaddr,servaddr;
  int listenfd,sockfd;
  int n,port;
  int i=0;
  socklen_t clilen;
  char receive[MAXLINE];
  char print[20];
  listenfd=socket(AF_INET, SOCK_STREAM, 0);
  while(serv_port[i]==1)
      i++;
  port=5000+i;
  serv_port[i]=1;


  //sprintf(print,"port:%d",port);
  //write(STDOUT_FILENO, print, strlen(print));
  
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(port);  //修改端口为传送文件端口。
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  
 
  bind(listenfd,(SA *)&servaddr, sizeof(servaddr));
  listen(listenfd, LISTENQ);
  send(sourcefd, &port,sizeof(int),0);
  sockfd=accept(listenfd, (SA *)&cliaddr, &clilen);
 

  
  int fd=open(path,O_RDONLY);

  bzero(receive, MAXLINE);
  while((n=read(fd, receive, MAXLINE))>0)//传送文件
    {
      send(sockfd, receive, n,0);
      bzero(receive, MAXLINE);
    }
  
  close(sockfd);
  close(listenfd);
  close(fd);
  // serv_port[i]=0;  
}

void put_serv(int sourcefd,char *path)
{
  //put 命令服务端 传入控制套接字 路径  打开相应路径 接受来自客户端的连接传送文件

  int fd;
  struct sockaddr_in servaddr,cliaddr;
  int listenfd,sockfd;
  int n,port;
  int i=0;
  socklen_t clilen;
  char receive[MAXLINE];
  char temp[MAXLINE];
  listenfd=socket(AF_INET, SOCK_STREAM, 0);
  while(serv_port[i]==1)
      i++;
  port=5000+i;
  serv_port[i]=1;

 
  bzero(&servaddr, sizeof(servaddr));
  bzero(temp, MAXLINE);
  
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(port);
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

   
  if(bind(listenfd,(SA *)&servaddr, sizeof(servaddr))<0)
    {
      perror("bind wrong");
      return;
    }
  listen(listenfd, LISTENQ);

  send(sourcefd, &port, sizeof(int), 0);
  sockfd=accept(listenfd, (SA *)&cliaddr, &clilen);


  

  if((fd=open(path,O_WRONLY|O_CREAT|O_EXCL|O_TRUNC,S_IRUSR|S_IWUSR))==-1)
    {
      perror("create file error:");
      send(sourcefd,"NO",2,0);
      sprintf(temp,"%s\n",strerror(errno));
      send(sockfd, temp, strlen(temp),0);
      goto done;
    }
  else
    send(sourcefd, "OK", 2,0);

  bzero(receive, MAXLINE);
  while((n=recv(sockfd, receive, MAXLINE,0))>0) //传送数据。。。
    {
      write(fd, receive, n);
      bzero(receive, MAXLINE);
    }
  done:
  close(sockfd);
  close(listenfd);
  close(fd);
 
  //  serv_port[i]=0;
}

void cd_serv(int sockfd,char *path){
  // cd命令 服务端 通过chdir()函数调用完成其功能。
  char *temp="CDEND\n"; //cd命令完成
  char *temp1="\nCDEND\n";
  char *err;
  if(path!=NULL)
    if(chdir(path)==-1)
      {
	err=strerror(errno);  //打开失败的情况下，向客户端返回 失败信息。
	send(sockfd,err , strlen(err),0);
	send(sockfd, temp1, strlen(temp1),0);
      }
      send(sockfd,temp,strlen(temp), 0);
 
}

