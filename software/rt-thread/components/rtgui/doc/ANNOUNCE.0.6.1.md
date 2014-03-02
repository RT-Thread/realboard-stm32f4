RTGUI 0.6.1 Release Notes

This is mainly a bug fixing release. Now the window system can be used
intensively.

# Running Environment

In theory, new RTGUI can run on all the devices running old RTGUI. There is
a simulator which run on Windows to help evaluating the new RTGUI. The default
running environment is RealTouch. The simulator in RT-Thread could also be used.

Because RT-Thread has removed all the RTGUI components from the source tree, in
order to run RTGUI in the upstream RT-Thread, you have to copy the
`components/rtgui` to the `components` folder in RTT. If you need demo, files
in `demo/examples` should also be copied into `examples/gui`.

# Main Changes

0. Simplified the windows showing algorithm. Children window will always be
   shown when parent window has been shown. 257589a
0. Move `calibration.c` in bsps into RTGUI. There is no need to write it's
   own `calibration.c` for all the bsps. calibration in RTGUI provides
   `calibration_set_restore` and `calibration_set_after` API, by which the user
   could use to skip the calibration or save calibration data. The example is
   in bsp/stm31f10x . bd7c6e4

# Bug fixes

- 3ce204d: fixed the children window of root window could not be modaled bug.
- 4d5564d: `rtgui_widget_update_clip` could not be used on window. Thanks to
  xiao苦 for reporting the bug.
- dfb477e: repaint the whole window tree when one window in the tree has been activated.
- f42adc2: delete useless RTM\_EXPORT. Thanks to prife for bug reporting.
- cb3bf9a: fixed clip updating bug when hiding widgets. Thanks to xiao苦 for
  bug reporting.
- 110e0c6: When file fonts has been enabled but the file could not be opened,
  fire a warning on console. Thanks to prife, 文哥 and wnnwoo .
- 7611e37: fixed compiling error in demo\_view\_listctrl . Thanks to znfc2 .
- 3b1f698: added a underflow check on `ref_count` in `rtgui_app_exit`.

