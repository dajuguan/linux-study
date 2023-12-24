#define MAX_BUFFER_SIZE 10
//共享内存和信号量：当前用户的进程可读写
#define SHM_MODE 0600
#define SEM_MODE 0600
//信号量的键值
#define SEM_FULL 1     //信号量 1 --- full
#define SEM_EMPTY 2    //信号量 2 --- empty 
#define MUTEX 3        //信号量 3 --- 互斥量
//共享内存的键值
#define SHMKEY 75

struct Buf
{
    int curr;  // current index in the buffer
    int data[MAX_BUFFER_SIZE];
    int done;
} *buf;


const int N_CONSUMER = 8;  // number of consumer
const int N_PRODUCER = 1;
int shm_id;  // share memory id
int shm_id; //shmget函数返回的共享内存标识符
int full, empty, mutex; //semget函数返回的信号量标识符


void Consume(struct Buf *buf);
void Produce(struct Buf *buf);