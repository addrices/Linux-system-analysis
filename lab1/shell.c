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
//		printf("input %s.",input);
	//printf("out %d \n",strlen(input));
	if(strncmp(input,exit,4) == 0 && strlen(input) == 4)
	{
		printf("exit\n");
		return -1;
	}
	else if(strncmp(input,cd,2) == 0)
	{
		printf("cd\n");
		strncpy(parameter,input+2,strlen(input)-2);
		parameter[strlen(input)-2] = '\0';
		printf("pa:%s.\n",parameter);
		if(clean_blank_1(parameter) == 1)
		{
			//printf("pa:NULL.\n");
			parameter[0] = ' ';
			parameter[1] = '\0';
			cds(parameter);
		}
		else{
			//printf("pa:%s.\n",parameter);
			parameter[0] = '/';
			cds(parameter);
		}
		return 1;
	}
	else if(strncmp(input,ls,2) == 0)
	{
		//printf("ls\n");
		lss();
	}
	else if(strncmp(input,history,7) == 0)
		printf("history\n");
	else if(strncmp(input,run,2) == 0)
		printf("./\n");
	else
		return 0;
//	printf("INPUT %s\n",input);
	return 1;
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
	printf("e %d i %d", e , i);
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
	printf("cd call\n");
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
