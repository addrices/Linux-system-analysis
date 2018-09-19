#include "shell.h"
#define MAX_PROMPT 256
#define MAX_COMMAND 128

int main(){
	char prompt[MAX_PROMPT];
	char command[MAX_COMMAND];
	char process[3][64];
	extern char pathname[128];
	hist_begin = 0;
	hist_end = 0;
	hist_num = 0;
	int process_num = 0;
	int file_num = 0;
	char file_name[64];
	while(1){
			get_prompt(prompt);
			printf("%s ",prompt);
			int read_s = get_command(command);
			if(read_s == -1)
				return 0;
			else if(read_s == 1)
			{
				printf("error instruction\n");
				continue;
			}
			else if(read_s == 2)
				continue;
	}
	return 0;
}
