#include <stdio.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static char* args[10];
static char buffer[1024];
static char args1[10];
static int zombie=0;
static int n = 0;
pid_t pid;
 
static void clean_up(int n)
{
	int i;
	for (i = 0; i < n; ++i) {
		wait(NULL); 
	}
}

static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}
 
static void split(char* cmd, int opcase)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');
	int i = 0;
		
	if(next==NULL && n==0 && opcase){
		args[i] = cmd;
		++i;
		char* end = strchr(cmd,'\0');
		cmd = end;
	}
	while(next != NULL) {
		next[0] = '\0';
		args[i] = cmd;
		++i;
		cmd = skipwhite(next + 1);
		next = strchr(cmd, ' ');
	}

	if (cmd[0] != '\0' && cmd[0] !='&') {
		args[i] = cmd;
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i; 
		zombie = 0;
	}
	else if (cmd[0] =='&')
	zombie = 1;
 
	args[i] = NULL;

}
static int command(int input, int first, int opcase)
{
	int pfd[2];
 
	/* Invoke pipe */
	pipe(pfd);	
	pid = fork();
 
	if (pid == 0) {
	
	switch(opcase){
	case 0:
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); // If child fails
		break;

	case 1:  //'|'
		if (first == 1 && input == 0) {
			// First command
			dup2( pfd[1], STDOUT_FILENO );
		} else if (first == 0 && input != 0) {
			// Second command
			dup2(input, STDIN_FILENO);
		}
 
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); // If child fails
		break;

	case 2:  //'>'
		if (first == 1 && input == 0) {
			// First command
			dup2( pfd[1], STDOUT_FILENO );

		} else if (first == 0 && input != 0) {
			// Second command
			int out = open(args[0],O_WRONLY|O_CREAT,0666);
			dup2(input,STDIN_FILENO);
		while(read(input, buffer, sizeof(buffer))!=0);		
			int wbytes = write(out,buffer,sizeof(buffer));
		}
 
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); // If child fails
		break;

	case 3:  //'<'
		if (first == 1 && input == 0) {
			// First command
		int out = open(args1,O_RDONLY|O_CREAT,0666);
			dup2( out, STDIN_FILENO );
		} 
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); // If child fails
		break;
 	}
}
	if (input != 0) 
		close(input);
 
	// Nothing more needs to be written
	close(pfd[1]);

	if (!first)
		close(pfd[0]);

	return pfd[0];
}
static int run(char* cmd, int input, int first, int opcase)
{
	split(cmd,opcase);
	
	if (args[0] != NULL) {
		if (!strcmp(args[0], "exit")) 
			exit(0);
		n += 1;
		return command(input, first,opcase);
	}
	
	return 0;
}


int main(){
while(1){	
	printf(">");
	fflush(NULL);
		
	int pfd[2];
	char input_line[100]; 	
	
	if(!fgets(input_line,100,stdin))
		return 0;
	
	char* cmd = input_line;
	int opcase=0;
	char* bar,*fuck;
	fuck = strchr(cmd,'\0');
	if(bar = strchr(cmd, '|')) 	opcase = 1;
	else if(bar = strchr(cmd, '>'))	opcase = 2;
	else if(bar = strchr(cmd, '<'))	opcase = 3;
	
	if(opcase==3){ 		////////file name
		if(*(bar+1) == ' ')
		strncpy(args1,bar+2,(fuck-bar-3));
		else
		strncpy(args1,bar+1,(fuck-bar-2));
	}
	int input = 0;

	if(opcase){
	*bar = '\0';	
	input = run(cmd, input, 1,opcase);
	cmd = bar + 1;
	}
	input = run(cmd, input, 0,opcase);
	if(zombie!=1)
	clean_up(n);
	else {	zombie=0;
		continue;
		pid_t p1;
		do{
			p1=waitpid(-1, NULL, WNOHANG);
		} while (p1==0);
		}
	n=0;
	}
return 0;
}