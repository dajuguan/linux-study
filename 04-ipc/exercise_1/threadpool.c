#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

struct ThreadPool *pool;
pthread_t parent_t;
int tid = 0;

void pool_init(int num_threads) {
    pool = (struct ThreadPool*) malloc(sizeof(struct ThreadPool));
    pool->head = NULL;
    pool->tail = NULL;
    pthread_mutex_init(&pool->mutex, NULL);
    sem_init(&pool->semaphore, 0, num_threads);
    for (int i=0; i < num_threads; i++){
        pthread_create(&parent_t, NULL, worker, (void *)i);
    }
}

// insert a task to the end of the dequeue
int enqueue(struct Task * t){
    if (pool->head == NULL) {
        pool->head = t;
        pool->tail = t;
    } else {
        pool->tail->next = t;
        pool->tail = t;
    }

    sem_post(&pool->semaphore);
    return 0;
}

// pop a task from the head of the dequeue
struct Task * dequeue() {
    struct Task *curr_task;
    curr_task = pool->head;
    if (curr_task == NULL) {
        return NULL;
    }
    pool->head = curr_task->next;
    return curr_task;
}


void pool_submit(FN_PT fn_pt, void *param) {
    pthread_mutex_lock(&pool->mutex);
    struct Task *t = (struct Task*) malloc(sizeof(struct Task));
    t->tid = tid;
    t->fn_pt = fn_pt;
    t->next = NULL;
    t->param = param;

    tid++;

    enqueue(t);

    pthread_mutex_unlock(&pool->mutex);
    
}

void pool_destroy(char *path) {
    pthread_join(parent_t, NULL);
    printf("All work is done!\nTotal file size in dir:%s is: %u bytes.\n", path, pool->count);
}

int execute(FN_PT fn_pt, void *param) {
    return (* fn_pt)(param);
}


// data structures
void *worker(void *param) {
    int thread_id = (int) param;
    int num_tasks = 0;
    int count_in_thread = 0;
    printf("starting thread %u...\n", thread_id);
    while (1)
    {
        sem_wait(&pool->semaphore);
        pthread_mutex_lock(&pool->mutex);
        struct Task *t;
        t = dequeue();
        pthread_mutex_unlock(&pool->mutex);
        if (t == NULL) {
            break;
        } 
        printf("thread %u get task: %d\n", thread_id, num_tasks);
        ++num_tasks;
        count_in_thread += execute(t->fn_pt, t->param);
        sem_post(&pool->semaphore);
    }
    pthread_mutex_lock(&pool->mutex);
    pool->count += count_in_thread;
    pthread_mutex_unlock(&pool->mutex);
    pthread_exit(0);
}