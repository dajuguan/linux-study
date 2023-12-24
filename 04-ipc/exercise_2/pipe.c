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
    read(pair[1], &buf, len);
    printf("rw success: %s", buf);
    buf[0] = '2';
    write(pair[1], &buf, len);
    read(pair[0], &buf, len);
    printf("rw success: %s", buf);
}