@echo off
echo 编译机器人路径规划项目 (控制台版本)...

:: 编译控制台版本 (包含PathPlanner.cpp和ConfigManager.cpp)
g++ -std=c++17 -Iinclude -Isrc -o RoboNavPlanner_Console.exe src/main_console.cpp src/core/PathPlanner.cpp src/config/ConfigManager.cpp

if %ERRORLEVEL% EQU 0 (
    echo 编译成功！
    echo 运行程序: RoboNavPlanner_Console.exe
    echo.
    echo 如果要运行程序，请输入: RoboNavPlanner_Console.exe
) else (
    echo 编译失败！
    echo 请检查编译器是否正确安装
)

pause 