#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
//宏定义能不用就不用
#define MAXPROCESS 100
#define MAXSIZE 100
#define MAXRESNUM 100
enum{
    READY = 0,
    RUN, 
    FINISH 
};
void err_log(const char* buf);


struct PCB
{
    int id;
    int priority;
    int cpuTime;
    int status;
    //所需资源数目
    int res_num;
    int submit_time;
    int start_time;
    int end_time;
};

struct PCB PCBarr[MAXPROCESS];


typedef struct  _Queue
{
    struct PCB* ready[MAXSIZE];
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

void Push(Queueptr q, struct PCB *pro){
    if(isFull(q)){err_log("Push: Queue is full ");return;}

    q->ready[q->rear] = pro;
    q->rear = (q->rear + 1)%MAXSIZE;    
}

void Pop(Queueptr q, struct PCB *pro){
    if(isEmpty(q)){err_log("Pop: Queue is empty");return ;}
    // 内容保留，front 更改  如果被覆盖掉，那么无所谓，本就应该放弃pro
    pro = q->ready[q->front];
    q->front = (q->front + 1)%MAXSIZE;
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
        PCBarr[n].status = READY;
        PCBarr[n].res_num = rand()%49 + 1;
        PCBarr[n].submit_time = rand()%9 + 1;
        PCBarr[n].start_time = -1;
        PCBarr[n].end_time = -1;
        n--;
    }
}

void sortFCFS(int n){   
    for (int i = 1; i <= n; i++){
        for(int j = i+1; j <= n; j++){
            if(PCBarr[i].submit_time > PCBarr[j].submit_time){
                struct PCB temp = PCBarr[i];
                PCBarr[i] = PCBarr[j];
                PCBarr[j] = temp;
            }
        }
    }
}
void sortSJF(int n){
    for (int i = 1; i <= n; i++){
        for(int j = i+1; j <= n; j++){
            if(PCBarr[i].cpuTime > PCBarr[j].cpuTime){
                struct PCB temp = PCBarr[i];
                PCBarr[i] = PCBarr[j];
                PCBarr[j] = temp;
            }
        }
    }
}


struct PCB PCBarr_hrrn[MAXSIZE];
int sortHRRN(int n, int cpu_run_time){   //不安全排序
    int id = 1;
    for(int i = 1; i <= n; i++){
        if(cpu_run_time >= PCBarr[i].submit_time){
            PCBarr_hrrn[id ++] = PCBarr[i];
        }
    }

    for (int i = 1; i < id; i++){
        for(int j = i+1; j < id; j++){
            if((cpu_run_time - PCBarr_hrrn[i].submit_time + PCBarr_hrrn[i].cpuTime) / PCBarr_hrrn[i].cpuTime
                < (cpu_run_time - PCBarr_hrrn[j].submit_time + PCBarr_hrrn[j].cpuTime) / PCBarr_hrrn[j].cpuTime) 
            {
                struct PCB temp = PCBarr_hrrn[i];
                PCBarr_hrrn[i] = PCBarr_hrrn[j];
                PCBarr_hrrn[j] = temp;
            }
        }
    }
    return id;
}

void SJF(Queueptr q, int n){
    sortSJF(n);

    int cpu_run_time = 0;
    int flag = 1; 
    while(flag){
        initQueue(q);
        flag = 0;
        for (int i = 1; i <= n; i++){
            if(PCBarr[i].status == READY)flag = 1;
            if(PCBarr[i].submit_time <= cpu_run_time && PCBarr[i].status == READY)Push(q,&PCBarr[i]);
        }
        if(isEmpty(q) == 1){cpu_run_time ++; sleep(1); continue;}
        
        q->ready[q->front]->status = RUN;
        q->ready[q->front]->start_time = cpu_run_time;
        cpu_run_time += q->ready[q->front]->cpuTime;
        sleep(q->ready[q->front]->cpuTime);
        q->ready[q->front]->end_time = cpu_run_time;
        q->ready[q->front]->status = FINISH;

        printf("Queue.front: \n id: %d;   submit_time: %d;  start_time: %d;  endTime: %d;  roundTime: %d   wightTime: %3f \n  ",
            q->ready[q->front]->id,q->ready[q->front]->submit_time,q->ready[q->front]->start_time,q->ready[q->front]->end_time,
            q->ready[q->front]->end_time - q->ready[q->front]->submit_time,
            (double)(q->ready[q->front]->end_time - q->ready[q->front]->submit_time) / q->ready[q->front]->cpuTime);
        Pop_over(q);
    }
}

