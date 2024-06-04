#include "../include/showFlameGraph.h"
#include "database_def.h"
#include "database_server.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sys/wait.h>
#include <thread>

namespace myflamegraph {

ShowFlameGraph::ShowFlameGraph() {
    // 检查 perf 是否已安装
    if (!isPerfInstalled()) {
        std::cout << "正在安装perf工具，请等待... ..." << std::endl;
        // 如果未安装，安装 perf
        installPerf();
        std::cout << "安装完毕... ..." << std::endl;
    }
}

nlohmann::json ShowFlameGraph::LoadJson(const std::string& json_file_path) {
    nlohmann::json json_data;
    std::ifstream json_file(json_file_path);
    if (json_file.is_open()) {
        json_file >> json_data;
        json_file.close();
        //std::cout << json_data.dump(4) << std::endl;
        return json_data;
    } else {
        throw std::runtime_error("无法打开JSON文件: " + json_file_path);
    }
}

// 假设您有一个函数来执行性能测试并生成火焰图
void ShowFlameGraph::performPerformanceTest(const std::string& testType, const std::string& samplingRate, const std::string& duration, const std::string& outputFilePath, const std::string& fileName, const std::string& off_cpu_events, const std::string& delay_time) {
    // 构建命令字符串
    std::string command;
    pid_t ppid = getppid(); //获取父进程的ppid，对该ppid性能进行检测
    std::string ppid_string = std::to_string(ppid);
    if (testType == "on-cpu") {
        // 使用 perf 来进行 CPU 密集型任务的采样
        command =  " echo \"1\" | sudo -S perf record -F " + samplingRate + " -ag -p " + ppid_string +" -- sleep " + duration;
        command += " && sudo perf script > perf.script";                    
        if(delay_time == "-1"){
            // 如果 delay_time 为-1，则表示为只运行 on-cpu
            command += " &&  /usr/bin/stackcollapse-perf.pl perf.script > out.folded";
            command += " &&  /usr/bin/flamegraph.pl --title=\"ON-CPU Time Flame Graph\" out.folded  > " + fileName + ".svg";
            command += " &&  mv " + fileName + ".svg " + outputFilePath;
            command += " &&  find . -type f -name 'perf*' -exec rm  -f {} + ";
            command += " &&  find . -type f -name 'out*' -exec rm  -f {} + ";
        }else if(delay_time == "-2"){
            // 如果 delay_time 为 -2，则表示运行的是diff，第二次运行，运行完之后需要求差分并删除无用数据
            std::cout<<"delay_time == -2"<<std::endl;
            command += " && /usr/bin/stackcollapse-perf.pl perf.script > out2.folded";
            command += " && /usr/bin/difffolded.pl out2.folded out1.folded > " + fileName + ".folded"; 
            command += " && /usr/bin/flamegraph.pl --title=\"DIFF-ON-CPU Time Flame Graph\" " + fileName + ".folded > " + fileName + ".svg";
            command += " && mv " + fileName + ".svg " + outputFilePath;
            command += " &&  find . -type f -name 'out*' -exec rm  -f {} + ";
            command += " &&  find . -type f -name 'perf*' -exec rm  -f {} + ";
        }else{
            // 如果 delay_time 不为 -1 -2 ，则表示运行的是diff第一次，需要保留folded数据，且不删除，但不生成svg
            command += " && ../third_party/FlameGraph/stackcollapse-perf.pl perf.script > out1.folded"; 
        }
    } else if (testType == "off-cpu") {
        // 使用 perf 来进行 I/O 密集型任务的采样
        command = "echo \"1\" | sudo -S perf record " + off_cpu_events + " -F " + samplingRate + " -ag -p " + ppid_string +" -- sleep " + duration;
        command += " && sudo perf script > perf.script";
        if(delay_time == "-1"){
            // 如果 delay_time 为-1，则表示为只运行 off-cpu
            command += " && /usr/bin/stackcollapse-perf.pl perf.script > out.folded";
            command += " && /usr/bin/flamegraph.pl --color=io --title=\"Off-CPU Time Flame Graph\" out.folded > " + fileName + ".svg";
            command += " && mv " + fileName + ".svg " + outputFilePath;
            command += " &&  find . -type f -name 'perf*' -exec rm  -f {} + ";
        }else if(delay_time == "-2"){
            // 如果 delay_time 为 -2，则表示运行的是diff，第二次运行，运行完之后需要求差分并删除无用数据
            command += " && /usr/bin/stackcollapse-perf.pl perf.script > out2.folded";
            command += " && /usr/bin/difffolded.pl out1.folded out2.folded > out.folded"; 
            command += " && /usr/bin/flamegraph.pl --title=\"DIFF-Off-CPU Time Flame Graph\" out.folded > " + fileName + ".svg";
            command += " && mv " + fileName + ".svg " + outputFilePath;
            command += " &&  find . -type f -name 'out*' -exec rm  -f {} + ";
            command += " &&  find . -type f -name 'perf*' -exec rm  -f {} + ";
        }else{
            // 如果 delay_time 不为 -1 -2 ，则表示运行的是diff第一次，需要保留folded数据，且不删除，但不生成svg
            command += " && /usr/bin/stackcollapse-perf.pl perf.script > out1.folded";
        }

    } else {
        // 如果 testType 不是预期的值，则报错
        std::cerr << "错误: 不支持的测试类型: " << testType << std::endl;
        return;
    }

    // 执行命令
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：执行命令
        int status = system(command.c_str());
        if (status == 0) {
            if(std::stoi(delay_time) >= 0){
                // 运行完diff中的第一次，需要等待
                std::cout << "等待"<<delay_time<<"s后,进行第二次采样"<< std::endl;
            }else{
                std::cout << "性能测试完成，火焰图已生成。保存路径: " << outputFilePath + fileName <<".svg"<< std::endl;               
            }
        } else {
            std::cerr << "性能测试失败，请检查命令是否正确执行。" << std::endl;
        }
        exit(0); // 子进程结束
    } else if (pid > 0) {
        // 父进程：等待子进程完成
        int status;
        int run_time = 0;   //实时返回检测运行时间
        if(std::stoi(delay_time) >= 0){
            std::cout<<"\n================== diff-";
            std::cout << testType << "1: start ==================\n" << std::flush;
        }else if(std::stoi(delay_time) == -2){
            std::cout<<"\n================== diff-";
            std::cout << testType << "2: start ==================\n" << std::flush;
        }else{
            std::cout<<"\n================== ";
            std::cout << testType << " start ==================\n" << std::flush;
        }
        
        while (true) {
            pid_t wpid = waitpid(pid, &status, WNOHANG);
            if (wpid == pid) {
                // 子进程已结束
                break;
            } else if (wpid == 0) {
                // 子进程仍在运行   
                sleep(1); // 等待1秒
                run_time++;
                if(run_time <= std::stoi(duration)){
                    std::cout << "已运行 : " << run_time << " s \n" << std::flush;
                }      
            } else {
                // waitpid出错
                std::cerr << "等待子进程时出错。" << std::endl;
                break;
            }
        }
    }  else {
        // fork 失败
        std::cerr << "无法创建子进程。" << std::endl;
    }
}

