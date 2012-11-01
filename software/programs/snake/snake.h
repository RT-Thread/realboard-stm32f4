/* snake.h */
/* author:master@aozima.cn */
/* update:Mar 5 2009 10:01 */

#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED

#define snake_length_max                  20  //最大蛇长
#define snake_length_init                 3   //初始化蛇长
#define snake_room_size_hight             8   //房子高 0-255 建议>8
#define snake_room_size_widht             8   //房子宽 0-255 建议>8

typedef struct
{
    unsigned char x[snake_length_max+1];       //蛇身X坐标值 下标为蛇身每一节的坐标
    unsigned char y[snake_length_max+1];       //同上,下标0为豆子的值
    unsigned char length_now;                  /* 当前蛇长 */
    signed   char addx;
    signed   char addy;
    unsigned char flag;                        //蛇当前标致
}snake_struct;

extern snake_struct snake;

void snake_init(void);                          /* init snake */
void snake_run_step(void);                      /* run a step */
void snake_key(unsigned char key);              /*            */

#endif // SNAKE_H_INCLUDED
