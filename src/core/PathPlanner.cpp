#include "PathPlanner.h"

// 静态成员变量定义
PathPlanner* PathPlanner::currentPlanner = nullptr;

// 构造函数实现
PathPlanner::PathPlanner(Map* m, const Point& start, const Point& end, 
                         std::unique_ptr<IPathPlanningAlgorithm> algo,
                         int waypoints) 
    : map(m), startPoint(start), endPoint(end), 
      numWaypoints(waypoints), algorithm(std::move(algo)),
      evaluator(m) {
    currentPlanner = this;
}

// 设置起点和终点
void PathPlanner::setStartEnd(const Point& start, const Point& end) {
    startPoint = start;
    endPoint = end;
}

// 切换算法（策略模式）
void PathPlanner::setAlgorithm(std::unique_ptr<IPathPlanningAlgorithm> algo) {
    algorithm = std::move(algo);
}

// 获取当前算法信息
std::string PathPlanner::getCurrentAlgorithmName() const {
    return algorithm ? algorithm->getAlgorithmName() : "None";
}

// 执行路径规划 - 现在通过抽象接口调用
PathPlanningResult PathPlanner::planPath() {
    if (!algorithm) {
        return {std::vector<Point>(), 0.0, 0.0, false, 0.0, "None"};
    }
    
    // 构造请求
    PathPlanningRequest request;
    request.startPoint = startPoint;
    request.endPoint = endPoint;
    request.map = map;
    request.numWaypoints = numWaypoints;
    request.maxTime = 30.0; // 默认30秒超时
    
    // 执行规划
    auto result = algorithm->planPath(request);
    
    if (result.success) {
        bestPath = result.path;
    }
    
    return result;
}

// 路径质量分析方法
double PathPlanner::calculatePathLength(const std::vector<Point>& path) const {
    return evaluator.calculatePathLength(path);
}

bool PathPlanner::isPathColliding(const std::vector<Point>& path) const {
    return evaluator.isPathColliding(path);
}

double PathPlanner::calculateSmoothness(const std::vector<Point>& path) const {
    return evaluator.calculateSmoothness(path);
}

// 获取最佳路径
const std::vector<Point>& PathPlanner::getBestPath() const {
    return bestPath;
} 