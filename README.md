# 基于粒子群优化(PSO)的机器人路径规划

这是一个使用粒子群优化算法进行机器人路径规划的C++项目，具有SFML可视化功能。

## 项目特性

- 🤖 基于PSO算法的智能路径规划
- 🗺️ 支持自定义障碍物地图
- 🎯 避障和路径长度优化
- 📊 实时可视化显示
- ⚡ 高效的碰撞检测算法

## 文件结构 
RoboNavPlanner/
├── README.md                     # 项目说明文档
├── CMakeLists.txt               # 现代化构建系统
├── Makefile                     # 备用构建系统
├── compile.bat                  # Windows快速编译脚本
├── 
├── src/                         # 源代码目录
│   ├── main.cpp                # 主程序入口
│   ├── algorithm/              # 算法模块
│   │   ├── ZPSOAlgorithm.h    # PSO算法头文件
│   │   └── ZPSOAlgorithm.cpp  # PSO算法实现
│   ├── core/                   # 核心模块
│   │   ├── Map.h              # 地图类头文件
│   │   ├── Map.cpp            # 地图类实现
│   │   ├── PathPlanner.h      # 路径规划器头文件
│   │   └── PathPlanner.cpp    # 路径规划器实现
│   └── visualization/          # 可视化模块
│       ├── Visualizer.h       # 可视化器头文件
│       └── Visualizer.cpp     # 可视化器实现
│
├── include/                     # 公共头文件目录
│   └── Common.h                # 公共类型定义
│
├── data/                       # 数据文件目录
│   ├── maps/                  # 地图文件
│   │   ├── map.txt           # 默认地图
│   │   └── complex_map.txt   # 复杂地图
│   └── config/               # 配置文件
│       └── pso_config.json   # PSO参数配置
│
├── build/                      # 构建输出目录
│   └── .gitkeep              # 保持目录结构
│
├── docs/                       # 文档目录
│   ├── algorithm.md           # 算法说明
│   ├── usage.md              # 使用说明
│   └── api.md                # API文档
│
├── tests/                      # 测试目录
│   ├── test_map.cpp          # 地图测试
│   ├── test_planner.cpp      # 规划器测试
│   └── test_data/            # 测试数据
│
└── scripts/                    # 构建和工具脚本
    ├── setup.sh              # Linux/Mac 环境设置
    ├── setup.bat             # Windows 环境设置
    └── run_tests.sh          # 运行测试脚本