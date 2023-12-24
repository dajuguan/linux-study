#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

__thread int count = 0;

void other() {
    count += 2;
}

void *add(void *arg) {
    int n = *(int *)arg;
    count = n;
    for (int i=0; i< 10000; i++) {
        other();
    }
    printf("final count in thread %d is: %d\n", n, count);

}


int main() {
    pthread_t pid_1;
    pthread_t pid_2;
    int n[] = {1, 2};
    pthread_create(&pid_1, NULL, add, (void *)n);
    pthread_create(&pid_2, NULL, add, (void *)(n+1));
    pthread_join(pid_1, NULL);
    pthread_join(pid_2, NULL);
    return 0;
}