#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include "../algorithm/ZPSOAlgorithm.h"
#include "Map.h"
#include <vector>
#include <cmath>
#include <iostream>

class PathPlanner {
private:
    Map* map;
    Point startPoint;
    Point endPoint;
    int numWaypoints;
    std::vector<Point> bestPath;
    // 添加 currentPlanner 静态声明
    static PathPlanner* currentPlanner;

public:
    PathPlanner(Map* m, const Point& start, const Point& end, int waypoints = 5) 
        : map(m), startPoint(start), endPoint(end), numWaypoints(waypoints) {}

    // 设置起点和终点
    void setStartEnd(const Point& start, const Point& end) {
        startPoint = start;
        endPoint = end;
    }

    // 从粒子位置解码路径
    std::vector<Point> decodePath(const ZPSO_Partical& particle) const {
        std::vector<Point> path;
        path.push_back(startPoint);

        // 添加中间航点
        for (int i = 0; i < numWaypoints; i++) {
            double x = particle._position[i * 2];
            double y = particle._position[i * 2 + 1];
            path.push_back(Point(x, y));
        }

        path.push_back(endPoint);
        return path;
    }

    // 计算路径总长度
    double calculatePathLength(const std::vector<Point>& path) const {
        double totalLength = 0.0;
        for (size_t i = 1; i < path.size(); i++) {
            double dx = path[i].x - path[i-1].x;
            double dy = path[i].y - path[i-1].y;
            totalLength += sqrt(dx * dx + dy * dy);
        }
        return totalLength;
    }

    // 检查路径是否与障碍物碰撞
    bool isPathColliding(const std::vector<Point>& path) const {
        for (size_t i = 1; i < path.size(); i++) {
            if (map->isLineColliding(path[i-1], path[i])) {
                return true;
            }
        }
        return false;
    }

    // 适应度函数（PSO要最大化此值）
    double evaluateFitness(ZPSO_Partical& particle) {
        std::vector<Point> path = decodePath(particle);
        
        // 基础适应度
        double fitness = 1000.0;

        // 碰撞惩罚（最重要）
        if (isPathColliding(path)) {
            fitness -= 10000.0; // 巨大惩罚
        }

        // 检查航点是否在边界内
        for (int i = 0; i < numWaypoints; i++) {
            double x = particle._position[i * 2];
            double y = particle._position[i * 2 + 1];
            if (!map->isInBounds(x, y)) {
                fitness -= 5000.0; // 边界惩罚
            }
        }

        // 路径长度惩罚
        double pathLength = calculatePathLength(path);
        fitness -= pathLength * 10.0; // 路径越长，适应度越低

        // 平滑度奖励（减少急转弯）
        double smoothnessBonus = 0.0;
        if (path.size() >= 3) {
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
                double mag1 = sqrt(v1x * v1x + v1y * v1y);
                double mag2 = sqrt(v2x * v2x + v2y * v2y);
                
                if (mag1 > 0 && mag2 > 0) {
                    double cosAngle = dot / (mag1 * mag2);
                    smoothnessBonus += cosAngle * 50.0; // 角度越小（越平滑），奖励越高
                }
            }
        }
        fitness += smoothnessBonus;

        return fitness;
    }

    // 创建一个静态成员函数作为包装器/跳板
    static double staticFitnessFuncton(ZPSO_Partical& particle) {
        if (currentPlanner != nullptr) {
            // 通过静态指针调用当前实例的非静态成员函数
            return currentPlanner->evaluateFitness(particle);
        } else {
            // 错误处理：currentPlaner 未设置
            // 对于最大化问题，返回一个非常差的适应度值
            std::cerr << "错误：PathPlanner 未被设置！" << std::endl;
            return -1.0e100;
        }
    }

    // 执行路径规划
    std::vector<Point> planPath(int generations = 200, int particleCount = 100) {
        // 设置PSO参数
        int dimension = numWaypoints * 2; // 每个航点有x,y两个坐标
        
        // 设置搜索边界
        double* minPos = new double[dimension];
        double* maxPos = new double[dimension];
        
        double mapWidth = map->getWidth() * map->getCellSize();
        double mapHeight = map->getHeight() * map->getCellSize();
        
        for (int i = 0; i < dimension; i += 2) {
            minPos[i] = 0.0;     // x坐标最小值
            maxPos[i] = mapWidth; // x坐标最大值
            minPos[i+1] = 0.0;   // y坐标最小值
            maxPos[i+1] = mapHeight; // y坐标最大值
        }

        // 设置全局指针以便静态函数访问
        currentPlanner = this;

        // 创建PSO算法实例
        ZPSO_Algorithm pso(
            // 传递静态成员函数的地址
            PathPlanner::staticFitnessFuncton,
            minPos, maxPos, dimension, particleCount,
            2.0, 2.0, 4.0 // globalGuideCoe, localGuideCoe, maxSpeed
        );

        // 执行优化
        ZPSO_Partical bestParticle;
        pso.findMax(generations, bestParticle);

        // 解码最佳路径
        bestPath = decodePath(bestParticle);

        // 清理内存
        delete[] minPos;
        delete[] maxPos;

        std::cout << "路径规划完成！" << std::endl;
        std::cout << "最佳适应度: " << bestParticle._fitness << std::endl;
        std::cout << "路径长度: " << calculatePathLength(bestPath) << std::endl;
        std::cout << "是否碰撞: " << (isPathColliding(bestPath) ? "是" : "否") << std::endl;

        return bestPath;
    }

    // 获取最佳路径
    const std::vector<Point>& getBestPath() const {
        return bestPath;
    }
};

#endif // PATHPLANNER_H 