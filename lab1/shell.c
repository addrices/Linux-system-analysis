#include "shell.h"
const int max_command_len = 64;
const int max_para_len = 64;

void get_prompt(char *prompt)
{
    extern char pathname[128];
	extern struct passwd *pwd;
	pwd = getpwuid(getuid());
	//printf("pwd %s\n",pwd->pw_dir);
    int length;
    getcwd(pathname,128);
    sprintf(prompt,"$%s",pathname);
    return;
}

int get_command(char *command)
{
	extern char history_instr[100][128];
	extern int hist_begin;
	extern int hist_end;
	extern int hist_num;
	char input[max_command_len];
	char parameter[max_para_len];
	char exit[4] = {'e','x','i','t'};
	char cd[2] = {'c','d'};
	char history[7] = {'h','i','s','t','o','r','y'};
	char run[3] = {'r','u','n'};
	char ls[2] = {'l','s'};
	fgets(input,64,stdin);
	if(clean_blank_2(input) == 1)
		return 2;
	else;
	A:
	strcpy(history_instr[hist_end],input);
	hist_end = (hist_end + 1) % 100;
	if(hist_num < 100)
		hist_num++;
	if(hist_begin == hist_end)
		hist_begin = (hist_begin + 1) % 100;
//		printf("input %s.",input);
	//printf("out %d \n",strlen(input));
	if(strncmp(input,exit,4) == 0 && strlen(input) == 4)
	{
		printf("exit\n");
		return -1;
	}
	else if(strncmp(input,cd,2) == 0)
	{
		//printf("cd\n");
		strncpy(parameter,input+2,strlen(input)-2);
		parameter[strlen(input)-2] = '\0';
		//printf("pa:%s.\n",parameter);
		if(clean_blank_1(parameter) == 1)
		{
			//printf("pa:NULL.\n");
			parameter[0] = ' ';
			parameter[1] = '\0';
			return cds(parameter);
		}
		else
		{
			//printf("pa:%s.\n",parameter);
			parameter[0] = '/';
			return cds(parameter);
		}
		
	}
	else if(strncmp(input,ls,2) == 0)
	{
		//printf("ls\n");
		lss();
	}
	else if(strncmp(input,history,7) == 0)
	{
		//printf("history\n");
		strncpy(parameter,input+7,strlen(input)-7);
		parameter[strlen(input)-7] = '\0';
		//printf("pa:%s.\n",parameter);
		if(clean_blank_1(parameter) == 1)
		{
			parameter[0] = ' ';
			parameter[1] = '\0';
			return historys(parameter);
		}
		else if(parameter[1] == '-' && parameter[2] == 'c')
		{
			parameter[0] = 'i';
			return historys(parameter);
		}
		else
		{
			parameter[0] = 'n';
			return historys(parameter);
		}

	}
	else if(input[0] == '!')
	{
		if(input[1] == '!' && strlen(input) == 2)
		{
			if(hist_num == 0)
			{
				printf("history is empty\n");
				return 0;
			}
			else
			{
				printf("%s\n",history_instr[(hist_end + 98)%100]);
				strcpy(input,history_instr[(hist_end + 98)%100]);
				//printf("%s\n",input);
				goto A;
			}
		}
		else 
		{
			strcpy(parameter,input+1);
			for(int i = 1; i < hist_num;i++)
				if(strncmp(history_instr[(hist_end-i+99)%100],parameter,strlen(parameter)) == 0)
				{
					strcpy(input,history_instr[(hist_end-i+99)%100]);
					printf("%s\n",input);
					goto A;
				}
			printf("<%s> can't found\n",parameter);
			return 0;
		}
	}
	else if(strncmp(input,run,2) == 0)
		printf("./\n");
	else
		return 1;
//	printf("INPUT %s\n",input);
	return 0;
}

int clean_blank_2(char *input)
{
	char cpystr[max_command_len];
	int i = 0;
	int e = strlen(input)-2;
	for(; i < strlen(input);i++)
		if(input[i] == ' ')
			;
		else
			break;
	for(; e > 0; e--)
		if(input[e] == ' ')
			;
		else
			break;
	if(e == i)
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
	//printf("e %d i %d", e , i);
	for(; i < strlen(input);i++)
		if(input[i] == ' ')
			;
		else
			break;
	for(; e > 0; e--)
		if(input[e] == ' ')
			;
		else
			break;
	if(e == i)
		return 1;
	strncpy(cpystr,input+i,e-i+1);
	strncpy(input+1,cpystr,e-i+1);
	input[e-i+2] = '\0';
	return 0;
}

int cds(char *parameter)
{
    extern char pathname[128];
	extern struct passwd *pwd;
	//printf("cd call\n");
	char *cd_path;
	if(parameter[0] == ' ' && parameter[1] == '\0')
	{
		cd_path = malloc(strlen(pwd->pw_dir));
		strcpy(cd_path,pwd->pw_dir);
	}
	else if(parameter[0] == '/')
	{
		cd_path = malloc(strlen(pathname)+strlen(parameter));
		strcpy(cd_path,pathname);
		strncpy(cd_path+strlen(pathname),parameter,strlen(parameter));
	}
	//printf("cd_path %s\n",cd_path);
	if(chdir(cd_path) != 0)
		printf("cd: %s:error\n",cd_path);
	free(cd_path);
	return 0;
}

int lss(){
	extern char pathname[128];
	DIR *dir = opendir(pathname);
	if(dir == NULL)
		printf("dir NULL\n");
	struct dirent *list = NULL;
	while((list = readdir(dir)))
	{	
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
	if(parameter[0] == ' ')
	{
		for(int i = 0; i < hist_num; i++)
			printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);
		return 0;
	}
	else if(parameter[0] == 'i')
	{
		hist_begin = 0;
		hist_end = 0;
		hist_num = 0;
		printf("history instr clear\n");
		return 0;
	}
	else if(parameter[0] == 'n')
	{
		int num = 0;
		printf("<%d>",strlen(parameter));
		for(int i = 1; i < strlen(parameter);i++)
			if(parameter[i] >= '0' && parameter[i] <= '9')
				num = parameter[i] - '0' + num*10;
			else
				return 1;
		printf("%d\n",num);
		if(num > hist_num)
		{
			printf("num is over\n");
			for(int i = 0; i < hist_num; i++)
				printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);	
		}
		else
		{
			for(int i = 0; i < num; i++)
				printf("%d %s\n",i,history_instr[(hist_end-i+99)%100]);
		}
	}
	return 1;
}