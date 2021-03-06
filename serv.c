#include "unp.h"
#include <stdio.h>
#include "ftp.h"
#include <sys/epoll.h>

int serv_port[1000];


int main(int argc,char **argv)
{


  daemon(1, 0);
  
  struct sockaddr_in servaddr,cliaddr;
  int listenfd,sockfd;
  int n;
  socklen_t clilen;
  char receive[MAXLINE];
  char temp[MAXLINE];

  struct epoll_event ev, events[MAX_EVENTS];
  int  nfds, epfd,connfd;
  char **cmdstr1=(char **)malloc(sizeof(char *));
  char **cmdstr2=(char **)malloc(sizeof(char *));
 

  
  
  pthread_t a_thread;
  T_args *ts;

  char *p[100];
  char **tp;
  FILE *fp;
  int passwdfd;
 
  tp=p;
  if((passwdfd=open("p.txt",O_RDONLY))<0)
    { perror("no password file."); return 0; }
      
  fp=fdopen(passwdfd, "r");
  int pass=0;
  set_null(tp, 100);
  readpawd(tp,fp);
   
  
  memset(serv_port, 0, 1000);
  listenfd=socket(AF_INET, SOCK_STREAM, 0);
  bzero(receive, MAXLINE);
  bzero(&servaddr,sizeof(servaddr));
  
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(SERV_PORT);
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

  bind(listenfd,(SA *)&servaddr, sizeof(servaddr));
  listen(listenfd, LISTENQ);

 
  
  // newline

  epfd=epoll_create(1000);
  ev.data.fd=listenfd;
  ev.events=EPOLLIN|EPOLLET;
  epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

  for( ; ; )
   {
      nfds = epoll_wait(epfd,events,20,500);
      for(int i=0;i<nfds;++i)
	{
	  if(events[i].data.fd==listenfd) //有新的连接
	    {
	      connfd = accept(listenfd,(SA *)&cliaddr, &clilen); //accept这个连接
	      ev.data.fd=connfd;
	      ev.events=EPOLLIN|EPOLLET;
	      epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev); //将新的fd添加到epoll的监听队列中
		            
	    }
	  else if( events[i].events&EPOLLIN ) //接收到数据，读socket
	    {
	      sockfd=events[i].data.fd;    //读
	      //md为自定义类型，添加数据

	      int pass=0;
	   
      
	   
	      *cmdstr2=NULL;
	      *cmdstr1=NULL;
	      bzero(receive, MAXLINE);
	      if((n=recv(sockfd, receive, MAXLINE,0))>0)
		{
		  if(strcmp(receive, "login")==0)
		    {
		      pass=login_serv(sockfd,fp,tp);
		      if(pass==0){
			ev.data.fd=sockfd;
			ev.events=EPOLLIN|EPOLLET;
			epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);
			close(sockfd);
		      }
		    }
		    
		  write(STDOUT_FILENO, receive,strlen(receive));
		  if(strncasecmp(receive,"ls ",3)==0||strcmp(receive, "ls")==0)
		    {
		      cmdstr(receive, cmdstr1, cmdstr2);
		      if(*cmdstr2!=NULL)
			ls_serv(sockfd,*cmdstr2);
		      else
			ls_serv(sockfd, ".");
		    }
		  else if(strcmp(receive,"pwd")==0||strncasecmp(receive, "pwd ", 4)==0)
		    pwd_serv(sockfd);
      
		  else if(strcmp(receive, "get")==0||strncasecmp(receive, "get ", 4)==0)
		    {
		      cmdstr(receive, cmdstr1, cmdstr2);
		      ts=malloc(sizeof(struct t_args));
		      ts->skfd=sockfd;
		      ts->path=*cmdstr2;
		      pthread_create(&a_thread, NULL, thread_get_serv,(void *)ts);
		    }
		  else if(strcmp(receive, "put")==0||strncasecmp(receive, "put ",4)==0)
		    {
		      cmdstr(receive, cmdstr1, cmdstr2);
		      // printf("come here."); fflush(stdout);
		      ts=malloc(sizeof(struct t_args));
		      ts->skfd=sockfd;
		      ts->path=*cmdstr2;
		      pthread_create(&a_thread, NULL, thread_put_serv, (void *)ts);
		  
		    }
		  else if(strcmp(receive,"bye")==0||strncasecmp(receive, "bye ", 4)==0)
		    {
		      ev.data.fd=sockfd;
		      ev.events=EPOLLIN|EPOLLET;
		      epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);
		      close(sockfd);
		    }
		  else if(strncasecmp(receive, "cd ", 3)==0)
		    {
		      cmdstr(receive,cmdstr1,cmdstr2);
		      cd_serv(sockfd,*cmdstr2);
		    }
		  else{
		    char *str1="Do not recognize the command.";
		    write(STDOUT_FILENO,str1,strlen(str1));
		  }
		  bzero(receive, MAXLINE);
		
		}
	                
	  
	        
      
	    }



	}
    // endline
     
   }
}



void *thread_get_serv(void *arg)
{
  T_args *ts;
  ts=(T_args *)arg;
  get_serv(ts->skfd,ts->path);
  pthread_exit(0);
}


void *thread_put_serv(void *arg){
  T_args *ts;
  ts=(T_args *)arg;
  put_serv(ts->skfd, ts->path);
  pthread_exit(0);
}




















