#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#define STR_SIZE 1024

int input_string(char *str);

int parser(char ***argv, int *argc, char *str);

int free_all(char ***argv, int *argc);

#endif