#include <rtthread.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_application.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/list_view.h>
#include <rtgui/rtgui_xml.h>

#ifdef _WIN32
#include <io.h>
#include <dirent.h>
#include <sys/stat.h>
#define PATH_SEPARATOR      '\\'
#else
#include <dfs_posix.h>
#define PATH_SEPARATOR      '/'
#endif

#define APP_PATH            "/programs"
#define ITEM_MAX            10

static struct rtgui_list_item *items = RT_NULL;
static rtgui_list_view_t* _view = RT_NULL;
static int index = -1;

typedef enum
{
    IDLE,
    READ_NAME,
    READ_ICON,
    READ_AUTHOR,
    READ_LICENSE,
}XML_STATUS;

static int xml_event_handler(rt_uint8_t event, const char* text, rt_size_t len, void* user)
{
    static XML_STATUS status = IDLE;
    char fn[64];

    if(event == EVENT_START)
    {
        if(rt_strcmp(text, "name") == 0)
            status = READ_NAME;
        else if(rt_strcmp(text, "image") == 0)
            status = READ_ICON;
        else if(rt_strcmp(text, "author") == 0)
            status = READ_AUTHOR;
        else if(rt_strcmp(text, "license") == 0)
            status = READ_LICENSE;
    }
    else if(event == EVENT_TEXT)
    {
        switch(status)
        {
        case READ_NAME:    
            items[++index].name = rt_strdup(text);
            items[index].parameter = items[index].name;
            break;
        case READ_ICON:
            rt_snprintf(fn, sizeof(fn), "%s/%s", APP_PATH, text);
            items[index].image = rtgui_image_create_from_file("bmp", fn, RT_FALSE);
            break;
        case READ_AUTHOR:
            break;
        case READ_LICENSE:
            break;
        }
        status = IDLE;
    }
        
    return 1;    
}

static int xml_load_items(const char* filename)
{    
    struct rtgui_filerw* filerw;
    char buffer[512];
    rtgui_xml_t *xml;
    int length;

    /* create filerw context */
    filerw = rtgui_filerw_create_file(filename, "rb");
    if (filerw == RT_NULL) 
    {
        rt_kprintf("read file fail\n");
        return 0;
    }

    length = rtgui_filerw_read(filerw, buffer, 512, 1);
    if(length <= 0)
    {
        rt_kprintf("read fail\n");
        rtgui_filerw_close(filerw);        
        return 0;
    }
    
    xml = rtgui_xml_create(512, xml_event_handler, RT_NULL);
    if (xml != RT_NULL)    
    {        
        rtgui_xml_parse(xml, buffer, length);
        rtgui_xml_destroy(xml);    
    }

    rtgui_filerw_close(filerw);        
    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(xml_load_items, filename ) ;
#endif

static void exec_app(rtgui_widget_t* widget, void* parameter)
{
    char path[64];

    RT_ASSERT(parameter != RT_NULL);

    rt_snprintf(path, sizeof(path), "%s/%s/%s.mo", APP_PATH, 
        (char*)parameter, (char*)parameter);
    
    rt_module_open(path);
}

static void scan_app_dir(const char* path)
{
    DIR* dir;
    struct dirent* entry;
    char fn[32];

    dir = opendir(path);
    if (dir == RT_NULL)
    {
        rt_kprintf("open directory %s failed\n", path);
        return;
    }

    do
    {
        entry = readdir(dir);
        if (entry != RT_NULL)
        {
            if(entry->d_type == DFS_DT_REG) break;
            rt_sprintf(fn, "%s/%s/%s.xml", path, entry->d_name, entry->d_name);
            xml_load_items(fn);
        }
    } while(entry != RT_NULL);

    /* close directory */
    closedir(dir);
}

void realtouch_entry(void* parameter)
{
    struct rtgui_application* application;
    struct rtgui_win* win;    
    rtgui_rect_t rect;
    int i = 0;

    items = (struct rtgui_list_item *) rtgui_malloc((ITEM_MAX) * sizeof(struct rtgui_list_item));
    for(i=0; i< ITEM_MAX; i++) items[i].action = exec_app;

    application = rtgui_application_create(rt_thread_self(), "rtouch");
    if (application != RT_NULL)
    {    
        /* do touch panel calibartion */
        calibration_init();
        
        rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);
        win = rtgui_win_create(RT_NULL, "rtouch", &rect, RTGUI_WIN_STYLE_DEFAULT);

        scan_app_dir(APP_PATH);
        if(index >= 0) 
        {
            _view = rtgui_list_view_create(items, index + 1, &rect, RTGUI_LIST_VIEW_ICON);
            rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(_view));
        }        
        rtgui_win_show(win, RT_FALSE);
        
        rtgui_application_run(application);
        rtgui_application_destroy(application);
    }
}

void realtouch_gui_init(void)
{
    rt_device_t device;    
    struct rt_device_rect_info info;    
    rt_thread_t tid;

    device = rt_device_find("lcd");    
    if (device != RT_NULL)    
    {        
        info.width = 800;        
        info.height = 480;        
        /* set graphic resolution */        
        rt_device_control(device, RTGRAPHIC_CTRL_SET_MODE, &info);    
    }    

    /* re-set graphic device */    
    rtgui_graphic_set_device(device);       

    picture_show();

#if 0                
    tid = rt_thread_create("rtouch", realtouch_entry, RT_NULL, 4096, 20, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);    
#endif      
}

