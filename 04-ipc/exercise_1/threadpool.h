#include <pthread.h>
#include <semaphore.h>
// functions
void pool_init(int);
void pool_submit(void (*fn_pt)(void *p), void *param);
void pool_destroy(void);
void execute(void (*fn_pt)(void *p), void *param);


// data structures
void *worker(void *param);
struct Task {
    int tid;
    void (*fn_pt)(void *p);
    struct Task *next;
    void *param; 
};

struct ThreadPool {
    struct Task *head;
    struct Task *tail;
    pthread_mutex_t mutex;
    sem_t semaphore;
};