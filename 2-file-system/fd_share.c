#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>

// 父子进程共享所有的file descriptor
main(){
    char str[4]="asd";
    pid_t pid=fork();
    if(pid==0){
        str[0]='b';
        printf("子进程中str=%s\n",str);
        write(STDIN_FILENO, &"aaa\n", 4);
        printf("子进程中str指向的首地址:%x\n",(unsigned int)str);


        pid_t bc_pid;
        char *const spawn_argv[] = {"bc" , NULL};
        int ret = posix_spawnp(&bc_pid, "bc", NULL, NULL, spawn_argv, NULL);
        if (ret != 0 ) {
            printf("spawn bc program error: %u\n", ret);
            exit(-1);
        }
    }
    else{
        sleep(1);
        printf("父进程中str=%s\n",str);
        printf("父进程中str指向的首地址:%x\n",(unsigned int)str);
    }
    wait(NULL);
}