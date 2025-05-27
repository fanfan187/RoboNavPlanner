#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <memory>
#include <cmath>

// 基础点结构
struct Point {
    double x, y;
    
    Point(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    double distanceTo(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// 路径类型定义
using Path = std::vector<Point>;
using PathPtr = std::shared_ptr<Path>;

// 常用常量
namespace Constants {
    constexpr double PI = 3.14159265358979323846;
    constexpr double EPSILON = 1e-8;
}

#endif // COMMON_H 