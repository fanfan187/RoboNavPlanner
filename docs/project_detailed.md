# RoboNavPlanner 项目技术文档

## 1. 项目概述

`RoboNavPlanner` 是一个使用 C++17 实现的模块化机器人路径规划系统。该项目旨在在一个给定的二维障碍物地图中，寻找从起点到终点的最优路径。

项目最初采用粒子群优化（PSO）算法，后续重构为一个灵活的、可扩展的框架，目前的核心是**混合A*与粒子群优化（Hybrid A*-PSO）算法**。这种混合策略结合了 A* 算法的全局路径搜索能力和 PSO 算法的局部精细优化能力，能够在复杂环境中高效地找到平滑且无碰撞的路径。

项目提供两种独立的运行模式：
* **图形化版本 (`RoboNavPlanner.exe`)**: 基于 **SFML** 库，提供动态、直观的路径和地图可视化界面，能同时展示 A* 的引导路径和 PSO 的最终优化路径。
* **控制台版本 (`RoboNavPlanner_Console.exe`)**: 无任何图形库依赖，在终端中通过 ASCII 字符打印地图和路径，方便在任何系统环境下进行快速验证、调试和测试。

## 2. 核心特性

* **高级混合算法**: 创新地将A*算法作为PSO的启发式引导，显著提高了收敛速度和路径质量。
* **高度解耦的架构**: 采用**策略（Strategy）设计模式**，将算法与路径规划器核心逻辑分离，使添加或切换新算法变得极为方便。
* **双版本支持**: 通过预处理器宏 (`USE_SFML` 和 `CONSOLE_VERSION`) 实现代码的条件编译，生成图形和控制台两个版本。
* **参数化配置**: 所有关键参数（如PSO参数、窗口大小、航点数量）都通过外部 `pso_config.json` 文件进行管理，无需重新编译即可调整算法行为。
* **高效碰撞检测**: 采用基于Bresenham直线算法的 `isLineColliding` 方法，快速检测路径段是否穿越障碍物。
* **现代化C++实践**: 广泛使用 C++17 标准特性，如智能指针 (`std::unique_ptr`)、`std::string`、`std::vector` 和 `std::function`，确保了代码的内存安全和可维护性。
* **清晰的构建系统**: 使用现代化 CMake (`CMakeLists.txt`) 进行项目管理，清晰地定义了目标、依赖和编译选项。

## 3. 架构设计

项目经过了精心的解耦重构，其核心是基于**策略设计模式**。这种设计使得系统灵活且极易扩展。

### 3.1. 抽象算法接口 (`IPathPlanningAlgorithm`)

这是所有路径规划算法的基石。它定义了一个所有具体算法都必须遵守的通用契约。

```cpp
// inlcude/algorithm/IPathPlanningAlgorithm.h
class IPathPlanningAlgorithm {
public:
    virtual ~IPathPlanningAlgorithm() = default;
    
    // 核心规划方法
    virtual PathPlanningResult planPath(const PathPlanningRequest& request) = 0;
    
    // 获取算法名称
    virtual std::string getAlgorithmName() const = 0;
    
    // 设置/获取参数的统一接口
    virtual void setParameter(const std::string& paramName, double value) = 0;
    virtual double getParameter(const std::string& paramName) const = 0;
    
    // ... 其他通用方法
};
```

`planPath`: 接受一个包含地图、起终点等信息的 `PathPlanningRequest` 结构体，并返回一个包含路径、耗时等信息的 `PathPlanningResult` 结构体。
`setParameter/getParameter`: 允许在运行时动态调整算法参数（例如，PSO的粒子数、迭代代数等）。
### 3.2. 路径规划器 (PathPlanner)
`PathPlanner` 类是路径规划任务的组织和协调者。它不关心当前使用的是哪种具体算法，只通过 `IPathPlanningAlgorithm` 接口来调用。

```C++

// src/core/PathPlanner.h
class PathPlanner {
private:
    // 持有一个指向算法接口的智能指针
    std::unique_ptr<IPathPlanningAlgorithm> algorithm;
    Map* map;
    Point startPoint, endPoint;
    // ...
public:
    // 通过构造函数注入具体的算法策略
    PathPlanner(Map* m, const Point& start, const Point& end, 
                std::unique_ptr<IPathPlanningAlgorithm> algo, ...);
    
    // 允许在运行时切换算法
    void setAlgorithm(std::unique_ptr<IPathPlanningAlgorithm> algo);
    
    // 执行规划（委托给当前的算法策略）
    PathPlanningResult planPath();
};
这种设计（依赖注入和策略模式）使得 PathPlanner 与具体算法实现完全解耦，未来可以轻松集成如 RRT*, Dijkstra 等新算法，而无需修改任何 PathPlanner 的代码。

### 3.3. 架构演进示意图
重构前 (紧耦合)

```text
+---------------+      includes/uses      +-----------------+
| PathPlanner   | ----------------------> | ZPSOAlgorithm   |
+---------------+                         +-----------------+
重构后 (松耦合，策略模式)

+---------------+      uses      +--------------------------+
| PathPlanner   | -------------> | IPathPlanningAlgorithm   |
+---------------+                +--------------------------+
                                     ^           ^
                                     |           | implements
                           +-------------------------+
                           | HybridAStarPSOAlgorithm |
                           +-------------------------+
