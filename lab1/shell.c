#include "shell.h"
const int max_command_len = 64;
const int max_para_len = 64;

void get_prompt()
{
    extern char pathname[128];	//
	extern struct passwd *pwd;
	pwd = getpwuid(getuid());
    getcwd(pathname,128);
	printf("$%s",pathname);
    return;
}

int get_command()
{
	extern char history_instr[100][128];
	extern int hist_begin;
	extern int hist_end;
	extern int hist_num;
	char input[max_command_len];
	char parameter[max_para_len];
//	char exit_[4] = {'e','x','i','t'};
//	char cd[2] = {'c','d'};
//	char history[7] = {'h','i','s','t','o','r','y'};
//	char run[3] = {'.','/'};
//	char ls[2] = {'l','s'};
	fgets(input,64,stdin);
	if (clean_blank_2(input) == 1)
		return 2;
	else;
	A:
	strcpy(history_instr[hist_end],input);
	hist_end = (hist_end + 1) % 100;
	if (hist_num < 100)
		hist_num++;
	if (hist_begin == hist_end)
		hist_begin = (hist_begin + 1) % 100;
	if (strncmp(input,"exit",4) == 0 && strlen(input) == 4){		//由于命令可能不止只有exit四个字符
		printf("exit\n");
		return -1;
	} else if (strncmp(input,"cd",2) == 0){
		//printf("cd\n");
		strncpy(parameter,input+2,strlen(input)-2);
		parameter[strlen(input)-2] = '\0';
		//printf("pa:%s.\n",parameter);
		if (clean_blank_1(parameter) == 1){
			//printf("pa:NULL.\n");
			parameter[0] = ' ';
			parameter[1] = '\0';
			return cds(parameter);
		} else {
			//printf("pa:%s.\n",parameter);
			parameter[0] = '/';
			return cds(parameter);
		}	
	} else if (strncmp(input,"ls",2) == 0){
		//printf("ls\n");
		return lss();
	} else if (strncmp(input,"history",7) == 0){
		//printf("history\n");
		strncpy(parameter,input+7,strlen(input)-7);
		parameter[strlen(input)-7] = '\0';
		//printf("pa:%s.\n",parameter);
		if (clean_blank_1(parameter) == 1){
			parameter[0] = ' ';
			parameter[1] = '\0';
			return historys(parameter);
		} else if (parameter[1] == '-' && parameter[2] == 'c'){
			parameter[0] = 'i';
			return historys(parameter);
		} else {
			parameter[0] = 'n';
			return historys(parameter);
		}
	} else if (input[0] == '!'){
		if (input[1] == '!' && strlen(input) == 2){
			if (hist_num == 0){
				printf("history is empty\n");
				return 0;
			} else {
				printf("%s\n",history_instr[(hist_end + 98)%100]);
				strcpy(input,history_instr[(hist_end + 98)%100]);
				//printf("%s\n",input);
				goto A;
			}
		} else {
			strcpy(parameter,input+1);
			for (int i = 1; i < hist_num;i++)
				if(strncmp(history_instr[(hist_end-i+99)%100],parameter,strlen(parameter)) == 0){
					strcpy(input,history_instr[(hist_end-i+99)%100]);
					printf("%s\n",input);
					goto A;
				}
			printf("<%s> can't found\n",parameter);
			return 0;
		}
	}
	else if(strncmp(input,"./",2) == 0){
		printf("run\n");
		int process_num = 1;
		char process[3][64];
		bool file_num = false;
		char file_name[64];
		int divide_num = 0;
		int divide[5] = {-1,0,0,0,0};
		bool flag = true;
		bool back = false;
		for (int i = 0; i < strlen(input); i++){
			if (input[i] == '|'){
				if (flag == false)
					return 1;
				process_num++;
				if (process_num > 3){
					printf("too much pipe\n");
					return 0;
				}
				divide_num++;
				divide[divide_num] = i;
			} else if (input[i] == '>'){
				if(file_num == true){
					printf("too much redirect\n");
					return 0;
				}
				file_num = true;
				divide_num++;
				divide[divide_num] = i;
				flag = false;
			}
		}
		if (input[strlen(input)-1] == '&'){
			back = true;
			input[strlen(input)-1] = '\0';
		}
		divide_num++;		
		divide[divide_num] = strlen(input);
		int n = 0;
		for (int i = 0; i < divide_num ;i++){
			if (i == divide_num-1 && file_num > 0){
				strncpy(file_name,input+divide[i]+1,divide[i+1]-divide[i]-1);
				file_name[divide[i+1]-divide[i]-1] = '\0';
				clean_blank_1(file_name);
				file_name[0] = 'f';
				//printf("%s.\n",file_name);
			} else {
				strncpy(process[n],input+divide[i]+1,divide[i+1]-divide[i]-1);
				process[n][divide[i+1]-divide[i]-1] = '\0';
				clean_blank_1(process[n]);
				process[n][0] = 'p';
				if(process[n][1] != '.' || process[n][2] != '/')
					return 1;
				strncpy(process[n]+1,process[n]+3,strlen(process[n]+3)+1);
				//printf("%s.\n",process[n]);
				n++;
			}
		}
		if(back){
			pid_t pid = fork();
			if(pid == 0)
			{
				pipe_redict(process_num,process,file_num,file_name);
				exit(0);
			}
		}
		else
			pipe_redict(process_num,process,file_num,file_name);
		return 0;
	} else
		return 1;
	return 0;
}

