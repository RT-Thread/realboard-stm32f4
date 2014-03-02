/*
 * File      : ethernetif.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2010, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-07-07     Bernard      fix send mail to mailbox issue.
 * 2011-12-11     aozima       list_if and set_if support multiple interface.
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include <rtthread.h>

#include "lwip/debug.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"

#include "netif/etharp.h"
#include "netif/ethernetif.h"

/* eth rx/tx thread */
static struct rt_mailbox eth_rx_thread_mb;
static struct rt_thread eth_rx_thread;
#ifndef RT_LWIP_ETHTHREAD_PRIORITY
#define RT_ETHERNETIF_THREAD_PREORITY	0x90
static char eth_rx_thread_mb_pool[48 * 4];
static char eth_rx_thread_stack[1024];
#else
#define RT_ETHERNETIF_THREAD_PREORITY	RT_LWIP_ETHTHREAD_PRIORITY
static char eth_rx_thread_mb_pool[RT_LWIP_ETHTHREAD_MBOX_SIZE * 4];
static char eth_rx_thread_stack[RT_LWIP_ETHTHREAD_STACKSIZE];
#endif

struct eth_tx_msg
{
	struct netif 	*netif;
	struct pbuf 	*buf;
};
static struct rt_mailbox eth_tx_thread_mb;
static struct rt_thread eth_tx_thread;
#ifndef RT_LWIP_ETHTHREAD_PRIORITY
static char eth_tx_thread_mb_pool[32 * 4];
static char eth_tx_thread_stack[512];
#else
static char eth_tx_thread_mb_pool[RT_LWIP_ETHTHREAD_MBOX_SIZE * 4];
static char eth_tx_thread_stack[RT_LWIP_ETHTHREAD_STACKSIZE];
#endif

err_t ethernetif_linkoutput(struct netif *netif, struct pbuf *p)
{
	struct eth_tx_msg msg;
	struct eth_device* enetif;

	enetif = (struct eth_device*)netif->state;

	/* send a message to eth tx thread */
	msg.netif = netif;
	msg.buf   = p;
	if (rt_mb_send(&eth_tx_thread_mb, (rt_uint32_t) &msg) == RT_EOK)
	{
		/* waiting for ack */
		rt_sem_take(&(enetif->tx_ack), RT_WAITING_FOREVER);
	}

	return ERR_OK;
}

/* ethernetif APIs */
rt_err_t eth_device_init(struct eth_device* dev, const char* name)
{
	struct netif* netif;

	netif = (struct netif*) rt_malloc (sizeof(struct netif));
	if (netif == RT_NULL)
	{
		rt_kprintf("malloc netif failed\n");
		return -RT_ERROR;
	}
	rt_memset(netif, 0, sizeof(struct netif));

	/* set netif */
	dev->netif = netif;
	/* register to rt-thread device manager */
	rt_device_register(&(dev->parent), name, RT_DEVICE_FLAG_RDWR);
	dev->parent.type = RT_Device_Class_NetIf;
	rt_sem_init(&(dev->tx_ack), name, 0, RT_IPC_FLAG_FIFO);

	/* set name */
	netif->name[0] = name[0];
	netif->name[1] = name[1];

	/* set hw address to 6 */
	netif->hwaddr_len	= 6;
	/* maximum transfer unit */
	netif->mtu			= ETHERNET_MTU;
	/* broadcast capability */
	netif->flags		= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
	
#if LWIP_IGMP
	/* igmp support */
	netif->flags |= NETIF_FLAG_IGMP;
#endif

	/* get hardware address */
	rt_device_control(&(dev->parent), NIOCTL_GADDR, netif->hwaddr);

	/* set output */
	netif->output		= etharp_output;
	netif->linkoutput	= ethernetif_linkoutput;

	return RT_EOK;
}

/* ethernet buffer */
void eth_tx_thread_entry(void* parameter)
{
	struct eth_tx_msg* msg;

	while (1)
	{
		if (rt_mb_recv(&eth_tx_thread_mb, (rt_uint32_t*)&msg, RT_WAITING_FOREVER) == RT_EOK)
 		{
			struct eth_device* enetif;

			RT_ASSERT(msg->netif != RT_NULL);
			RT_ASSERT(msg->buf   != RT_NULL);

			enetif = (struct eth_device*)msg->netif->state;
			if (enetif != RT_NULL)
			{
				/* call driver's interface */
				if (enetif->eth_tx(&(enetif->parent), msg->buf) != RT_EOK)
				{
					rt_kprintf("transmit eth packet failed\n");
				}
			}

			/* send ack */
			rt_sem_release(&(enetif->tx_ack));
		}
	}
}

