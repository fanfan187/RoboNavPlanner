#include <optional>
#include "Visualizer.h"
#include <iostream>
#include <cmath>
#include "../../include/Common.h"

// 确保在使用 SFML 之前已经定义了 USE_SFML
#ifndef USE_SFML
#define USE_SFML
#endif

#ifdef USE_SFML
#include <SFML/Graphics.hpp>

// Visualizer类的所有方法实现 
void Visualizer::drawMap() {
    // 绘制网格和障碍物
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            sf::RectangleShape cell;
            cell.setSize({
                static_cast<float>(map->getCellSize() * scale),
                static_cast<float>(map->getCellSize() * scale)
            });
            cell.setPosition({
                static_cast<float>(x * map->getCellSize() * scale),
                static_cast<float>(y * map->getCellSize() * scale)
            });

            if (map->getCell(x, y) == 1) {
                cell.setFillColor(sf::Color::Black); // 障碍物
            } else {
                cell.setFillColor(sf::Color::White); // 可通行区域
            }
            cell.setOutlineThickness(1.0f);
            cell.setOutlineColor(sf::Color{200, 200, 200});

            window.draw(cell);
        }
    }
}

// ... 其他方法的实现 

Visualizer::Visualizer(Map* m, PathPlanner* p, int windowWidth, int windowHeight) 
    : map(m), planner(p), pathFound(false) {
    
    // 计算缩放比例
    double scaleX = static_cast<double>(windowWidth) / (map->getWidth() * map->getCellSize());
    double scaleY = static_cast<double>(windowHeight) / (map->getHeight() * map->getCellSize());
    scale = std::min(scaleX, scaleY) * 0.9; // 留一些边距

    window.create(sf::VideoMode({static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight)}), "机器人路径规划 - PSO算法");
    window.setFramerateLimit(60);
}

void Visualizer::setStartEnd(const Point& start, const Point& end) {
    startPoint = start;
    endPoint = end;
}

void Visualizer::setPath(const std::vector<Point>& path) {
    currentPath = path;
    pathFound = !path.empty();
}

sf::Vector2f Visualizer::worldToScreen(const Point& worldPos) {
    return {
        static_cast<float>(worldPos.x * scale),
        static_cast<float>(worldPos.y * scale)
    };
}

Point Visualizer::screenToWorld(const sf::Vector2f& screenPos) {
    return Point(
        screenPos.x / scale,
        screenPos.y / scale
    );
}

void Visualizer::drawStartEnd() {
    // 绘制起点（绿色圆圈）
    sf::CircleShape startCircle(static_cast<float>(map->getCellSize() * scale * 0.3));
    sf::Vector2f startScreen = worldToScreen(startPoint);
    startCircle.setPosition({
        startScreen.x - startCircle.getRadius(),
        startScreen.y - startCircle.getRadius()
    });
    startCircle.setFillColor(sf::Color::Green);
    window.draw(startCircle);

    // 绘制终点（红色圆圈）
    sf::CircleShape endCircle(static_cast<float>(map->getCellSize() * scale * 0.3));
    sf::Vector2f endScreen = worldToScreen(endPoint);
    endCircle.setPosition({
        endScreen.x - endCircle.getRadius(),
        endScreen.y - endCircle.getRadius()
    });
    endCircle.setFillColor(sf::Color::Red);
    window.draw(endCircle);
}

void Visualizer::drawPath() {
    if (!pathFound || currentPath.size() < 2) return;

    // 绘制路径线段
    for (size_t i = 1; i < currentPath.size(); i++) {
        sf::Vertex line[] = {
            {worldToScreen(currentPath[i-1]), sf::Color::Blue},
            {worldToScreen(currentPath[i]), sf::Color::Blue}
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }

    // 绘制航点（小蓝圆）
    for (size_t i = 1; i < currentPath.size() - 1; i++) {
        sf::CircleShape waypoint(static_cast<float>(map->getCellSize() * scale * 0.15));
        sf::Vector2f waypointScreen = worldToScreen(currentPath[i]);
        waypoint.setPosition({
            waypointScreen.x - waypoint.getRadius(),
            waypointScreen.y - waypoint.getRadius()
        });
        waypoint.setFillColor(sf::Color::Blue);
        window.draw(waypoint);
    }
}

void Visualizer::drawText() {
    // 这里可以添加文本显示，需要加载字体
    // 暂时省略，可以在控制台输出信息
}

bool Visualizer::handleEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false;
        }
        
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                Point clickPos = screenToWorld(sf::Vector2f(
                    static_cast<float>(mouseButtonPressed->position.x),
                    static_cast<float>(mouseButtonPressed->position.y)
                ));
                std::cout << "点击位置: (" << clickPos.x << ", " << clickPos.y << ")" << std::endl;
            }
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
                return false;
            }
        }
    }
    return true;
}

void Visualizer::render() {
    window.clear(sf::Color::White);
    
    drawMap();
    drawStartEnd();
    drawPath();
    drawText();
    
    window.display();
}

bool Visualizer::isOpen() {
    return window.isOpen();
}

void Visualizer::run() {
    while (isOpen()) {
        if (!handleEvents()) break;
        render();
    }
}

#endif // USE_SFML 