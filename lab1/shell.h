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


struct passwd *pwd;
char pathname[128];
char history_instr[100][128];
int hist_begin;
int hist_end;
int hist_num;

void get_prompt(char *prompt);
int get_command(char *command);
int clean_blank_1(char *input);
int clean_blank_2(char *input);
int lss();
int cds(char *parameter);
int historys(char *parameter);
int pipe_redict(int process_num,char process[][64],bool file_num,char *file_name);

