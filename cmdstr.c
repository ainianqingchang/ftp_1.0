#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#define MAXLINE 4096

void cmdstr(char *str,char **str1,char **str2)
{
  char *p,*q;
  int count=0;
  p=str;
  q=p;
  char *str_t1,*str_t2;
  while((*p)!=' '&&(*p)!='\0')
    {
      p++;
      count++;
    }
  str_t1=(char *)malloc(count+1);
  *str1=str_t1;
  while((*q)!=' '&&(*q)!='\0')
    {
      *str_t1=*q;
      str_t1++;
      q++;
    }

  *str_t1='\0';

  if((*q)=='\0'){
    str_t2=NULL;
    return;
  }

  
  while((*p)==' '&&p!='\0')
    p++;
  q=p;
  count=0;
  while(*p!=' '&&*p!='\0')
    {
      p++;
      count++;
    }
  str_t2=(char *)malloc(count+1);
  *str2=str_t2;
  while(*q!=' '&&*q!='\0')
    {
      *str_t2=*q;
      str_t2++;
      q++;
    }
  *str_t2='\0';  
  
  
}


void readpawd(char **p,FILE *fp){
  char temp[MAXLINE];
  bzero(temp,MAXLINE);
  if(fgets(temp,MAXLINE,fp)!=NULL)
    {
      *p=(char *)malloc(strlen(temp));
      strncpy(*p,temp,strlen(temp));
      p++;
    }
  return;
  
  }

void set_null(char **p,int n)
{
  for(int i=0;i<n;i++)
    {
      *p=NULL;
      p++;
    }
}



