void ShowFlameGraph::ExecuteFlameGraph(const std::string& json_file_path) {
    try {
        // 读取并解析JSON配置文件
        nlohmann::json config = ShowFlameGraph::LoadJson(json_file_path);

        // 遍历配置数组中的每个配置项
        for (auto& config_item : config) {
            std::string type = config_item["type"];
            auto& params = config_item;

            // 提取配置参数
            std::string sampling_rate = params["sampling_rates"];
            std::string duration = params["durations"];
            std::string output_file_path = params["output_file_path"];
            std::string generate_svg_name = params["generate_svg_names"];

            // 处理events参数
            std::vector<std::string> events;
            std::string delay_time; 
            std::string diff_type;  // 测试on-cpu还是off-cpu的diff

            if (type == "diff") {
                // 对于diff类型，我们需要提取type和delay_time
                auto& diff_params = params["events"].at(0);
                diff_type = diff_params["type"];
                delay_time = diff_params["delay_time"];
                if(diff_type == "off-cpu"){
                    events = diff_params["diff_off_cpu_events"]; // 读取diff中指定的测试off-cpu哪些性能
                } else if(diff_type == "on-cpu") {
                    events = diff_params["diff_on_cpu_events"]; // 读取diff中指定的测试on-cpu哪些性能
                }
            } else {
                // 对于其他类型，直接使用events数组
                delay_time = "-1";    // 除了diff，其他均默认为 -1
                events = params["events"];
            }

            std::string events_str;
            if (!events.empty()) {    
                for (size_t j = 0; j < events.size(); ++j) {
                    events_str += "-e " + events[j];
                    if (j < events.size() - 1) {
                        events_str += " "; // 不是最后一个元素后面加空格
                    }
                }
            }
            std::cout << events_str << std::endl;
            // 执行性能测试
            if (type == "diff") {
                performPerformanceTest(diff_type, sampling_rate, duration, output_file_path, generate_svg_name, events_str, delay_time);
                std::this_thread::sleep_for(std::chrono::seconds(std::stoi(delay_time))); // 等待delay_time秒
                delay_time = std::to_string(-2);    // 提示，是第二次运行on-cpu了，运行完毕之后需要将folded做查分，并删除无用数据
                performPerformanceTest(diff_type, sampling_rate, duration, output_file_path, generate_svg_name, events_str, delay_time);
            } else {
                delay_time = std::to_string(-1);
                performPerformanceTest(type, sampling_rate, duration, output_file_path, generate_svg_name, events_str, delay_time);
            }

            //将执行信息保存在数据库
            database_server::ProfileConfig config;
            database_server::DataBaseServer db = database_server::DataBaseServer::GetInstance();
            config.id = db.GetCurrentTimeString();
            config.type = type;
            config.durations = duration;
            config.output_file_path = output_file_path + generate_svg_name + ".svg";
            config.sampling_rates = sampling_rate;
            if(!db.AddConfigInfo(config)){
                std::cout<<"AddConfigInfo defeat!"<<std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}



pid_t ShowFlameGraph::child_pid = 0;
void ShowFlameGraph::GenerateFlameGraph(const std::string& json_file_path) {
    child_pid = fork(); // 用于存储子进程的 PID
        
    if (child_pid == 0) {
        // 子进程：执行性能测试和生成火焰图
        ExecuteFlameGraph(json_file_path);
        exit(0); // 子进程结束
    } else if (child_pid > 0) {
        // 父进程：注册退出处理函数（父进程运行结束，单子进程还在运行中，则强制结束子进程）
        atexit(Cleanup);
    } else {
        // fork 失败
        std::cerr << "无法创建子进程。" << std::endl;
    }
}

bool ShowFlameGraph::isPerfInstalled() {
    std::string command = "perf --version";
    int status = system(command.c_str());
    return WEXITSTATUS(status) == 0;
}

void ShowFlameGraph::installPerf() {
    std::string command = "sudo apt-get update && sudo apt install linux-tools-5.15.0-105-generic && sudo apt install linux-cloud-tools-5.15.0-105-generic";
    int status = system(command.c_str());
    if (WEXITSTATUS(status) != 0) {
        std::cerr << "Error: Failed to install perf." << std::endl;
    }
}

void ShowFlameGraph::Cleanup() {
    // 主进程退出时的清理函数
    if (child_pid > 0) {
        kill(child_pid, SIGTERM); // 发送 SIGTERM 信号给子进程
        std::cout << "检测到测试函数已运行结束，正在终止子进程... ..." << std::endl;
    }
}

} // namespace flamegraph
