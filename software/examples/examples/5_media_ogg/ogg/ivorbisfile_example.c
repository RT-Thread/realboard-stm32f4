/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: simple example decoder using vorbisidec

 ********************************************************************/

/* Takes a vorbis bitstream from stdin and writes raw stereo PCM to
   stdout using vorbisfile. Using vorbisfile is much simpler than
   dealing with libvorbis. */

#include "ivorbiscodec.h"
#include "ivorbisfile.h"


#include <inttypes.h>
#include <rtthread.h>
#include "codec.h"
#include <dfs_posix.h>

char pcmout1[4096]; /* take 4k out of the data segment, not the stack */
char pcmout2[4096]; /* take 4k out of the data segment, not the stack */

static struct rt_semaphore ogg_sem;

static rt_err_t flac_decoder_tx_done(rt_device_t dev, void *buffer)
{
    /* release memory block */
	rt_sem_release(&ogg_sem);

	return RT_EOK;
}


int ogg(char * path)
{

	OggVorbis_File vf;
	int eof=0;
	int current_section;
	int fd,i=0;
	/* audio device */
	rt_device_t snd_device;
	
	extern void vol(uint16_t v) ;
	vol(50); 
	
	if (rt_sem_init(&ogg_sem, "ogg_sem", 2, RT_IPC_FLAG_FIFO) != RT_EOK)
			rt_kprintf("init ogg_sem semaphore failed\n");

    fd = open(path,0,0);

    if(ov_open(fd, &vf, NULL, 0) < 0) {
      rt_kprintf("Input does not appear to be an Ogg bitstream.\n");
	  return -1;
    }

	  	/* open audio device */
	snd_device = rt_device_find("snd");
	if (snd_device != RT_NULL)
	{
		//设置回调函数,当DMA传输完毕时,会执行ogg_decoder_tx_done
		rt_device_set_tx_complete(snd_device, flac_decoder_tx_done);
		rt_device_open(snd_device, RT_DEVICE_OFLAG_WRONLY);
	}


  /* Throw the comments plus a few lines about the bitstream we're
     decoding */
  {
    vorbis_comment *pov_comment;
	char **ptr;
	vorbis_info *vi;

	pov_comment 	=(vorbis_comment *)ov_comment(&vf,-1);
    ptr= pov_comment->user_comments;

    vi=(vorbis_info *)ov_info(&vf,-1);
    while(*ptr){
      rt_kprintf("\n%s\n",*ptr);
      ++ptr;
    }
    rt_kprintf("\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    rt_kprintf("\nDecoded length: %ld samples\n",
	    (long)ov_pcm_total(&vf,-1));
    rt_kprintf("Encoded by: %s\n\n",pov_comment->vendor);

	rt_device_control(snd_device, CODEC_CMD_SAMPLERATE, (void *)vi->rate);

  }

  
  while(!eof)
  {
    rt_sem_take(&ogg_sem, RT_WAITING_FOREVER);
	if(i++ & 1)
	{

	    long ret=ov_read(&vf,pcmout1,sizeof(pcmout1),&current_section);
	    if (ret == 0) {
	      /* EOF */
	      eof=1;
	    } else if (ret < 0) {
	      /* error in the stream.  Not a problem, just reporting it in
		 case we (the app) cares.  In this case, we don't. */
	    } else {
	      /* we don't bother dealing with sample rate changes, etc, but
		 you'll have to*/
	    //  fwrite(pcmout,1,ret,stdout);
			rt_device_write(snd_device, 0, (int16_t *)pcmout1,  ret);

	    }
	}
	else
	{
	 	long ret=ov_read(&vf,pcmout2,sizeof(pcmout2),&current_section);
	    if (ret == 0) {
	      /* EOF */
	      eof=1;
	    } else if (ret < 0) {
	      /* error in the stream.  Not a problem, just reporting it in
		 case we (the app) cares.  In this case, we don't. */
	    } else {
	      /* we don't bother dealing with sample rate changes, etc, but
		 you'll have to*/
	    //  fwrite(pcmout,1,ret,stdout);
		    rt_device_write(snd_device, 0, (int16_t *)pcmout2,  ret);

	    }

	
	}


  }

  /* cleanup */
  ov_clear(&vf);
    
  rt_kprintf("Done.\n");
  return(0);
}

#include "finsh.h"
FINSH_FUNCTION_EXPORT(ogg, int ogg(char * path));

