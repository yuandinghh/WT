#ifndef _LCD12864_H
#define _LCD12864_H

#include "sys.h"
#include "delay.h"

#define RS PGout(13)
#define RW PGout(14)
#define EN PGout(15)
#define PSB PBout(3)
#define RES PGout(4)

//PC0~7,作为数据线
#define DataPort(x) GPIOC->ODR=(GPIOC->ODR&0xff00)|(x&0x00ff); //输出

extern u8 num1632[11][64];

void LCD12864_init(void);
void write_com(u8 cmd);
void write_data(u8 dat);
void lcm_w_word(u8 *s);
void lcm_w_test(u8 i,u8 word);
void lcm_clr(void); 
void lcm_clr2(void);   
void write1616GDRAM(u8 x,u8 y,u8 sign,u8 *bmp); 
void set1616pic(u8 x,u8 y,u8 sign,u8 tt);         
void write1632GDRAM(u8 x,u8 y,u8 *bmp); 
void init_12864_GDRAM(void);
void Clean_12864_GDRAM(void);

#endif
