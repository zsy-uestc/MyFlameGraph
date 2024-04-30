#include "testFunction.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <sys/wait.h>
#include <thread>
#include <pthread.h>
#include <unistd.h>

namespace myflamegraph {

MyTestFunction::MyTestFunction(){
}

unsigned long MyTestFunction::fibonacci(unsigned int n) {    
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 修改后的函数，接受一个表示运行时间的参数
void MyTestFunction::CostCpuResource(double durationSeconds) {
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() < durationSeconds) {
        // 循环执行一些计算密集型任务
        fibonacci(20);
    }
}

void MyTestFunction::CostIoResource(double durationSeconds) {
    std::ofstream outputFile("/home/zsy/code/FlameGraph/svgs/useIO.txt", std::ios::out | std::ios::binary);
    
    if (!outputFile.is_open()) {
        std::cerr << "无法打开文件进行写入。" << std::endl;
        return;
    }
    
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(static_cast<long>(durationSeconds));
    
    while (std::chrono::steady_clock::now() < end) {
        // 写入一些数据
        outputFile << "write write write... ..." << std::endl;
        
        // 刷新输出缓冲区，确保数据被写入文件
        outputFile.flush();
        
        // 稍作休息，避免过快的写入操作
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    outputFile.close();
}

void MyTestFunction::contextSwitchingFunction(int id, double durationSeconds) {
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() < durationSeconds) {
        std::cout << "Thread " << id << " is running." << std::endl;
        std::this_thread::yield();
    }
}

void MyTestFunction::CostContextSwitching(double durationSeconds) {
    std::thread t1(contextSwitchingFunction, 1, durationSeconds / 2);
    std::thread t2(contextSwitchingFunction, 2, durationSeconds / 2);

    t1.join();
    t2.join();
}


pthread_mutex_t lock1, lock2;

void *MyTestFunction::sleepInThread(void *arg) {
    // Cast the argument to the correct type
    long tid = (long)arg;

    // Lock the mutexes
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);

    // Simulate work
    for (int k = 0; k < 5; ++k) {
        std::cout << "Thread #" << tid << " Job " << k << " printing" << std::endl;
        // Simulate CPU-bound work
        for (int i = 0; i < 100000; ++i) {
            for (int j = 0; j < 6666; ++j) {
                // Do nothing, just waste time
            }
        }
        // Sleep for a short period
        usleep(500000); // 500000 microseconds = 500 milliseconds
    }

    // Unlock the mutexes
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);

    return NULL;
}

void MyTestFunction::CostkernelResource(double durationSeconds) {
    // Initialize mutexes
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    // Calculate the end time
    const auto endTime = std::chrono::steady_clock::now() + std::chrono::duration<double>(durationSeconds);

    // Create and detach threads
    while (std::chrono::steady_clock::now() < endTime) {
        pthread_t thread;
        pthread_create(&thread, NULL, sleepInThread, (void *)1);
        pthread_detach(thread);
        sleep(2); // Sleep for 2 seconds between threads
    }

    // Destroy mutexes
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
}


void MyTestFunction::TestFlameGraph() {
    while(loop_count / 8 < 3){
        //模拟循环三次

        // 循环执行四个函数，每个函数的运行时间都是随机的
        std::cout<<"\""<<loop_count<<". cpuTime = "<< runtime[loop_count % 8] <<std::endl;
        MyTestFunction::CostCpuResource(runtime[loop_count % 8]);
        loop_count++;

        std::cout<<"\""<<loop_count<<". ioTime = "<< runtime[loop_count % 8] <<std::endl;
        CostIoResource(runtime[loop_count % 8]);
        loop_count++;

        std::cout<<"\""<<loop_count<<". sleepTime = "<< runtime[loop_count % 8] <<std::endl;
        CostkernelResource(runtime[loop_count % 8]);
        loop_count++;

        std::cout<<"\""<<loop_count<<". contextSwitchingTime = "<< runtime[loop_count % 8] <<std::endl;
        CostContextSwitching(runtime[loop_count % 8]);
        loop_count++;
    }
}

} // namespace flamegraph
