// #include "Visualizer.h"
// #include <iostream>

// 只有在定义了 USE_SFML 时才编译 SFML 相关代码
#ifdef USE_SFML

// Visualizer类的所有方法实现 
void Visualizer::drawMap() {
    // 绘制网格和障碍物
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            sf::RectangleShape cell;
            cell.setSize(sf::Vector2f(
                static_cast<float>(map->getCellSize() * scale),
                static_cast<float>(map->getCellSize() * scale)
            ));
            cell.setPosition(
                static_cast<float>(x * map->getCellSize() * scale),
                static_cast<float>(y * map->getCellSize() * scale)
            );

            if (map->getCell(x, y) == 1) {
                cell.setFillColor(sf::Color::Black); // 障碍物
            } else {
                cell.setFillColor(sf::Color::White); // 可通行区域
            }
            cell.setOutlineThickness(1.0f);
            cell.setOutlineColor(sf::Color(200, 200, 200));

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

    window.create(sf::VideoMode(windowWidth, windowHeight), "机器人路径规划 - PSO算法");
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
    return sf::Vector2f(
        static_cast<float>(worldPos.x * scale),
        static_cast<float>(worldPos.y * scale)
    );
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
    startCircle.setPosition(
        startScreen.x - startCircle.getRadius(),
        startScreen.y - startCircle.getRadius()
    );
    startCircle.setFillColor(sf::Color::Green);
    window.draw(startCircle);

    // 绘制终点（红色圆圈）
    sf::CircleShape endCircle(static_cast<float>(map->getCellSize() * scale * 0.3));
    sf::Vector2f endScreen = worldToScreen(endPoint);
    endCircle.setPosition(
        endScreen.x - endCircle.getRadius(),
        endScreen.y - endCircle.getRadius()
    );
    endCircle.setFillColor(sf::Color::Red);
    window.draw(endCircle);
}

void Visualizer::drawPath() {
    if (!pathFound || currentPath.size() < 2) return;

    // 绘制路径线段
    for (size_t i = 1; i < currentPath.size(); i++) {
        sf::Vertex line[] = {
            sf::Vertex(worldToScreen(currentPath[i-1]), sf::Color::Blue),
            sf::Vertex(worldToScreen(currentPath[i]), sf::Color::Blue)
        };
        window.draw(line, 2, sf::Lines);
    }

    // 绘制航点（小蓝圆）
    for (size_t i = 1; i < currentPath.size() - 1; i++) {
        sf::CircleShape waypoint(static_cast<float>(map->getCellSize() * scale * 0.15));
        sf::Vector2f waypointScreen = worldToScreen(currentPath[i]);
        waypoint.setPosition(
            waypointScreen.x - waypoint.getRadius(),
            waypointScreen.y - waypoint.getRadius()
        );
        waypoint.setFillColor(sf::Color::Blue);
        window.draw(waypoint);
    }
}

void Visualizer::drawText() {
    // 这里可以添加文本显示，需要加载字体
    // 暂时省略，可以在控制台输出信息
}

bool Visualizer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }
        
        // 可以添加鼠标点击设置起点终点的功能
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                Point clickPos = screenToWorld(sf::Vector2f(
                    static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y)
                ));
                std::cout << "点击位置: (" << clickPos.x << ", " << clickPos.y << ")" << std::endl;
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