cmake_minimum_required(VERSION 3.16)
project(RoboNavPlanner VERSION 1.0.0 LANGUAGES CXX)

# 设置编码以处理中文路径
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=GBK")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置SFML路径
set(SFML_ROOT "C:/Tools/SFML")
set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")

# 设置静态链接（重要：基于lib目录结构）
set(SFML_STATIC_LIBRARIES TRUE)

# 查找SFML（不指定版本号，让它自动检测）
find_package(SFML REQUIRED COMPONENTS Graphics Window System)

# 设置源文件
set(SOURCES
    src/main.cpp
    src/core/PathPlanner.cpp
    src/visualization/Visualizer.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/algorithm
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/visualization
)

# 链接SFML库（使用现代CMake目标）
target_link_libraries(${PROJECT_NAME}
    SFML::Graphics
    SFML::Window
    SFML::System
)

# 添加编译宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    USE_SFML
    SFML_STATIC
)

# Windows特定的系统库依赖
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        opengl32    # OpenGL
        winmm       # Windows多媒体
        gdi32       # GDI
    )
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 显示SFML信息用于调试
message(STATUS "SFML_FOUND: ${SFML_FOUND}")
message(STATUS "SFML_VERSION: ${SFML_VERSION}")
message(STATUS "SFML_ROOT: ${SFML_ROOT}")
message(STATUS "SFML_DIR: ${SFML_DIR}")