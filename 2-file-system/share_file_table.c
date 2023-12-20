// 父子进程复制 file_table, 之后各自有各自的offset
// 同一进程中dup，则是共享file_table, 只有一个offset
#include<stdio.h>
#include <fcntl.h>

int main() {
    int fd = open("./test.txt", O_RDWR | O_CREAT);
    write(fd, "1234", strlen("1234"));
    int pid = fork();
    if (pid == 0) {
        for (int i=0; i<100; i++) {
            write(fd, "abcd", strlen("abcd"));
        }
        exit(0);
    }
    for (int i=0; i<100; i++) {
        write(fd, "5678", strlen("5678"));
    }

}