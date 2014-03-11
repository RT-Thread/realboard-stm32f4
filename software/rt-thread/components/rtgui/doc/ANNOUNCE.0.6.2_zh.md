RTGUI 0.6.2 版发布纪要

本月是寂静的一个月。RTGUI 的文档已经在《RT-Thread 编程手册》的框架下开始着手编
写了。项目地址在 https://github.com/RT-Thread/manual-doc 。欢迎大家提出意见和建
议。

# 主要变化

0. 用 RT-Thread 主干 bsp/simulator 替换了 RTGUI 中 与 RTT 相关的部分。使得其能
   够跟得上 RTT 内核的更新。感谢 prife 的大力支持。
0. 添加了 `--build-tests` 选项用来设置编译测试用例。

# Bug fixes

- d9eb416: 修正了窗口子控件销毁窗口会引起崩溃的问题。感谢论坛上的 softwind。

