#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define UNIX_STREAM_PATH "/tmp/bc_unix_steam"

#define MAXLINE 4096
int handle(int sockfd) {
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        printf("receive buf: %s\n", buf);
        write(sockfd, buf, n);
    }

    if (n < 0 && errno == EINTR) {
        goto again;
    } else if (n < 0) {
        printf("str_echo read error: %s\n", strerror(errno));
        exit(-1);
    }
}

int start_listen(){
    int ret;
    int serv_fd, conn_fd;
    pid_t child_pid;
    socklen_t cliaddr_len;
    struct sockaddr_un serv_addr, cli_addr;
    
    serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        printf("server socket error: %s\n", strerror(errno));
        return -1;
    }

    unlink(UNIX_STREAM_PATH);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, UNIX_STREAM_PATH, sizeof(serv_addr.sun_path) - 1);

    ret = bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret) {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    ret = listen(serv_fd, 1);
    if (ret) {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }
    printf("server start listening...\n");

    while(1) {
        cliaddr_len = sizeof(cli_addr);
        if ((conn_fd = accept(serv_fd, (struct sockaddr *)&cli_addr, &cliaddr_len)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                printf("accept error: %s\n", strerror(errno));
                return -1;
            }
        }
        printf("got a client connection.\n");

        if ((child_pid = fork()) == 0) {
            /* child close listening socket and process connected socket */
            close(serv_fd);
            handle(conn_fd);
            exit(0);
        }
        /* parent close connected socket */
        close(conn_fd);
    }
}

int main() {
    return start_listen();
}