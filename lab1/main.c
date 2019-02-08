#include "shell.h"


int main()
{
	hist_begin = 0;
	hist_end = 0;
	hist_num = 0;
	while(1){
			get_prompt();
			int read_s = get_command();
			if(read_s == -1)
				return 0;
			else if(read_s == 1){
				printf("error instruction\n");
				continue;
			}
			else if(read_s == 2)
				continue;
	}
	return 0;
}
