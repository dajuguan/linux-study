#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>

#define MAX_LINE 100
// 父子进程共享所有的file descriptor
main(){
    char str[4]="asd";
    int num_read;
    char buf[MAX_LINE];
    pid_t pid=fork();
    int PARENT_IN_FD = dup(STDIN_FILENO);
    if(pid==0){
        str[0]='b';
        printf("子进程中str=%s\n",str);
        write(STDIN_FILENO, &"aaa\n", 4);
        printf("子进程中str指向的首地址:%x\n",(unsigned int)str);

        
        while(num_read = read(PARENT_IN_FD, &buf, MAX_LINE)) {
            printf("child read: %s", buf);  // when parent read, this will not be able to read
            //  each file descriptor in the child refers to the
            //   same open file description (see open(2)) as the corresponding
            //   file descriptor in the parent.  This means that the two file
            //   descriptors share open file status flags, file offset, and
            //   signal-driven I/O attributes
        }

        // pid_t bc_pid;
        // char *const spawn_argv[] = {"bc" , NULL};
        // int ret = posix_spawnp(&bc_pid, "bc", NULL, NULL, spawn_argv, NULL);
        // if (ret != 0 ) {
        //     printf("spawn bc program error: %u\n", ret);
        //     exit(-1);
        // }
    }
    else{
        close(STDIN_FILENO);  // 只是关闭了fd,不是stream
        while(num_read = read(STDIN_FILENO, &buf, MAX_LINE)) {
            if (num_read == -1) {
                perror("read wrong\n");
                break;
            }
            printf("parent read: %s", buf);
        }
        printf("父进程中str=%s\n",str);
        printf("父进程中str指向的首地址:%x\n",(unsigned int)str);
    }
    wait(NULL);
}