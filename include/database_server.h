#pragma once
#include "database_def.h"

namespace database_server{

class DataBaseServer{
public:
    static DataBaseServer& GetInstance(){
        static DataBaseServer instance;
        return instance;
    }
    
    std::string GetCurrentTimeString();             //计算当前函数运行的时间戳
    bool DatabaseExists();                   //检查数据库是否已经创建
    void Init();                                    //数据库初始化
    bool AddConfigInfo(const ProfileConfig& task);  //数据库中插入数据
    auto& CreateStorage();                          //创建数据库表

private:
    std::string database_path = "/home/zsy/code/MyFlameGraph/resources/profile.db";
    DataBaseServer() {}
}; 
// database_path = "/home/zsy/code/MyFlameGraph/build/profile.db"

}//namespace database_server