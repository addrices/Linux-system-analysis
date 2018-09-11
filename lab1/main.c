#include "shell.h"
#define MAX_PROMPT 256
#define MAX_COMMAND 128

int main(){
	char prompt[MAX_PROMPT];
	char command[MAX_COMMAND];
	while(1){
			get_prompt(prompt);
			printf("%s ",prompt);
			int read_s = get_command(command);
			if(read_s == -1)
				return 0;
			else if(read_s == 0)
			{
				printf("error instruction\n");
				continue;
			}
			else if(read_s == 2)
				continue;
/*			pid_t pid = fork();
			if(pid == -1)
				printf("fork error\n");
			else if(pid != 0){
				int status;
				waitpid(pid, &status, 0);
			}
			else{
//				printf("welcome child process!\n");
			}*/
	}
	return 0;
}
