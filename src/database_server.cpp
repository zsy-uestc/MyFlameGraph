#include "database_server.h"
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include "sqlite_orm/sqlite_orm.h"
#include "database_def.h" // 包含定义了 ProfileConfig 结构体的头文件
#include <fstream>
#include <chrono>

using namespace sqlite_orm;
namespace database_server {

std::string DataBaseServer::GetCurrentTimeString() {
    //获取当前函数运行的时间戳
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&now_time_t);

    std::stringstream ss;
    ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
    
// 创建数据库和表
auto& DataBaseServer::CreateStorage() {
    // 创建数据库表，id 设置为自增主键
    static auto storage = make_storage(database_path,
                        make_table("profile_configs",
                            make_column("run_time", &ProfileConfig::id),
                            make_column("type", &ProfileConfig::type),
                            make_column("sampling_rates", &ProfileConfig::sampling_rates),
                            make_column("durations", &ProfileConfig::durations),
                            make_column("output_file_paths", &ProfileConfig::output_file_path),
                            make_column("other", &ProfileConfig::other)));

    return storage;
}

bool DataBaseServer::DatabaseExists(){
    // 检查数据库文件是否存在
    std::ifstream dbFile(database_path);
    return dbFile.good();  // 如果文件存在，则返回 true
}


bool DataBaseServer::AddConfigInfo(const ProfileConfig& task){
    try{
        //执行完火焰图后，将执行信息保存在数据库中
        DataBaseServer& db =  DataBaseServer::GetInstance();
        auto& storage = this->CreateStorage();
        storage.insert(task);
        std::cout<<"Add flamegraph information to database"<<std::endl;
        return true;
    }catch(const std::exception& err){
        std::cerr << "Error adding flamegraph information to database: " << err.what() << std::endl;
        return false;
    }
}

void DataBaseServer::Init(){
    //数据库初始化
    if(!DatabaseExists()){
        auto& storage = CreateStorage();
        storage.sync_schema();
    }else{
        std::cout<<"数据库已经存在，无需创建"<<std::endl;
    }
}

}//namespace database_server
