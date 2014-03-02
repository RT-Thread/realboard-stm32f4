RTGUI 0.7 版发布纪要

本版本在 bug 修复的基础上添加了一些新的功能，主要是 textbox 和 edit 的宽字符输
入，编辑；rockbox fnt 字体的支持和 object id 的支持。之前的 RTGUI 作为RT-Thread
组件的一员，一直以和 RT-Thread 相同的许可证(开源的 GPLv2 和特殊商业许可证双许可
)发布的。秉承技术交流，共同进步的原则，从这个版本开始，RTGUI 将明确的完全以
GPLv2 和商业许可双许可证的方式发布，随源代码有 `LICENSE` 文件。同时我们欢迎各公
司将 RTGUI 用于产品当中，关于商业的许可问题请另行咨询。个人学习研究或用于学术目
的的源代码获取和使用不在此列。

# 主要变化

0. 使用了新的 init 机制。`rtgui_system_server_init` 在 APP 级别初始化。 350378e
0. textbox 控件支持宽字符的光标移动，字符删除和输入。edit 控件支持宽字符的输入(
   删除和光标移动之前就已经支持)。 ca9c9d9, dd48df9, dd0cf33
0. 添加了对 rockbox fnt 非等宽字体的支持。 09441ec
0. 添加 object id。在对 object 设置好 id 之后，用户可以拿 id 去查询 object 的指
   针。方便模块间引用相同的 object。并且可以通过 `RTGUI_USING_ID_CHECK` 来检查
   id 的唯一性。示例代码在 `demo_view_window.c`。 70b93a6
0. calibration.c 独立出来放到 app 目录下。

# Bug fixes

- 310c7b2: 修正了在 `last_mevent_widget` 已经将事件处理了的情况下的事件传递问题
  。感谢 softwind 报告 bug 并提供测试用例。
- c7f36ad: 在关闭窗口时添加对 `WINTITLE_CLOSEBOX` 的检查。感谢 winfeng 报告 bug
  ，提交补丁。
- ab9d9b5: 在 `rtgui_checkbox_set_checked` 中更新控件。感谢 hexlog 报告 bug。
- a4b6829: 修正了当多窗口树下有模态窗口时的鼠标事件派发问题。感谢 limxuzheng 报告 bug。
- 1c685bd: 修正的 ARGB 的 png 图片的透明问题。感谢 limxuzheng 报告 bug 并提交补丁。
- fcda3a5, 4c081e1: 添加了 `RTGUI_ONLY_ONE_WINDOW_TREE` 选项，使得在激活窗口的
  时候不会重绘父窗口。感谢 limxuzheng 和 hexlog 报告 bug。
- b9335b9: 修复了改变 textbox 之后引起的光标位置问题。感谢 hexlog 报告 bug 并提交补丁。
- 6a0cfb0, 77dbc66: Linux 平台模拟器和 SDL 驱动的修正。
- 09441ec: 修复了一些编译警告(Bernard)。

