1.	#include <stdio.h>
2.	#include <unistd.h>		// fork,pid_t
3.	#include <stdlib.h>		// execlp, exit
4.	#include <time.h>		// time
5.	#include <sys/wait.h> 	// waitpid
6.	#include <string.h>		// strlen, strsep
7.	 
8.	#define MAXLINE 300
9.	#define MAXARG 10
10.	#define KST 9 		// korea standard time
11.	#define CYN "\x1B[36m"  // ANSI COLOR 
12.	#define GRN "\x1b[32m"
13.	#define RESET "\033[0m"
14.	 
15.	void printPrompt(){
16.	  char cwd[200];
17.	  char hostname[100];
18.	  char time_str[15];
19.	  time_t t;
20.	  struct tm *info;
21.	 
22.	  time(&t);				// get seconds since 1970/01/01
23.	  info = gmtime(&t);			// convert seconds into Greenwich mean time
24.	  getcwd(cwd,sizeof(cwd));	// get current working directory
25.	  gethostname(hostname,sizeof(hostname));
26.	 
27.	  info->tm_hour = (info->tm_hour + KST) % 24;  // make Korea standard time
28.	  strftime(time_str, 15,"[%I :%M :%S %p]", info); // make string form using struct ‘info’
29.	
30.	  // print time, colored user/host name, and current working directory 
31.	  printf("%s" GRN " %s@%s" RESET CYN "(%s)" RESET "$ ",
32.			  time_str,getenv("USER"),hostname,cwd);
33.	}



34.	int parseInput(char* str, char** parsed){  // split inputs into command and arguments
35.	  for(int i = 0; i < MAXARG; i++){
36.	    parsed[i] = strsep(&str, " ");	// get pointer to first string after blank
37.	    if(parsed[i] == NULL) break;	// until get NULL ptr
38.	  }
39.	  if(!strcmp(parsed[0], "exit"))	// deal with some special commands like “exit”, “cd”
40.	    return 1;
41.	  if(!strcmp(parsed[0], "cd")){
42.	    if(chdir(parsed[1])){
43.	      printf("failed to cd\n");
44.	    }
45.	    return 2;
46.	  }
47.	  return 0;
48.	}
49.	 

50.	int main(){
51.	  char buf[MAXLINE];
52.	  char *parsed[MAXLINE];
53.	  int status;
54.	  pid_t pid;
55.	 
56.	  printPrompt();
57.	  while(fgets(buf, MAXLINE, stdin) != NULL){ //get input
58.	    buf[strlen(buf) - 1] = 0; // input from fgets have ‘next line’, so delete it before parsing 
59.	    int tag = parseInput(buf, parsed);	// then do parsing, return ‘tag’
60.	    if(tag == 1){
61.	      printf("Good bye\n");
62.	      exit(0);   
63.	    }
64.	    if(tag==2){
65.	      printPrompt();
66.	      continue;
67.	    }
68.	    if((pid = fork()) < 0){
69.	      fprintf(stderr, "fork error");
70.	    }else if (pid == 0){
71.	      execvp(parsed[0], parsed);	// run command with arguments by child process   
72.	      fprintf(stderr, "couldn't execute: %s\n", buf); // this will not printed unless having error
73.	      exit(127);
74.	    }
75.	    if((pid = waitpid(pid, &status,0)) < 0){
76.	      fprintf(stderr, "waitpid error\n");
77.	    }
78.	    printPrompt();	// after jobs done, print prompt and wait for input
79.	  } // end of while loop
80.	  exit(0);
81.	}
