#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#define STR_SIZE 1024

#define ERROR_FUNC(str, status, ret) if(ret == status) { \
                                        perror(str); \
                                        exit(EXIT_FAILURE); \
                                    }

int input_string(char *str);

// Обработка введенной строки для первого процесса
int parser(char ***argv, int *argc, char *str);

// Обработка введенной строки для второго процесса
int add_parser(char ***argv, int *argc, char *str);

int free_all(char ***argv, int *argc);

#endif