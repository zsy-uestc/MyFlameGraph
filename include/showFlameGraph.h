#ifndef SHOWFLAMEGRAPH_H
#define SHOWFLAMEGRAPH_H

#include <string>
#include <nlohmann/json.hpp>
#include <unistd.h>

namespace myflamegraph {

class ShowFlameGraph{
public:
    ShowFlameGraph();
    // 解析json中的内容参数
    static nlohmann::json LoadJson(const std::string& json_file_path);    // 执行性能测试并生成火焰图
    
    void performPerformanceTest(const std::string& testType, const std::string& samplingRate, const std::string& duration, const std::string& outputFilePath, const std::string& generateSvgName, const std::string& off_cpu_events, const std::string& delay_time);
   
    void ExecuteFlameGraph(const std::string& json_file_path);  // 执行火焰图，并生成svg文件

    static void RunFlameGraph(const std::string& config_file_path, std::function<void()> test_function);

    virtual void GenerateFlameGraph(const std::string& json_file_path); //运行FlameGraph的函数

    static pid_t child_pid; // 用于存储子进程的 PID

    static void Cleanup();  //注册退出处理函数（父进程运行结束，单子进程还在运行中，则强制结束子进程）
private:

    bool isPerfInstalled(); // 判断是否安装了perf工具

    void installPerf();     // 安装perf工具
};

} // namespace flamegraph

#endif // SHOWFLAMEGRAPH_H


