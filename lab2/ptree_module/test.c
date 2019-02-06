#include <syscall.h>
#include <stdio.h>
#include <sys/types.h>

struct prinfo {
        pid_t parent_pid;
        pid_t pid;
        pid_t first_child_pid;
        pid_t next_sibling_pid;
        long state;
        long uid;
        char comm[64];
	int num;
};

int main()
{
	struct prinfo buffer[256];
	int b;
	int num;
	int ret = syscall(325,&buffer,&b);	//按深度优先遍历的顺序将进程数放到该数组中。b表示其在递归树中的深度。
	printf("test syscall hello\n");		
	printf("%d process\n",b);
	for(int i = 0; i < (64>b?b:64); i++){
		num = buffer[i].num;
		//printf("num %d",num);
		for(int b = 0; b < num; b++)
			printf("  ");
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", buffer[i].comm, buffer[i].pid, buffer[i].state, buffer[i].parent_pid, buffer[i].first_child_pid, buffer[i].next_sibling_pid, buffer[i].uid);
	}
	return 0;
}
