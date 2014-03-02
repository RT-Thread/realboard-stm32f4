#include <rtthread.h>
#include <lwip/netdb.h> /* Ϊ�˽�������������Ҫ����netdb.hͷ�ļ� */
#include <lwip/sockets.h> /* ʹ��BSD socket����Ҫ����sockets.hͷ�ļ� */

void tcp_senddata(const char* url, int port, int length)
{
	struct hostent *host;
	int sock, err, result, timeout, index;
	struct sockaddr_in server_addr;
	rt_uint8_t *buffer_ptr;

	/* ͨ��������ڲ���url���host��ַ��������������������������� */
	host = gethostbyname(url);
	/* ����һ��socket��������SOCKET_STREAM��TCP���� */
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		/* ����socketʧ�� */
		rt_kprintf("Socket error\n");
		return;
	}

	/* �����ڴ� */
	buffer_ptr = rt_malloc(length);
	/* ���췢������ */
	for (index = 0; index < length; index ++)
		buffer_ptr[index] = index & 0xff;

	timeout = 100;
	/* ���÷��ͳ�ʱʱ��100ms */
	lwip_setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	/* ��ʼ��Ԥ���ӵķ���˵�ַ */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

	/* ���ӵ������ */
	err = connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	rt_kprintf("TCP thread connect error code: %d\n", err);

	while(1)
	{
		/* �������ݵ�sock���� */
		result = send(sock, buffer_ptr, length, MSG_DONTWAIT);
		if(result == -1) //���ݷ��ʹ�����
		{
			rt_kprintf("TCP thread send error: %d\n", result);
			lwip_close(sock);	//�ر����ӣ����´�������
			rt_thread_delay(10);
			if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
				rt_kprintf("TCP Socket error:%d\n",sock);
			err = connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
			rt_kprintf("TCP thread connect error code: %d\n", err);
		}
	}
}

#ifdef RT_USING_FINSH
#include <finsh.h>
/* ���tcpclient������finsh shell�� */
FINSH_FUNCTION_EXPORT(tcp_senddata, send a packet through tcp connection);
#endif

