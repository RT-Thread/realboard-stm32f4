/* snake.c */
/* author:master@aozima.cn */
/* update:Mar 5 2009 10:01 */

#include "snake.h"

snake_struct snake;

void snake_init(void) /* 蛇初始化 */
{
    unsigned char i;
    snake.flag = 0x00;

    snake.x[0] = 5;//(snake_romm_size_widht / 2); /* 设定第一次豆子出现的位置 */
    snake.y[0] = 5;//(snake_room_size_hight / 2); /* 默认为最中心             */

    snake.length_now = snake_length_init;     /* 初始化当前蛇长           */

    for(i=1;i<snake_length_init+1;i++){snake.y[i] = snake_length_init-i;}
    for(i=snake_room_size_hight;i>snake_length_init;i--){snake.y[i+1] = snake_room_size_hight;}

    for(i=0;i<snake_length_init;i++){snake.x[i+1] = 0;}
    for(;i<snake_room_size_widht;i++){snake.x[i+1] = snake_room_size_widht;}

    snake.addx = 0;
    snake.addy = 0;
}

void snake_run_step(void) /* 蛇运行一步 */
{
    unsigned char i;

    if((snake.addx ==0) & (snake.addy == 0) ){return;}

    /* 判断是否吃东西 */
    if( (snake.x[0] == snake.x[1] + snake.addx) && (snake.y[0] == snake.y[1] + snake.addy) )
    {
        snake.length_now++;                                          /* 蛇长+1             */
        if(snake.length_now == snake_length_max)                     /* 判断是否通关       */
        {
            snake.flag |= (1<<7);                                    /* 设置通关标致       */
            snake.addx = 0;snake.addy = 0;                           /* 如果通关就停止运行 */
        }
        else
        {
            snake.x[0] = snake.x[snake.length_now - 1];
            snake.y[0] = snake.y[snake.length_now - 1];
        }
    }
    for(i=snake.length_now;i>1;i--)
    {
        snake.x[i] = snake.x[i-1];
        snake.y[i] = snake.y[i-1];
    }
    snake.x[1] = snake.x[2] + snake.addx;
    snake.y[1] = snake.y[2] + snake.addy;


    /* 判断撞墙 */
    if( (snake.x[1] > (snake_room_size_widht - 1)) || (snake.y[1] > (snake_room_size_hight - 1)) )
    {
        snake.addx = 0;snake.addy = 0;
        snake.flag |= (1<<0);
    }

    /* 判断是否撞自己 */
    for(i=2;i<snake.length_now;i++)
    {
        if( (snake.x[1] == snake.x[i]) && (snake.y[1] == snake.y[i]) )
        {
            snake.addx = 0;snake.addy = 0;
            snake.flag |= (1<<1);
        }
    }
}

void snake_key(unsigned char key)
{
    if(snake.flag){return;}

    switch(key)
    {
        case 1 : if( (snake.y[1] + 1) != snake.y[2] ){ snake.addx = 0;snake.addy =  1;}break; /* up    */
        case 2 : if( (snake.y[1] - 1) != snake.y[2] ){ snake.addx = 0;snake.addy = -1;}break; /* down  */
        case 3 : if( (snake.x[1] - 1) != snake.x[2] ){ snake.addy = 0;snake.addx = -1;}break; /* left  */
        case 4 : if( (snake.x[1] + 1) != snake.x[2] ){ snake.addy = 0;snake.addx =  1;}break; /* right */
        default:break;
    }
}
/* end file */
