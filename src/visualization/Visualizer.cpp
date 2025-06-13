#include "Visualizer.h"

#ifdef USE_SFML

Visualizer::Visualizer(Map* m, PathPlanner* p, int windowWidth, int windowHeight)
    : map(m), planner(p), pathFound(false) {
    
    // 计算缩放比例，基于网格地图大小
    double mapWidth = map->getWidth() * map->getCellSize();
    double mapHeight = map->getHeight() * map->getCellSize();
    double scaleX = (windowWidth - 100) / mapWidth;
    double scaleY = (windowHeight - 100) / mapHeight;
    scale = std::min(scaleX, scaleY);
    
    // 创建窗口 (SFML 3.x 语法)
    window.create(sf::VideoMode({static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight)}), "Robot Navigation Planner");
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

void Visualizer::setAStarGuidePath(const std::vector<Point>& guidePath) {
    astarGuidePath = guidePath;
}

sf::Vector2f Visualizer::worldToScreen(const Point& worldPos) {
    return sf::Vector2f(
        static_cast<float>(worldPos.x * scale + 50),
        static_cast<float>(window.getSize().y - (worldPos.y * scale + 50))
    );
}

Point Visualizer::screenToWorld(const sf::Vector2f& screenPos) {
    double worldX = (screenPos.x - 50) / scale;
    double worldY = (window.getSize().y - screenPos.y - 50) / scale;
    
    return Point(worldX, worldY);
}

void Visualizer::drawMap() {
    // 绘制网格地图中的障碍物
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            if (map->getCell(x, y) == 1) {  // 障碍物
                sf::RectangleShape rect;
                
                // 计算网格单元的世界坐标
                Point worldTopLeft(x * map->getCellSize(), (y + 1) * map->getCellSize());
                Point worldBottomRight((x + 1) * map->getCellSize(), y * map->getCellSize());
                
                sf::Vector2f screenTopLeft = worldToScreen(worldTopLeft);
                sf::Vector2f screenBottomRight = worldToScreen(worldBottomRight);
                
                rect.setPosition(screenTopLeft);
                rect.setSize(sf::Vector2f(
                    screenBottomRight.x - screenTopLeft.x,
                    screenBottomRight.y - screenTopLeft.y
                ));
                rect.setFillColor(sf::Color(200, 50, 50));  // 深红色障碍物
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(1.0f);
                
                window.draw(rect);
            }
        }
    }
}

void Visualizer::drawStartEnd() {
    // 绘制起点 (绿色圆圈)
    sf::CircleShape startCircle(8.0f);
    startCircle.setFillColor(sf::Color::Green);
    sf::Vector2f startPos = worldToScreen(startPoint);
    startCircle.setPosition(sf::Vector2f(startPos.x - 8.0f, startPos.y - 8.0f));
    window.draw(startCircle);
    
    // 绘制终点 (红色圆圈)
    sf::CircleShape endCircle(8.0f);
    endCircle.setFillColor(sf::Color::Red);
    sf::Vector2f endPos = worldToScreen(endPoint);
    endCircle.setPosition(sf::Vector2f(endPos.x - 8.0f, endPos.y - 8.0f));
    window.draw(endCircle);
}

void Visualizer::drawPath() {
    if (currentPath.size() < 2) return;
    
    for (size_t i = 0; i < currentPath.size() - 1; i++) {
        sf::Vertex line[] = {
            sf::Vertex{worldToScreen(currentPath[i]), sf::Color::Blue},
            sf::Vertex{worldToScreen(currentPath[i + 1]), sf::Color::Blue}
        };
        
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    
    // 绘制路径点
    for (const auto& point : currentPath) {
        sf::CircleShape pathPoint(3.0f);
        pathPoint.setFillColor(sf::Color::Blue);
        sf::Vector2f pos = worldToScreen(point);
        pathPoint.setPosition(sf::Vector2f(pos.x - 3.0f, pos.y - 3.0f));
        window.draw(pathPoint);
    }
}

void Visualizer::drawAStarGuidePath() {
    if (astarGuidePath.size() < 2) return;
    
    // 用黄色绘制A*引导路径
    for (size_t i = 0; i < astarGuidePath.size() - 1; i++) {
        sf::Vertex line[] = {
            sf::Vertex{worldToScreen(astarGuidePath[i]), sf::Color::Yellow},
            sf::Vertex{worldToScreen(astarGuidePath[i + 1]), sf::Color::Yellow}
        };
        
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    
    // 绘制引导路径点
    for (const auto& point : astarGuidePath) {
        sf::CircleShape guidePoint(2.0f);
        guidePoint.setFillColor(sf::Color::Yellow);
        sf::Vector2f pos = worldToScreen(point);
        guidePoint.setPosition(sf::Vector2f(pos.x - 2.0f, pos.y - 2.0f));
        window.draw(guidePoint);
    }
}

void Visualizer::drawText() {
    // 由于文本渲染需要字体文件，这里提供一个简化版本
    // 在实际使用中，您需要加载字体文件
    
    // 可以添加状态信息的文本显示
    // 例如路径长度、算法状态等
}

bool Visualizer::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false;
        }
        
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseButtonPressed->position.x), 
                                     static_cast<float>(mouseButtonPressed->position.y));
                Point worldPos = screenToWorld(mousePos);
                
                // 可以在这里添加鼠标交互逻辑
                // 例如设置新的起点或终点
                std::cout << "鼠标点击位置: (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;
            }
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Space) {
                // 空格键可以触发路径规划
                if (planner) {
                    auto result = planner->planPath();
                    if (result.success) {
                        setPath(result.path);
                    }
                }
            }
            
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
    drawAStarGuidePath();  // 先绘制A*引导路径（在底层）
    drawPath();           // 再绘制最终路径（在上层）
    drawStartEnd();
    drawText();
    
    window.display();
}

bool Visualizer::isOpen() {
    return window.isOpen();
}

void Visualizer::run() {
    while (isOpen()) {
        if (!handleEvents()) {
            break;
        }
        render();
    }
}

#endif // USE_SFML 