## 项目概述
这是一个基于混合A*-PSO算法的机器人路径规划系统，提供图形界面和控制台两种版本。

---

## 系统要求

### 必要依赖
- **C++编译器**：支持C++17标准
  - **Windows**: MinGW-w64 或 Visual Studio 2019+
  - **Linux**: GCC 7+ 或 Clang 5+
- **SFML图形库**（仅图形版本需要）
  - **Windows**: SFML 2.5+
  - **安装路径**：`C:\Tools\SFML`（或修改`CMakeLists.txt`中的路径）
- **CMake**（可选，推荐）
  - 版本 3.16+

---

## 环境配置

### Windows环境配置

#### 方法1：使用预编译的批处理脚本（推荐）
1.  **配置SFML路径**：打开 `compile.bat` 文件，确认 `SFML_DIR` 变量指向您本地的SFML安装路径。
2.  **编译图形版本**：
    ```bash
    ./compile.bat
    ```
3.  **编译控制台版本**（无需SFML）：
    ```bash
    ./compile_console.bat 
    ```

#### 方法2：使用CMake构建
1.  **配置SFML路径**：修改 `CMakeLists.txt` 中 `set(SFML_DIR "C:/Tools/SFML-2.5.1")` 的路径。
2.  **创建构建目录**：
    ```bash
    mkdir build
    cd build
    ```
3.  **生成构建文件**（以MinGW为例）：
    ```bash
    cmake .. -G "MinGW Makefiles"
    ```
4.  **编译项目**：
    ```bash
    cmake --build .
    ```

### Linux环境配置

1.  **安装依赖**：
    ```bash
    sudo apt-get update
    sudo apt-get install build-essential libsfml-dev cmake
    ```
2.  **编译项目**：
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

---

## 运行项目

### 图形界面版本
进入 `bin` 目录并运行可执行文件：
```bash
./RoboNavPlanner.exe
```
**可视化说明：**
- 🟩 **绿色圆圈**：起点
- 🔴 **红色圆圈**：终点
- 🟨 **黄色虚线**：A*引导路径
- 🔵 **蓝色实线**：PSO优化路径
- ⚫ **黑色区域**：障碍物
- ⚪ **白色区域**：可通行区域

### 控制台版本
进入 `bin` 目录并运行可执行文件：
```bash
./RoboNavPlanner_Console.exe
```

---

## 配置文件

- **路径规划参数** (`data/config/pso_config.json`):
  ```json
  {
    "particleCount": 50,
    "generations": 100,
    "inertiaWeight": 0.5,
    "cognitiveWeight": 1.5,
    "socialWeight": 1.5,
    "deviationWeight": 5.0
  }
  ```
- **地图文件格式** (`data/maps/map.txt`):
  - `0` 代表可通行区域
  - `1` 代表障碍物

---

## 使用说明

### 基本操作
1.  **启动程序**：运行 `bin` 目录下对应的可执行文件。
2.  **查看结果**：程序会自动规划从起点(2,2)到终点(17,17)的路径。
3.  **分析路径**：观察A*引导路径和PSO优化路径的差异。

### 自定义参数
修改 `src/main.cpp` 中的参数：
```cpp
// 示例：修改起点和终点
Point start(2, 2);
Point end(17, 17);
```

---

## 故障排除

### 常见问题

1.  **编译错误：找不到SFML**
    - **解决方案**：
      - 确保SFML正确安装。
      - 检查`compile.bat`或`CMakeLists.txt`中的SFML路径配置是否正确。

2.  **运行时错误：缺少DLL文件**
    - **解决方案**：
      - 将SFML的DLL文件（如 `sfml-graphics-2.dll`, `openal32.dll` 等）复制到可执行文件所在的 `bin` 目录。
      - 或者将SFML的`bin`目录添加到系统环境变量`PATH`中。

3.  **配置文件找不到**
    - **解决方案**：
      - 确保从项目根目录运行程序，或者可执行文件能够正确访问到 `data` 目录。
      - 检查代码中引用的配置文件路径是否正确。

4.  **地图文件加载失败**
    - **解决方案**：
      - 检查 `data/maps/map.txt` 文件是否存在。
      - 确认地图文件内容格式正确（只包含0, 1和空格/换行符）。

### 性能优化建议

- **调整PSO参数**：
  - 减少粒子数量 (`particleCount: 30-50`)
  - 减少迭代次数 (`generations: 50-100`)
  - *适用于快速测试。*
- **调整A*引导权重**：
  - 增加 `deviationWeight` (5.0-10.0)：使PSO路径更贴近A*路径。
  - 减少 `deviationWeight` (1.0-3.0)：给予PSO优化更大的自由度。
- **修改地图复杂度**：
  - **简单地图**：减少障碍物，降低算法计算复杂度。
  - **复杂地图**：增加障碍物，测试算法的鲁棒性。

---

## 项目结构

```
RoboNavPlanner/
├── bin/                # 可执行文件
├── build/              # 编译目录
├── data/
│   ├── config/pso_config.json
│   └── maps/map.txt
├── src/
│   ├── a_star.cpp
│   ├── a_star.h
│   ├── main.cpp
│   ├── pso.cpp
│   ├── pso.h
│   └── ...             # 其他源文件
├── .gitignore
├── CMakeLists.txt
└── compile.bat
```

---

## 项目特色
- **混合算法**：结合A*的全局搜索能力和PSO的局部优化能力。
- **实时可视化**：直观显示两种算法的路径对比。
- **参数可调**：支持运行时调整核心算法参数。
- **跨平台**：支持Windows和Linux系统。
- **模块化设计**：代码结构清晰，易于扩展和维护。