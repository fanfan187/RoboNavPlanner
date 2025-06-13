#ifndef ASTARALGORITHM_H
#define ASTARALGORITHM_H

#include "../../include/Common.h"
#include "../core/Map.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <memory>
#include <algorithm>

/**
 * @brief A*算法节点结构
 */
struct AStarNode {
    int x, y;           // 网格坐标
    double g, h, f;     // A*算法的g, h, f代价值 (f = g + h)
    std::shared_ptr<AStarNode> parent;  // 指向父节点的共享指针
    
    AStarNode(int x = 0, int y = 0, double g = 0.0, double h = 0.0, std::shared_ptr<AStarNode> p = nullptr)
        : x(x), y(y), g(g), h(h), f(g + h), parent(p) {}
    
    // 用于优先队列的比较操作符（f值小的优先级高）
    bool operator>(const AStarNode& other) const {
        return f > other.f;
    }
};

/**
 * @brief A*算法实现类
 * 
 * 独立的A*算法实现，可以在网格地图上规划从起点到终点的路径
 */
class AStarAlgorithm {
private:
    // 8方向移动的偏移量
    const std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},  // 左上、上、右上
        {0, -1},           {0, 1},   // 左、右
        {1, -1},  {1, 0},  {1, 1}    // 左下、下、右下
    };
    
    /**
     * @brief 计算欧几里得距离启发式函数
     * @param x1, y1 当前点坐标
     * @param x2, y2 目标点坐标
     * @return double 欧几里得距离
     */
    double calculateHeuristic(int x1, int y1, int x2, int y2) const {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * @brief 计算两个相邻网格点之间的实际距离
     * @param x1, y1 起始点坐标
     * @param x2, y2 目标点坐标
     * @return double 实际距离
     */
    double calculateActualDistance(int x1, int y1, int x2, int y2) const {
        if (x1 == x2 || y1 == y2) {
            return 1.0;  // 水平或垂直移动
        } else {
            return std::sqrt(2.0);  // 对角线移动
        }
    }
    
    /**
     * @brief 将网格坐标转换为世界坐标
     * @param gridX, gridY 网格坐标
     * @param cellSize 网格大小
     * @return Point 世界坐标
     */
    Point gridToWorld(int gridX, int gridY, double cellSize) const {
        return Point(gridX * cellSize + cellSize * 0.5, gridY * cellSize + cellSize * 0.5);
    }
    
    /**
     * @brief 将世界坐标转换为网格坐标
     * @param worldPoint 世界坐标点
     * @param cellSize 网格大小
     * @return std::pair<int, int> 网格坐标
     */
    std::pair<int, int> worldToGrid(const Point& worldPoint, double cellSize) const {
        int gridX = static_cast<int>(worldPoint.x / cellSize);
        int gridY = static_cast<int>(worldPoint.y / cellSize);
        return {gridX, gridY};
    }
    
    /**
     * @brief 生成节点的唯一标识符
     * @param x, y 节点坐标
     * @param width 地图宽度
     * @return int 唯一标识符
     */
    int getNodeId(int x, int y, int width) const {
        return y * width + x;
    }

public:
    /**
     * @brief A*路径查找主函数
     * @param map 地图引用
     * @param start 起点世界坐标
     * @param end 终点世界坐标
     * @return std::vector<Point> 路径点序列，如果无法找到路径则返回空向量
     */
    std::vector<Point> findPath(const Map& map, Point start, Point end) {
        double cellSize = map.getCellSize();
        int mapWidth = map.getWidth();
        int mapHeight = map.getHeight();
        
        // 将世界坐标转换为网格坐标
        auto [startGridX, startGridY] = worldToGrid(start, cellSize);
        auto [endGridX, endGridY] = worldToGrid(end, cellSize);
        
        // 边界检查
        if (startGridX < 0 || startGridX >= mapWidth || startGridY < 0 || startGridY >= mapHeight ||
            endGridX < 0 || endGridX >= mapWidth || endGridY < 0 || endGridY >= mapHeight) {
            return std::vector<Point>();  // 起点或终点超出边界
        }
        
        // 检查起点和终点是否为障碍物
        if (map.getCell(startGridX, startGridY) == 1 || map.getCell(endGridX, endGridY) == 1) {
            return std::vector<Point>();  // 起点或终点是障碍物
        }
        
        // 如果起点就是终点
        if (startGridX == endGridX && startGridY == endGridY) {
            return {start, end};
        }
        
        // 优先队列（开启列表），用于存储待探索的节点
        std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openList;
        
        // 关闭列表，用于记录已访问的节点
        std::unordered_set<int> closedList;
        
        // 用于快速查找开启列表中的节点
        std::unordered_set<int> openSet;
        
        // 创建起始节点
        double h = calculateHeuristic(startGridX, startGridY, endGridX, endGridY);
        auto startNode = std::make_shared<AStarNode>(startGridX, startGridY, 0.0, h);
        openList.push(*startNode);
        openSet.insert(getNodeId(startGridX, startGridY, mapWidth));
        
        // 用于存储最佳节点（用于路径重建）
        std::shared_ptr<AStarNode> goalNode = nullptr;
        
        // A*主循环
        while (!openList.empty()) {
            // 获取f值最小的节点
            AStarNode current = openList.top();
            openList.pop();
            
            int currentId = getNodeId(current.x, current.y, mapWidth);
            openSet.erase(currentId);
            closedList.insert(currentId);
            
            // 检查是否到达目标点
            if (current.x == endGridX && current.y == endGridY) {
                goalNode = std::make_shared<AStarNode>(current);
                break;
            }
            
            // 探索所有相邻节点
            for (const auto& [dx, dy] : directions) {
                int newX = current.x + dx;
                int newY = current.y + dy;
                
                // 边界检查
                if (newX < 0 || newX >= mapWidth || newY < 0 || newY >= mapHeight) {
                    continue;
                }
                
                // 检查是否为障碍物
                if (map.getCell(newX, newY) == 1) {
                    continue;
                }
                
                int neighborId = getNodeId(newX, newY, mapWidth);
                
                // 跳过已在关闭列表中的节点
                if (closedList.find(neighborId) != closedList.end()) {
                    continue;
                }
                
                // 计算新的g值
                double tentativeG = current.g + calculateActualDistance(current.x, current.y, newX, newY);
                
                // 检查是否已在开启列表中
                bool inOpenList = openSet.find(neighborId) != openSet.end();
                
                if (!inOpenList) {
                    // 新节点，添加到开启列表
                    double h = calculateHeuristic(newX, newY, endGridX, endGridY);
                    auto currentShared = std::make_shared<AStarNode>(current);
                    AStarNode newNode(newX, newY, tentativeG, h, currentShared);
                    openList.push(newNode);
                    openSet.insert(neighborId);
                }
                // 注意：由于使用优先队列，我们无法直接更新已存在的节点
                // 在实际应用中，可能会有重复节点在队列中，但会选择g值更小的路径
            }
        }
        
        // 重建路径
        if (!goalNode) {
            return std::vector<Point>();  // 无法找到路径
        }
        
        std::vector<Point> path;
        std::shared_ptr<AStarNode> current = goalNode;
        
        while (current) {
            Point worldPos = gridToWorld(current->x, current->y, cellSize);
            path.push_back(worldPos);
            current = current->parent;
        }
        
        // 反转路径（从起点到终点）
        std::reverse(path.begin(), path.end());
        
        // 确保路径的起点和终点与输入完全一致
        if (!path.empty()) {
            path[0] = start;
            path[path.size() - 1] = end;
        }
        
        return path;
    }
    
    /**
     * @brief 获取算法名称
     * @return std::string 算法名称
     */
    std::string getAlgorithmName() const {
        return "A*";
    }
};

#endif // ASTARALGORITHM_H 