```

## 4. 模块详解
src/main.cpp & src/main_console.cpp (程序入口)
main.cpp: 图形化版本的主入口。

创建并加载 ConfigManager 来读取 pso_config.json 的配置。
创建 Map 对象并从 data/maps/map.txt 加载地图数据。
创建 HybridAStarPSOAlgorithm 算法实例，并从配置中设置其参数。
将算法实例注入 PathPlanner。
调用 planner.planPath() 执行规划。
创建 Visualizer 对象，并传入地图、路径等数据进行渲染。
进入 visualizer.run() 的主循环。
main_console.cpp: 控制台版本的主入口。

执行流程与图形版本基本相同（1-5步）。
规划完成后，不创建 Visualizer，而是调用自定义的 printMap 函数。
printMap 函数在控制台使用ASCII字符（S, E, *, #）绘制地图和路径的概览。
src/core (核心逻辑)
Map 类:

职责: 表示二维环境，包括可通行区域和障碍物，并提供碰撞检测功能。
数据结构: 内部使用 std::vector<std::vector<int>> grid，0 代表可通行，1 代表障碍物。
碰撞检测: isLineColliding(start, end) 是其核心功能，使用 Bresenham's line algorithm 判断两点之间的连线是否经过障碍物栅格。
地图加载: loadFromFile(filename) 方法可以从文本文件加载地图布局。
PathPlanner 类:

职责: 作为任务协调者，整合地图、算法和规划请求。
主要方法: planPath() 是其核心，它直接调用当前 algorithm 策略的 planPath 方法来完成实际的计算工作。
src/algorithm (算法实现)
HybridAStarPSOAlgorithm 类:

职责: 实现 IPathPlanningAlgorithm 接口，是当前系统的核心算法。
工作流程:
第一阶段 (全局引导): 实例化一个内部的 AStarAlgorithm 对象，在栅格地图上快速计算出一条从起点到终点的全局引导路径 astarGuidePath。
第二阶段 (局部优化): 实例化 ZPSO_Algorithm 对象。其适应度函数 (evaluateFitness) 是此算法的精髓，它不仅评估路径长度和碰撞，还引入了一个偏离惩罚项：PSO生成的路径点如果偏离A*引导路径太远，则适应度会降低。
结果: 最终输出的是经过PSO优化后的平滑、无碰撞且大致遵循A*引导方向的路径。
AStarAlgorithm 类:

一个经典的A*算法实现，用于在网格上寻找最短路径。它使用优先队列管理开放列表，并能处理8方向移动。
ZPSOAlgorithm 类:

一个功能完整的PSO算法库，定义了 ZPSO_Partical (粒子) 和 ZPSO_Algorithm (算法主体)。它负责管理粒子群的初始化、速度/位置更新、适应度评估和全局最优解的追踪。
src/visualization (可视化)
Visualizer 类:
职责: 使用SFML库将路径规划的整个过程和结果绘制到窗口中。
条件编译: 整个类的实现被包裹在 #ifdef USE_SFML ... #endif 中。当 CMakeLists.txt 定义了 USE_SFML 宏时（即编译图形版本），这部分代码才生效。否则，会使用一个空的占位符类，避免链接错误。
绘制内容:
drawMap(): 绘制障碍物。
drawStartEnd(): 绘制起点（绿）和终点（红）。
drawAStarGuidePath(): 绘制A*的引导路径（黄线）。
drawPath(): 绘制PSO的最终路径（蓝线）。
src/config (配置管理)
ConfigManager 类:
职责: 负责解析 data/config/pso_config.json 文件。
实现: 它实现了一个简单的手动JSON解析器，逐行读取并解析键值对，然后填充到 AppConfig 结构体中。
## 5. 构建与运行
依赖项
C++17 编译器 (如 GCC/MinGW, Clang, MSVC)
CMake (版本 >= 3.16)
SFML 库 (版本 >= 2.5，仅图形版本需要)
构建步骤 (使用CMake)
配置SFML: 确保SFML库被安装在 C:/Tools/SFML，或者相应地修改 CMakeLists.txt 中的 SFML_ROOT 路径。
创建构建目录:
Bash

mkdir build
cd build
生成构建文件:
Bash

# 对于 MinGW Makefiles
cmake .. -G "MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=TRUE
编译:
Bash

cmake --build .
运行: 生成的可执行文件位于 build/bin/ 目录下。
Bash

# 运行图形版本
./bin/RoboNavPlanner.exe

# 运行控制台版本
./bin/RoboNavPlanner_Console.exe
6. 数据格式说明
地图文件 (data/maps/map.txt)
这是一个简单的文本文件：

第一行: 两个整数，分别代表地图的 高度(height) 和 宽度(width)。
后续行: 一个 height x width 的矩阵，由 0 或 1 组成，用空格分隔。
0: 可通行的自由空间。
1: 障碍物。
示例:

20 20
1 1 1 ...
1 0 0 ...
...
配置文件 (data/config/pso_config.json)
一个JSON文件，用于配置程序的所有可调参数。

JSON

{
    "pso": {
        "generations": 300,       // PSO迭代代数
        "particleCount": 150,     // 粒子数量
        "globalGuideCoe": 2.0,    // 全局引导系数
        "localGuideCoe": 2.0,     // 局部引导系数
        "maxSpeed": 4.0           // 粒子最大速度
    },
    "pathPlanning": {
        "numWaypoints": 6,        // 中间航点数量
        "mapCellSize": 1.0        // 地图单元格大小
    },
    "visualization": {
        "windowWidth": 800,       // 窗口宽度
        "windowHeight": 600,      // 窗口高度
        "frameRate": 60           // 帧率
    }
}
