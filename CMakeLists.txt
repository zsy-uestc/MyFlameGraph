cmake_minimum_required(VERSION 3.10)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置项目名称
project(project)

find_package(Threads REQUIRED)
# 查找JSON库
find_package(nlohmann_json REQUIRED)
# 查找sqlite库
find_package(PkgConfig)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# 添加头文件目录
include_directories(
    include
)
include_directories(${SQLITE3_INCLUDE_DIRS})
link_directories(${SQLITE3_LIBRARY_DIRS})
add_definitions(${SQLITE3_CFLAGS_OTHER})


# 添加源文件
add_executable(myFlameGraph 
                src/main.cpp 
                src/testFunction.cpp 
                src/showFlameGraph.cpp
                src/database_server.cpp
            )

# 生成静态库
add_library(myflamegraph STATIC src/showFlameGraph.cpp)

target_include_directories(myflamegraph PUBLIC ${nlohmann_json_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(myFlameGraph Threads::Threads)
target_link_libraries(myFlameGraph nlohmann_json)
target_link_libraries(myFlameGraph ${SQLITE3_LIBRARIES})

# 安装规则
install(TARGETS myflamegraph DESTINATION lib)
install(FILES include/showFlameGraph.h DESTINATION include/myflamegraph)

# 添加Perl脚本到安装规则
install(FILES third_party/FlameGraph/difffolded.pl third_party/FlameGraph/flamegraph.pl third_party/FlameGraph/stackcollapse-perf.pl DESTINATION bin)
