#include <stdint.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <lwip/sockets.h>

#include "http.h"

#define RESOURCE_DIR                "/resource"
#define RETRY_MAX                   3
#define BUFFER_SIZE                 4096

struct resource_item
{
    char * name;
    rt_size_t size;
    char * url;
};

static const struct resource_item resource_table[] =
{
    {
        "/resource/gbk2uni.tbl",
        1024 * 128,
        "http://www.rt-thread.org/realtouch/resource/gbk2uni.tbl"
    },
    {
        "/resource/uni2gbk.tbl",
        1024 * 128,
        "http://www.rt-thread.org/realtouch/resource/uni2gbk.tbl"
    },
};

#define ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))

const char _get[] = "GET %s HTTP/1.0\r\nHost: %s:%d\r\nUser-Agent: RT-Thread HTTP Agent\r\nConnection: close\r\n\r\n";

static int servicer_connect(struct sockaddr_in* server, char* host_addr, const char* url)
{
    int socket_handle;
    int peer_handle;
    int rc;
    char mimeBuffer[256];

    if((socket_handle = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0)
    {
        rt_kprintf( "RLUS: SOCKET FAILED\n" );
        return -1;
    }

    peer_handle = connect( socket_handle, (struct sockaddr *) server, sizeof(*server));
    if ( peer_handle < 0 )
    {
        closesocket(socket_handle);
        rt_kprintf( "RLUS: CONNECT FAILED %i\n", peer_handle );
        return -1;
    }

    {
        char *buf;
        rt_uint32_t length;

        buf = rt_malloc (512);
        if (*url)
            length = rt_snprintf(buf, 512, _get, url, host_addr, ntohs(server->sin_port));
        else
            length = rt_snprintf(buf, 512, _get, "/", host_addr, ntohs(server->sin_port));

        rc = send(peer_handle, buf, length, 0);

        /* release buffer */
        rt_free(buf);
    }
    /* read the header information */
    while ( 1 )
    {
        // read a line from the header information.
        rc = http_read_line(peer_handle, mimeBuffer, sizeof(mimeBuffer));
        rt_kprintf(">>%s", mimeBuffer);

        if ( rc < 0 )
        {
            closesocket(peer_handle);
            return rc;
        }
        // End of headers is a blank line.  exit.
        if (rc == 0) break;
        if ((rc == 2) && (mimeBuffer[0] == '\r')) break;

        if (strstr(mimeBuffer, "HTTP/1."))
        {
            rc = http_is_error_header(mimeBuffer);
            if(rc)
            {
                rt_kprintf("HTTP: status code = %d!\n", rc);
                closesocket(peer_handle);
                return -rc;
            }
        }

        if (strstr(mimeBuffer, "content-type:"))
        {
            /* check content-type */
            if (strstr(mimeBuffer, "text/plain") == RT_NULL)
            {
                rt_kprintf("radio list update content is not text/plain.\n");
                closesocket(peer_handle);
                return -1;
            }
        }

        if (strstr(mimeBuffer, "Content-Type:"))
        {
#if 0       // not check content-type anymore
            /* check content-type */
            if (strstr(mimeBuffer, "text/plain") == RT_NULL)
            {
                rt_kprintf("radio list update content is not text/plain.\n");
                closesocket(peer_handle);
                return -1;
            }
#endif
        }
    }

    return peer_handle;
}

static int servicer_session_open(const char* url)
{
    int peer_handle = -1;
    struct sockaddr_in server;
    char *request, host_addr[32];

    {
        uint32_t dns_try = RETRY_MAX;
        while(dns_try--)
        {
            if(http_resolve_address(&server, url, &host_addr[0], &request) == 0)
            {
                break;
            }
            rt_kprintf("[INFO] dns try...!\r\n");
        }
        if(dns_try == 0)
        {
            rt_kprintf("[ERR] dns retry timeout!\r\n");
            return -1;
        }
    }

    rt_kprintf("connect to: %s...\n", host_addr);

    if((peer_handle = servicer_connect(&server, host_addr, request)) < 0)
    {
        rt_kprintf("radio list update: failed to connect to '%s'!\n", host_addr);
    }
    return peer_handle;

}

int http_down(const char * file_name, const char * url)
{
    int peer_handle = 0;
    int fd;
    int rc;

    peer_handle = servicer_session_open(url);

    if(peer_handle < 0)
    {
        return -1;
    }

    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if(fd < 0)
    {
        return -1;
    }

    /* get and write */
    {
        uint32_t get_size = 0;
        uint32_t get_count = 0;
        uint8_t * buf = NULL;
        uint8_t * get_buffer;

        buf = rt_malloc (BUFFER_SIZE * 2);
        if(buf == RT_NULL)
        {
            return -1;
        }
        else
        {
            get_buffer = buf;
        }

        while ( 1 )
        {
            // read a line from the header information.
            rc = recv(peer_handle, get_buffer, BUFFER_SIZE, 0);

            if ( rc < 0 ) break;

            // End of headers is a blank line.  exit.
            if (rc == 0) break;

            rt_kprintf("[http] recv %u\r\n", rc);

            get_buffer += rc;
            get_count += rc;

            get_size += rc;
            if(get_size >= BUFFER_SIZE)
            {
                write(fd, buf, BUFFER_SIZE);
                rt_kprintf("#");
                rt_kprintf("[http] write BUFFER_SIZE %u, get_count %u.\r\n", BUFFER_SIZE, get_count);
                get_size -= BUFFER_SIZE;

                if(get_size > 0)
                {
                    memcpy(buf, buf + BUFFER_SIZE, get_size);
                    get_buffer = buf + get_size;
                }
                else
                {
                    get_buffer = buf;
                }
            }
        }

        if(get_size > 0)
        {
            write(fd, buf, get_size);
            rt_kprintf("*");
            rt_kprintf("[http] write %u\r\n", get_size);
        }
        rt_kprintf("\r\nget_count %u.\r\n", get_count);
        rt_free(buf);
    } /* get and write */

    closesocket(peer_handle);

    if(close(fd) == 0)
    {
        rt_kprintf("Update radio list succeed \r\n");
    }

    return 0;
}

rt_err_t resource_download(void)
{
    uint32_t i;
    struct stat file_stat;

    /* check RESOURCE DIR */
    {
        int fd;

        fd = open(RESOURCE_DIR, DFS_O_DIRECTORY, 0);
        if(fd < 0)
        {
            rt_kprintf("[INFO] %s is not exits, create it!\r\n", RESOURCE_DIR);
            fd = open(RESOURCE_DIR, DFS_O_DIRECTORY | DFS_O_CREAT, 0);
            if(fd >= 0)
            {
                close(fd);
            }
            else
            {
                rt_kprintf("[ERR] %s create failed!\r\n", RESOURCE_DIR);
            }
        }

        /* /SD */
        fd = open("/SD", DFS_O_DIRECTORY, 0);
        if(fd < 0)
        {
            rt_kprintf("[INFO] %s is not exits, create it!\r\n", "/SD");
            fd = open("/SD", DFS_O_DIRECTORY | DFS_O_CREAT, 0);
            if(fd >= 0)
            {
                close(fd);
            }
            else
            {
                rt_kprintf("[ERR] %s create failed!\r\n", "/SD");
            }
        }
    } /* check RESOURCE_DIR */

    for(i=0; i<ARRAY_SIZE(resource_table); i++)
    {
        stat(resource_table[i].name, &file_stat);
        if(file_stat.st_size != resource_table[i].size)
        {
            rt_kprintf("[INFO] download %s\r\n", resource_table[i].name);
            http_down(resource_table[i].name, resource_table[i].url);
        }
    }

    return RT_EOK;
}

