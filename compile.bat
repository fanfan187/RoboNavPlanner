@echo off
echo 编译机器人路径规划项目...

:: 用户配置: 请根据实际SFML安装路径修改下面这一行
SET SFML_DIR=C:\Tools\SFML

:: 检查SFML库路径
if not exist "%SFML_DIR%\include\SFML" (
    echo 错误: 在 "%SFML_DIR%\include" 中未找到SFML头文件目录。
    echo 请确保SFML安装在指定目录下，或修改此脚本顶部的 SFML_DIR 路径。
    pause
    exit /b 1
)

:: 设置mingw32编译环境路径
SET PATH=C:\Tools\mingw32\bin;%PATH%

:: 编译命令 - 使用C++14标准
echo 正在使用 g++ 编译器进行编译 (C++14标准)...
g++ -std=c++14 -Wall -O2 ^
    -Iinclude -Isrc ^
    -I"%SFML_DIR%\include" ^
    -o build\RoboNavPlanner.exe ^
    src\main.cpp ^
    src\core\Map.cpp ^
    src\core\PathPlanner.cpp ^
    src\visualization\Visualizer.cpp ^
    -L"%SFML_DIR%\lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system

if %ERRORLEVEL% EQU 0 (
    echo 编译成功！build\RoboNavPlanner.exe 已生成。
    echo 重要提示: 程序运行时可能需要SFML的DLL文件。
) else (
    echo 编译失败！请检查错误信息。
)

pause