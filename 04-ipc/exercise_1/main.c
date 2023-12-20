#include "threadpool.h"

struct Param {
    char *path;
};

void worker_fn(void *param) {
    struct Param *p = (struct Param *)param;
    printf("get path: %s", p->path);
}

int main() {
    int thread_num = 8;
    pool_init(thread_num);
    struct Param p1;
    p1.path = "aaa";
    pool_submit(&worker_fn, &p1);
    pool_destroy();
}