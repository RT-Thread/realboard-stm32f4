RTGUI 0.6.2 release notes

March is a silent month.

# Main Changes

0. Substituted the RT-Thread kernel part with the bsp/simulator in the trunk
   RTT. This makes the kernel of RTGUI is always up-to-date with current
   kernel. Thank prife for his kindness help and awesome work.
0. add `--build-tests` option to build all the test cases.

# Bug fixes

- d9eb416: fix a bug in win_event_handler that destroying window in child
  widgets may crash the program. Thank softwind on the forum.

