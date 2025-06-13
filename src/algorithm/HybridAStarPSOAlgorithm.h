#ifndef HYBRIDASTAR_PSO_ALGORITHM_H
#define HYBRIDASTAR_PSO_ALGORITHM_H

#include "../../include/algorithm/IPathPlanningAlgorithm.h"
#include "AStarAlgorithm.h"
#include "ZPSOAlgorithm.h"
#include "../core/Map.h"
#include <chrono>
#include <iostream>

/**
 * @brief 混合A*和PSO的路径规划算法
 * 
 * 该算法结合了A*算法的全局引导能力和PSO算法的局部优化能力：
 * 1. 首先使用A*算法规划出一条全局引导路径
 * 2. 然后使用PSO算法进行局部优化，引导路径作为适应度函数的一部分
 */
class HybridAStarPSOAlgorithm : public IPathPlanningAlgorithm {
private:
    // 算法组件
    AStarAlgorithm astarSolver;
    std::unique_ptr<ZPSO_Algorithm> psoSolver;
    
    // 算法参数
    int particleCount;
    int generations;
    int numWaypoints;
    double deviationWeight;  // A*引导路径偏离惩罚权重
    double maxSpeed;
    double globalGuideCoe;
    double localGuideCoe;
    
    // 引导路径
    std::vector<Point> astarGuidePath;
    
    // 地图和边界信息
    const Map* currentMap;
    Point currentStart, currentEnd;
    
    /**
     * @brief 计算点到线段的距离
     * @param point 目标点
     * @param lineStart 线段起点
     * @param lineEnd 线段终点
     * @return double 点到线段的最短距离
     */
    double pointToLineSegmentDistance(const Point& point, const Point& lineStart, const Point& lineEnd) const {
        double A = point.x - lineStart.x;
        double B = point.y - lineStart.y;
        double C = lineEnd.x - lineStart.x;
        double D = lineEnd.y - lineStart.y;
        
        double dot = A * C + B * D;
        double lenSq = C * C + D * D;
        
        // 如果线段长度为0（起点和终点相同）
        if (lenSq < 1e-8) {
            return point.distanceTo(lineStart);
        }
        
        double param = dot / lenSq;
        
        Point closest;
        if (param < 0) {
            closest = lineStart;
        } else if (param > 1) {
            closest = lineEnd;
        } else {
            closest.x = lineStart.x + param * C;
            closest.y = lineStart.y + param * D;
        }
        
        return point.distanceTo(closest);
    }
    
    /**
     * @brief 将PSO粒子解码为路径
     * @param particle PSO粒子
     * @return std::vector<Point> 解码后的路径
     */
    std::vector<Point> decodePath(const ZPSO_Partical& particle) const {
        std::vector<Point> path;
        path.push_back(currentStart);
        
        // 添加中间航点
        for (int i = 0; i < numWaypoints; i++) {
            Point waypoint(particle._position[i * 2], particle._position[i * 2 + 1]);
            path.push_back(waypoint);
        }
        
        path.push_back(currentEnd);
        return path;
    }
    
