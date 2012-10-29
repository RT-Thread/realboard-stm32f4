#include <rtthread.h>
#include <dfs_posix.h>
#include <mp3/pub/mp3dec.h>
#include <string.h>

#include "board.h"
#include "netbuffer.h"
#include "codec_wm8978_i2c.h"

#define MP3_AUDIO_BUF_SZ    (5 * 1024)
#ifndef MIN
#define MIN(x, y)			((x) < (y)? (x) : (y))
#endif

rt_uint8_t mp3_fd_buffer[MP3_AUDIO_BUF_SZ];
int current_sample_rate = 0;

struct mp3_decoder
{
    /* mp3 information */
    HMP3Decoder decoder;
    MP3FrameInfo frame_info;
    rt_uint32_t frames;

    /* mp3 file descriptor */
	rt_size_t (*fetch_data)(void* parameter, rt_uint8_t *buffer, rt_size_t length);
	void* fetch_parameter;

    /* mp3 read session */
    rt_uint8_t *read_buffer, *read_ptr;
    rt_int32_t  read_offset;
    rt_uint32_t bytes_left, bytes_left_before_decoding;

	/* audio device */
	rt_device_t snd_device;
};

static rt_err_t mp3_decoder_tx_done(rt_device_t dev, void *buffer)
{
	/* release memory block */
	sbuf_release(buffer);

	return RT_EOK;
}

void mp3_decoder_init(struct mp3_decoder* decoder)
{
    RT_ASSERT(decoder != RT_NULL);

	/* init read session */
	decoder->read_ptr = RT_NULL;
	decoder->bytes_left_before_decoding = decoder->bytes_left = 0;
	decoder->frames = 0;

    // decoder->read_buffer = rt_malloc(MP3_AUDIO_BUF_SZ);
    decoder->read_buffer = &mp3_fd_buffer[0];
	if (decoder->read_buffer == RT_NULL) return;

    decoder->decoder = MP3InitDecoder();

	/* open audio device */
	decoder->snd_device = rt_device_find("snd");
	if (decoder->snd_device != RT_NULL)
	{
		/* set tx complete call back function */
		rt_device_set_tx_complete(decoder->snd_device, mp3_decoder_tx_done);
		rt_device_open(decoder->snd_device, RT_DEVICE_OFLAG_WRONLY);
	}
}

void mp3_decoder_detach(struct mp3_decoder* decoder)
{
    RT_ASSERT(decoder != RT_NULL);

	/* close audio device */
	if (decoder->snd_device != RT_NULL)
		rt_device_close(decoder->snd_device);

	/* release mp3 decoder */
    MP3FreeDecoder(decoder->decoder);
}

struct mp3_decoder* mp3_decoder_create()
{
    struct mp3_decoder* decoder;

	/* allocate object */
    decoder = (struct mp3_decoder*) rt_malloc (sizeof(struct mp3_decoder));
    if (decoder != RT_NULL)
    {
        mp3_decoder_init(decoder);
    }

    return decoder;
}

void mp3_decoder_delete(struct mp3_decoder* decoder)
{
    RT_ASSERT(decoder != RT_NULL);

	/* de-init mp3 decoder object */
	mp3_decoder_detach(decoder);
	/* release this object */
    rt_free(decoder);
}

rt_uint32_t current_offset = 0;
static rt_int32_t mp3_decoder_fill_buffer(struct mp3_decoder* decoder)
{
	rt_size_t bytes_read;
	rt_size_t bytes_to_read;

	// rt_kprintf("left: %d. refilling inbuffer...\n", decoder->bytes_left);
	if (decoder->bytes_left > 0)
	{
		// better: move unused rest of buffer to the start
		rt_memmove(decoder->read_buffer, decoder->read_ptr, decoder->bytes_left);
	}

	bytes_to_read = (MP3_AUDIO_BUF_SZ - decoder->bytes_left) & ~(512 - 1);

	bytes_read = decoder->fetch_data(decoder->fetch_parameter,
		(rt_uint8_t *)(decoder->read_buffer + decoder->bytes_left),
        bytes_to_read);

	if (bytes_read != 0)
	{
		decoder->read_ptr = decoder->read_buffer;
		decoder->read_offset = 0;
		decoder->bytes_left = decoder->bytes_left + bytes_read;
		return 0;
	}
	else
	{
		rt_kprintf("can't read more data\n");
		return -1;
	}
}

