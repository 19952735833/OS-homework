//
//  main.cpp
//  OS3
//
//  Created by 胡海波 on 2021/6/7.
//

#include <iostream>
#include <time.h>

using namespace std;
const int MMSIZE = 8;
enum insStatus{
    ORDER = 1,
    BEFORE,
    AFTER
};
enum pageStatus{
    INPAGE = 0,
    OUTPAGE
};
int seq[320];   //指令排序后
class Instruction{
public:
    insStatus _status;
    int id;//Page id
    bool finish;
};
Instruction ins_arr[320];
class Page{
public:
    Instruction ins[10];
    int id;
    int last_use_time;
    int num_of_use;
};

class Algorithm{
public:
    Page virtual_memory[32];
    Page physical_memory[MMSIZE];
    void OPT();
    void FIFO();
    void LRU();
    void LFU();
    int sonFunction_OPT(Page lhd, int location);
    
    int hit_count = 0;
    int miss_count = 0;
};
Algorithm agrm;

int Algorithm::sonFunction_OPT(Page lhd, int location){ //location is the index of OUTPAGE seq
    int distance = 0;
    while(location < 320){
        distance ++;
        if(ins_arr[seq[location]].id == lhd.id)return distance;
        location++;
    }
    return -1;
}

void Algorithm::OPT(){

    /* seq from 0 to 319 */
    for(int i = 0; i < 320; i++){
        pageStatus ins_status = OUTPAGE;
        /* tell the instruction whether is in physical_memory */
        for(int j = 0; j < MMSIZE; j++){
            if(physical_memory[j].id == ins_arr[seq[i]].id){
                ins_status = INPAGE;
                break;
            }
        }

        if(ins_status == INPAGE){
            hit_count ++;
            continue;
        }
        else {
            int max_distance = 0;
            int page_to_change_index = 0;
            /** elect the most dictanct Page*/
            for(int j = 0; j < MMSIZE; j++){
                int temp = sonFunction_OPT(physical_memory[j], i);
                if(temp > max_distance){
                    max_distance = temp;
                    page_to_change_index = j;
                }
            }
            /**exchange Page */
            physical_memory[page_to_change_index] = virtual_memory[ins_arr[seq[i]].id];
            miss_count ++;
        }
    }
}
void Algorithm::FIFO(){
    for(int i = 0; i < 320; i++){
        pageStatus ins_status = OUTPAGE;
        /* tell the instruction whether is in physical_memory */
        for(int j = 0; j < MMSIZE; j++){
            if(physical_memory[j].id == ins_arr[seq[i]].id){
                ins_status = INPAGE;
                break;
            }
        }
        for(int j = 0; j < MMSIZE; j++){
            physical_memory[j].last_use_time ++;
        }
        if(ins_status == INPAGE){
            hit_count ++;
            continue;
        }
        else {
            int max_use_time = 0;
            int page_to_change_index = 0;
            /** elect the most dictanct Page*/
            for(int j = 0; j < MMSIZE; j++){
                int temp = physical_memory[j].last_use_time;
                if(temp >= max_use_time){
                    max_use_time = temp;
                    page_to_change_index = j;
                }
            }
            /**exchange Page */
            physical_memory[page_to_change_index].last_use_time = 0;
            physical_memory[page_to_change_index] = virtual_memory[ins_arr[seq[i]].id];
            miss_count ++;
        }
    }
}
void Algorithm::LRU(){
    for(int i = 0; i < 320; i++){
        pageStatus ins_status = OUTPAGE;
        /* tell the instruction whether is in physical_memory */
        for(int j = 0; j < MMSIZE; j++){
            if(physical_memory[j].id == ins_arr[seq[i]].id){
                ins_status = INPAGE;
                physical_memory[j].last_use_time = 0;
                break;
            }
        }
        for(int j = 0; j < MMSIZE; j++){
            physical_memory[j].last_use_time ++;
        }
        if(ins_status == INPAGE){
            hit_count ++;
            continue;
        }
        else {
            int max_use_time = 0;
            int page_to_change_index = 0;
            /** elect the most dictanct Page*/
            for(int j = 0; j < MMSIZE; j++){
                int temp = physical_memory[j].last_use_time;
                if(temp >= max_use_time){
                    max_use_time = temp;
                    page_to_change_index = j;
                }
            }
            /**exchange Page */
            physical_memory[page_to_change_index].last_use_time = 0;
            physical_memory[page_to_change_index] = virtual_memory[ins_arr[seq[i]].id];
            miss_count ++;
        }
    }
}
void Algorithm::LFU(){
    for(int i = 0; i < 320; i++){
        pageStatus ins_status = OUTPAGE;
        /* tell the instruction whether is in physical_memory */
        for(int j = 0; j < MMSIZE; j++){
            if(physical_memory[j].id == ins_arr[seq[i]].id){
                ins_status = INPAGE;
                physical_memory[j].num_of_use ++;
                break;
            }
        }
        if(ins_status == INPAGE){
            hit_count ++;
            continue;
        }
        else {
            int min_use = 0;
            int page_to_change_index = 0;
            /** elect the most dictanct Page*/
            for(int j = 0; j < MMSIZE; j++){
                int temp = physical_memory[j].num_of_use;
                if(temp <= min_use){
                    min_use = temp;
                    page_to_change_index = j;
                }
            }
            /**exchange Page */
            physical_memory[page_to_change_index].num_of_use = 0;
            physical_memory[page_to_change_index] = virtual_memory[ins_arr[seq[i]].id];
            miss_count ++;
        }
    }
}


