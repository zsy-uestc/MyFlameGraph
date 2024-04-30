#ifndef TESTFUNCTION_H
#define TESTFUNCTION_H

#include <vector>
namespace myflamegraph {

class MyTestFunction {
public:
    MyTestFunction();

    unsigned long fibonacci(unsigned int n);  

    void TestFlameGraph();  //测试函数：[占用CPU资源、占用IO、上下文切换、占用内核态]

    void CostCpuResource(double durationSeconds);       //模拟占用CPU资源(on-cpu)

    void CostIoResource(double durationSeconds);        //模拟占用IO(off-cpu)

    static void contextSwitchingFunction(int id, double durationSeconds);  //模拟上下文切换(off-cpu)

    void CostContextSwitching(double durationSeconds);  //模拟上下文切换(off-cpu)

    void CostkernelResource(double durationSeconds);    //创建多线程，使用互斥锁，不释放资源，其他线程只能等待。模拟占用内核资源(off-cpu)

    static void *sleepInThread(void *arg);                     //线程实现函数

private:
    std::vector<int> runtime = {5,5,5,5,3,7,7,3};   //为了演示效果，定义各个函数运行的时间。

    int loop_count = 0;
};

} // namespace flamegraph

#endif // TESTFUNCTION_H
