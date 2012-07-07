#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

// <RDTConfigurator URL="http://www.rt-thread.com/eclipse">

// <integer name="RT_NAME_MAX" description="Maximal size of kernel object name length" default="6" />
#define RT_NAME_MAX	8
// <integer name="RT_ALIGN_SIZE" description="Alignment size for CPU architecture data access" default="4" />
#define RT_ALIGN_SIZE	4
// <integer name="RT_THREAD_PRIORITY_MAX" description="Maximal level of thread priority" default="2">
// <item description="8">8</item>
// <item description="32">32</item>
// <item description="256">256</item>
// </integer>
#define RT_THREAD_PRIORITY_MAX	32
// <integer name="RT_TICK_PER_SECOND" description="OS tick per second" default="100" />
#define RT_TICK_PER_SECOND	100
// <integer name="IDLE_THREAD_STACK_SIZE" description="The stack size of idle thread" default="512" />
#define IDLE_THREAD_STACK_SIZE	512
// <bool name="RT_USING_MODULE" description="Using Application Module" default="true" />
#define RT_USING_MODULE
// <section name="RT_DEBUG" description="Kernel Debug Configuration" default="true" >
#define RT_DEBUG
// <bool name="RT_THREAD_DEBUG" description="Thread debug enable" default="false" />
// #define RT_THREAD_DEBUG
// <bool name="RT_USING_OVERFLOW_CHECK" description="Thread stack over flow detect" default="true" />
#define RT_USING_OVERFLOW_CHECK
// </section>

// <bool name="RT_USING_HOOK" description="Using hook functions" default="true" />
#define RT_USING_HOOK
// <section name="IPC" description="Inter-Thread communication" default="always" >
// <bool name="RT_USING_SEMAPHORE" description="Using semaphore in the system" default="true" />
#define RT_USING_SEMAPHORE
// <bool name="RT_USING_MUTEX" description="Using mutex in the system" default="true" />
#define RT_USING_MUTEX
// <bool name="RT_USING_EVENT" description="Using event group in the system" default="true" />
#define RT_USING_EVENT
// <bool name="RT_USING_MAILBOX" description="Using mailbox in the system" default="true" />
#define RT_USING_MAILBOX
// <bool name="RT_USING_MESSAGEQUEUE" description="Using message queue in the system" default="true" />
#define RT_USING_MESSAGEQUEUE
// </section>

// <section name="MM" description="Memory Management" default="always" >
// <bool name="RT_USING_MEMPOOL" description="Using Memory Pool Management in the system" default="true" />
#define RT_USING_MEMPOOL
// <bool name="RT_USING_HEAP" description="Using Dynamic Heap Management in the system" default="true" />
#define RT_USING_HEAP
// <bool name="RT_USING_SMALL_MEM" description="Optimizing for small memory" default="false" />
#define RT_USING_SMALL_MEM
// <bool name="RT_USING_SLAB" description="Using SLAB memory management for large memory" default="false" />
// #define RT_USING_SLAB
// </section>

// <section name="RT_USING_DEVICE" description="Using Device Driver Framework" default="true" >
#define RT_USING_DEVICE
// <bool name=RT_USING_DEVICE_IPC description="Using IPC in Device Driver Framework" default="true" />
#define RT_USING_DEVICE_IPC
// <bool name="RT_USING_SERIAL" description="Using Serial Device Driver Framework" default="true" />
#define RT_USING_SERIAL
// <bool name="RT_USING_SPI" description="Using SPI Device Driver Framework" default="true" />
#define RT_USING_SPI
// <integer name="RT_UART_RX_BUFFER_SIZE" description="The buffer size for UART reception" default="64" />
#define RT_UART_RX_BUFFER_SIZE	64
// </section>

// <section name="RT_USING_USB_HOST" description="USB Host stack" default="true" >
#define RT_USING_USB_HOST
// <bool name="RT_USB_CLASS_MASS_STORAGE" description="Using USB mass storage class" default="true" />
//#define RT_USB_CLASS_MASS_STORAGE
// <bool name="RT_USB_CLASS_ADK" description="Using ADK class" default="true" />
#define RT_USB_CLASS_ADK
// <string name="UDISK_MOUNTPOINT" description="udisk mount point" default="/" />
#define UDISK_MOUNTPOINT	"/sd"
// <bool name="RT_USB_HCD_STM32" description="Using STM32 usb host controller driver" default="true" />
#define RT_USB_HCD_STM32
// </section>

// <section name="RT_USING_CONSOLE" description="Using console" default="true" >
#define RT_USING_CONSOLE
// <integer name="RT_CONSOLEBUF_SIZE" description="The buffer size for console output" default="128" />
#define RT_CONSOLEBUF_SIZE	128
// <string name="RT_CONSOLE_DEVICE_NAME" description="console device name" default="uart3" />
#define RT_CONSOLE_DEVICE_NAME	"uart3"
// </section>