int mp3_decoder_run(struct mp3_decoder* decoder)
{
	int err;
	rt_uint16_t* buffer;
	rt_uint32_t  delta;

    RT_ASSERT(decoder != RT_NULL);

	if ((decoder->read_ptr == RT_NULL) || decoder->bytes_left < 2*MAINBUF_SIZE)
	{
		if(mp3_decoder_fill_buffer(decoder) != 0)
			return -1;
	}

	// rt_kprintf("read offset: 0x%08x\n", decoder->read_ptr - decoder->read_buffer);
	decoder->read_offset = MP3FindSyncWord(decoder->read_ptr, decoder->bytes_left);
	if (decoder->read_offset < 0)
	{
		/* discard this data */
		rt_kprintf("outof sync, byte left: %d\n", decoder->bytes_left);

		decoder->bytes_left = 0;
		return 0;
	}

	decoder->read_ptr += decoder->read_offset;
	delta = decoder->read_offset;
	decoder->bytes_left -= decoder->read_offset;
	if (decoder->bytes_left < 1024)
	{
		/* fill more data */
		if(mp3_decoder_fill_buffer(decoder) != 0)
			return -1;
	}

    /* get a decoder buffer */
    buffer = (rt_uint16_t*)sbuf_alloc();
	decoder->bytes_left_before_decoding = decoder->bytes_left;

	err = MP3Decode(decoder->decoder, &decoder->read_ptr,
        (int*)&decoder->bytes_left, (short*)buffer, 0);
	delta += (decoder->bytes_left_before_decoding - decoder->bytes_left);

	current_offset += delta;

	decoder->frames++;

	if (err != ERR_MP3_NONE)
	{
		switch (err)
		{
		case ERR_MP3_INDATA_UNDERFLOW:
			rt_kprintf("ERR_MP3_INDATA_UNDERFLOW\n");
			decoder->bytes_left = 0;
			if(mp3_decoder_fill_buffer(decoder) != 0)
			{
				/* release this memory block */
				sbuf_release(buffer);
				return -1;
			}
			break;

		case ERR_MP3_MAINDATA_UNDERFLOW:
			/* do nothing - next call to decode will provide more mainData */
			rt_kprintf("ERR_MP3_MAINDATA_UNDERFLOW\n");
			break;

		default:
			rt_kprintf("unknown error: %d, left: %d\n", err, decoder->bytes_left);

			// skip this frame
			if (decoder->bytes_left > 0)
			{
				decoder->bytes_left --;
				decoder->read_ptr ++;
			}
			else
			{
				// TODO
				RT_ASSERT(0);
			}
			break;
		}

		/* release this memory block */
		sbuf_release(buffer);
	}
	else
	{
		int outputSamps;
		/* no error */
		MP3GetLastFrameInfo(decoder->decoder, &decoder->frame_info);

        /* set sample rate */
		if (decoder->frame_info.samprate != current_sample_rate)
		{
			current_sample_rate = decoder->frame_info.samprate;
			rt_device_control(decoder->snd_device, CODEC_CMD_SAMPLERATE, &current_sample_rate);
		}

		/* write to sound device */
		outputSamps = decoder->frame_info.outputSamps;
		if (outputSamps > 0)
		{
			if (decoder->frame_info.nChans == 1)
			{
				int i;
				for (i = outputSamps - 1; i >= 0; i--)
				{
					buffer[i * 2] = buffer[i];
					buffer[i * 2 + 1] = buffer[i];
				}
				outputSamps *= 2;
			}

			rt_device_write(decoder->snd_device, 0, buffer, outputSamps * sizeof(rt_uint16_t));
		}
		else
		{
			/* no output */
			sbuf_release(buffer);
		}
	}

	return 0;
}

#include <finsh.h>
rt_size_t fd_fetch(void* parameter, rt_uint8_t *buffer, rt_size_t length)
{
	int fd = (int)parameter;
	int read_bytes;

	read_bytes = read(fd, (char*)buffer, length);
	if (read_bytes <= 0) return 0;

	return read_bytes;
}

void mp3(char* filename)
{
	int fd;
	struct mp3_decoder* decoder;
	
	fd = open(filename, O_RDONLY, 0);
	if (fd >= 0)
	{
		decoder = mp3_decoder_create();
		if (decoder != RT_NULL)
		{
			decoder->fetch_data = fd_fetch;
			decoder->fetch_parameter = (void*)fd;

			current_offset = 0;
			while (mp3_decoder_run(decoder) != -1);

			/* delete decoder object */
			mp3_decoder_delete(decoder);
		}
		close(fd);
	}
}
FINSH_FUNCTION_EXPORT(mp3, mp3 decode test);

#if STM32_EXT_SRAM
/* http mp3 */
#include "http.h"
static rt_size_t http_fetch(rt_uint8_t* ptr, rt_size_t len, void* parameter)
{
	struct http_session* session = (struct http_session*)parameter;
	RT_ASSERT(session != RT_NULL);

	return http_session_read(session, ptr, len);
}

static void http_close(void* parameter)
{
	struct http_session* session = (struct http_session*)parameter;
	RT_ASSERT(session != RT_NULL);

	http_session_close(session);
}

rt_size_t http_data_fetch(void* parameter, rt_uint8_t *buffer, rt_size_t length)
{
	return net_buf_read(buffer, length);
}

