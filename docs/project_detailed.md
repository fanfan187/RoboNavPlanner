# RoboNavPlanner 项目文档

## 1. 项目概述
RoboNavPlanner 是一个使用 C++ 实现的机器人路径规划项目，其核心算法是**粒子群优化（PSO）**。该项目旨在在一个包含障碍物的二维地图中，找到从起点到终点的最优（或接近最优）路径。路径的优化目标主要有两个：**避开所有障碍物**和**最小化路径总长度**。

### 项目提供了两种运行模式：
- **图形化版本**：基于 SFML 库，提供动态、直观的路径和地图可视化界面。
- **控制台版本**：无外部库依赖，在终端中通过 ASCII 字符打印地图和路径，方便在不支持图形环境的系统上运行和调试。

### 核心特性:
- **智能路径规划**: 采用粒子群优化（PSO）算法搜索最佳路径。
- **自定义地图**: 支持从文本文件加载用户自定义的障碍物地图。
- **高效碰撞检测**: 使用 Bresenham 直线算法快速检测路径段是否与障碍物碰撞。
- **双版本支持**: 提供带 SFML 图形界面的版本和纯控制台版本。

---

## 2. 文件结构与模块说明
```text
RoboNavPlanner/
├── README.md                     # 项目说明文档
├── CMakeLists.txt                # CMake 构建配置文件
├── data/                         # 数据目录
│   ├── maps/                     # 地图文件
│   │   └── map.txt               # 默认地图数据
│   └── config/                   # 配置文件
│       └── pso_config.json       # PSO参数配置 (注意：当前代码未实际加载此文件)
├── src/                          # 源代码目录
│   ├── main.cpp                  # 图形化版本主程序入口
│   ├── main_console.cpp          # 控制台版本主程序入口
│   ├── algorithm/                # 算法模块
│   │   └── ZPSOAlgorithm.h       # PSO 算法的实现
│   ├── core/                     # 核心逻辑模块
│   │   ├── Common.h              # 通用类型定义 (当前基本为空)
│   │   ├── Map.h                 # 地图类的定义与实现
│   │   ├── Map.cpp               # (在项目中，Map类的实现被直接放在了.h文件中)
│   │   ├── PathPlanner.h         # 路径规划器的定义与实现
│   │   └── PathPlanner.cpp       # 路径规划器静态成员的定义
│   └── visualization/            # 可视化模块
│       ├── Visualizer.h          # SFML可视化器的定义
│       └── Visualizer.cpp        # SFML可视化器的实现
├── include/                      # 公共头文件目录
│   └── Common.h                  # 另一个通用头文件，定义了Point等结构
└── build/                        # 构建输出目录
```

### 模块说明:

- **src/algorithm**: 包含路径规划所使用的核心智能算法。`ZPSOAlgorithm.h` 文件完整实现了粒子群优化算法的逻辑。
- **src/core**: 包含项目的主要业务逻辑。`Map` 类负责地图的表示和碰撞检测，而 `PathPlanner` 类是连接地图和 PSO 算法的桥梁，负责定义适应度函数并执行规划流程。
- **src/visualization**: 包含图形化显示功能。`Visualizer` 类使用 SFML 库将地图、障碍物、起点、终点和规划出的路径绘制在窗口中。这部分代码通过宏 `USE_SFML` 进行条件编译，以支持无 SFML 依赖的控制台版本。
- **data**: 存放程序运行时所需的外部数据，如地图文件和配置文件。
- **include**: 用于存放可能在项目多个模块间共享的公共头文件。

---

## 3. 核心类与算法详解

### 3.1 地图模块 (`Map` Class)

`Map` 类定义了机器人所处的环境。

#### 数据结构:
- 内部使用 `std::vector<std::vector<int>> grid` 表示地图。
- `0` 表示可通行区域，`1` 表示障碍物。

#### 地图加载:
- 可通过构造函数创建默认地图。
- 也可通过 `loadFromFile(filename)` 方法从文本文件加载地图。
  - 第一行是地图的高度和宽度。
  - 后续行为具体的网格数据。

#### 碰撞检测:
- 核心方法为 `isLineColliding(start, end)`。
- 使用 **Bresenham 直线算法** 来判断直线段是否穿过障碍物。

#### 坐标系统:
- 处理世界坐标（`double` 类型）和网格坐标（`int` 类型）。
- 提供 `isInBounds` 和 `isObstacle` 方法检查点的有效性。

---

### 3.2 路径规划器 (`PathPlanner` Class)

`PathPlanner` 是整个路径规划任务的协调者。

#### 职责:
- 连接 `Map` 对象和 `ZPSOAlgorithm` 算法。
- 将 PSO 中的粒子解码为具体路径。
- 定义并提供适应度函数给 PSO 算法。

#### 路径解码 (`decodePath`):
- 粒子由一维数组 `_position` 表示。
- 解码为航点，并在首尾加上起点和终点。

#### 适应度函数 (`evaluateFitness`):
评估路径优劣的标准包括：
- **巨大碰撞惩罚**：路径与障碍物碰撞会受到极大负惩罚。
- **边界惩罚**：航点超出地图边界受到较大负惩罚。
- **长度惩罚**：路径越长，适应度越低。
- **平滑度奖励**：对更平滑路径给予奖励。

#### 静态函数跳板 (`staticFitnessFuncton`):
解决非静态成员函数作为适应度函数的问题，使用静态指针调用。

