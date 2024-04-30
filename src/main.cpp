#include "showFlameGraph.h"
 #include "testFunction.h"
#include <unistd.h>
#include <pthread.h>

int main() {
    std::string config_file_path = "/home/zsy/code/FlameGraph/config/myconfig.json";
    myflamegraph::MyTestFunction mtf;
    myflamegraph::ShowFlameGraph::StaticRunFlameGraph(config_file_path, std::bind(&myflamegraph::MyTestFunction::TestFlameGraph, &mtf));
    return 0;
}

