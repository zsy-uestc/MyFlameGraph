#include "testFunction.h"

int main() {
    std::string config_file_path = "/home/zsy/code/MyFlameGraph/config/myconfig.json";
    myflamegraph::MyTestFunction mtf;
    mtf.GenerateFlameGraph(config_file_path);   //注意要在测试函数前面运行
    mtf.TestFlameGraph();
    return 0;
}
