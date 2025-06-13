@echo off
echo 编译算法解耦演示程序...

:: 编译算法演示版本
g++ -std=c++17 -Iinclude -Isrc -o AlgorithmDemo.exe ^
    examples/algorithm_demo.cpp ^
    src/core/Map.cpp ^
    src/core/PathPlanner.cpp ^
    src/algorithm/AlgorithmRegistry.cpp

if %ERRORLEVEL% EQU 0 (
    echo 编译成功！
    echo 运行演示程序: AlgorithmDemo.exe
    echo.
    echo 该程序将演示：
    echo - PSO vs A* 算法性能对比
    echo - 解耦架构的可扩展性
    echo - 动态算法切换
    echo - 参数调优演示
) else (
    echo 编译失败！
    echo 请检查编译器是否正确安装
)

pause 