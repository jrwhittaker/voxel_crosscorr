
#include <stdio.h>

#define ERREX(str) (fprintf(stdout, "\n** %s\n",str),exit(1))

int get_arg(int argc, char* argv[], char* argstr, int strlen, char** arg);
//int get_arg_array(int argc, char* argv[], char* argstr, int strlen, char** arg[]);
int get_arg_array(int argc, char* argv[], char* argstr, int strlen, char* arg);
