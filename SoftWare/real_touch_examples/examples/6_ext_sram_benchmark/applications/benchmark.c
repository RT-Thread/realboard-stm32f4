#include <rtthread.h>
#include <finsh.h>
#include "board.h"

#define BENCHMARK_SIZE	(1024 * 8)
#define BENCHMARK_LOOP	(4096)

#define TEST_SIZE_KB_S        ((rt_uint32_t) ((BENCHMARK_SIZE/1024)     \
                                              * BENCHMARK_LOOP          \
                                                * RT_TICK_PER_SECOND)   \
                               / tick)

rt_uint8_t benchmark_buffer[BENCHMARK_SIZE];

static rt_tick_t rt_tick_after(rt_tick_t start, rt_tick_t end)
{
    rt_tick_t after_tick;

    if(end >= start)
    {
        after_tick = end - start;
    }
    else
    {
        after_tick = RT_TICK_MAX - start + end;
    }

    return after_tick;
}

/* calculate speed */
static void calculate_speed_print(rt_uint32_t kbyte_s)
{
    rt_uint32_t m;

    m = kbyte_s/1024UL;
    if( m )
    {
        rt_kprintf("%3d.%02dMbyte/s", m, (kbyte_s%1024UL)*100/1024UL);
    }
    else
    {
        rt_kprintf("%3dKbyte/s", kbyte_s);
    }
}

void benchmark(void)
{
    rt_uint32_t tick;
    rt_uint32_t index, loop;

    rt_kprintf("BENCHMARK_SIZE:%ubyte BENCHMARK_LOOP:%u\r\n",
               BENCHMARK_SIZE, BENCHMARK_LOOP);
    rt_kprintf("---------- item ------------ tick -- speed -----\r\n");

    /* benchmark for 8bit access */
    {
        volatile rt_uint8_t *ptr;
        volatile rt_uint8_t tmp;

        /* write */
        ptr = (volatile rt_uint8_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                *ptr = (index & 0xff);
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [8bit]  write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint8_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                *ptr = (index & 0xff);
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [8bit]  write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        /* read */
        ptr = (volatile rt_uint8_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [8bit]  read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint8_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [8bit]  read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");
    }

    /* benchmark for 16bit access */
    {
        volatile rt_uint16_t *ptr;
        volatile rt_uint16_t tmp;

        /* write */
        ptr = (volatile rt_uint16_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP/2; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                *ptr = (index & 0xffff);
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [16bit] write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint16_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/2; index ++)
            {
                *ptr = (index & 0xffff);
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [16bit] write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        /* read */
        ptr = (volatile rt_uint16_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP/2; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [16bit] read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint16_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/2; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [16bit] read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");
    }

    /* benchmark for 32bit access */
    {
        volatile rt_uint32_t *ptr;
        volatile rt_uint32_t tmp;

        /* write */
        ptr = (volatile rt_uint32_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/4; index ++)
            {
                *ptr = index;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [32bit] write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint32_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/4; index ++)
            {
                *ptr = index;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [32bit] write : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        /* read */
        ptr = (volatile rt_uint32_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/4; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [32bit] read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (volatile rt_uint32_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            for (index = 0; index < BENCHMARK_SIZE/4; index ++)
            {
                tmp = *ptr;
            }
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [32bit] read  : %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");
    }

    /* benchmark for memset access */
    {
        rt_uint32_t *ptr;

        /* memory set */
        ptr = (rt_uint32_t*) benchmark_buffer;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            memset(ptr, 0xae, BENCHMARK_SIZE);
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("internal sram [memset] write: %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        ptr = (rt_uint32_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            memset(ptr, 0xae, BENCHMARK_SIZE);
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external sram [memset] write: %3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");

        /* memory copy */
        ptr = (rt_uint32_t*) STM32_EXT_SRAM_BEGIN;
        tick = rt_tick_get();
        for (loop = 0; loop < BENCHMARK_LOOP; loop ++)
        {
            memcpy(benchmark_buffer, ptr, BENCHMARK_SIZE);
        }
        tick = rt_tick_after(tick, rt_tick_get());
        rt_kprintf("external to internal [memcpy]:%3d ", tick);
        calculate_speed_print( TEST_SIZE_KB_S );
        rt_kprintf("\r\n");
    }
}
FINSH_FUNCTION_EXPORT(benchmark, an internal and external SRAM benchmark);