---

### 3.3 粒子群优化算法 (`ZPSOAlgorithm.h`)

#### `ZPSO_Partical` 类:
- `_position`: 当前位置（航点坐标）。
- `_velocity`: 当前飞行速度。
- `_bestPosition`: 历史最佳位置。
- `_fitness`: 当前适应度。
- `_bestFitness`: 历史最佳适应度。

#### `ZPSO_Algorithm` 类:
- 管理一群粒子。
- 在每次迭代中更新粒子的速度和位置。

##### 初始化:
- 在指定范围内随机初始化粒子。

##### 更新规则 (`update`):
- 速度根据全局最优和个体最优更新。

##### 寻优 (`findMax`):
- 执行指定代数的迭代，返回全局最优粒子。

---

### 3.4 可视化模块 (`Visualizer` Class)

负责将规划结果以图形方式呈现。

#### 依赖:
- SFML 图形库（`sf::RenderWindow`, `sf::RectangleShape`, `sf::CircleShape` 等）。

#### 条件编译:
- 使用 `#ifdef USE_SFML` 包裹整个类，仅当启用 SFML 时才编译。

#### 绘制内容:
- `drawMap()`: 绘制地图网格与障碍物（黑色方块）。
- `drawStartEnd()`: 绘制起点（绿色圆）、终点（红色圆）。
- `drawPath()`: 绘制路径（蓝色线段）及航点（蓝色小圆）。

#### 主循环 (`run`):
事件处理和渲染循环，直到窗口关闭。

---

## 4. 数据格式

### 4.1 地图文件 (`map.txt`)
简单文本文件格式如下：

- 第一行：两个整数 `height width`。
- 后续行为 `height x width` 的矩阵，由 `0` 和 `1` 组成。

#### 示例：
```text
20 20
1 1 1 1 1 ...
1 0 0 0 0 ...
...
```

### 4.2 配置文件 (`pso_config.json`)
- 用于定义 PSO 参数（如进化代数、粒子数量等）。
- 注意：当前版本并未实际解析和使用此文件，参数均硬编码。

---

## 5. 构建与运行

### 5.1 依赖
- **控制台版本**：仅需支持 C++14 或更高版本的编译器（如 g++）。
- **图形版本**：还需安装 SFML 库（graphics, window, system）。

### 5.2 使用 CMake 构建 (推荐)
1. 安装 CMake 和编译器（如 MinGW）。
2. 安装 SFML（如需图形版本）。
3. 在项目根目录创建 `build` 文件夹。
4. 进入 build 文件夹，运行 `cmake ..`，然后运行 `make` (或 `mingw32-make`)。
5. 生成的可执行文件将位于  `build/bin/` 目录下。

### 5.3. 手动编译
`README.md` 也提供了直接使用 g++ 的编译指令。

- 控制台版本 (Linux/Mac):
```Bash

g++ -std=c++17 -Isrc -o RoboNavPlanner_Console src/main_console.cpp
```
- 图形版本: 需要手动链接 SFML 库，相对复杂，推荐使用CMake。

---
  
## 6. 程序入口与执行流程
### 6.1. 图形版本 (main.cpp)
创建 `Map` 对象并从 `data/maps/map.txt` 加载地图。
设置起点和终点。
创建 `PathPlanner` 对象，并传入地图、起点、终点和航点数量。
调用 `planner.planPath()`，传入进化代数和粒子数量，执行PSO算法进行路径规划。
`planPath` 函数返回找到的最佳路径 `bestPath`。
创建 `Visualizer` 对象。
将最佳路径、起点、终点设置给 `visualizer`。
调用 `visualizer.run()` 启动可视化窗口并进入主循环，直到用户关闭窗口。
### 6.2. 控制台版本 (main_console.cpp)
执行流程与图形版本的前4步完全相同。
`planPath` 函数返回最佳路径 `bestPath`。
在控制台输出路径的详细信息（坐标、长度、是否碰撞等）。
调用 `printMap()` 函数，该函数在字符矩阵中用 S(起点), E(终点), *(路径), #(障碍物) 来绘制ASCII艺术形式的地图和路径。

---

## 7. 代码问题与改进建议
1. 重复的 `Point` 结构定义: 在 `src/core/Map.h` 和 `include/Common.h` 中都定义了 `Point` 结构。这可能导致定义冲突或不一致。建议统一使用一个 `Point` 定义（例如 `include/Common.h` 中功能更丰富的版本），并在整个项目中包含它。
2. 未使用的配置文件: `data/config/pso_config.json` 从未被加载。建议在 `main` 函数中添加读取此JSON文件的逻辑，使程序参数化，而不是硬编码。
3. 内存管理: 在 `PathPlanner.h` 的 `planPath` 方法中，使用了 `new double[...]` 来分配内存，并在函数末尾使用 `delete[]` 释放。这在C++中是容易出错的实践。建议使用 `std::vector<double>` 来自动管理内存，更加安全和现代化。`ZPSOAlgorithm.h` 中也存在大量的裸指针和手动内存管理，可以考虑用智能指针或标准容器进行重构。
4. 算法与核心逻辑耦合: `PathPlanner.h` 直接包含了 `ZPSOAlgorithm.h`。更灵活的设计可能是定义一个抽象的 `IAlgorithm` 接口，使 `PathPlanner` 能与任何实现了该接口的算法（如A*, RRT*等）协同工作，提高可扩展性。