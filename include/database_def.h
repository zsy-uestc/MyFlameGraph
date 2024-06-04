#pragma once

#include <string>
namespace database_server{

// 定义数据库表结构
struct ProfileConfig {
    std::string id;                 //运行时刻的时间戳，作为每一条记录的唯一编号
    std::string type;               //采样类型
    std::string sampling_rates;     //采样率
    std::string durations;          //采样时间
    std::string output_file_path;   //生成svg的保存路径
    std::string other;              //保留字段
};
}