#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
int main()
{
    int pipe_fd[2];
	int status;
    char pathname[128];
    getcwd(pathname,128);
	char * argv[] = {"hello",0};
	char run_pro[64];
	sprintf(run_pro,"%s/%s",pathname,"hello");
	printf("pa%s.\n",run_pro);
    char * argv_1[] = {"hello2",0};
	char run_pro_1[64];
	sprintf(run_pro_1,"%s/%s",pathname,"hello2");
	printf("pa%s.\n",run_pro_1);
	pipe(pipe_fd);
    printf("pa%s.\n",run_pro);
    pid_t child_1, child_2;
		if((child_2 = fork())==0){
            printf("child2\n");
            close(pipe_fd[0]);
            close(1);
            dup2(pipe_fd[1],1);
            close(pipe_fd[1]);
            execvp(run_pro,argv);
        }
        else{
//            close ( pipe_fd[0]);
 //           close ( pipe_fd[1]);
            printf("child1\n");
           close(pipe_fd[1]);
           close(0);
             dup2(pipe_fd[0],0);
         close(pipe_fd[0]);
           execvp(run_pro_1,argv_1);
            waitpid ( child_2 , &status , 0 );
        }
      //  waitpid(child_1,&status,0);

   /*     printf("child1\n");
        close(pipe_fd[1]);
        close(0);
        dup2(pipe_fd[0],0);
        close(pipe_fd[0]);
        execvp(run_pro_1,argv_1);*/
}
