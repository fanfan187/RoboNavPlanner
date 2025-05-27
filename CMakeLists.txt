cmake_minimum_required(VERSION 3.16)
project(RoboNavPlanner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找SFML
find_package(sfml REQUIRED COMPONENTS graphics window system)

# 设置源文件
set(SOURCES
    src/main.cpp
    src/core/Map.cpp
    src/core/PathPlanner.cpp
    src/visualization/Visualizer.cpp
)

# 设置头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include       # ${PROJECT_SOURCE_DIR} 指向项目根目录
    ${PROJECT_SOURCE_DIR}/src/algorithm
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/visualization
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接SFML库
target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 