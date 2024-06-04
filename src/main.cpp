#include "testFunction.h"
#include "database_server.h"

using namespace database_server;
int main() {
    DataBaseServer::GetInstance().Init();       //数据库初始化
    std::string config_file_path = "/home/zsy/code/MyFlameGraph/config/myconfig.json";
    myflamegraph::MyTestFunction mtf;
    mtf.GenerateFlameGraph(config_file_path);   //先启动FlameGraph工具
    mtf.TestFlameGraph();                       //需要进行性能分析的函数。FlameGraph会对该函数进行采样
    return 0;
}