RTGUI 0.7 Release Notes

Along with many bug fixes, this release brings many new features, including
wide character support in textbox and edit widget, rockbox fnt font support and
object id support. In the past, as part of the components within RT-Thread,
RTGUI is released in the same license with RT-Thread --- GPLv2 and commercial
dual license. According to the rule of technical exchange and common progress,
RTGUI will be released specifically in dual license: GPLv2 for non-commercial
usage and a seperate commercial license for commercial usage. For the
commercial license, please consult http://www.rt-thread.com . Individual study
and research for academic purposes is not included in such condition.

# Main Changes

0. make use of the new init scheme, `rtgui_system_server_init` is registered as
   APP level. 350378e
0. textbox widget support wide char cursor moving, deleting and inputing. edit
   widget support wide char inputing. ca9c9d9, dd48df9, dd0cf33
0. add support for rockbox fnt font. 09441ec
0. add object id feature. One could obtain the pointer to the object after it
   set the id to the object. You could also enable the `RTGUI_USING_ID_CHECK`
   option to let RTGUI check the uniqueness of the id. Example code is in
   `demo_view_window.c`. 70b93a6
0. calibration.c moved to app/.

# Bug fixes

- 310c7b2: fixed event propagation in `last_mevent_widget`. Thanks to softwind
  for the bug report and test case.
- c7f36ad: add a check on `WINTITLE_CLOSEBOX` when closing the window. Thanks
  to winfeng for bug report and patch.
- ab9d9b5: update widget in `rtgui_checkbox_set_checked`. Thanks to hexlog for
  the bug report.
- a4b6829: fixed mouse event dispatch in modal window in multiply window tree.
  Thanks to limxuzheng for the bug report.
- 1c685bd: fixed ARGB transparent bug in png image. Thanks to limxuzheng for
  the bug report and patch.
- fcda3a5, 4c081e1: added `RTGUI_ONLY_ONE_WINDOW_TREE` option, which will
  prevent repainting the parent window when activating the child window.
  Thanks for limxuzheng and hexlog for bug report.
- b9335b9: fixed textbox cursor position bug after setting the font. Thanks to
  hexlog for bug report and patch.
- 6a0cfb0, 77dbc66: fixed bugs in simulator in Linux and SDL driver.
- 09441ec: fixed some compiling warning (Bernard)。

