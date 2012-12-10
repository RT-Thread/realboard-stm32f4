/* WIFI API implementation on ART board */

#include "libwifi.h"
#include <finsh.h>

#if defined(RT_USING_LWIP) && defined(RT_USING_WIFI)

#include <rt_wlan_dev.h>
#include <dfs_posix.h>
#include <string.h>
//#include "service.h"

#define WLAN_NVM_FILE	    "/wlan.nvm"
#define WLAN_DEVICE_NAME    "w0"

struct wlan_nvm
{
    char ssid[SSID_NAME_MAX];
    char password[PASSWORD_LENGTH_MAX];
    rt_uint8_t bs_addr[MAC_LENGTH_MAX];		/* base station MAC address */

    rt_uint8_t security;
    rt_uint8_t channel;
};

static rt_err_t _save_cfg(struct rt_wlan_device * wlan)
{
    int fd;
    char line_buf[128];
    int line_length;

    fd = open(WLAN_NVM_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd >= 0)
    {
        line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                  "SSID=%s\n", wlan->ssid);
        write(fd, line_buf, line_length);

        if(wlan->security != 0)
        {
            line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                      "password=%s\n", wlan->password);
            write(fd, line_buf, line_length);
        }
        else
        {
            line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                      "password=\n");
            write(fd, line_buf, line_length);
        }

        line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                  "security=%d\n", wlan->security);
        write(fd, line_buf, line_length);

        line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                  "channel=%d\n", wlan->channel);
        write(fd, line_buf, line_length);

        line_length = rt_snprintf(line_buf, sizeof(line_buf),
                                  "addr=%02x-%02x-%02x-%02x-%02x-%02x\n",
                                  wlan->bs_addr[0], wlan->bs_addr[1], wlan->bs_addr[2],
                                  wlan->bs_addr[3], wlan->bs_addr[4], wlan->bs_addr[5]);
        write(fd, line_buf, line_length);

        close(fd);

        return RT_EOK;
    }

    return -RT_ERROR;
}

static int _get_line(int fd, char *line_buf, int line_size)
{
    char ch;
    int length = 0;

    rt_memset(line_buf, 0x00, line_size);

    while (1)
    {
        if (read(fd, &ch, 1) != 1)
        {
            return length;
        }

        if (ch == '\r') continue;
        if (ch == '\n') break;

        line_buf[length] = ch;
        length ++;
    }

    return length;
}

static int str_begin_with(const char *s, const char *t)
{
    if (strncasecmp(s, t, strlen(t)) == 0) return 1;
    return 0;
}

rt_inline int tohex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static rt_err_t _load_cfg(struct wlan_nvm* config)
{
    int fd;
    int length;
    char line_buf[128];

    fd = open(WLAN_NVM_FILE, O_RDONLY, 0);
    if (fd >= 0)
    {
        while (1)
        {
            length = _get_line(fd, line_buf, sizeof(line_buf));
            if (length == 0) break;

            if (str_begin_with(line_buf, "SSID="))
            {
                strcpy(config->ssid, line_buf + 5);
            }
            else if (str_begin_with(line_buf, "password="))
            {
                strcpy(config->password, line_buf + 9);
            }
            else if (str_begin_with(line_buf, "security="))
            {
                config->security = atoi(&line_buf[9]);
            }
            else if (str_begin_with(line_buf, "channel="))
            {
                config->channel = atoi(&line_buf[8]);
            }
            else if (str_begin_with(line_buf, "addr="))
            {
                char* ptr = line_buf + 5;
                int index;

                for (index = 0; index < 6; index ++)
                {
                    if (*ptr == '\0') break;

                    config->bs_addr[index] = (tohex(*ptr) << 4) | tohex(*(ptr + 1));
                    ptr += 3;
                }
            }
        }

        close(fd);
        return RT_EOK;
    }

    return -RT_ERROR;
}

