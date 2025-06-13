@echo off
chcp 65001 >nul
echo Compiling RoboNavPlanner Console Version...

:: Add mingw32 to environment PATH
SET PATH=C:\Tools\mingw32\bin;%PATH%

:: Create build directory if it doesn't exist
if not exist "build" mkdir build

:: Start compilation - using C++17 standard
echo Compiling console version source files with g++ (C++17 standard)...
g++ -std=c++17 -Wall -O2 ^
    -DCONSOLE_VERSION ^
    -Iinclude -Isrc ^
    -o build\RoboNavPlanner_Console.exe ^
    src\main_console.cpp ^
    src\core\Map.cpp ^
    src\core\PathPlanner.cpp ^
    src\config\ConfigManager.cpp

if %ERRORLEVEL% EQU 0 (
    echo Console version compilation successful! build\RoboNavPlanner_Console.exe has been generated.
) else (
    echo Console version compilation failed. Please check the error messages above.
)

pause 