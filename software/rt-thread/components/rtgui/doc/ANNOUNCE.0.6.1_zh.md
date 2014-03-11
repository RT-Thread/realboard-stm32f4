RTGUI 0.6.1 版发布纪要

本版本主要是 bug 修复。修正了很多窗口系统的 bug，使其达到可以大规模使用的程度。

# 运行环境

理论上原来可以运行 RTGUI 的设备上都可以运行新的 RTGUI。本版本自带一个 Windows
模拟器，用来方便地评估运行效果。默认的运行环境是 realtouch。RT-Thread 中的模拟
器也是可以运作的。

由于 RT-Thread 上游已经将 RTGUI 组件删除，所以如果需要在 RT-Thread 中运行 RTGUI
的话需要将 components/rtgui 复制到 RTT 的 components 文件夹之下。如果需要运行
demo，需要将 demo/examples 内的所有文件复制到 RTT 的 examples/gui/ 目录下(没有
目录的需要创建一个)。

# 主要变化

0. 简化了窗口显示算法，子窗口会随着父窗口的显示而显示。 257589a
0. 将原来在各个 bsp 里面的触摸屏校准程序 calibration.c 移进 RTGUI。各个 bsp 今
   后不用再编写自己的屏幕校准程序。同时，RTGUI 内的 calibration 提供了
   `calibration_set_restore` 和 `calibration_after`接口，供应用程序直接跳过屏幕
   校准和存储通过屏幕触摸得来的校准数据。这部分的应用实例在bsp/stm31f10x 中。
   bd7c6e4

# Bug fixes

- 3ce204d: 修正了模态窗口未能使根窗口的一级子窗口进入模态的 bug。
- 4d5564d: `rtgui_widget_update_clip` 不能应用于窗口控件。感谢 xiao苦 报告bug。
- dfb477e: 当窗口被激活时，重绘整个窗口树。修复了当有窗口遮盖被激活窗口的父窗口
  时的 bug。
- f42adc2: 删除了无用的 RTM\_EXPORT。感谢 prife 报告 bug。
- cb3bf9a: 修复了隐藏控件时更新剪切域的 bug。感谢 xiao苦 报告bug。
- 110e0c6: 如果使能了字体文件但是文件打开失败，打印一条警告信息。感谢 prife,
  文哥 和 wnnwoo 。
- 7611e37: 修复了 demo\_view\_listctrl 的编译错误。感谢 znfc2 。
- 3b1f698: 在 `rtgui_app_exit` 中加入了对 `ref_count` 的检查。

