#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#define MAXPROCESS 100
#define MAXSIZE 100
#define TIMESLICE 1

void err_log(const char* buf);


struct PCB
{
    int id;
    int priority;
    int cpuTime;
};

struct PCB PCBarr[MAXPROCESS];
// 循环队列

// typedef struct _Node
// { 
//     struct PCB ready[MAXSIZE];
//     struct Node* next;
// }Node;

typedef struct  _Queue
{
    struct PCB ready[MAXSIZE];
    int front;
    int  rear;
}Queue,*Queueptr;

void initQueue(Queueptr q){
    if(q == NULL)return;
    q->front = 0;
    q->rear = 0;
}

int isFull(Queueptr q){
    return (q->rear + 1)%MAXSIZE == q->front;
}

int isEmpty(Queueptr q){
    return q->rear == q->front;
}

void Push(Queueptr q, struct PCB pro){
    if(isFull(q)){err_log("Push: Queue is full ");return;}

    q->ready[q->rear] = pro;
    q->rear = (q->rear + 1)%MAXSIZE;    
}

void Pop(Queueptr q, struct PCB* pro){
    if(isEmpty(q)){err_log("Pop: Queue is empty");return ;}
    // 内容保留，front 更改
    struct PCB temp = q->ready[q->front];
    q->front = (q->front + 1)%MAXSIZE;
    *pro = temp;
}
void Pop_over(Queueptr q){
    if(isEmpty(q)){err_log("Pop: Queue is empty");return ;}
    q->front = (q->front + 1)%MAXSIZE;
}


void err_sys(const char* buf){
    perror(buf);
    exit(0);
}

void err_log(const char* buf){
    printf("warning! %s",buf);
}

/**
 *@brief:
 *@param :
 *@return : the number of process
 */
int input(){
    int n;
    printf("## 请输入模拟进程数目：");
    scanf("%d",&n);
    return n;
}
/**
 *@brief : init PCB, the id of Process form 1 to n; the priority from 1 to 10;
 *         CPUtime from 1 to 20   
 *@param : n
 *@return :
 */
void init(int n){
    srand(time(0));
    if (n > MAXPROCESS)err_sys("Line39:the number of process overflow:");
    while(n){
        PCBarr[n].id = n;
        PCBarr[n].priority = rand()%9 + 1;
        PCBarr[n].cpuTime = rand()%19 + 1;
        n--;
    }
}

void sort(int n){
    for (int i = 1; i <= n; i++){
        for(int j = i+1; j <= n; j++){
            if(PCBarr[i].priority > PCBarr[j].priority){
                struct PCB temp = PCBarr[i];
                PCBarr[i] = PCBarr[j];
                PCBarr[j] = temp;
            }
        }
    }
}

// 队列
void insertsort(Queueptr q,struct PCB pro){
    int i = 0;
    while((q->front + i)%MAXSIZE != q->rear){
        if(q->ready[q->front + i].priority > pro.priority){
            int j = 0;
            if(isFull(q)){err_log("insertsort Push: Queue is full ");return;}
            
            while((q->rear - j)%MAXSIZE != (q->front + i)%MAXSIZE){
                q->ready[(q->rear -j)%MAXSIZE] = q->ready[(q->rear -j - 1)%MAXSIZE];
                j++;
            }

            q->ready[(q->rear -j)%MAXSIZE] = pro;
            q->rear = (q->rear + 1)%MAXSIZE;
            return;
        }
        i++;
    }
    if(isFull(q)){err_log("insertsort Push: Queue is full ");return;}
    Push(q,pro);
}


void Delete(Queueptr q,int n){
    printf("sss \n");
    if((q->front+n+1)%MAXSIZE == q->rear){q->rear = (q->rear - 1)%MAXSIZE; return;}
    while((q->front + n + 1)%MAXSIZE != q->rear){
        q->ready[(q->front + n)%MAXSIZE] = q->ready[(q->front + n + 1)%MAXSIZE];
        n++;
    }
    q->rear = (q->rear - 1)%MAXSIZE;
}





void time_rotation(Queueptr q,int n){
// 时间片轮转
    for (int i = 1; i <= n; i++){
        Push(q,PCBarr[i]);
    }

    while(0 == isEmpty(q)){
        int i = 0;

        while((q->front + i)%MAXSIZE != q->rear){
            
            printf("Queue.front: \n i: %d  id: %d;   priority: %d;  cpuTime: %d \n",i,
            q->ready[q->front+i].id,q->ready[q->front+i].priority,q->ready[q->front+i].cpuTime);
            
            q->ready[(q->front + i)%MAXSIZE].cpuTime -= TIMESLICE;

            if(q->ready[(q->front + i)%MAXSIZE].cpuTime == 0){ Delete(q,i%MAXSIZE); sleep(1); continue;}
              
            sleep(1);
            i++;
        }
    }
}

void priority_schedule(Queueptr q,int n){
// 优先级调度
    /**
     *@brief : 每次调度时间为1， 调度完后，进程优先级+3，插入到就绪队列
     *@param :
     *@return :
     */
    sort(n);
    for (int i = 1; i <= n; i++){
        Push(q,PCBarr[i]);
        // printf("Queue.front: \n id: %d;   priority: %d;  cpuTime: %d \n",PCBarr[i].id,PCBarr[i].priority,PCBarr[i].cpuTime);
        // printf("\n");
    }
    while(0 == isEmpty(q)){
        printf("Queue.front: \n id: %d;   priority: %d;  cpuTime: %d \n",
            q->ready[q->front].id,q->ready[q->front].priority,q->ready[q->front].cpuTime);
        
        q->ready[q->front].cpuTime--;
        if(q->ready[q->front].cpuTime == 0){ Pop_over(q); sleep(1); continue;}

        int temp = q->ready[q->front].priority + 3;
        struct PCB temp_pcb;
        Pop(q, &temp_pcb);
        
        if(temp > 99)temp_pcb.priority = 99;
        else temp_pcb.priority = temp;

        insertsort(q, temp_pcb);
        sleep(1);
    }
}

int main(){
    int n = input();
    init(n);
    Queue que;
    Queueptr q = &que;
    initQueue(q);

    int flag;
    printf("选择调度算法： 1 时间片轮转  0 优先级调度 \n");
    scanf("%d",&flag);
    if(flag)time_rotation(q,n);
    else priority_schedule(q,n);
}