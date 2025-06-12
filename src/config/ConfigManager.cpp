#include "config/ConfigManager.h"
#include <regex>
#include <sstream>
#include <algorithm>

ConfigManager::ConfigManager() {
    // 使用默认配置
}

bool ConfigManager::loadConfig(const std::string& configPath) {
    std::cout << "正在加载配置文件: " << configPath << std::endl;
    
    if (!parseJsonFile(configPath)) {
        std::cout << "无法加载配置文件，使用默认配置" << std::endl;
        return false;
    }
    
    std::cout << "配置文件加载成功" << std::endl;
    return true;
}

bool ConfigManager::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '/' || line == "{" || line == "}") {
            continue;
        }
        
        // 处理节标识符
        if (line.find("\"pso\"") != std::string::npos) {
            currentSection = "pso";
            continue;
        } else if (line.find("\"pathPlanning\"") != std::string::npos) {
            currentSection = "pathPlanning";
            continue;
        } else if (line.find("\"visualization\"") != std::string::npos) {
            currentSection = "visualization";
            continue;
        }
        
        // 解析键值对
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // 移除引号和逗号
            key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
            if (value.back() == ',') {
                value.pop_back();
            }
            value = trim(value);
            
            // 根据当前节设置配置值
            if (currentSection == "pso") {
                if (key == "generations") {
                    config.pso.generations = parseInt(value);
                } else if (key == "particleCount") {
                    config.pso.particleCount = parseInt(value);
                } else if (key == "globalGuideCoe") {
                    config.pso.globalGuideCoe = parseDouble(value);
                } else if (key == "localGuideCoe") {
                    config.pso.localGuideCoe = parseDouble(value);
                } else if (key == "maxSpeed") {
                    config.pso.maxSpeed = parseDouble(value);
                }
            } else if (currentSection == "pathPlanning") {
                if (key == "numWaypoints") {
                    config.pathPlanning.numWaypoints = parseInt(value);
                } else if (key == "mapCellSize") {
                    config.pathPlanning.mapCellSize = parseDouble(value);
                }
            } else if (currentSection == "visualization") {
                if (key == "windowWidth") {
                    config.visualization.windowWidth = parseInt(value);
                } else if (key == "windowHeight") {
                    config.visualization.windowHeight = parseInt(value);
                } else if (key == "frameRate") {
                    config.visualization.frameRate = parseInt(value);
                }
            }
        }
    }
    
    file.close();
    return true;
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

double ConfigManager::parseDouble(const std::string& value) {
    try {
        return std::stod(value);
    } catch (const std::exception&) {
        return 0.0;
    }
}

int ConfigManager::parseInt(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

void ConfigManager::printConfig() const {
    std::cout << "\n=== 当前配置 ===" << std::endl;
    std::cout << "PSO参数:" << std::endl;
    std::cout << "  进化代数: " << config.pso.generations << std::endl;
    std::cout << "  粒子数量: " << config.pso.particleCount << std::endl;
    std::cout << "  全局引导系数: " << config.pso.globalGuideCoe << std::endl;
    std::cout << "  局部引导系数: " << config.pso.localGuideCoe << std::endl;
    std::cout << "  最大速度: " << config.pso.maxSpeed << std::endl;
    
    std::cout << "路径规划参数:" << std::endl;
    std::cout << "  中间航点数量: " << config.pathPlanning.numWaypoints << std::endl;
    std::cout << "  地图单元格大小: " << config.pathPlanning.mapCellSize << std::endl;
    
    std::cout << "可视化参数:" << std::endl;
    std::cout << "  窗口宽度: " << config.visualization.windowWidth << std::endl;
    std::cout << "  窗口高度: " << config.visualization.windowHeight << std::endl;
    std::cout << "  帧率: " << config.visualization.frameRate << std::endl;
} 