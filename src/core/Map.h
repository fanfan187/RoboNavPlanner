#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include "../../include/Common.h"

class Map {
private:
    std::vector<std::vector<int>> grid;
    int width, height;
    double cellSize;

public:
    // 构造函数
    Map(int w = 20, int h = 20, double cs = 1.0);

    // 从文件加载地图
    bool loadFromFile(const std::string& filename);

    // 创建默认地图（用于演示）
    void createDefaultMap();

    // 检查点是否在地图边界内
    bool isInBounds(double x, double y) const;

    // 检查点是否为障碍物
    bool isObstacle(double x, double y) const;

    // Bresenham直线算法检查线段是否与障碍物碰撞
    bool isLineColliding(const Point& start, const Point& end) const;

    // 获取地图尺寸
    int getWidth() const;
    int getHeight() const;
    double getCellSize() const;
    
    // 获取网格值（用于可视化）
    int getCell(int x, int y) const;
    
    // 设置网格值（用于演示）
    void setCell(int x, int y, int value);
};

#endif // MAP_H 