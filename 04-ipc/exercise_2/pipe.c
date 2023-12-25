#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int pair[2];
    int len = 3;
    char buf[3] = "12\n";

    pipe(pair);
    write(pair[0], &buf, len);
    buf[0] = '0';
    write(pair[0], &buf, len);
    read(pair[1], &buf, len);
    printf("rw success 0: %s", buf);

    read(pair[0], &buf, len);  // blocked, because write(pair[0]) can only be read on pair[1]
    printf("rw success 1: %s", buf);
    
    buf[0] = '2';
    write(pair[1], &buf, len);
    read(pair[0], &buf, len);
    printf("rw success 2: %s", buf);
}