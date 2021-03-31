#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define true 1
#define false 0

int main(void)
{
	const char delim[3] = " \n";
	int should_run = 1; /*flag to determine when to exit program*/

	while(should_run) {
		printf("osh>");
		fflush(stdout);
		
		/**
		* your code!
		* After reading user input, the step are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will not invoke wait()
		*/

		char* arg[MAX_LINE/2+1]; /*command line arguments*/
		for(int i = 0; i < MAX_LINE/2+1; i++)
			arg[i] = NULL;

		char inputBuffer[MAX_LINE];
		fgets(inputBuffer, MAX_LINE, stdin);

		/*Break buffer into commands*/
		int length = 0;
		char* token = strtok(inputBuffer, delim);
		while(token != NULL && length < MAX_LINE/2+1) {
			arg[length++] = token;
			if(strcmp(arg[0], "exit") == 0) { /*Exit the shell*/
				should_run = 0;
				break;
			}
			token = strtok(NULL, delim);
		}
		if(!should_run) /*Exit the shell*/
			break;

		/*Erase the "&" and end argv*/
		int match = false;
		if(strcmp(arg[length-1], "&") == 0) {
			arg[--length] = NULL;
			match = true;
		}
		if(!match){
			if(length == MAX_LINE/2)
				arg[length-1] = NULL;
			else
				arg[length] = NULL;
		}

		/*Process command*/
		pid_t pid = fork(); /*Generate a child*/

		if(pid < 0) {
			/*Error*/
			printf("*** Error: fork() failed!!!\n");
			exit(1);
		} else if(pid == 0) {
			/*Child*/
			if(execvp(arg[0], arg) < 0){
				printf("*** Error: execvp() failed!!!\n");
				exit(1);
			}
		} else {
			/*Parent*/
			if(!match){ /*If there is no '&' in the command, parent proccess has to wait the child*/
				waitpid(pid, NULL, 0);
			}
		}
	}

	return 0;
}

