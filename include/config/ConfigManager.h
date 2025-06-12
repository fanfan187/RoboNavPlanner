#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <fstream>
#include <iostream>

// 配置结构体
struct PSOConfig {
    int generations = 300;
    int particleCount = 150;
    double globalGuideCoe = 2.0;
    double localGuideCoe = 2.0;
    double maxSpeed = 4.0;
};

struct PathPlanningConfig {
    int numWaypoints = 6;
    double mapCellSize = 1.0;
};

struct VisualizationConfig {
    int windowWidth = 800;
    int windowHeight = 600;
    int frameRate = 60;
};

struct AppConfig {
    PSOConfig pso;
    PathPlanningConfig pathPlanning;
    VisualizationConfig visualization;
};

class ConfigManager {
public:
    ConfigManager();
    
    // 加载配置文件
    bool loadConfig(const std::string& configPath);
    
    // 获取配置
    const AppConfig& getConfig() const { return config; }
    
    // 打印配置信息
    void printConfig() const;

private:
    AppConfig config;
    
    // 简单的JSON解析函数
    bool parseJsonFile(const std::string& filepath);
    std::string trim(const std::string& str);
    double parseDouble(const std::string& value);
    int parseInt(const std::string& value);
};

#endif // CONFIG_MANAGER_H 