    /**
     * @brief PSO适应度函数（包含A*引导）
     * @param particle PSO粒子
     * @return double 适应度值（越高越好）
     */
    double evaluateFitness(ZPSO_Partical& particle) {
        double fitness = 1000.0;  // 基础适应度
        
        // 解码粒子为路径
        std::vector<Point> path = decodePath(particle);
        
        // 1. 碰撞惩罚
        double collisionPenalty = 0.0;
        for (size_t i = 1; i < path.size(); i++) {
            if (currentMap->isLineColliding(path[i-1], path[i])) {
                collisionPenalty += 500.0;  // 严重惩罚碰撞
            }
        }
        
        // 2. 边界惩罚
        double boundaryPenalty = 0.0;
        for (size_t i = 1; i < path.size() - 1; i++) {  // 不检查起点和终点
            if (!currentMap->isInBounds(path[i].x, path[i].y)) {
                boundaryPenalty += 300.0;
            }
        }
        
        // 3. 路径长度惩罚
        double pathLength = 0.0;
        for (size_t i = 1; i < path.size(); i++) {
            pathLength += path[i-1].distanceTo(path[i]);
        }
        double lengthPenalty = pathLength * 0.5;  // 较轻的长度惩罚
        
        // 4. A*引导路径偏离惩罚（核心创新）
        double deviationPenalty = 0.0;
        if (!astarGuidePath.empty()) {
            // 遍历PSO路径上的每一个中间航点
            for (size_t i = 1; i < path.size() - 1; ++i) {
                const Point& waypoint = path[i];
                double minDistanceToGuidePath = 99999.0;
                
                // 找到这个航点到A*引导路径的最短距离
                for (size_t j = 0; j < astarGuidePath.size() - 1; ++j) {
                    double dist = pointToLineSegmentDistance(waypoint, astarGuidePath[j], astarGuidePath[j+1]);
                    if (dist < minDistanceToGuidePath) {
                        minDistanceToGuidePath = dist;
                    }
                }
                deviationPenalty += minDistanceToGuidePath;
            }
        }
        
        // 5. 路径平滑度奖励
        double smoothnessBonus = 0.0;
        if (path.size() >= 3) {
            for (size_t i = 1; i < path.size() - 1; i++) {
                Point prev = path[i-1];
                Point curr = path[i];
                Point next = path[i+1];
                
                // 计算转角余弦值
                Point v1 = curr - prev;
                Point v2 = next - curr;
                
                double dot = v1.x * v2.x + v1.y * v2.y;
                double mag1 = sqrt(v1.x * v1.x + v1.y * v1.y);
                double mag2 = sqrt(v2.x * v2.x + v2.y * v2.y);
                
                if (mag1 > 1e-8 && mag2 > 1e-8) {
                    double cosAngle = dot / (mag1 * mag2);
                    smoothnessBonus += cosAngle * 10.0;  // 奖励平滑路径
                }
            }
        }
        
        // 计算最终适应度
        fitness = fitness - collisionPenalty - boundaryPenalty - lengthPenalty 
                 - deviationPenalty * deviationWeight + smoothnessBonus;
        
        return fitness;
    }

public:
    /**
     * @brief 构造函数
     */
    HybridAStarPSOAlgorithm() 
        : particleCount(50), generations(100), numWaypoints(5), 
          deviationWeight(5.0), maxSpeed(1.0), globalGuideCoe(2.0), 
          localGuideCoe(2.0), currentMap(nullptr) {}
    
    /**
     * @brief 析构函数
     */
    virtual ~HybridAStarPSOAlgorithm() = default;
    
    /**
     * @brief 执行混合路径规划
     * @param request 路径规划请求
     * @return PathPlanningResult 规划结果
     */
    virtual PathPlanningResult planPath(const PathPlanningRequest& request) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 保存当前请求信息
        currentMap = request.map;
        currentStart = request.startPoint;
        currentEnd = request.endPoint;
        numWaypoints = request.numWaypoints;
        
        std::cout << "开始混合A*-PSO路径规划..." << std::endl;
        
        // 第一阶段：使用A*算法生成全局引导路径
        std::cout << "阶段1: 执行A*算法生成引导路径..." << std::endl;
        astarGuidePath = astarSolver.findPath(*currentMap, currentStart, currentEnd);
        
        if (astarGuidePath.empty()) {
            std::cout << "警告: A*未能找到全局路径！PSO将在无引导下运行。" << std::endl;
            deviationWeight = 0.0;  // 无引导路径时不使用偏离惩罚
        } else {
            std::cout << "A*引导路径包含 " << astarGuidePath.size() << " 个点" << std::endl;
        }
        
        // 第二阶段：使用PSO算法优化路径
        std::cout << "阶段2: 执行PSO算法优化路径..." << std::endl;
        
        // 设置PSO搜索边界
        int dimension = numWaypoints * 2;  // 每个航点有x,y坐标
        std::vector<double> minBounds(dimension);
        std::vector<double> maxBounds(dimension);
        
        double mapWidth = currentMap->getWidth() * currentMap->getCellSize();
        double mapHeight = currentMap->getHeight() * currentMap->getCellSize();
        