// <section name="RT_USING_FINSH" description="Using finsh as shell, which is a C-Express shell" default="true" >
#define RT_USING_FINSH
// <bool name="FINSH_USING_SYMTAB" description="Using symbol table in finsh shell" default="true" />
#define FINSH_USING_SYMTAB
// <bool name="FINSH_USING_DESCRIPTION" description="Keeping description in symbol table" default="true" />
#define FINSH_USING_DESCRIPTION
// <integer name="FINSH_THREAD_STACK_SIZE" description="The stack size for finsh thread" default="4096" />
#define FINSH_THREAD_STACK_SIZE	4096
// </section>

// <section name="LIBC" description="C Runtime library setting" default="always" >
// <bool name="RT_USING_NEWLIB" description="Using newlib library, only available under GNU GCC" default="true" />
// #define RT_USING_NEWLIB
// <bool name="RT_USING_PTHREADS" description="Using POSIX threads library" default="true" />
#define RT_USING_PTHREADS
// </section>

// <section name="RT_USING_DFS" description="Device file system" default="true" >
#define RT_USING_DFS
// <bool name="DFS_USING_WORKDIR" description="Using working directory" default="true" />
#define DFS_USING_WORKDIR
// <integer name="DFS_FILESYSTEMS_MAX" description="The maximal number of mounted file system" default="4" />
#define DFS_FILESYSTEMS_MAX	2
// <integer name="DFS_FD_MAX" description="The maximal number of opened files" default="4" />
#define DFS_FD_MAX	4
// <bool name="RT_USING_DFS_ELMFAT" description="Using ELM FatFs" default="true" />
#define RT_USING_DFS_ELMFAT
// <integer name="RT_DFS_ELM_USE_LFN" description="Support long file name" default="0">
// <item description="LFN1">1</item>
// <item description="LFN1">2</item>
// </integer>
#define RT_DFS_ELM_USE_LFN	1
// <integer name="RT_DFS_ELM_MAX_LFN" description="Maximal size of file name length" default="256" />
#define RT_DFS_ELM_MAX_LFN	256
// <bool name="RT_USING_DFS_YAFFS2" description="Using YAFFS2" default="false" />
// #define RT_USING_DFS_YAFFS2
// <bool name="RT_USING_DFS_UFFS" description="Using UFFS" default="false" />
// #define RT_USING_DFS_UFFS
// <bool name="RT_USING_DFS_DEVFS" description="Using devfs for device objects" default="true" />
#define RT_USING_DFS_DEVFS
// <bool name="RT_USING_DFS_ROMFS" description="Using ROMFS" default="true" />
#define RT_USING_DFS_ROMFS
// </section>

// <section name="RT_USING_RTGUI" description="RTGUI, a graphic user interface" default="true" >
#define RT_USING_RTGUI
// <integer name="RTGUI_NAME_MAX" description="Maximal size of RTGUI object name length" default="16" />
#define RTGUI_NAME_MAX	16
// <bool name="RTGUI_USING_FONT16" description="Support 16 weight font" default="true" />
#define RTGUI_USING_FONT16
// <bool name="RTGUI_USING_FONT12" description="Support 12 weight font" default="true" />
#define RTGUI_USING_FONT12
// <bool name="RTGUI_USING_FONTHZ" description="Support Chinese font" default="true" />
#define RTGUI_USING_FONTHZ
// <bool name="RTGUI_USING_DFS_FILERW" description="Using DFS as file interface " default="true" />
#define RTGUI_USING_DFS_FILERW
// <bool name="RTGUI_USING_HZ_FILE" description="Using font file as Chinese font" default="false" />
// #define RTGUI_USING_HZ_FILE
// <bool name="RTGUI_USING_HZ_BMP" description="Using Chinese bitmap font" default="true" />
#define RTGUI_USING_HZ_BMP
// <bool name="RTGUI_USING_SMALL_SIZE" description="Using small size in RTGUI" default="false" />
// #define RTGUI_USING_SMALL_SIZE
// <bool name="RTGUI_USING_MOUSE_CURSOR" description="Using mouse cursor in RTGUI" default="false" />
// #define RTGUI_USING_MOUSE_CURSOR
// <bool name="RTGUI_IMAGE_XPM" description="Using xpm image in RTGUI" default="true" />
#define RTGUI_IMAGE_XPM
// <bool name="RTGUI_IMAGE_JPEG" description="Using jpeg image in RTGUI" default="true" />
#define RTGUI_IMAGE_JPEG
// <bool name="RTGUI_IMAGE_PNG" description="Using png image in RTGUI" default="true" />
#define RTGUI_IMAGE_PNG
// <bool name="RTGUI_IMAGE_BMP" description="Using bmp image in RTGUI" default="true" />
#define RTGUI_IMAGE_BMP
// </section>

// </RDTConfigurator>

#endif
