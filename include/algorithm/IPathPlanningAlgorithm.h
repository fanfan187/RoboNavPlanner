#ifndef IPATHPLANNINGALGORITHM_H
#define IPATHPLANNINGALGORITHM_H

#include "../Common.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

// 前向声明
class Map;

// 路径规划请求结构
struct PathPlanningRequest {
    Point startPoint;           // 起点
    Point endPoint;             // 终点
    Map* map;                   // 地图指针
    int numWaypoints;           // 中间航点数量
    double maxTime;             // 最大计算时间（秒）
    
    PathPlanningRequest() 
        : startPoint(0, 0), endPoint(0, 0), map(nullptr), 
          numWaypoints(5), maxTime(30.0) {}
};

// 路径规划结果结构
struct PathPlanningResult {
    std::vector<Point> path;    // 规划出的路径
    double pathLength;          // 路径长度
    double fitness;             // 适应度值
    bool success;               // 是否成功
    double computationTime;     // 计算耗时（秒）
    std::string algorithmName;  // 算法名称
    
    PathPlanningResult(const std::vector<Point>& p = std::vector<Point>(),
                      double length = 0.0, double fit = 0.0, bool succ = false,
                      double time = 0.0, const std::string& name = "Unknown")
        : path(p), pathLength(length), fitness(fit), success(succ),
          computationTime(time), algorithmName(name) {}
};

/**
 * @brief 路径规划算法抽象接口
 * 
 * 这个接口定义了所有路径规划算法必须实现的基本功能。
 * 通过策略模式，PathPlanner 可以与任何实现此接口的算法协同工作。
 * 
 * 支持的算法类型包括但不限于：
 * - PSO (粒子群优化)
 * - A* (A星算法)
 * - RRT* (快速随机树)
 * - Dijkstra
 * - Genetic Algorithm (遗传算法)
 */
class IPathPlanningAlgorithm {
public:
    /**
     * @brief 虚析构函数
     * 确保派生类的析构函数被正确调用
     */
    virtual ~IPathPlanningAlgorithm() = default;
    
    /**
     * @brief 执行路径规划
     * @param request 路径规划请求，包含起点、终点、地图等信息
     * @return PathPlanningResult 规划结果，包含路径、性能指标等
     */
    virtual PathPlanningResult planPath(const PathPlanningRequest& request) = 0;
    
    /**
     * @brief 获取算法名称
     * @return std::string 算法的名称标识
     */
    virtual std::string getAlgorithmName() const = 0;
    
    /**
     * @brief 设置算法参数
     * @param paramName 参数名称
     * @param value 参数值
     * 
     * 不同算法可能有不同的参数，通过字符串键值对的方式统一设置
     * 例如：PSO算法可以设置 "particleCount", "generations" 等参数
     */
    virtual void setParameter(const std::string& paramName, double value) = 0;
    
    /**
     * @brief 获取算法参数
     * @param paramName 参数名称
     * @return double 参数值，如果参数不存在返回 0.0
     */
    virtual double getParameter(const std::string& paramName) const = 0;
    
    /**
     * @brief 重置算法状态
     * 
     * 清除算法的内部状态，为下一次规划做准备
     */
    virtual void reset() = 0;
    
    /**
     * @brief 检查算法是否支持动态重新规划
     * @return bool 如果支持返回 true，否则返回 false
     */
    virtual bool supportsDynamicReplanning() const { return false; }
    
    /**
     * @brief 获取算法的复杂度特征
     * @return std::string 算法复杂度描述（如 "O(n log n)", "启发式算法"等）
     */
    virtual std::string getComplexityInfo() const { return "未知复杂度"; }
    
    /**
     * @brief 获取算法的适用场景描述
     * @return std::string 适用场景描述
     */
    virtual std::string getUsageDescription() const { return "通用路径规划算法"; }
};

/**
 * @brief 算法工厂函数类型定义
 * 
 * 用于创建算法实例的工厂函数类型
 */
using AlgorithmFactory = std::function<std::unique_ptr<IPathPlanningAlgorithm>()>;

/**
 * @brief 算法注册器
 * 
 * 用于注册和创建不同类型的路径规划算法
 */
class AlgorithmRegistry {
public:
    /**
     * @brief 注册算法
     * @param name 算法名称
     * @param factory 算法工厂函数
     */
    static void registerAlgorithm(const std::string& name, AlgorithmFactory factory);
    
    /**
     * @brief 创建算法实例
     * @param name 算法名称
     * @return std::unique_ptr<IPathPlanningAlgorithm> 算法实例，如果算法不存在返回 nullptr
     */
    static std::unique_ptr<IPathPlanningAlgorithm> createAlgorithm(const std::string& name);
    
    /**
     * @brief 获取所有已注册的算法名称列表
     * @return std::vector<std::string> 算法名称列表
     */
    static std::vector<std::string> getAvailableAlgorithms();
    
private:
    static std::unordered_map<std::string, AlgorithmFactory> algorithms_;
};

#endif // IPATHPLANNINGALGORITHM_H 