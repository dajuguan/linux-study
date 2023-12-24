#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define UNIX_STREAM_PATH "/tmp/bc_unix_steam"

#define MAXLINE 1024

int conn_2() { // 不可行，因为必须得有read, write
    int ret;
    int conn_fd;
    pid_t child_pid;
    struct sockaddr_un serv_addr;

    char buf[MAXLINE];
    ssize_t num_read;

    int pipefd_stdin[2];
    int pipefd_stdout[2];

    if(pipe(pipefd_stdin) == -1) {
        printf("pipe create error\n");
    };
    if(pipe(pipefd_stdout) == -1) {
        printf("pipe create error\n");
    };

    conn_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (conn_fd < 0) {
        printf("server socket error: %s\n", strerror(errno));
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, UNIX_STREAM_PATH, sizeof(serv_addr.sun_path) - 1);

    ret = connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret) {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    
    dup2(STDOUT_FILENO, pipefd_stdin[0]);
    dup2(conn_fd, pipefd_stdin[1]);
    dup2(conn_fd, pipefd_stdout[0]);
    dup2(STDIN_FILENO, pipefd_stdout[1]);

    // close(pipefd_stdin[0]);
    // close(pipefd_stdin[1]);

    // buf[0] = '3';
    // buf[1] = '/';
    // buf[2] = '4';
    // buf[3] = '\n';
    // num_read = write(STDOUT_FILENO, &buf, 4);
    // if (num_read == -1) {
    //     printf("write error\n");
    //     return -1;
    // }
    while (1) {}
}

int conn(){
    int ret;
    int conn_fd;
    pid_t child_pid;
    struct sockaddr_un serv_addr;

    char buf[MAXLINE];
    ssize_t num_read;

    conn_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (conn_fd < 0) {
        printf("server socket error: %s\n", strerror(errno));
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, UNIX_STREAM_PATH, sizeof(serv_addr.sun_path) - 1);

    ret = connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret) {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }



    child_pid = fork();
    if(child_pid == 0) { //child process for reading
        dup2(STDOUT_FILENO, conn_fd);
        // while((num_read = read(conn_fd, &buf, MAXLINE)) > 0) {
        //     write(conn_fd, &buf, num_read);
        // }
    } else {
        close(STDOUT_FILENO);
        while((num_read = read(STDIN_FILENO, &buf, MAXLINE)) > 0) {
            num_read = write(conn_fd, buf, num_read);
            if (num_read == -1) {
                printf("write error\n");
                return -1;
            }
        }
    }
}

int main() {
    return conn();
    // return conn_2();
}