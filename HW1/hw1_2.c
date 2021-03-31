#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	
	printf("Main process id : %d\n", getpid());
	
	pid_t pid = fork(); // fork0
	if(pid == 0) {
		// Child
		printf("Fork0, I'm the child %d, my parent is %d\n", getpid(), getppid());
	} else if(pid > 0) {
		// Parent
		waitpid(pid, NULL, 0);
	} else {
		printf("*** Error: fork0 failed!!!\n");
	}
	
	for(int i = 0; i < 2; i++) {
		if(pid == 0) {
			pid = fork(); // fork1
			if(pid == 0) {
				// Child
				printf("Fork1, i = %d, I'm the child %d, my parent is %d\n", i, getpid(), getppid());
			} else if(pid > 0) {
				// Parent
				waitpid(pid, NULL, 0);
			} else {
				printf("*** Error: fork1 failed!!!\n");
			}
			
			pid = fork(); // fork2
			if(pid == 0) {
				// Child
				printf("Fork2, i = %d, I'm the child %d, my parent is %d\n", i, getpid(), getppid());
			} else if(pid > 0) {
				// Parent
				waitpid(pid, NULL, 0);
			} else {
				printf("*** Error: fork2 failed!!!\n");
			}
		} else if(pid > 0){
			pid = fork(); // fork3
			if(pid == 0) {
				// Child
				printf("Fork3, i = %d, I'm the child %d, my parent is %d\n", i, getpid(), getppid());
			} else if(pid > 0) {
				// Parent
				waitpid(pid, NULL, 0);
			} else {
				printf("*** Error: fork3 failed!!!\n");
			}
		} else if(pid > 0) {
			// Parent
			waitpid(pid, NULL, 0);
		} else {
			printf("*** Error: fork0 failed!!!\n");
		}
	}
	
	return 0;
}