int wlan_system_init_kernel(void)
{
    struct eth_device* dev;

    if (rt_device_find(WLAN_DEVICE_NAME) != RT_NULL)
        return 0;

//    /* try to initialize SPI hardware and tcp/ip stack */
//    if (rt_device_find("spi22") == RT_NULL)
//    {
//		extern void rt_hw_spi1_init(void);
//
//        /* SPI1 initialization */
//        rt_hw_spi1_init();
//    }

    mrvl_wlan_hw_init("spi22");
    rt_kprintf("wlan initialized\n");

    if (rt_thread_find("tcpip") == RT_NULL)
    {
        /* startup tcp/ip network stack */
        eth_system_device_init();
        /* Initialize lwip system */
        lwip_system_init();
        rt_kprintf("TCP/IP initialized!\n");
    }

    dev = (struct eth_device*) (rt_device_find(WLAN_DEVICE_NAME));
    if (dev != RT_NULL)
    {
        /* link down for wlan device */
        eth_device_linkchange(dev, RT_FALSE);
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(wlan_system_init_kernel, wlan, initialize wlan sub system);

int wlan_system_init(void)
{
//    service_do((service_func_t)wlan_system_init_kernel, RT_NULL, RT_NULL, RT_NULL, RT_NULL);
    return 0;
}

int wlan_begin_kernel(char* ssid, const char *passphrase)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;
    rt_uint8_t mode, channel;
    rt_uint8_t bs_addr[6];
    struct wlan_nvm wlan_cfg;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return -RT_ERROR;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    /* try to load wlan setting and compare the ssid and passphrase */
    _load_cfg(&wlan_cfg);
    if (rt_strcmp(ssid, wlan_cfg.ssid) == 0)
    {
        if( ((passphrase == RT_NULL) && (wlan_cfg.security == 0))
                || (rt_strcmp(passphrase, wlan_cfg.password) == 0) )
        {
            int save_flag = 0;

            /* use the saved cfg to associate wifi network */
            mode = WLAN_MODE_INFRA;
            rt_device_control(dev, WLAN_CTRL_SET_MODE, &mode);
            rt_device_control(dev, WLAN_CTRL_SET_SSID, wlan_cfg.ssid);
            rt_device_control(dev, WLAN_CTRL_SET_PASSWORD, wlan_cfg.password);
            rt_device_control(dev, WLAN_CTRL_SET_BSADDR, wlan_cfg.bs_addr);
            rt_device_control(dev, WLAN_CTRL_SET_CHANNEL, &wlan_cfg.channel);
            rt_device_control(dev, WLAN_CTRL_SET_SECURITY, &wlan_cfg.security);
            if (wlan_cfg.channel == 0xff) save_flag = 1; /* set save flag */

            if (rt_wlan_associate(wlan) == RT_EOK)
            {
                /* associate successfully */
                if (save_flag) _save_cfg(wlan);
                return RT_EOK;
            }
        }
    }

    rt_device_control(dev, WLAN_CTRL_SET_SSID, ssid);
    if (passphrase != RT_NULL)
    {
        rt_device_control(dev, WLAN_CTRL_SET_PASSWORD, (char*)passphrase);
    }

    mode = WLAN_MODE_INFRA;
    rt_device_control(dev, WLAN_CTRL_SET_MODE, &mode);

    /* set channel and base station address */
    channel = 0xff;
    rt_device_control(dev, WLAN_CTRL_SET_CHANNEL, &channel);
    rt_memset(bs_addr, 0xff, sizeof(bs_addr));
    rt_device_control(dev, WLAN_CTRL_SET_BSADDR, bs_addr);

    /* try to associate AP */
    if (rt_wlan_associate(wlan) == RT_EOK)
    {
        /* associate successfully */
        /* try to save setting */
        _save_cfg(wlan);

        return RT_EOK;
    }

    rt_kprintf("Can not associate AP.\n");
    return -RT_ERROR;
}
FINSH_FUNCTION_EXPORT_ALIAS(wlan_begin_kernel, wlan_begin, begin wlan network);

int wlan_begin(char* ssid, const char *passphrase)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return -RT_ERROR;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

//    service_do((service_func_t)wlan_begin_kernel, (void*)ssid, (void*)passphrase, RT_NULL, RT_NULL);

    return wlan->status;
}

