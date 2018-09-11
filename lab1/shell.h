#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <pwd.h>
#include <stdlib.h>
#include <dirent.h>

struct passwd *pwd;
char pathname[128];

void get_prompt(char *prompt);
int get_command(char *command);
int clean_blank_1(char *input);
int clean_blank_2(char *input);
int lss();
int cds(char *parameter);
