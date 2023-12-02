#include <stdio.h>
#include <fcntl.h> 
#include <string.h>
int main() {
    int fd = open("./test.txt", O_RDWR | O_CREAT);
    int fd1 = dup(fd);
    write(fd,  "12", strlen("12"));
    write(fd1,  "567", strlen("567"));
    write(fd,  "89\n", strlen("89\n"));
}