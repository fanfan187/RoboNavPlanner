@echo off
chcp 65001 >nul
echo Compiling Robot Navigation Planner...

:: User Configuration: Modify the SFML installation path below as needed
SET SFML_DIR=C:\Tools\SFML

:: Check SFML path
if not exist "%SFML_DIR%\include\SFML" (
    echo Error: SFML headers not found in "%SFML_DIR%\include".
    echo Please ensure SFML is installed in the specified directory, or modify the SFML_DIR path in this script.
    pause
    exit /b 1
)

:: Add mingw32 to environment PATH
SET PATH=C:\Tools\mingw32\bin;%PATH%

:: Create build directory if it doesn't exist
if not exist "build" mkdir build

:: Start compilation - using C++17 standard
echo Compiling project source files with g++ (C++17 standard)...
g++ -std=c++17 -Wall -O2 ^
    -DUSE_SFML ^
    -Iinclude -Isrc ^
    -I"%SFML_DIR%\include" ^
    -o build\RoboNavPlanner.exe ^
    src\main.cpp ^
    src\core\Map.cpp ^
    src\core\PathPlanner.cpp ^
    src\config\ConfigManager.cpp ^
    src\visualization\Visualizer.cpp ^
    -L"%SFML_DIR%\lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful! build\RoboNavPlanner.exe has been generated.
    echo Important note: SFML DLL files may be required when running the program.
) else (
    echo Compilation failed. Please check the error messages above.
)

pause