void sonFunction(Queueptr q, int n){
    for(int i = 1; i <= n; i++){
        if(PCBarr[i].id == q->ready[q->front]->id){
            PCBarr[i] = *q->ready[q->front];
        }
    }
}
void HRRN(Queueptr q,int n){
    
    int cpu_run_time = 0;
    int flag = 1; 
    while(flag){
        flag = 0;
        initQueue(q);
        int num = sortHRRN(n,cpu_run_time);
        for (int i = 1; i <= n; i++){
            if(PCBarr[i].status == READY)flag = 1;
            if( i < num && PCBarr_hrrn[i].status == READY)
                Push(q,&PCBarr_hrrn[i]);
        }
        if(isEmpty(q) == 1){cpu_run_time ++; sleep(1); continue;}

        q->ready[q->front]->status = RUN;
        q->ready[q->front]->start_time = cpu_run_time;
        cpu_run_time += q->ready[q->front]->cpuTime;
        sleep(q->ready[q->front]->cpuTime);
        q->ready[q->front]->end_time = cpu_run_time;
        q->ready[q->front]->status = FINISH;

        printf("Queue.front: \n id: %d;   submit_time: %d;  start_time: %d;  endTime: %d;  roundTime: %d   wightTime: %3f \n  ",
            q->ready[q->front]->id,q->ready[q->front]->submit_time,q->ready[q->front]->start_time,q->ready[q->front]->end_time,
            q->ready[q->front]->end_time - q->ready[q->front]->submit_time,
            (double)(q->ready[q->front]->end_time - q->ready[q->front]->submit_time) / q->ready[q->front]->cpuTime);
        sonFunction(q,n);
        Pop_over(q);
    }
}


void FCFS(Queueptr q,int n){
    
    sortFCFS(n);
    for (int i = 1; i <= n; i++){
        Push(q,&PCBarr[i]);
    }
    int cpu_run_time = 0;

    while(isEmpty(q) == 0){
        while(cpu_run_time < q->ready[q->front]->submit_time){cpu_run_time ++; sleep(1);}
        q->ready[q->front]->status = RUN;
        q->ready[q->front]->start_time = cpu_run_time;
        cpu_run_time += q->ready[q->front]->cpuTime;
        sleep(q->ready[q->front]->cpuTime);
        q->ready[q->front]->end_time = cpu_run_time;
        q->ready[q->front]->status = FINISH;
        printf("Queue.front: \n id: %d;   submit_time: %d;  start_time: %d;  endTime: %d;  roundTime: %d   wightTime: %3f \n  ",
            q->ready[q->front]->id,q->ready[q->front]->submit_time,q->ready[q->front]->start_time,q->ready[q->front]->end_time,
            q->ready[q->front]->end_time - q->ready[q->front]->submit_time,
            (double)(q->ready[q->front]->end_time - q->ready[q->front]->submit_time) / q->ready[q->front]->cpuTime);
        Pop_over(q);
    }
}






int main(){
    int n = input();
    init(n);
    Queue que;
    Queueptr q = &que;
    initQueue(q);
    printf("请输入调度算法： 0 FCFS； 1 SJF； 2HRRN \n");
    int x;
    scanf("%d",&x);
    if(x == 0)
        FCFS(q,n);
    else if(x == 1)    
        SJF(q,n);
    else HRRN(q,n);
    // int flag;
    // printf("选择调度算法：  \n");
    // scanf("%d",&flag);
}