#include <pthread.h>
#include <semaphore.h>
// functions
typedef int (*FN_PT)(void *p);
void pool_init(int);
void pool_submit(FN_PT fn_pt, void *param);
void pool_destroy(char *path);
int execute(FN_PT fn_pt, void *param);


// data structures
void *worker(void *param);
struct Task {
    int tid;
    FN_PT fn_pt;
    struct Task *next;
    void *param; 
};

struct ThreadPool {
    struct Task *head;
    struct Task *tail;
    pthread_mutex_t mutex;
    sem_t semaphore;
    int count;
};