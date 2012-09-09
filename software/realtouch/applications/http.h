#ifndef __HTTP_H__
#define __HTTP_H__

#include <rtthread.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

struct http_session
{
    char* user_agent;
	int   socket;

    /* size of http file */
    rt_size_t size;
    rt_off_t  position;
};

struct shoutcast_session
{
	int   socket;

	/* shoutcast name and bitrate */
	char* station_name;
	int   bitrate;

	/* size of meta data */
	rt_size_t metaint;
	rt_size_t current_meta_chunk;
};

struct http_session* http_session_open(const char* url);
rt_size_t http_session_read(struct http_session* session, rt_uint8_t *buffer, rt_size_t length);
rt_off_t http_session_seek(struct http_session* session, rt_off_t offset, int mode);
int http_session_close(struct http_session* session);

struct shoutcast_session* shoutcast_session_open(const char* url);
rt_size_t shoutcast_session_read(struct shoutcast_session* session, rt_uint8_t *buffer, rt_size_t length);
rt_off_t shoutcast_session_seek(struct shoutcast_session* session, rt_off_t offset, int mode);
int shoutcast_session_close(struct shoutcast_session* session);

int http_resolve_address(struct sockaddr_in *server, const char * url, char *host_addr, char** request);
int http_is_error_header(char *mime_buf);
int http_read_line( int socket, char * buffer, int size );

#endif
