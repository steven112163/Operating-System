#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	
	printf("Main process id : %d\n", getpid());
	
	pid_t pid = fork(); // fork1
	
	if(pid == 0) {
		// Child
		printf("Fork1, I'm the child %d, my parent is %d\n", getpid(), getppid());
		
		pid = fork(); // fork2
		
		if(pid == 0) {
			// Child
			printf("Fork2, I'm the child %d, my parent is %d\n", getpid(), getppid());
		} else if(pid > 0) {
			// Parent
			waitpid(pid, NULL, 0);
		} else {
			printf("*** Error: fork2 failed!!!\n");
		}
	} else if(pid > 0) {
		// Parent
		wait(NULL);
	} else {
		printf("*** Error: fork1 failed!!!\n");
	}
	
	pid = fork(); // fork3
	
	if(pid == 0) {
		// Child
		printf("Fork3, I'm the child %d, my parent is %d\n", getpid(), getppid());
	} else if(pid > 0) {
		// Parent
		waitpid(pid, NULL, 0);
	} else {
		printf("*** Error: fork3 failed!!!\n");
	}
	
	return 0;
}
