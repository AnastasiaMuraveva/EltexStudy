#ifndef __LIST_H__
#define __LIST_H__
#include "drivers.h"

#define INPUT_LEN 30
#define PIPE_NAME_LEN 20

// функция ввода строки
int input_string(char *str, int size);

// функция добавления водителя в список опорного пункта
int add_driver(struct drivers_list **head, struct drivers_list **tail, int fd, int pid);

// функция, которая отправляет дочерним процессам сообщение о завершении,
// ожидает их завершения и освобождает память
void free_all(struct drivers_list **head);

// функция получения дескриптора по pid
int get_fd(struct drivers_list **head, int pid);

// функция отправки задачи дочернему процессу
int send_task(int child_fd, int parent_fd, int time);

// функция получения статуса дочернего процесса
int get_status(int child_fd, int parent_fd);

// функция получения статусов всех дочерних процессов
int get_all_status(struct drivers_list **head, int parent_fd);

#endif