// #include "algorithm/ZPSOAlgorithm.h"  // 粒子群优化算法
#include "core/Map.h"
#include "core/PathPlanner.h"
#include "visualization/Visualizer.h"
#include <math.h>
#include <iostream>
#include <chrono>
using namespace std;


int main() {
    std::cout << "=== 基于PSO的机器人路径规划系统 ===" << std::endl;

    // 创建地图
    Map robotMap(20, 20, 1.0); // 20x20的网格，每个单元格大小为1.0
    
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

    // 创建路径规划器
    int numWaypoints = 6; // 中间航点数量
    PathPlanner planner(&robotMap, startPoint, endPoint, numWaypoints);

    std::cout << "\n开始路径规划..." << std::endl;
    std::cout << "中间航点数量: " << numWaypoints << std::endl;
    std::cout << "粒子维度: " << numWaypoints * 2 << std::endl;

    // 记录开始时间
    auto startTime = std::chrono::high_resolution_clock::now();

    // 执行路径规划
    int generations = 300;    // 进化代数
    int particleCount = 150;  // 粒子数量
    
    std::cout << "PSO参数:" << std::endl;
    std::cout << "  进化代数: " << generations << std::endl;
    std::cout << "  粒子数量: " << particleCount << std::endl;

    std::vector<Point> bestPath = planner.planPath(generations, particleCount);

    // 记录结束时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "\n路径规划耗时: " << duration.count() << " 毫秒" << std::endl;

    // 输出路径信息
    std::cout << "\n找到的路径包含 " << bestPath.size() << " 个点:" << std::endl;
    for (size_t i = 0; i < bestPath.size(); i++) {
        std::cout << "  点 " << i << ": (" << bestPath[i].x << ", " << bestPath[i].y << ")";
        if (i == 0) std::cout << " [起点]";
        else if (i == bestPath.size() - 1) std::cout << " [终点]";
        else std::cout << " [航点]";
        std::cout << std::endl;
    }

    // 创建可视化器
    std::cout << "\n启动可视化窗口..." << std::endl;
    Visualizer visualizer(&robotMap, &planner, 800, 600);
    visualizer.setStartEnd(startPoint, endPoint);
    visualizer.setPath(bestPath);

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