/* ethernet buffer */
void eth_rx_thread_entry(void* parameter)
{
	struct eth_device* device;

	while (1)
	{
		if (rt_mb_recv(&eth_rx_thread_mb, (rt_uint32_t*)&device, RT_WAITING_FOREVER) == RT_EOK)
 		{
			struct pbuf *p;

			/* check link status */
			if (device->link_changed)
			{
				int status;
				rt_uint32_t level;

				level = rt_hw_interrupt_disable();
				status = device->link_status;
				device->link_changed = 0x00;
				rt_hw_interrupt_enable(level);

				if (status)
					netifapi_netif_set_link_up(device->netif);
				else
					netifapi_netif_set_link_down(device->netif);
			}

			/* receive all of buffer */
			while (1)
			{
				p = device->eth_rx(&(device->parent));
				if (p != RT_NULL)
				{
					/* notify to upper layer */
					if( device->netif->input(p, device->netif) != ERR_OK )
					{
						LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: Input error\n"));
       					pbuf_free(p);
       					p = NULL;
					}
				}
				else break;
			}
		}
	}
}

rt_err_t eth_rx_ready(struct eth_device* dev)
{
	/* post message to ethernet thread */
	return rt_mb_send(&eth_rx_thread_mb, (rt_uint32_t)dev);
}

rt_err_t eth_device_ready(struct eth_device* dev)
{
	return eth_rx_ready(dev);
}

rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up)
{
	rt_uint32_t level;
	
	RT_ASSERT(dev != RT_NULL);

	level = rt_hw_interrupt_disable();
	dev->link_changed = 0x01;
	if (up == RT_TRUE)
		dev->link_status = 0x01;
	else
		dev->link_status = 0x00;
	rt_hw_interrupt_enable(level);

	/* post message to ethernet thread */
	return rt_mb_send(&eth_rx_thread_mb, (rt_uint32_t)dev);
}

rt_err_t eth_system_device_init()
{
	rt_err_t result = RT_EOK;

	/* init rx thread */
	/* init mailbox and create ethernet thread */
	result = rt_mb_init(&eth_rx_thread_mb, "erxmb",
		&eth_rx_thread_mb_pool[0], sizeof(eth_rx_thread_mb_pool)/4,
		RT_IPC_FLAG_FIFO);
	RT_ASSERT(result == RT_EOK);

	result = rt_thread_init(&eth_rx_thread, "erx", eth_rx_thread_entry, RT_NULL,
		&eth_rx_thread_stack[0], sizeof(eth_rx_thread_stack),
		RT_ETHERNETIF_THREAD_PREORITY, 16);
	RT_ASSERT(result == RT_EOK);

	result = rt_thread_startup(&eth_rx_thread);
	RT_ASSERT(result == RT_EOK);

	/* init tx thread */
	/* init mailbox and create ethernet thread */
	result = rt_mb_init(&eth_tx_thread_mb, "etxmb",
		&eth_tx_thread_mb_pool[0], sizeof(eth_tx_thread_mb_pool)/4,
		RT_IPC_FLAG_FIFO);
	RT_ASSERT(result == RT_EOK);

	result = rt_thread_init(&eth_tx_thread, "etx", eth_tx_thread_entry, RT_NULL,
		&eth_tx_thread_stack[0], sizeof(eth_tx_thread_stack),
		RT_ETHERNETIF_THREAD_PREORITY, 16);
	RT_ASSERT(result == RT_EOK);

	result = rt_thread_startup(&eth_tx_thread);
	RT_ASSERT(result == RT_EOK);

	return result;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
    struct ip_addr *ip;
    struct in_addr addr;
    struct netif * netif = netif_list;

    if(strlen(netif_name) > sizeof(netif->name))
    {
        rt_kprintf("network interface name too long!\r\n");
        return;
    }

    while(netif != RT_NULL)
    {
        if(strncmp(netif_name, netif->name, sizeof(netif->name)) == 0)
            break;

        netif = netif->next;
        if( netif == RT_NULL )
        {
            rt_kprintf("network interface: %s not found!\r\n", netif_name);
            return;
        }
    }

    ip = (struct ip_addr *)&addr;

    /* set ip address */
    if ((ip_addr != RT_NULL) && inet_aton(ip_addr, &addr))
    {
        netif_set_ipaddr(netif, ip);
    }

    /* set gateway address */
    if ((gw_addr != RT_NULL) && inet_aton(gw_addr, &addr))
    {
        netif_set_gw(netif, ip);
    }

    /* set netmask address */
    if ((nm_addr != RT_NULL) && inet_aton(nm_addr, &addr))
    {
        netif_set_netmask(netif, ip);
    }
}
FINSH_FUNCTION_EXPORT(set_if, set network interface address);

