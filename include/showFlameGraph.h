#ifndef SHOWFLAMEGRAPH_H
#define SHOWFLAMEGRAPH_H

#include "testFunction.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <unistd.h>

namespace myflamegraph {

class ShowFlameGraph {
public:
    ShowFlameGraph();
    // 解析json中的内容参数
    static nlohmann::json LoadJson(const std::string& json_file_path);    // 执行性能测试并生成火焰图
    
    void performPerformanceTest(const std::string& testType, const std::string& samplingRate, const std::string& duration, const std::string& outputFilePath, const std::string& generateSvgName, const std::string& off_cpu_events, const std::string& delay_time);
   
    void ExecuteFlameGraph(const std::string& json_file_path);  // 执行火焰图，并生成svg文件
    
    template <typename TestFunctionType>
    static void RunFlameGraph(const std::string& config_file_path, TestFunctionType test_function);
    

    static void StaticRunFlameGraph(const std::string& config_file_path, std::function<void()> test_function){
 
        myflamegraph::ShowFlameGraph sfg;

        pid_t pid = fork();
        if (pid == 0) {
            // 子进程：执行性能测试和生成火焰图
            sfg.ExecuteFlameGraph(config_file_path);
            exit(0); // 子进程结束
        } else if (pid > 0) {
            // 父进程：运行待测试性能的函数
            while(1) {
                test_function(); // 调用其他开发人员提供的函数
            }
        } else {
            // fork 失败
            std::cerr << "无法创建子进程。" << std::endl;
        }

    }    

    static void why(){
        std::cout<<"static in .h"<<std::endl;
    }

private:
    bool isPerfInstalled(); // 判断是否安装了perf工具

    void installPerf();     // 安装perf工具
};

} // namespace flamegraph

#endif // SHOWFLAMEGRAPH_H
