本例中realtouch将使用DHCP获取ip地址，并演示realtouch做ftp server的效果。
本例中，ftp server的用户名为rtt，密码为demo.

使用方法：
连接硬件，将串口线与硬件连接；将网线与硬件连接
编译并烧录程序后，复位硬件，应该会看到类似如下信息：
 \ | /
- RT -     Thread Operating System
 / | \     1.1.0 build Jan  1 2013
 2006 - 2012 Copyright by rt-thread team
TCP/IP initialized!
...
...
finsh>>flash0 mount to /.
ftp server begin...
ftp server started!!

然后在finsh中执行list_if()，可以查看当前开发板的ip地址

finsh>>list_if()
network interface: e0 (Default)
MTU: 1500
MAC: 00 80 e1 04 2f 3c 
FLAGS: UP LINK_UP DHCP ETHARP
ip address: 222.197.173.170
gw address: 222.197.173.129
net mask  : 255.255.255.192

dns server #0: 202.112.14.151
dns server #1: 202.112.14.161
        30, 0x0000001e
finsh>>


然后在打开windows命令行窗口，通过ftp命令即可实现文件的上传与下载。
ftp的基本用法可以参考
http://www.blueidea.com/computer/soft/2004/2228.asp