#if LWIP_DNS
#include <lwip/dns.h>
void set_dns(char* dns_server)
{
	struct in_addr addr;
	
	if ((dns_server != RT_NULL) && inet_aton(dns_server, &addr))
	{
		dns_setserver(0, (struct ip_addr *)&addr);
	}
}
FINSH_FUNCTION_EXPORT(set_dns, set DNS server address);
#endif

void list_if(void)
{
    rt_ubase_t index;
    struct netif * netif;

    netif = netif_list;

    while( netif != RT_NULL )
    {
        rt_kprintf("network interface: %c%c%s\n", netif->name[0], netif->name[1], (netif == netif_default)?" (Default)":"");
        rt_kprintf("MTU: %d\n", netif->mtu);
        rt_kprintf("MAC: ");
        for (index = 0; index < netif->hwaddr_len; index ++)
            rt_kprintf("%02x ", netif->hwaddr[index]);
        rt_kprintf("\nFLAGS:");
        if (netif->flags & NETIF_FLAG_UP) rt_kprintf(" UP");
        else rt_kprintf(" DOWN");
        if (netif->flags & NETIF_FLAG_LINK_UP) rt_kprintf(" LINK_UP");
        else rt_kprintf(" LINK_DOWN");
        if (netif->flags & NETIF_FLAG_DHCP) rt_kprintf(" DHCP");
        if (netif->flags & NETIF_FLAG_POINTTOPOINT) rt_kprintf(" PPP");
        if (netif->flags & NETIF_FLAG_ETHARP) rt_kprintf(" ETHARP");
        if (netif->flags & NETIF_FLAG_IGMP) rt_kprintf(" IGMP");
        rt_kprintf("\n");
        rt_kprintf("ip address: %s\n", inet_ntoa(*((struct in_addr*)&(netif->ip_addr))));
        rt_kprintf("gw address: %s\n", inet_ntoa(*((struct in_addr*)&(netif->gw))));
        rt_kprintf("net mask  : %s\n", inet_ntoa(*((struct in_addr*)&(netif->netmask))));
        rt_kprintf("\r\n");

        netif = netif->next;
    }

#if LWIP_DNS
    {
        struct ip_addr ip_addr;

        for(index=0; index<DNS_MAX_SERVERS; index++)
        {
            ip_addr = dns_getserver(index);
            rt_kprintf("dns server #%d: %s\n", index, inet_ntoa(*((struct in_addr*)&(ip_addr))));
        }
    }
#endif /**< #if LWIP_DNS */
}
FINSH_FUNCTION_EXPORT(list_if, list network interface information);

#include <lwip/tcp.h>
void list_tcps()
{
  struct tcp_pcb *pcb;
  extern struct tcp_pcb *tcp_active_pcbs;  
  extern union tcp_listen_pcbs_t tcp_listen_pcbs;
  extern struct tcp_pcb *tcp_tw_pcbs;
  extern const char *tcp_state_str[];

  rt_enter_critical();
  rt_kprintf("Active PCB states:\n");
  for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
  {
    rt_kprintf("%s:%d <==> %s:%d snd_nxt %d rcv_nxt %d ",
                       inet_ntoa(*((struct in_addr*)&(pcb->local_ip))), pcb->local_port, 
                       inet_ntoa(*((struct in_addr*)&(pcb->remote_ip))), pcb->remote_port,
                       pcb->snd_nxt, pcb->rcv_nxt);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }

  rt_kprintf("Listen PCB states:\n");
  for(pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next)
  {
    rt_kprintf("local port %d ", pcb->local_port);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }
  
  rt_kprintf("TIME-WAIT PCB states:\n");
  for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
  {
    rt_kprintf("%s:%d <==> %s:%d snd_nxt %d rcv_nxt %d ",
                       inet_ntoa(*((struct in_addr*)&(pcb->local_ip))), pcb->local_port, 
                       inet_ntoa(*((struct in_addr*)&(pcb->remote_ip))), pcb->remote_port,
                       pcb->snd_nxt, pcb->rcv_nxt);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }
  rt_exit_critical();
}
FINSH_FUNCTION_EXPORT(list_tcps, list all of tcp pcb);

#endif
