@echo off
echo 正在编译机器人路径规划项目...

:: 用户配置: 请根据实际的SFML安装路径修改下面这一行
SET SFML_DIR=C:\Tools\SFML

:: 检查SFML的路径
if not exist "%SFML_DIR%\include\SFML" (
    echo 错误: 在 "%SFML_DIR%\include" 中未找到SFML头文件目录。
    echo 请确保SFML安装在指定目录下，或者修改此脚本中的 SFML_DIR 路径。
    pause
    exit /b 1
)

:: 添加mingw32到环境变量路径
SET PATH=C:\Tools\mingw32\bin;%PATH%

:: 开始编译 - 使用C++17标准
echo 正在使用 g++ 来编译项目源文件 (C++17标准)...
g++ -std=c++17 -Wall -O2 ^
    -DUSE_SFML ^
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
    echo 重要提示: 运行程序时可能需要SFML的DLL文件。
) else (
    echo 编译失败，请检查错误信息。
)

pause