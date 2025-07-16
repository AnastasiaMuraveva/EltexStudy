#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(){
    const char str[] = "String from file"; // Строка для записи в файл
    int str_size = sizeof(str); // Размер строки для записи
    char read_str[2]; // Строка для чтения
    int fd; // Дескриптор
    fd = open("output.txt", O_RDWR | O_CREAT, 0644);
    if(fd < 0){
        perror("Open error\n");
        exit(EXIT_FAILURE);
    }
    if(write(fd, str, str_size) < 0){
        perror("Write error\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i <= str_size; i++){
        if(lseek(fd, str_size - i, SEEK_SET) < 0){
            perror("Lseek error\n");
            exit(EXIT_FAILURE);
        }
        if(read(fd, read_str, 1) < 0){
            perror("Read error\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", read_str);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}