RoboNavPlanner 项目运行指南
项目概述
这是一个基于混合A-PSO算法的机器人路径规划系统，提供图形界面和控制台两种版本。
📋 系统要求
必要依赖
C++编译器：支持C++17标准
Windows: MinGW-w64 或 Visual Studio 2019+
Linux: GCC 7+ 或 Clang 5+
SFML图形库（仅图形版本需要）
Windows: SFML 2.5+
安装路径：C:\Tools\SFML（或修改CMakeLists.txt中的路径）
CMake（可选，推荐）
版本 3.16+
🔧 环境配置
Windows环境配置
方法1：使用预编译的批处理脚本（推荐）
配置SFML路径
Apply to execution_ou...
编译图形版本
Apply to execution_ou...
编译控制台版本（无需SFML）
Apply to execution_ou...
方法2：使用CMake构建
配置SFML路径
Apply to execution_ou...
创建构建目录
Apply to execution_ou...
Run
生成构建文件
Apply to execution_ou...
Run
编译项目
Apply to execution_ou...
Run
Linux环境配置
安装依赖
Apply to execution_ou...
Run
编译项目
Apply to execution_ou...
Run
🚀 运行项目
图形界面版本
Apply to execution_ou...
Run
可视化说明：
🟩 绿色圆圈：起点
🔴 红色圆圈：终点
🟨 黄色虚线：A引导路径
🔵 蓝色实线：PSO优化路径
⚫ 黑色区域：障碍物
⚪ 白色区域：可通行区域
控制台版本
Apply to execution_ou...
Run
⚙️ 配置文件
路径规划参数 (data/config/pso_config.json)
Apply to execution_ou...
地图文件格式 (data/maps/map.txt)
Apply to execution_ou...
🎮 使用说明
基本操作
启动程序：运行对应的可执行文件
查看结果：程序会自动规划从起点(2,2)到终点(17,17)的路径
分析路径：观察A引导路径和PSO优化路径的差异
自定义参数
修改 src/main.cpp 中的参数：
Apply to execution_ou...
🛠️ 故障排除
常见问题
1. 编译错误：找不到SFML
Apply to execution_ou...
解决方案：
确保SFML正确安装
检查compile.bat或CMakeLists.txt中的SFML路径配置
2. 运行时错误：缺少DLL文件
Apply to execution_ou...
解决方案：
将SFML的DLL文件复制到可执行文件目录
或者将SFML的bin目录添加到系统PATH
3. 配置文件找不到
Apply to execution_ou...
解决方案：
确保在项目根目录运行程序
检查配置文件路径是否正确
4. 地图文件加载失败
Apply to execution_ou...
解决方案：
检查data/maps/map.txt文件是否存在
确认地图文件格式正确
性能优化建议
调整PSO参数：
减少粒子数量（particleCount: 30-50）
减少迭代次数（generations: 50-100）
适用于快速测试
调整A引导权重：
增加deviationWeight（5.0-10.0）：更贴近A路径
减少deviationWeight（1.0-3.0）：更自由的PSO优化
修改地图复杂度：
简单地图：减少障碍物，降低算法复杂度
复杂地图：增加障碍物，测试算法鲁棒性
📁 项目结构
Apply to execution_ou...
🎯 项目特色
混合算法：结合A全局搜索和PSO局部优化
实时可视化：直观显示两种算法的路径对比
参数可调：支持运行时调整算法参数
跨平台：支持Windows和Linux系统
模块化设计：易于扩展和维护