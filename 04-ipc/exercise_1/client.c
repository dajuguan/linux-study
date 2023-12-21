#include<stdio.h>

#include "threadpool.h"

struct Param {
    char *path;
};

void worker_fn(void *param) {
    struct Param *p = (struct Param *)param;
    printf("get path: %s\n", p->path);
}

int main() {
    int thread_num = 2;
    pool_init(thread_num);
    for (int i=0; i< 10; i++) {
        struct Param p;
        p.path = "aaa";
        pool_submit(&worker_fn, &p);
    }
    pool_destroy();
}