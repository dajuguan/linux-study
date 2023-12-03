#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>  /* for lockf(), lseek() */
#include <fcntl.h>
#include <sys/file.h>

#define NUM_PROCESSES 5
#define DATA_SIZE 50

void writeToFile(int processID, FILE *file) {
    for (int i = 0; i < DATA_SIZE; ++i) {
        // fprintf(file, "Process %d: Data %d: fd %d\n", processID, i, fileno(file));
        for (int j = 0; j < 2048; ++j) {
            fprintf(file, "%d", processID);
        }
        fprintf(file, "\n");
    }
}

int main() {
    FILE *file;
    file = fopen("output.txt", "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    pid_t pid;

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            printf("pid: %d, F_TEST:%d\n", i, lockf(file->_fileno, F_TEST, 0));
            while (lockf(file->_fileno, F_TEST, 0) == -1) {
                sleep(1);
            }
            printf("pid: %d, F_TEST: %d\n", i, lockf(file->_fileno, F_TEST, 0));
            lockf(file->_fileno, F_LOCK, 0);
            printf("pid: %d, get lock, ", i);
            writeToFile(i, file);
            lockf(file->_fileno, F_ULOCK, 0);
            printf("pid: %d, release lock\n", i);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        wait(NULL);
    }

    fclose(file);

    return 0;
}
