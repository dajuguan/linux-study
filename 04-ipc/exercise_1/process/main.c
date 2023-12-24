// 简单的生产者消费者模型
// 使用共享内存 + 信号量
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>

#include "main.h"

void Sem_Wait(int sem_id) //P操作
{
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1; //Wait(-1)
	sb.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sb, 1) < 0) //P操作，判断是否成功
	{
		perror("P操作失败!\n"); //输出错误信息
		exit(1);
	}
}


void Sem_Signal(int sem_id) //V操作 
{
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1; //Signal(+1)
	sb.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sb, 1) < 0) //V操作，判断是否成功
	{
		perror("V操作失败!\n");
		exit(1);
	}
}

void Produce(struct Buf *buf) {
    for (int i=0; i< N_PRODUCER; i++) {
        int pid = fork();
        if (pid < 0) {
			perror("创建Producer进程失败\n");
			exit(1);
		}

        if (pid == 0 ) { // child process
            // get memory
            void *addr = shmat(shm_id, 0, 0);
            buf = (struct Buf *)addr;  

            for (int j = 0; j < 15; j++) {
                Sem_Wait(empty);
                Sem_Wait(mutex);
                buf->data[buf->curr] = j;
                buf->curr += 1;
                Sem_Signal(mutex);
                Sem_Signal(full);
				if (j >= N_CONSUMER) {
					// usleep(10);
				}
            }

			for (int j =0; j < N_CONSUMER; j++) {  // 当produce的signal小于consumer进程时，需要增加这一行来保证consumer进程能正常退出。
				Sem_Wait(empty);  // 同步等待Consumer进程完成，以防Sem_Signal丢失
				Sem_Signal(full);
			}

			//取消对共享内存段的连接
			shmdt(addr);
			exit(0);  // 这里必须退出，否则会继续执行main进程之类的程序
        }
    }
}

void Consume(struct Buf *buf) {
    for (int i=0; i< N_CONSUMER; i++) {
        int pid = fork();
        if (pid < 0) {
			perror("创建Consumer进程失败\n");
			exit(1);
		}

        if (pid == 0 ) { // child process
            // get memory
            void *addr = shmat(shm_id, 0, 0);
            buf = (struct Buf *)addr;  

			while(1){
                Sem_Wait(full);
                Sem_Wait(mutex);
				if (buf-> curr == 0) {  // no data to consume, exit
					Sem_Signal(mutex);
					Sem_Signal(empty);
					break;
				}
                buf->curr -= 1;
				printf("%d-th:consume: %d at curr: %d\n", i, buf->data[buf->curr], buf->curr);
                Sem_Signal(mutex);
                Sem_Signal(empty);
			}

			//取消对共享内存段的连接
			shmdt(addr);
			exit(0);  
        }
    }
}

int main() {
	shm_id = shmget(SHMKEY, MAX_BUFFER_SIZE, IPC_CREAT | SHM_MODE); //创建共享内存
	if (shm_id < 0) {
		perror("共享内存创建失败\n");
		exit(1);
	}

	struct Buf *bf; //定义一个循环队列指针
	//将共享内存段连接到一个进程的地址空间中
	void *addr;
	addr = shmat(shm_id, 0, 0);
	bf = (struct Buf *) addr;
	if (bf == (void *) -1) {
		perror("连接共享内存失败!\n");
		exit(1);
	}
	bf->curr = 0;
	empty = semget(SEM_EMPTY, 1, IPC_CREAT | SEM_MODE);
	full = semget(SEM_FULL, 1, IPC_CREAT | SEM_MODE);
	mutex = semget(MUTEX, 1, IPC_CREAT | SEM_MODE);
	semctl(empty, 0, SETVAL, MAX_BUFFER_SIZE);
	semctl(full, 0, SETVAL, 0);
	semctl(mutex, 0, SETVAL, 1);

	Produce(bf);
	Consume(bf);
	while (wait(0) != -1);

	//取消对共享内存的连接
	shmdt(addr);
	//删除共享内存段
	shmctl(shm_id, IPC_RMID, 0);
	//撤销信号量
	shmctl(full, IPC_RMID, 0);
	shmctl(empty, IPC_RMID, 0);
	shmctl(mutex, IPC_RMID, 0);

	return 0;
	
}




