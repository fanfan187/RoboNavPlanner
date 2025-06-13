// #include "algorithm/ZPSOAlgorithm.h"  // 粒子群优化算法
#include "core/Map.h"
#include "core/PathPlanner.h"
#include "algorithm/HybridAStarPSOAlgorithm.h"
#include "visualization/Visualizer.h"
#include "config/ConfigManager.h"
#include <math.h>
#include <iostream>
#include <chrono>
using namespace std;


int main() {
    std::cout << "=== 基于PSO的机器人路径规划系统 ===" << std::endl;

    // 加载配置文件
    ConfigManager configManager;
    configManager.loadConfig("data/config/pso_config.json");
    const AppConfig& config = configManager.getConfig();
    
    // 打印加载的配置
    configManager.printConfig();

    // 创建地图
    Map robotMap(20, 20, config.pathPlanning.mapCellSize); // 使用配置文件中的单元格大小
    
    // 尝试从文件加载地图
    if (!robotMap.loadFromFile("data/maps/map.txt")) {
        std::cout << "使用默认地图" << std::endl;
    }

    // 设置起点和终点
    Point startPoint(2.0, 2.0);   // 起点
    Point endPoint(17.0, 17.0);   // 终点

    std::cout << "起点: (" << startPoint.x << ", " << startPoint.y << ")" << std::endl;
    std::cout << "终点: (" << endPoint.x << ", " << endPoint.y << ")" << std::endl;

    // 检查起点和终点是否有效
    if (robotMap.isObstacle(startPoint.x, startPoint.y)) {
        std::cout << "警告: 起点位于障碍物中！" << std::endl;
    }
    if (robotMap.isObstacle(endPoint.x, endPoint.y)) {
        std::cout << "警告: 终点位于障碍物中！" << std::endl;
    }

    // 创建算法实例
    auto algorithm = std::make_unique<HybridAStarPSOAlgorithm>();
    
    // 设置算法参数
    algorithm->setParameter("particleCount", config.pso.particleCount);
    algorithm->setParameter("generations", config.pso.generations);
    algorithm->setParameter("globalGuideCoe", config.pso.globalGuideCoe);
    algorithm->setParameter("localGuideCoe", config.pso.localGuideCoe);
    algorithm->setParameter("maxSpeed", config.pso.maxSpeed);

    // 创建路径规划器 - 使用配置文件中的参数
    PathPlanner planner(&robotMap, startPoint, endPoint, std::move(algorithm), config.pathPlanning.numWaypoints);

    std::cout << "\n开始路径规划..." << std::endl;
    std::cout << "使用算法: " << planner.getCurrentAlgorithmName() << std::endl;
    std::cout << "中间航点数量: " << config.pathPlanning.numWaypoints << std::endl;
    std::cout << "粒子维度: " << config.pathPlanning.numWaypoints * 2 << std::endl;

    // 记录开始时间
    auto startTime = std::chrono::high_resolution_clock::now();

    // 执行路径规划 - 使用配置文件中的PSO参数
    auto result = planner.planPath();

    // 记录结束时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "\n路径规划耗时: " << duration.count() << " 毫秒" << std::endl;

    // 输出路径信息
    std::cout << "\n找到的路径包含 " << result.path.size() << " 个点:" << std::endl;
    for (size_t i = 0; i < result.path.size(); i++) {
        std::cout << "  点 " << i << ": (" << result.path[i].x << ", " << result.path[i].y << ")";
        if (i == 0) std::cout << " [起点]";
        else if (i == result.path.size() - 1) std::cout << " [终点]";
        else std::cout << " [航点]";
        std::cout << std::endl;
    }

    // 创建可视化器 - 使用配置文件中的窗口大小
    std::cout << "\n启动可视化窗口..." << std::endl;
    Visualizer visualizer(&robotMap, &planner, 
                         config.visualization.windowWidth, 
                         config.visualization.windowHeight);
    visualizer.setStartEnd(startPoint, endPoint);
    visualizer.setPath(result.path);

    std::cout << "可视化说明:" << std::endl;
    std::cout << "  白色区域: 可通行" << std::endl;
    std::cout << "  黑色区域: 障碍物" << std::endl;
    std::cout << "  绿色圆圈: 起点" << std::endl;
    std::cout << "  红色圆圈: 终点" << std::endl;
    std::cout << "  蓝色线段: 规划路径" << std::endl;
    std::cout << "  蓝色小圆: 中间航点" << std::endl;
    std::cout << "\n关闭窗口以退出程序。" << std::endl;

    // 运行可视化
    visualizer.run();

    std::cout << "程序结束。" << std::endl;
    return 0;
}
