//
//  main.cpp
//  OS3
//
//  Created by 胡海波 on 2021/6/7.
//

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

const int MAX_USER = 10;
const int MAX_OPEN_FILE = 5;  // max user open file
const int MAX_USER_FILE = 10;
const int MAX_FILE = 100;



/**
 *@brief 返回值设置： 0 正常退出
 *                 -1 错误
 */
void handle_error(int err){
    switch (err) {
        case -1:
            cout << "error -1: memory overflow" << endl;
            break;
        case -2:
            cout << "error -2: user or file not found" << endl;
            break;
            
        case -3:
            cout << "error -3: mutex error" << endl;
            break;
        case -4:
            cout << "error -4: file not open" << endl;
            break;
        default:
            break;
    }
}

class File{
public:
    string filename;
    int mutex;      // 互斥锁
    bool isOpen;    // 1 : 打开
    int permision;      // wrx   0b000
    int length;
    string username;
    File(){}
    File(const string& str, int per, const string& username):filename(str),permision(per),length(10),username(username),mutex(0),isOpen(0){}
    string file_read(){
        return "Now, we are reading "+filename;
    }
    string file_write(){
        return "Now, we are writting "+filename;
    }
    string file_open(){
        return "Now, we are opening "+filename;
    }
    string file_lose(){
        return "Now, we are shutting down "+filename;
    }
    string to_string(){
        string temp;
        switch (permision) {
            case 7:
                temp = "wrx";
                break;
            case 3:
                temp = "-rx";break;
            case 1:
                temp = "--x";break;
            case 5:
                temp = "w-x";break;
            case 6:
                temp = "wr-";break;
            case 4:
                temp = "-rx";break;
            case 2:
                temp = "-r-";break;
            default:
                temp = "";
                break;
        }
        return "    " + temp + "    " + filename + "    " + username;
    }

};
class User{
public:
    string name;
    User(){}
    User(const string& str):name(str){}
};
class FileSys{
public:
    int num_user;
    int num_file;
    string current_username;  //当前使用用户
    vector<User> user_set;
    vector<File> file_set;
    
    FileSys():num_user(0),num_file(0){}
    int add_user(const string& str){
        if(user_set.size() >= MAX_USER)return -1;   //memory overflow
        User user(str);
        user_set.push_back(user);
        return 0;
    }
    int change_user(const string& str){
        for (int i = 0; i < user_set.size(); i++){
            if(str == user_set[i].name){
                current_username = user_set[i].name;
                return 0;
            }
        }
        return -2;  // user not found
    }
    int delete_user(const string& str){     // 迭代器写法
        for(vector<User>::iterator it = user_set.begin(); it != user_set.end(); ){
            if(str == it->name){
                it = user_set.erase(it);
                return 0;
            }
            else    it++;
        }
        return -2; // user not found
    }
    int Create(const string& str){
        if(file_set.size() > MAX_FILE)return -1;   // memory overflow
        int num = 0;
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == current_username)num++;
        }
        if(num > MAX_USER_FILE)return -1;      //memory overflow
        File file(str,0b111,current_username);
        file_set.push_back(file);
        return 0;
    }
    int Delete(const string& str){
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == str){
                file_set.erase(file_set.begin() + i);
                return 0;
            }
        }
        return -1;  //  file not found
    }
    int open(const string& str){
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == str){
                if(file_set[i].mutex == 1)return -3;    // mutex error
                file_set[i].mutex = 1;
                file_set[i].isOpen = 1;
                cout << file_set[i].file_open() << endl;
                return 0;
            }
        }
        return -2;
    }
    int lose(const string& str){
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == str){
                file_set[i].mutex = 0;
                file_set[i].isOpen = 0;
                cout << file_set[i].file_lose() << endl;
                return 0;
            }
        }
        return -2;
    }
    int read(const string& str){
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == str){
                if(file_set[i].isOpen == 0)return -4;   // file is not opened
                cout << file_set[i].file_read() << endl;
                return 0;
            }
        }
        return -2;
    }
    int write(const string& str){
        for(int i = 0; i < file_set.size(); i++){
            if(file_set[i].filename == str){
                if(file_set[i].isOpen == 0)return -4;   // file is not opened
                cout << file_set[i].file_write() << endl;
                return 0;
            }
        }
        return -2;
    }
    int list(){
        cout << "    " << "权限"  << "    " << "文件名" << "    " << "拥有者" << endl;
        for(int i = 0; i < file_set.size(); i++){
            cout << file_set[i].to_string() << endl;
        }
        return 0;
    }
};
FileSys fileSys;

/**
 *exit ：退出该系统
 *ls    ：列出所有文件
 *su   ：改变当前用户
 *adduser: 添加用户
 *rm   ：删除文件
 *userdel ：删除用户
 *add :
 *write :
 *read :
 *open :
 *shutdown :
 */
int main(){
    fileSys.add_user("root");
    fileSys.change_user("root");
    string ins;
    int err;
    while(1){
        cout << "[" << fileSys.current_username << "@" << "localhost" << "]# ";  // # 还是$ 就不纠结了
        getline(cin, ins);
        if( ins == "exit"){
            cout << "logout" << endl;
            cout << "Connect to localhost closed." << endl;
            break;
        }
        else if( ins == "ls"){
            fileSys.list();
            continue;
        }
        int pos = ins.find(" ");
        if(pos == ins.length()){
            cout << "cmd not found 请输入参数！"; continue;
            
        }
        string header = ins.substr(0, pos);
        string content = ins.substr(pos+1,ins.length());

        if(header == "su"){
            err = fileSys.change_user(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "adduser"){
            err = fileSys.add_user(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "rm"){
            err = fileSys.Delete(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "userdel"){
            err = fileSys.delete_user(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "add"){
            err = fileSys.Create(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "open"){
            err = fileSys.open(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "write"){
            err = fileSys.write(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "read"){
            err = fileSys.read(content);
            if(err < 0){handle_error(err);}
            continue;
        }
        else if (header == "shutdown"){
            err = fileSys.lose(content);
            if(err < 0){handle_error(err);}
            continue;
        }
    }
}

