#include "../include/Common.h"
#include "../src/core/Map.h"
#include <iostream>

int main() {
    // 测试Common.h中的Point结构
    Point p1(1.0, 2.0);
    Point p2(4.0, 6.0);
    
    std::cout << "Point p1: (" << p1.x << ", " << p1.y << ")" << std::endl;
    std::cout << "Point p2: (" << p2.x << ", " << p2.y << ")" << std::endl;
    
    // 测试Point的运算符重载
    Point p3 = p1 + p2;
    std::cout << "p1 + p2 = (" << p3.x << ", " << p3.y << ")" << std::endl;
    
    Point p4 = p2 - p1;
    std::cout << "p2 - p1 = (" << p4.x << ", " << p4.y << ")" << std::endl;
    
    // 测试距离计算
    double distance = p1.distanceTo(p2);
    std::cout << "Distance from p1 to p2: " << distance << std::endl;
    
    // 测试Map类是否能正确使用Point
    Map testMap;
    bool collision = testMap.isLineColliding(p1, p2);
    std::cout << "Line collision test: " << (collision ? "collision" : "no collision") << std::endl;
    
    std::cout << "Point structure unification test completed successfully!" << std::endl;
    return 0;
} 