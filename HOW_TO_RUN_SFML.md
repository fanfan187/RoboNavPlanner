# 如何运行基于SFML的机器人路径规划系统

## 运行状态 ✅

✅ **编译成功** - 所有错误已修复  
✅ **控制台版本可运行** - 已验证正常工作  
✅ **SFML版本已生成** - 可执行文件已创建  

## 快速运行

### 1. 控制台版本（推荐先尝试）
```bash
# 运行控制台版本，查看算法效果
.\build\RoboNavPlanner_Console.exe
```
**特点：**
- 无需额外依赖
- ASCII字符显示地图和路径
- 显示详细的算法执行过程

### 2. SFML图形版本
```bash
# 运行SFML图形版本
.\build\RoboNavPlanner.exe
```

**如果出现DLL缺失错误，请按以下方式解决：**

#### 方法1：复制SFML DLL文件
```bash
# 将以下DLL文件从 C:\Tools\SFML\bin\ 复制到 build\ 目录：
copy "C:\Tools\SFML\bin\sfml-graphics-3.dll" build\
copy "C:\Tools\SFML\bin\sfml-window-3.dll" build\
copy "C:\Tools\SFML\bin\sfml-system-3.dll" build\
```

#### 方法2：添加SFML到系统PATH
```bash
# 将 C:\Tools\SFML\bin 添加到系统环境变量PATH中
```

## 可视化说明

### 控制台版本输出：
- `S` = 起点
- `E` = 终点  
- `*` = 路径点
- `#` = 障碍物
- `.` = 可通行区域

### SFML图形版本：
- **白色区域** = 可通行空间
- **深红色方块** = 障碍物
- **绿色圆圈** = 起点
- **红色圆圈** = 终点
- **蓝色线段** = PSO优化路径
- **蓝色小圆** = 中间航点
- **黄色线段** = A*引导路径

### 交互功能（SFML版本）：
- **鼠标左键** = 获取点击坐标
- **空格键** = 重新规划路径
- **ESC键** = 退出程序

## 算法信息

**当前使用的算法：** Hybrid A*-PSO（混合A*粒子群优化算法）

**算法流程：**
1. 使用A*算法生成全局引导路径
2. 使用PSO算法进行局部优化
3. 结合引导路径偏离惩罚和平滑度奖励

**性能参数：**
- 粒子数量：150
- 进化代数：300
- 中间航点：6个
- 全局/局部引导系数：2.0

## 故障排除

### 如果SFML版本无法运行：
1. 确保 `C:\Tools\SFML` 路径存在
2. 确保 MinGW 编译器已正确安装
3. 尝试先运行控制台版本验证算法逻辑
4. 检查DLL文件是否在正确位置

### 如果控制台版本字符显示异常：
- 这是编码问题，算法本身运行正常
- 可以通过路径坐标和统计信息验证结果

## 文件说明

- `build\RoboNavPlanner.exe` - SFML图形版本
- `build\RoboNavPlanner_Console.exe` - 控制台版本
- `data\config\pso_config.json` - 算法参数配置
- `data\maps\map.txt` - 地图文件 