        for (int i = 0; i < dimension; i++) {
            minBounds[i] = 0.0;
            maxBounds[i] = (i % 2 == 0) ? mapWidth : mapHeight;
        }
        
        // 创建适应度函数
        auto fitnessFunc = [this](ZPSO_Partical& p) -> double {
            return this->evaluateFitness(p);
        };
        
        // 创建PSO求解器
        psoSolver = std::make_unique<ZPSO_Algorithm>(
            fitnessFunc,
            minBounds.data(),
            maxBounds.data(),
            dimension,
            particleCount,
            globalGuideCoe,
            localGuideCoe,
            maxSpeed
        );
        
        // 运行PSO优化
        ZPSO_Partical bestParticle(dimension);
        psoSolver->findMax(generations, bestParticle);
        
        // 构建最终结果
        std::vector<Point> finalPath = decodePath(bestParticle);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double computationTime = duration.count() / 1000.0;
        
        // 计算路径长度
        double pathLength = 0.0;
        for (size_t i = 1; i < finalPath.size(); i++) {
            pathLength += finalPath[i-1].distanceTo(finalPath[i]);
        }
        
        // 检查路径是否有效（无碰撞）
        bool success = true;
        for (size_t i = 1; i < finalPath.size(); i++) {
            if (currentMap->isLineColliding(finalPath[i-1], finalPath[i])) {
                success = false;
                break;
            }
        }
        
        std::cout << "混合算法完成! 用时: " << computationTime << "秒" << std::endl;
        std::cout << "路径长度: " << pathLength << ", 成功: " << (success ? "是" : "否") << std::endl;
        
        return PathPlanningResult(
            finalPath,
            pathLength,
            bestParticle._bestFitness,
            success,
            computationTime,
            "Hybrid A*-PSO"
        );
    }
    
    /**
     * @brief 获取算法名称
     */
    virtual std::string getAlgorithmName() const override {
        return "Hybrid A*-PSO";
    }
    
    /**
     * @brief 设置算法参数
     */
    virtual void setParameter(const std::string& paramName, double value) override {
        if (paramName == "particleCount") {
            particleCount = static_cast<int>(value);
        } else if (paramName == "generations") {
            generations = static_cast<int>(value);
        } else if (paramName == "deviationWeight") {
            deviationWeight = value;
        } else if (paramName == "maxSpeed") {
            maxSpeed = value;
        } else if (paramName == "globalGuideCoe") {
            globalGuideCoe = value;
        } else if (paramName == "localGuideCoe") {
            localGuideCoe = value;
        }
    }
    
    /**
     * @brief 获取算法参数
     */
    virtual double getParameter(const std::string& paramName) const override {
        if (paramName == "particleCount") {
            return particleCount;
        } else if (paramName == "generations") {
            return generations;
        } else if (paramName == "deviationWeight") {
            return deviationWeight;
        } else if (paramName == "maxSpeed") {
            return maxSpeed;
        } else if (paramName == "globalGuideCoe") {
            return globalGuideCoe;
        } else if (paramName == "localGuideCoe") {
            return localGuideCoe;
        }
        return 0.0;
    }
    
    /**
     * @brief 重置算法状态
     */
    virtual void reset() override {
        astarGuidePath.clear();
        psoSolver.reset();
        currentMap = nullptr;
    }
    
    /**
     * @brief 获取A*引导路径（用于可视化）
     */
    const std::vector<Point>& getAStarGuidePath() const {
        return astarGuidePath;
    }
    
    /**
     * @brief 获取算法复杂度信息
     */
    virtual std::string getComplexityInfo() const override {
        return "A*: O(b^d), PSO: O(P*G*D) where P=粒子数, G=代数, D=维度";
    }
    
    /**
     * @brief 获取使用场景描述
     */
    virtual std::string getUsageDescription() const override {
        return "混合算法：适用于需要全局最优性保证和局部精细优化的复杂环境路径规划";
    }
};

#endif // HYBRIDASTAR_PSO_ALGORITHM_H 