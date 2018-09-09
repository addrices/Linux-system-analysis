#include "shell.h"

int main(){
//		type_prompt();
//		read_command();
		pid_t pid = fork();
		if(pid == -1)
			printf("fork error\n");
		else if(pid != 0){
			int status;
			waitpid(pid, &status, 0);
		}
		else{
			printf("welcome child process!\n");
		}
		return 0;
}
