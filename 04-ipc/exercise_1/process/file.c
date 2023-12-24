#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include<dirent.h>
#include <sys/stat.h>  

#include "file.h"

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


float get_dir_size(const char*pathname)
{
  struct stat s_buf;
  stat(pathname,&s_buf);
  float size = 0;
  if(S_ISDIR(s_buf.st_mode)) {
      struct dirent *entry;
      int parent_dir_len = strlen(pathname);
      DIR *dir = opendir(pathname);
    	while((entry = readdir(dir))) {
        if (!(strcmp(entry->d_name,".") & strcmp(entry->d_name,".."))) {
          continue;
        }
        char *fullpath = malloc(parent_dir_len + strlen(entry->d_name) + 2);
        sprintf(fullpath, "%s/%s", pathname, entry->d_name);
        size += get_dir_size(fullpath);
      }
  } else {
    size += (float)s_buf.st_size;
  }
  return size;
}

void Produce(struct Buf *buf, char *dirpath) {
	// get memory
	void *addr = shmat(shm_id, 0, 0);
	buf = (struct Buf *)addr;  

	struct dirent *entry;
	DIR *dir = opendir(dirpath);

	if (dir == NULL)
	{
		perror("Directory cannot be opened!");
		exit(0);
	}
	int parent_dir_len = strlen(dirpath);

	int j = 0;
	while ((entry = readdir(dir)) != NULL) {	
		if (!(strcmp(entry->d_name,".") & strcmp(entry->d_name,".."))) {
		continue;
		}

		Sem_Wait(empty);
		Sem_Wait(mutex);
		memset(&(buf->paths[buf->curr]), "\0", MAX_LINE);
		sprintf(&(buf->paths[buf->curr]), "%s/%s", dirpath, entry->d_name);
		buf->curr += 1;
		Sem_Signal(mutex);
		Sem_Signal(full);
		if (j >= N_CONSUMER) {
			usleep(10);
		}
		j++;
	}

	for (int j =0; j < N_CONSUMER; j++) {  // 当produce的signal小于consumer进程时，需要增加这一行来保证consumer进程能正常退出。
		Sem_Wait(empty);  // 阻塞等待子进程完成
		Sem_Signal(full);
	}

	printf("producer done!\n");
	//取消对共享内存段的连接
	shmdt(addr);
	exit(0);  
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

			float p_total_size = 0.0;
			char path[MAX_LINE];
			memset(&path, 0, MAX_LINE);
			while(1){
                Sem_Wait(full);
                Sem_Wait(mutex);
				if (buf-> curr == 0) {  // no data to consume, exit
					Sem_Signal(mutex);
					Sem_Signal(empty);
					break;
				}
                buf->curr -= 1;
				strcpy(&path, &(buf->paths[buf->curr]));
				printf("%d-th:consume: %s\n", i, buf->paths[buf->curr]);
                Sem_Signal(mutex);
				p_total_size += get_dir_size(path);
				printf("%d-th:consume: %s, size: %f\n", i, path, p_total_size);
                Sem_Signal(empty);
				memset(&path, "\0", MAX_LINE);
			}

			Sem_Wait(mutex);
			buf->count += p_total_size;
			Sem_Signal(mutex);

			printf("%d-th consumer done!\n", i);
			//取消对共享内存段的连接
			shmdt(addr);
			exit(0);
        }
    }
}

int main() {
	// 注意这里内存分配一定要大于所需的，否则会出现未知bug
	shm_id = shmget(SHMKEY, 1024, IPC_CREAT | SHM_MODE); //创建共享内存
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
	bf->count = 2.0;
	empty = semget(SEM_EMPTY, 1, IPC_CREAT | SEM_MODE);
	full = semget(SEM_FULL, 1, IPC_CREAT | SEM_MODE);
	mutex = semget(MUTEX, 1, IPC_CREAT | SEM_MODE);
	semctl(empty, 0, SETVAL, MAX_BUFFER_SIZE);
	semctl(full, 0, SETVAL, 0);
	semctl(mutex, 0, SETVAL, 1);

	int pid = fork();
	if (pid < 0) {
		perror("创建Producer进程失败\n");
		exit(1);
	} else if(pid == 0){
		Produce(bf, &"/root/now/linux-study/");
	} else {
		Consume(bf);
		while (wait(0) != -1);
		Sem_Wait(mutex);
		void *addr;
		addr = shmat(shm_id, 0, 0);
		bf = (struct Buf *) addr;
		printf("total file size is: %f\n", bf->count);
		Sem_Signal(mutex);

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
}