void http_mp3(char* url)
{
    struct http_session* session;
	struct mp3_decoder* decoder;
// 	extern rt_bool_t is_playing;
// 	
// 	is_playing = RT_TRUE;

	session = http_session_open(url);
	if (session != RT_NULL)
	{
		/* start a job to netbuf worker */
		if (net_buf_start_job(http_fetch, http_close, (void*)session) == 0)
		{
			decoder = mp3_decoder_create();
			if (decoder != RT_NULL)
			{
				decoder->fetch_data = http_data_fetch;
				decoder->fetch_parameter = RT_NULL;

				current_offset = 0;
				while (mp3_decoder_run(decoder) != -1);

				/* delete decoder object */
				mp3_decoder_delete(decoder);
			}
			session = RT_NULL;
		}
		else
		{
			/* start job failed, close session */
			http_session_close(session);
		}
	}
}
FINSH_FUNCTION_EXPORT(http_mp3, http mp3 decode test);
/* ice mp3 */
static rt_size_t ice_fetch(rt_uint8_t* ptr, rt_size_t len, void* parameter)
{
	struct shoutcast_session* session = (struct shoutcast_session*)parameter;
	RT_ASSERT(session != RT_NULL);

	return shoutcast_session_read(session, ptr, len);
}

static void ice_close(void* parameter)
{
	struct shoutcast_session* session = (struct shoutcast_session*)parameter;
	RT_ASSERT(session != RT_NULL);

	shoutcast_session_close(session);
}

rt_size_t ice_data_fetch(void* parameter, rt_uint8_t *buffer, rt_size_t length)
{
	return net_buf_read(buffer, length);
}

void ice_mp3(const char* url, const char* station)
{
    struct shoutcast_session* session;
	struct mp3_decoder* decoder;
	extern rt_bool_t is_playing;

	//is_playing = RT_TRUE;

	//player_notify_info("¨¢??¨®¦Ì?¨¬¡§...");
	session = shoutcast_session_open(url);
	if (session != RT_NULL)
	{
		//player_set_title(station);
		//player_notify_info("¨¢??¨®3¨¦1|¡ê??o3??D...");
		/* start a job to netbuf worker */
		if (net_buf_start_job(ice_fetch, ice_close, (void*)session) == 0)
		{
			decoder = mp3_decoder_create();
			if (decoder != RT_NULL)
			{
				decoder->fetch_data = ice_data_fetch;
				decoder->fetch_parameter = RT_NULL;

				current_offset = 0;
				while (mp3_decoder_run(decoder) != -1);

				/* delete decoder object */
				mp3_decoder_delete(decoder);
			}
			session = RT_NULL;
		}
		else
		{
			/* start a job failed, close session */
			shoutcast_session_close(session);
		}
	}
}
FINSH_FUNCTION_EXPORT(ice_mp3, shoutcast mp3 decode test);
/* douban radio */
#include "douban_radio.h"
static rt_size_t doubarn_radio_fetch(rt_uint8_t* ptr, rt_size_t len, void* parameter)
{
	struct douban_radio* douban = (struct douban_radio*)parameter;
	RT_ASSERT(douban != RT_NULL);

	return douban_radio_read(douban, ptr, len);
}

static void doubarn_radio_connection_close(void* parameter)
{
	struct douban_radio* douban = (struct douban_radio*)parameter;
	RT_ASSERT(douban != RT_NULL);

	douban_radio_close(douban);
}

rt_size_t douban_radio_data_fetch(void* parameter, rt_uint8_t *buffer, rt_size_t length)
{
	return net_buf_read(buffer, length);
}

void douban_radio()
{
    struct douban_radio* douban;
	struct mp3_decoder* decoder;
	//extern rt_bool_t is_playing;

	//is_playing = RT_TRUE;

	//player_notify_info("¨¢??¨®?1¡ã¨º?D...");
	douban = douban_radio_open(1);
	if (douban != RT_NULL)
	{
		//player_set_title(station);
		//player_notify_info("¨¢??¨®3¨¦1|¡ê??o3??D...");
		/* start a job to netbuf worker */
		if (net_buf_start_job(doubarn_radio_fetch, 
			doubarn_radio_connection_close, (void*)douban) == 0)
		{
			decoder = mp3_decoder_create();
			if (decoder != RT_NULL)
			{
				decoder->fetch_data = douban_radio_data_fetch;
				decoder->fetch_parameter = RT_NULL;

				current_offset = 0;
				while (mp3_decoder_run(decoder) != -1);

				/* delete decoder object */
				mp3_decoder_delete(decoder);
			}
			douban = RT_NULL;
		}
		else
		{
			/* start a job failed, close session */
			douban_radio_close(douban);
			douban = RT_NULL;
		}
	}
}
FINSH_FUNCTION_EXPORT(douban_radio, douban radio test);
#endif
