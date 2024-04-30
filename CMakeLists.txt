cmake_minimum_required(VERSION 3.10)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置项目名称
project(project)

find_package(Threads REQUIRED)

# 添加头文件目录
# target_include_directories(myFlameGraph PUBLIC include)
include_directories(
    include
)

add_subdirectory(third_party/nlohmann_json)



# 添加源文件
add_executable(myFlameGraph 
                src/main.cpp 
                src/testFunction.cpp 
                src/showFlameGraph.cpp
            )

target_link_libraries(myFlameGraph Threads::Threads)
target_link_libraries(myFlameGraph nlohmann_json)