int wlan_begin_auto_kernel(void)
{
    rt_device_t dev;
    int save_flag = 0;
    rt_uint8_t mode;
    struct rt_wlan_device *wlan;
    struct wlan_nvm cfg;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return -RT_ERROR;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    memset(&cfg, 0x0, sizeof(cfg));
    cfg.channel = 0xff;

    /* read the configuration file */
    if (_load_cfg(&cfg) != RT_EOK)
    {
        rt_kprintf("load wlan nvm failed.\n");
        return -RT_ERROR;
    }

    rt_kprintf("wlan config: ssid: %s, channel: %d, security: %d\n",
               cfg.ssid, cfg.channel, cfg.security);
    rt_kprintf("addr=%02x-%02x-%02x-%02x-%02x-%02x\n",
               cfg.bs_addr[0], cfg.bs_addr[1], cfg.bs_addr[2],
               cfg.bs_addr[3], cfg.bs_addr[4], cfg.bs_addr[5]);

    mode = WLAN_MODE_INFRA;
    rt_device_control(dev, WLAN_CTRL_SET_MODE, &mode);
    rt_device_control(dev, WLAN_CTRL_SET_SSID, cfg.ssid);
    rt_device_control(dev, WLAN_CTRL_SET_PASSWORD, cfg.password);
    rt_device_control(dev, WLAN_CTRL_SET_BSADDR, cfg.bs_addr);
    rt_device_control(dev, WLAN_CTRL_SET_CHANNEL, &cfg.channel);
    rt_device_control(dev, WLAN_CTRL_SET_SECURITY, &cfg.security);
    if (cfg.channel == 0xff) save_flag = 1; /* set save flag */

    if (rt_wlan_associate(wlan) == RT_EOK)
    {
        /* associate successfully */

        if (save_flag) _save_cfg(wlan);
        return RT_EOK;
    }

    return RT_EOK;
}
FINSH_FUNCTION_EXPORT_ALIAS(wlan_begin_auto_kernel, wlan_auto, begin wlan network automatically);
int wlan_begin_auto(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return -RT_ERROR;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

//	service_do((service_func_t)wlan_begin_auto_kernel, RT_NULL, RT_NULL, RT_NULL, RT_NULL);

    return wlan->status;
}

int wlan_disconnect(void)
{
    return 0;
}
FINSH_FUNCTION_EXPORT(wlan_disconnect, disconnect wlan network);

char* wlan_SSID(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return RT_NULL;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    return wlan->ssid;
}

int32_t wlan_RSSI(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return -RT_ERROR;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    return wlan->rssi;
}
FINSH_FUNCTION_EXPORT(wlan_RSSI, get wlan RSSI);

const char* wlan_BSSID(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return RT_NULL;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    return (char*)wlan->bs_addr;
}

uint8_t wlan_encryptionType(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return WLAN_SECURITY_OPEN;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    return wlan->security;
}

uint8_t wlan_status(void)
{
    rt_device_t dev;
    struct rt_wlan_device * wlan;

    dev = rt_device_find(WLAN_DEVICE_NAME);
    if (dev == RT_NULL)
    {
        rt_kprintf("no wlan[%s] device found.\n", WLAN_DEVICE_NAME);
        return WLAN_STATUS_IDLE;
    }

    /* get wlan device */
    wlan = (struct rt_wlan_device*) dev;

    return wlan->status;
}

RTM_EXPORT(wlan_system_init);
RTM_EXPORT(wlan_begin);
RTM_EXPORT(wlan_begin_auto);
RTM_EXPORT(wlan_disconnect);
RTM_EXPORT(wlan_SSID);
RTM_EXPORT(wlan_BSSID);
RTM_EXPORT(wlan_RSSI);
RTM_EXPORT(wlan_encryptionType);
RTM_EXPORT(wlan_status);

#endif
