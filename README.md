# 基于瑞萨电子 RA6M5 的智能导盲系统
## 系统介绍
这是一个基于`R7FA6M5BH3CFP`芯片的瑞萨开发板为主控制器，`ESP32-C3`作为网络模块的智能导盲系统雏形，通过OLED12864显示数据、TOF400C模块进行测距、AD+光敏电阻读取光照，并以此进行一系列对应操作（如发出声、光、语音提示）。并可以通过与ESP模块的联动，获取实时位置和发送电子邮件。

## 文件介绍
`\RA6M5` - 瑞萨芯片的主程序，使用e2 studio编译、J-Link下载 
- `\src` - 源代码
- `hal_entry.c` - 入口函数
- `mk_pinctrl` - GPIO兼容层
- `mk_rtt` - UART与JLink RTT Serial兼容层
- `OLED` - IIC OLED驱动程序
- `tof400c` - IIC TOF测距传感器驱动
- `\Debug` - 生成的二进制和中间文件
- `configure.xml` - 可视化配置工具

`\ESP32_C3_Arduino` - ESP32的C++代码，使用Arduino安装库后可以编译运行

## 参见
**演示视频：** [bilibili](https://www.bilibili.com/video/BV1hxGgzbE6b)
**开发文档：** *(原始STM32项目)* [个人主页](https://snowmiku-home.top/2025/06/24/post-2025624/)