# MyFlameGraph 性能可视化

FlameGraph 是一个用于生成火焰图的工具，可以用来分析程序的性能问题。本设计从``on-cpu``、``off-cpu``以及``diff``的角度出发，通过加载库的方式调用性能检测函数，而仅需对JSON文件来修改需求，即可根据需求自定义采样率、采样周期、文件保存路径等。采样完毕后会生成svg图，可以直观的找到程序的性能瓶颈所在。同时会将运行的相关信息实时记录在数据库中，方便后续的查询。



## 性能检测可选类型

### on-cpu

on-cpu 类型的火焰图用于分析程序在 CPU 上运行时的性能问题。生成 on-cpu 火焰图的方法如下：

1. 配置参数：设置 `sampling_rates`、`durations`、`output_file_path` 和 `generate_svg_names` 参数。
2. 运行程序：在程序运行期间，GenerateFlameGraph 会按照指定的采样率和持续时间进行采样。
3. 生成火焰图：程序运行结束后，GenerateFlameGraph 会生成一个 SVG 文件，其中包含了程序在 CPU 上运行时的调用栈信息。

### off-cpu

off-cpu 类型的火焰图用于分析程序在等待 I/O、锁或其他资源时的性能问题。生成 off-cpu 火焰图的方法如下：

1. 配置参数：设置 `sampling_rates`、`durations`、`output_file_path`、`generate_svg_names` 和 `events` 参数。
2. 运行程序：在程序运行期间，GenerateFlameGraph 会按照指定的采样率和持续时间进行采样，并记录 off-cpu 事件。
3. 生成火焰图：程序运行结束后，GenerateFlameGraph 会生成一个 SVG 文件，其中包含了程序在 off-cpu 状态时的调用栈信息。

### diff

diff 类型的火焰图用于比较两个不同时间点的程序性能差异。生成 diff 火焰图的方法如下：

1. 配置参数：设置 `sampling_rates`、`durations`、`output_file_path`、`generate_svg_names` 和 `events` 参数。在 `events` 参数中，可以自定义 diff_on_cpu_events 和 diff_off_cpu_events 事件。
2. 采集数据：分别在两个不同时间点采集程序的性能数据，可以使用 on-cpu 或 off-cpu 方式。
3. 生成火焰图：使用 FlameGraph 工具对两个时间点的数据进行处理，生成一个 SVG 文件，其中包含了两个时间点程序性能的差异信息。



## JSON 结构

JSON 文件应具有以下结构，可以根据需要选择一项或多项（on-cpu、off-cpu、diff）：

```json
[
  {
    "type": "on-cpu",
    "sampling_rates": "99",
    "durations": "4",
    "output_file_path": "/home/zsy/code/MyFlameGraph/svgs/",
    "generate_svg_names": "generate-on-cpu",
    "events": []
  },
  {
    "type": "off-cpu",
    "sampling_rates": "99",
    "durations": "4",
    "output_file_path": "/home/zsy/code/MyFlameGraph/svgs/",
    "generate_svg_names": "generate-off-cpu",
    "events": [
      "sched:sched_switch",
      "sched:sched_stat_sleep",
      "sched:sched_stat_iowait",
      "sched:sched_stat_blocked",
      "sched:sched_stat_wait"
    ]
  },
  {
    "type": "diff",
    "sampling_rates": "99",
    "durations": "4",
    "output_file_path": "/home/zsy/code/MyFlameGraph/svgs/",
    "generate_svg_names": "generate-diff",
    "events": [
      {
        "delay_time": "0",
        "type": "off-cpu",
        "diff_off_cpu_events": [
          "sched:sched_switch",
          "sched:sched_stat_sleep",
          "sched:sched_stat_iowait",
          "sched:sched_stat_blocked",
          "sched:sched_stat_wait"
        ],
        "diff_on_cpu_events": []
      }
    ]
  }
]

```



## 参数说明

- **type**：指定要运行的性能测试类型。可选项：on-cpu、off-cpu、diff

- **sampling_rates**：采样率，表示每秒采样的次数。较高的采样率可以得到更精确的数据，但会增加系统的开销。默认值为 99。

- **durations**：采样持续时间，指定采样持续的时间长度。默认值为 20。

- **output_file_path**：输出文件路径，指定生成的 SVG 文件的存放路径。

