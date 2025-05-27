// #include "../src/algorithm/ZPSOAlgorithm.h"

#include "core/Map.h"
#include "core/PathPlanner.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

// 简单的ASCII可视化函数
void printMap(const Map& map, const std::vector<Point>& path, const Point& start, const Point& end) {
    cout << "\n=== 地图和路径可视化 ===" << endl;
    
    // 创建显示矩阵
    vector<vector<char>> display(map.getHeight(), vector<char>(map.getWidth(), ' '));
    
    // 填充障碍物
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            if (map.getCell(x, y) == 1) {
                display[y][x] = '#';  // 障碍物
            } else {
                display[y][x] = '.';  // 可通行区域
            }
        }
    }
    
    // 标记路径
    for (size_t i = 1; i < path.size() - 1; i++) {
        int x = static_cast<int>(path[i].x / map.getCellSize());
        int y = static_cast<int>(path[i].y / map.getCellSize());
        if (x >= 0 && x < map.getWidth() && y >= 0 && y < map.getHeight()) {
            display[y][x] = '*';  // 路径点
        }
    }
    
    // 标记起点和终点
    int startX = static_cast<int>(start.x / map.getCellSize());
    int startY = static_cast<int>(start.y / map.getCellSize());
    int endX = static_cast<int>(end.x / map.getCellSize());
    int endY = static_cast<int>(end.y / map.getCellSize());
    
    if (startX >= 0 && startX < map.getWidth() && startY >= 0 && startY < map.getHeight()) {
        display[startY][startX] = 'S';  // 起点
    }
    if (endX >= 0 && endX < map.getWidth() && endY >= 0 && endY < map.getHeight()) {
        display[endY][endX] = 'E';  // 终点
    }
    
    // 打印地图
    cout << "图例: S=起点, E=终点, *=路径, #=障碍物, .=可通行" << endl;
    cout << "  ";
    for (int x = 0; x < map.getWidth(); x++) {
        cout << setw(2) << x % 10;
    }
    cout << endl;
    
    for (int y = 0; y < map.getHeight(); y++) {
        cout << setw(2) << y % 10;
        for (int x = 0; x < map.getWidth(); x++) {
            cout << setw(2) << display[y][x];
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    cout << "=== 基于PSO的机器人路径规划系统 (控制台版本) ===" << endl;

    // 创建地图
    Map robotMap(20, 20, 1.0);
    
    // 尝试从文件加载地图
    if (!robotMap.loadFromFile("data/maps/map.txt")) {
        cout << "无法加载地图文件，使用默认地图" << endl;
    }

    // 设置起点和终点
    Point startPoint(2.0, 2.0);
    Point endPoint(17.0, 17.0);

    cout << "起点: (" << startPoint.x << ", " << startPoint.y << ")" << endl;
    cout << "终点: (" << endPoint.x << ", " << endPoint.y << ")" << endl;

    // 检查起点和终点是否有效
    if (robotMap.isObstacle(startPoint.x, startPoint.y)) {
        cout << "警告: 起点位于障碍物中！" << endl;
    }
    if (robotMap.isObstacle(endPoint.x, endPoint.y)) {
        cout << "警告: 终点位于障碍物中！" << endl;
    }

    // 创建路径规划器
    int numWaypoints = 6;
    PathPlanner planner(&robotMap, startPoint, endPoint, numWaypoints);

    cout << "\n开始路径规划..." << endl;
    cout << "中间航点数量: " << numWaypoints << endl;
    cout << "粒子维度: " << numWaypoints * 2 << endl;

    // 记录开始时间
    auto startTime = chrono::high_resolution_clock::now();

    // 执行路径规划
    int generations = 300;
    int particleCount = 150;
    
    cout << "PSO参数:" << endl;
    cout << "  进化代数: " << generations << endl;
    cout << "  粒子数量: " << particleCount << endl;

    vector<Point> bestPath = planner.planPath(generations, particleCount);

    // 记录结束时间
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    cout << "\n路径规划耗时: " << duration.count() << " 毫秒" << endl;

    // 输出路径信息
    cout << "\n找到的路径包含 " << bestPath.size() << " 个点:" << endl;
    for (size_t i = 0; i < bestPath.size(); i++) {
        cout << "  点 " << i << ": (" << fixed << setprecision(2) 
             << bestPath[i].x << ", " << bestPath[i].y << ")";
        if (i == 0) cout << " [起点]";
        else if (i == bestPath.size() - 1) cout << " [终点]";
        else cout << " [航点]";
        cout << endl;
    }

    // ASCII可视化
    printMap(robotMap, bestPath, startPoint, endPoint);

    // 路径质量分析
    double totalLength = planner.calculatePathLength(bestPath);
    bool hasCollision = planner.isPathColliding(bestPath);
    
    cout << "=== 路径质量分析 ===" << endl;
    cout << "总路径长度: " << fixed << setprecision(2) << totalLength << endl;
    cout << "是否有碰撞: " << (hasCollision ? "是" : "否") << endl;
    
    if (!hasCollision) {
        cout << "✓ 路径规划成功！找到了一条无碰撞路径。" << endl;
    } else {
        cout << "✗ 警告：路径存在碰撞，可能需要调整PSO参数。" << endl;
    }

    cout << "\n程序结束。" << endl;
    return 0;
} 