int sonFunction_get_seq(int &m, int &sum){
    if(ins_arr[m].finish == 1){
        m++;
        if(m > 319) m = 0;
        return 0;
    }
    seq[sum++] = m;
    ins_arr[m].finish = 1;
    return 1;
}
void get_seq(int m){  //0 <= m < 320
    int sum = 0;
    while(sum < 320){
        if(sonFunction_get_seq(m, sum) == 0)continue;
        m = rand() % m;
        if(sonFunction_get_seq(m, sum) == 0)continue;
        m = rand() % (319 - m) + m;
        if(sonFunction_get_seq(m, sum) == 0)continue;
    }
}



void init(){
    int n = (int)time(0);
    srand(n);
    for(int i = 0; i < 32; i++){
        Page temp;
        temp.id = i;
        temp.last_use_time = 0;
        temp.num_of_use = 0;
        for(int index_ins = 0; index_ins < 10; index_ins++){
            temp.ins[index_ins].finish = 0;
            ins_arr[i*10 + index_ins].finish = 0;
            temp.ins[index_ins].id = i;
            ins_arr[i*10 + index_ins].id = i;
            int propotion = rand()%4;
            if(propotion < 2){
                temp.ins[index_ins]._status = ORDER;
                ins_arr[i*10 + index_ins]._status = ORDER;
            }
            else if (propotion == 3){
                temp.ins[index_ins]._status = BEFORE;
                ins_arr[i*10 + index_ins]._status = BEFORE;
            }
            else {
                temp.ins[index_ins]._status = AFTER;
                ins_arr[i*10 + index_ins]._status = AFTER;
            }
        }
        agrm.virtual_memory[i] = temp;
    }
    for(int i = 0; i < MMSIZE; i++)
        agrm.physical_memory[i] = agrm.virtual_memory[i];
}


int main(){
    init();
    get_seq(88);
    agrm.OPT();
    cout << "OPT:" << endl;
    cout << "页面失效次数：" << agrm.miss_count << endl;
    cout << "页面命中次数：" << agrm.hit_count << endl;
    agrm.hit_count = 0;
    agrm.miss_count = 0;
    agrm.FIFO();
    cout << "FIFO:" << endl;
    cout << "页面失效次数：" << agrm.miss_count << endl;
    cout << "页面命中次数：" << agrm.hit_count << endl;
    agrm.hit_count = 0;
    agrm.miss_count = 0;
    agrm.LRU();
    cout << "LRU:" << endl;
    cout << "页面失效次数：" << agrm.miss_count << endl;
    cout << "页面命中次数：" << agrm.hit_count << endl;
    agrm.hit_count = 0;
    agrm.miss_count = 0;
    agrm.LFU();
    cout << "LFU:" << endl;
    cout << "页面失效次数：" << agrm.miss_count << endl;
    cout << "页面命中次数：" << agrm.hit_count << endl;
    agrm.hit_count = 0;
    agrm.miss_count = 0;
    
    return 0;
}

