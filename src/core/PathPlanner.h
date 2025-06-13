#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include "../../include/Common.h"
#include "../../include/algorithm/IPathPlanningAlgorithm.h"
#include "Map.h"
#include <vector>
#include <memory>
#include <chrono>

class PathPlanner {
private:
    Map* map;
    Point startPoint;
    Point endPoint;
    int numWaypoints;
    std::vector<Point> bestPath;
    
    // 算法策略：使用抽象接口而非具体实现
    std::unique_ptr<IPathPlanningAlgorithm> algorithm;
    
    // 静态成员变量
    static PathPlanner* currentPlanner;
    
    // 路径评估器（独立于算法）
    class PathEvaluator {
    private:
        const Map* map;
        
    public:
        explicit PathEvaluator(const Map* m) : map(m) {}
        
        double calculatePathLength(const std::vector<Point>& path) const {
            double totalLength = 0.0;
            for (size_t i = 1; i < path.size(); i++) {
                totalLength += path[i-1].distanceTo(path[i]);
            }
            return totalLength;
        }
        
        bool isPathColliding(const std::vector<Point>& path) const {
            for (size_t i = 1; i < path.size(); i++) {
                if (map->isLineColliding(path[i-1], path[i])) {
                    return true;
                }
            }
            return false;
        }
        
        double calculateSmoothness(const std::vector<Point>& path) const {
            if (path.size() < 3) return 1.0;
            
            double totalSmoothness = 0.0;
            for (size_t i = 1; i < path.size() - 1; i++) {
                Point prev = path[i-1];
                Point curr = path[i];
                Point next = path[i+1];
                
                // 计算转角
                double v1x = curr.x - prev.x;
                double v1y = curr.y - prev.y;
                double v2x = next.x - curr.x;
                double v2y = next.y - curr.y;
                
                double dot = v1x * v2x + v1y * v2y;
                double mag1 = std::sqrt(v1x * v1x + v1y * v1y);
                double mag2 = std::sqrt(v2x * v2x + v2y * v2y);
                
                if (mag1 > 0 && mag2 > 0) {
                    double cosAngle = dot / (mag1 * mag2);
                    totalSmoothness += cosAngle;
                }
            }
            return totalSmoothness / (path.size() - 2);
        }
    };
    
    PathEvaluator evaluator;

public:
    // 构造函数：接受算法策略
    PathPlanner(Map* m, const Point& start, const Point& end, 
                std::unique_ptr<IPathPlanningAlgorithm> algo,
                int waypoints = 5);

    // 设置起点和终点
    void setStartEnd(const Point& start, const Point& end);
    
    // 切换算法（策略模式）
    void setAlgorithm(std::unique_ptr<IPathPlanningAlgorithm> algo);
    
    // 获取当前算法信息
    std::string getCurrentAlgorithmName() const;

    // 执行路径规划 - 现在通过抽象接口调用
    PathPlanningResult planPath();
    
    // 路径质量分析方法
    double calculatePathLength(const std::vector<Point>& path) const;
    
    bool isPathColliding(const std::vector<Point>& path) const;
    
    double calculateSmoothness(const std::vector<Point>& path) const;

    // 获取最佳路径
    const std::vector<Point>& getBestPath() const;
};

#endif // PATHPLANNER_H 