- **generate_svg_names**：生成的 SVG 文件名，可以根据需要自定义。默认值为 `generate-on-cpu`、`generate-off-cpu` 和 `generate-diff`。

- **events**：事件列表，用于指定要采集的事件类型。在 on-cpu 中，默认为空；在 off-cpu 中，可以根据需要选择适合的参数，默认值为 `sched:sched_switch`、`sched:sched_stat_sleep`、`sched:sched_stat_iowait`、`sched:sched_stat_blocked` 和 `sched:sched_stat_wait`。

- **diff的events中，delay_time**：diff中对两段性能检测的间隔时间，单位为秒。默认值为5。

- **diff的events中，type**：可选on-cpu或off-cpu。表示间隔delay_time秒度on-cpu或off-cpu重复采样，生成性能的前后对比差分图。

- **diff的events中，diff_off_cpu_events**：在选择off-cpu时生效，参数选择见下。

- **off-cpu's events**可选参数如下：

  > 1. `sched:sched_stat_sleep` - 此事件用于捕获进程进入睡眠状态时的时间点。它通常用于分析进程的睡眠时间，包括进程等待 I/O 完成、等待信号处理等。
  > 2. `sched:sched_switch` - 此事件用于捕获进程切换的时间点，即一个进程从运行状态切换到就绪状态，或者从就绪状态切换到运行状态。它可以帮助您分析进程的上下文切换次数，包括进程因为等待资源而进入睡眠状态的情况。
  > 3. `sched:sched_process_exit` - 此事件用于捕获进程退出或终止的时间点。它可以帮助您分析进程的退出时间，包括进程因为等待资源而无法继续执行的情况。
  > 4. `sched:sched_wakeup` - 此事件用于捕获进程从睡眠状态被唤醒的时间点。它可以帮助您分析进程从等待状态恢复到运行状态的时间。
  > 5. `sched:sched_wakeup_new` - 此事件用于捕获新进程被唤醒的时间点。它可以帮助您分析新进程从创建到运行状态的时间。
  > 6. `sched:sched_waking` - 此事件用于捕获唤醒其他进程的时间点。它可以帮助您分析进程之间的唤醒关系。` 
  > 7. `sched:sched_yield` - 此事件用于捕获进程主动让出 CPU 的时间点。它可以帮助您分析进程的主动让步行为。
  > 8. `sched:sched_wait_task` - 此事件用于捕获进程等待另一个进程的时间点。它可以帮助您分析进程之间的等待关系。
  > 9. `sched:sched_process_wait` - 此事件用于捕获进程等待其他进程的时间点。它可以帮助您分析进程之间的等待关系。
  > 10. `sched:sched_wakeup_idle` - 此事件用于捕获唤醒空闲 CPU 的时间点。它可以帮助您分析 CPU 的空闲状态。





# myflamegraph 库的安装方法

### 1. 代码git clone 到本地

```shell
git clone git@github.com:zsy-uestc/MyFlameGraph.git
```

### 2. 运行写好的安装库脚本

```sh
./run.sh
```























# myflamegraph 测试函数使用说明

# myflamegraph 库的使用方法

### 1. 概述

`myflamegraph` 是一个用于生成火焰图的测试函数库。其中提供的主要测试函数：

- `GenerateFlameGraph`: 生成火焰图

### 2. 函数的使用方法

对于待测试的函数，请确保继承`ShowFlameGraph` 类。

```c++
#include "showFlameGraph.h"

class MyTestFunction : public myflamegraph::ShowFlameGraph{
    
}    
```

在主函数中，首先创建测试类的实例，并正确传入JSON的绝对路径至GenerateFlameGraph() 方法。

注意GenerateFlameGraph() 方法一定要在测试函数前运行，才能达到检测的效果。

```c++
#include "testFunction.h"
int main() {
    std::string config_file_path = "/home/zsy/code/MyFlameGraph/config/myconfig.json";
    MyTestFunction mtf;
    mtf.GenerateFlameGraph(config_file_path);   //启动检测函数
    mtf.OtherFuntion();							//其他的函数正常运行
    return 0;
}
```

### 3. 注意事项

- 在调用测试函数之前，请确保已经成功运行了 `GenerateFlameGraph` 函数。
- 请确保 `config_file_path` 变量指向一个有效的 JSON 配置文件路径。
- 请确保json配置文件格式正确。



## 运行效果