int pipe_redict(int process_num,char process[][64],bool file_num,char *file_name)
{
	extern char pathname[128];
	if (process_num == 1){							//只有一个进程需要执行
		char * argv[] = {process[0]+1,0};			//当前进程所需要的管道参数
		char run_pro[64];
		sprintf(run_pro,"%s/%s",pathname,process[0]+1);
		int status = 0;
		pid_t pid = fork();							//fork出一个进程给子程序运行
		if (pid == 0){
			if (file_num == false){
				if(execvp(run_pro,argv) == -1)
					printf("run error\n");
			} else {
				int fd = open(file_name + 1,O_WRONLY);
				if(fd == -1)
					printf("worry file\n");
				else{
					dup2(fd,1);						//将输出文件重定位一下
					execvp(run_pro,argv);
				}
			}
		} else {
			waitpid(pid,&status,0);
		}
	} else {
		int pipe_fd[2];
  		int status;
		char * argv[] = {process[0]+1,0};
		char run_pro[64];
		sprintf(run_pro,"%s/%s",pathname,process[0]+1);
 		pipe(pipe_fd);
		pid_t child1,child2;
/*
需要fork出2个子进程，第一个子进程的输出和第二个子进程的输入相连
将第一个程序放在第一个子进程中执行，同时在第二个子进程中递归执行之
后的程序，可以形成一个树状的进程树。
*/
		if ((child1 = fork()) != 0){		
			if ((child2 = fork()) == 0){	
				close(pipe_fd[0]);
  	    		close(1);
				dup2(pipe_fd[1],1);
				close(pipe_fd[1]);
				execvp(run_pro,argv);
			} else {
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				waitpid(child2,&status,0);
			}
			waitpid(child1, &status,0);
		} else {
			close(pipe_fd[1]);
            close(0);
			dup2(pipe_fd[0],0);
			close(pipe_fd[0]);
			pipe_redict(process_num-1,process+1,file_num,file_name);
			exit(0);
		}
	}
	return 0;
}

int clean_blank_2(char *input)
{
	char cpystr[max_command_len];
	int i = 0;
	int e = strlen(input)-2;
	for (; i < strlen(input);i++)
		if (input[i] == ' ')
			;
		else
			break;
	for (; e > 0; e--)
		if(input[e] == ' ')
			;
		else
			break;
	if (e == i)
		return 1;
	strncpy(cpystr,input+i,e-i+1);
	strncpy(input,cpystr,e-i+1);
	input[e-i+1] = '\0';
	return 0;
}

