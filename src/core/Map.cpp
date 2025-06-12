#include "Map.h"

Map::Map(int w, int h, double cs) 
    : width(w), height(h), cellSize(cs) {
    grid.resize(height, std::vector<int>(width, 0));
    createDefaultMap();
}

bool Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "无法打开地图文件: " << filename << std::endl;
        return false;
    }

    file >> height >> width;
    grid.clear();
    grid.resize(height, std::vector<int>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file >> grid[i][j];
        }
    }
    file.close();
    return true;
}

void Map::createDefaultMap() {
    // 清空地图
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            grid[i][j] = 0;
        }
    }

    // 添加边界
    for (int i = 0; i < height; i++) {
        grid[i][0] = 1;
        grid[i][width-1] = 1;
    }
    for (int j = 0; j < width; j++) {
        grid[0][j] = 1;
        grid[height-1][j] = 1;
    }

    // 添加一些障碍物
    // 垂直障碍物
    for (int i = 5; i < 15; i++) {
        grid[i][8] = 1;
    }
    
    // 水平障碍物
    for (int j = 12; j < 18; j++) {
        grid[6][j] = 1;
    }

    // L形障碍物
    for (int i = 10; i < 15; i++) {
        grid[i][15] = 1;
    }
    for (int j = 15; j < 18; j++) {
        grid[14][j] = 1;
    }
}

bool Map::isInBounds(double x, double y) const {
    int gridX = static_cast<int>(x / cellSize);
    int gridY = static_cast<int>(y / cellSize);
    return gridX >= 0 && gridX < width && gridY >= 0 && gridY < height;
}

bool Map::isObstacle(double x, double y) const {
    if (!isInBounds(x, y)) return true;
    int gridX = static_cast<int>(x / cellSize);
    int gridY = static_cast<int>(y / cellSize);
    return grid[gridY][gridX] == 1;
}

bool Map::isLineColliding(const Point& start, const Point& end) const {
    int x0 = static_cast<int>(start.x / cellSize);
    int y0 = static_cast<int>(start.y / cellSize);
    int x1 = static_cast<int>(end.x / cellSize);
    int y1 = static_cast<int>(end.y / cellSize);

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int x = x0;
    int y = y0;
    int n = 1 + dx + dy;
    int x_inc = (x1 > x0) ? 1 : -1;
    int y_inc = (y1 > y0) ? 1 : -1;
    int error = dx - dy;

    dx *= 2;
    dy *= 2;

    for (; n > 0; --n) {
        // 检查当前点是否为障碍物
        if (x >= 0 && x < width && y >= 0 && y < height) {
            if (grid[y][x] == 1) {
                return true; // 碰撞
            }
        } else {
            return true; // 超出边界
        }

        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }
    return false; // 无碰撞
}

int Map::getWidth() const { 
    return width; 
}

int Map::getHeight() const { 
    return height; 
}

double Map::getCellSize() const { 
    return cellSize; 
}

int Map::getCell(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return grid[y][x];
    }
    return 1; // 边界外视为障碍物
} 