#include <stdio.h>
#include <unistd.h>	// fork,pid_t
#include <stdlib.h>	// execlp, exit
#include <time.h>	// time
#include <sys/wait.h> 	// waitpid
#include <string.h>	// strlen, strsep

#define MAXLINE 300
#define MAXARG 10
#define KST 9 		// korea standard time
#define CYN "\x1B[36m"  // ANSI COLOR 
#define GRN "\x1b[32m"
#define RESET "\033[0m"

void printPrompt(){
  char cwd[200];
  char hostname[100];
  char time_str[15];
  time_t t;
  struct tm *info;
  
  time(&t);
  info = gmtime(&t);
  getcwd(cwd,sizeof(cwd));
  gethostname(hostname,sizeof(hostname));

  info->tm_hour = (info->tm_hour + KST)%24;
  strftime(time_str, 15,"[%I:%M:%S %p]", info);
  printf("%s" GRN " %s@%s" RESET CYN "(%s)"RESET"$" ,
		  time_str,hostname,getenv("USER"),cwd);
}

int parseInput(char* str, char** parsed){
  for(int i = 0 ; i < MAXARG ; i++){
    parsed[i] = strsep(&str, " ");
    if(parsed[i] == NULL) break;
    if(strlen(parsed[i])==0) i--;
  }
  if(!strcmp(parsed[0],"exit")) return 1;
  if(!strcmp(parsed[0],"cd")){
    chdir(parsed[1]);
    return 2;
  }
  return 0;
}

int main(){
  char buf[MAXLINE];
  char *parsed[MAXLINE];
  int status;
  pid_t pid;
  
  printPrompt();
  while(fgets(buf,MAXLINE,stdin)!=NULL){ 
    if(buf[strlen(buf)-1] = '\n')
      buf[strlen(buf)-1] = 0;
    int tag = parseInput(buf,parsed);
    if(tag==1){
      printf("Good bye\n");
      exit(0);   
    }
    if(tag==2){
      printPrompt();
      continue;
    }
    if((pid = fork())<0){
      fprintf(stderr,"fork error");
      printf("fork error\n");
    }else if (pid == 0 ){
      execvp(parsed[0],parsed);    
      fprintf(stderr,"couldn't execute: %s \n",buf);
      exit(127);
    }
    if((pid = waitpid(pid,&status,0))<0){
      fprintf(stderr,"waitpid error\n");
      printf("child end");
    }
    printPrompt();
  }
  exit(0);
}
