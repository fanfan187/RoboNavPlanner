#ifndef VISUALIZER_H
#define VISUALIZER_H

#ifdef USE_SFML
#include <SFML/Graphics.hpp>
#endif

#include "../core/Map.h"
#include "../core/PathPlanner.h"
#include <vector>
#include <iostream>
#include <algorithm>

#ifdef USE_SFML
class Visualizer {
private:
    sf::RenderWindow window;
    Map* map;
    PathPlanner* planner;
    double scale;
    Point startPoint, endPoint;
    std::vector<Point> currentPath;
    bool pathFound;

public:
    Visualizer(Map* m, PathPlanner* p, int windowWidth = 800, int windowHeight = 600);
    
    void setStartEnd(const Point& start, const Point& end);
    void setPath(const std::vector<Point>& path);
    
    // 坐标转换
    sf::Vector2f worldToScreen(const Point& worldPos);
    Point screenToWorld(const sf::Vector2f& screenPos);
    
    // 绘制方法
    void drawMap();
    void drawStartEnd();
    void drawPath();
    void drawText();
    
    // 事件处理和运行
    bool handleEvents();
    void render();
    bool isOpen();
    void run();
};
#else
// 空的占位符类，避免编译错误
class Visualizer {
public:
    Visualizer(Map*, PathPlanner*, int = 800, int = 600) {}
    void setStartEnd(const Point&, const Point&) {}
    void setPath(const std::vector<Point>&) {}
    void run() { std::cout << "SFML 可视化未启用" << std::endl; }
};
#endif

#endif // VISUALIZER_H 