int clean_blank_1(char *input)
{
	char cpystr[max_command_len];
	int i = 0;
	int e = strlen(input);
	if (e == i)
		return 1;
	e--;
	//printf("e %d i %d", e , i);
	for (; i < strlen(input);i++)
		if(input[i] == ' ')
			;
		else
			break;
	for (; e > 0; e--)
		if(input[e] == ' ')
			;
		else
			break;
//	if (e == i)
//		return 1;
	strncpy(cpystr,input+i,e-i+1);
	strncpy(input+1,cpystr,e-i+1);
	input[e-i+2] = '\0';
	return 0;
}

/*int cds(char *parameter)
{
    extern char pathname[128];
	extern struct passwd *pwd;
	char *cd_path;
	if (parameter[0] == ' ' && parameter[1] == '\0'){
		cd_path = malloc(strlen(pwd->pw_dir)+1);
		strcpy(cd_path,pwd->pw_dir);
		cd_path[strlen(pwd->pw_dir)] = '\0';
	} else if (parameter[0] == '/'){
		cd_path = malloc(strlen(pathname)+strlen(parameter)+1);
		strcpy(cd_path,pathname);
		strncpy(cd_path+strlen(pathname),parameter,strlen(parameter));
		cd_path[strlen(pathname)+strlen(parameter)] = '\0';
	}
	if (chdir(cd_path) != 0)
		printf("cd: %s:error\n",cd_path);
	free(cd_path);
	return 0;
}*/

int cds(char *parameter)
{
    extern char pathname[128];
	extern struct passwd *pwd;
	char *cd_path;
	if (parameter[0] == ' ' && parameter[1] == '\0'){		//直接到当前用户的根目录
		cd_path = pwd->pw_dir;
	}
	else if (parameter[0] == '/'){							//将路径和需要打开的目录拼接起来
		cd_path = malloc(strlen(pathname)+strlen(parameter)+1);
		strcpy(cd_path,pathname);
		strncpy(cd_path+strlen(pathname),parameter,strlen(parameter));
		cd_path[strlen(pathname)+strlen(parameter)] = '\0';	
	}
	if (chdir(cd_path) != 0)
		printf("cd: %s:error\n",cd_path);
	free(cd_path);
	return 0;
}

int lss()
{
	extern char pathname[128];
	DIR *dir = opendir(pathname);		//打开当前地址
	if(dir == NULL)
		printf("dir NULL\n");
	struct dirent *list = NULL;			
	while((list = readdir(dir))){		//将该地址下的所有文件逐个打印出来
		if(strcmp(list->d_name,".") == 0|| strcmp(list->d_name,"..") == 0)
			continue;
		printf("%s  ",list->d_name);
	}
	printf("\n");
	return 0;
}

int historys(char *parameter)
{
	extern int hist_begin;
	extern int hist_end;
	extern int hist_num;
	extern char history_instr[100][128];
	if(parameter[0] == ' '){
		for (int i = 0; i < hist_num; i++)
			printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);
		return 0;
	} else if (parameter[0] == 'i'){
		hist_begin = 0;
		hist_end = 0;
		hist_num = 0;
		printf("history instr clear\n");
		return 0;
	} else if(parameter[0] == 'n'){
		int num = 0;
		//printf("<%d>\n",strlen(parameter));
		for (int i = 1; i < strlen(parameter);i++)
			if (parameter[i] >= '0' && parameter[i] <= '9')
				num = parameter[i] - '0' + num*10;
			else
				return 1;
		printf("%d\n",num);
		if (num > hist_num){
			printf("num is over\n");
			for (int i = 0; i < hist_num; i++)
				printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);	
		} else {
			for (int i = 0; i < num; i++)
				printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);
		}
		return 0;
	}
	return 1;
}