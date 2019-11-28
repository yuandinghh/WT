/**************************************************************************************
*	2019-4-15 温度控制   袁丁				***/
#include "REG52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include "XPT2046.h"
#include "I2C.h"
#define TIMESEC 1000   //定时显示 当前温度
typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;  
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))	  //取字 高8位
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))	   //取 低8位
#define ON   0    //点亮led灯
#define OFF   1   //关led灯
 sbit keyincadd = P1^0;	 //定义P10口是	增加温度的 加 +
sbit keyincsub = P1^1;	 //	 	减温度的 减 -
sbit keysubadd = P1^2;	 //定义P10口是	增加温度的 加 +
sbit keysubsub = P1^3;	 //	 	减温度的 减 -
sbit keyfun =    P1^4;	 //	 功能键

sbit led=P2^7;	 //定义P20口是led
sbit led2=P2^6;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4 ;
u8 data   time100ms;	 //延时 100ms
u8 data	currenttemp;   //当前温度
u8 data	setaddtemp;   //设置的增加（关闭的温度） 温度
u8 data	setsubtemp;   //设置的减少（起始打开电器）温度
bdata  tfun ;      // 功能键 +   几个标志位   00000XXX  ;XXX 为功能键。 设置位字节
sbit relay=tfun^4 ;		//当前 继电器开状态 

u8  data  timei	; //临时变量
u16 data timesec ;  //1 秒 延时
u16 data  temp;		//当前温度
u8  data  disp[8] ;		 //当前 温度数据
u8 code smgduan[17]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,
						0x5e,0x79,0x71,0x40};   //16 为  - 

/***函 数 名         : delay	   1ms		  ***/
void delay()
{
   	   time100ms = 3;
	   while(1) {
	   	if (  time100ms == 0)  return;
		}
}

/** 函数名   :DigDisplay()  * 函数功能		 :数码管显示函数   ****/
void DigDisplay()  {
    u8 i;
	timei =  tfun&0x0f;
//	if ( timei != 0 ) return ;
	    for(i=0; i<8; i++)
	    {
	       switch(i) {	 //位选，选择点亮的数码管，
		        case(0):
		            LSA=0;  LSB=0; LSC=0;	              break;//显示第0位
		        case(1):
		            LSA=1; LSB=0;    LSC=0;	              break;//显示第1位
		        case(2):
		            LSA=0;     LSB=1;  LSC=0;             break;//显示第2位
		        case(3):
		            LSA=1;	 LSB=1;  LSC=0;	              break;//显示第3位
				case(4):
					LSA=0;LSB=0;LSC=1; break;//显示第4位
				case(5):
					LSA=1;LSB=0;LSC=1; break;//显示第5位
				case(6):
					LSA=0;LSB=1;LSC=1; break;//显示第6位
				case(7):
					LSA=1;LSB=1;LSC=1; break;//显示第7位	
	        }
	        P0=disp[i];//发送数据
	        delay(); //间隔一段时间扫描
	    //    P0=0x00;//消隐
	    }
}
 /*******************************************************************************
* 函 数 名         : void Timer0() interrupt 1
* 函数功能		   : 定时器0中断函数
*******************************************************************************/
void Timer0() interrupt 1	{
    static u16 ini;
    TH0=0XFC;	//给定时器赋初值，定时1ms
    TL0=0X18;
    ini++;
    if(ini==1000)
    {
        ini=0;
      }
    if ( time100ms != 0 ) {
        time100ms--;
    }
}
 void datapros()	 {
//	static u8 i;
//	if(i==10)	 	{
//		i=0;
		temp = Read_AD_Data(0x94);		//   AIN0 电位器
//	}
//	i++;
	timei =  tfun&0x0f;
	if ( timei != 0 ) return ;	   		 //不显示温度  进入功能键状态
	disp[2]=smgduan[temp/1000];			//千位
	timei = smgduan[temp%1000/100];	
	disp[3]=timei|0x80;;		//百位
 	disp[4]=smgduan[temp%1000%100/10];//个位
	disp[5]=smgduan[temp%1000%100%10];		
}
void disp0(u8 fun) {
  	disp[0]=smgduan[0];//千位
	disp[1]=smgduan[0];//百位
  	disp[2]=smgduan[15];//千位
	disp[3]=smgduan[16];//百位
	disp[4]=smgduan[fun];//个位
	disp[5]=smgduan[0xff];
	disp[6]=smgduan[0];//千位
	disp[7]=smgduan[0];//百位	
		
 }
u8 gettemp() {
		timei = (temp/1000);			//换算温度    
		timei = timei*10 + (temp%1000/100);	
		return 	timei;
}
/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
*******************************************************************************/
void main()
{		//	timesec	= 0xaa55;  		timei= BYTE0(timesec) ;		timei= BYTE1(timesec) ;
    TMOD|=0X01;//Timer0Init()    选择为定时器0模式，工作方式1，仅用TR0打开启动。
    TH0=0XFC;	//给定时器赋初值，定时1ms
    TL0=0X18;
    ET0=1;//打开定时器0中断允许
    EA=1;//打开总中断
    TR0=1;//打开定时器
	timei = At24c02Read(0);	  currenttemp = At24c02Read(1);
	if ( timei != 0x55 && currenttemp != 0XAA) {	   //是否已经 初次运行
		setaddtemp =25;  //关闭温度
		At24c02Write(0,0x55); 	At24c02Write(1,0xAA); At24c02Write(2,25); 
     	 At24c02Write(3,18);   	 At24c02Write(4,0);  
	 	setsubtemp =18	;	  //开机温度
 	}
	else {		 //已经初始化
		setaddtemp =  At24c02Read(2);	setsubtemp =  At24c02Read(3);
		tfun =  At24c02Read(4);
	}
	disp[0]=smgduan[setaddtemp/10];//10位
	disp[1]=smgduan[setaddtemp%10%10];//个位位
    disp[6]=smgduan[setsubtemp/10];//10位
	disp[7]=smgduan[setsubtemp%10%10];//个位位

      while(1)   {	   //主循环
	   datapros();	 	DigDisplay() ; 	    //显示当前温度
	   	timei =  tfun&0x0f;
     	if ( timei != 0 ) goto jmpkey ;	  //功能键1-2  非 0 
	 
	   if(keyincadd==0)	 {	  //检测按键增加按下
  	        delay();   //消除抖动 一般大约10ms
	        if(keyincadd==0)	 //再次判断按键是否按下
	        {
	          setaddtemp++;
			  if (setaddtemp > 99 )  { setaddtemp--; }
			  disp[0]=smgduan[setaddtemp/10];//10位
			  disp[1]=smgduan[setaddtemp%10%10];//个位
	        }
       	 while(!keyincadd);	 //检测按键是否松开
		  At24c02Write(2,setaddtemp); 
		  	timei=gettemp(); 
			if ( timei < setaddtemp )  	led = ON;	
       }
	  	if(keyincsub==0)	 {	  //检测按键--按下	+++++++------------
  	        delay(); 
	        if(keyincsub==0)  {	 //再次判断按键是否按下
			  if ( setaddtemp != 0 )  setaddtemp--;
			  disp[0]=smgduan[setaddtemp/10];//10位
			  disp[1]=smgduan[setaddtemp%10%10];//个位位
	        }
     	   while(!keyincsub);	 //检测按键是否松开
		   At24c02Write(2,setaddtemp); 
		   	timei=gettemp();
			if ( timei > setaddtemp )  	led = OFF;
       }
 	   	   if(keysubadd==0)	 {	  //SUB 检测-- 增加按  -----------++++++++++
  	        delay();  
	        if(keysubadd == 0)	 //再次判断按键是否按下
	        {
	          setsubtemp++;
			  if (setsubtemp > 99 )  { setsubtemp--; }
			  disp[6]=smgduan[setsubtemp/10];//10位
			  disp[7]=smgduan[setsubtemp%10%10];//个位位
	        }
       	 while(!keysubadd);	 //检测按键是否松开
		 At24c02Write(3,setsubtemp); 
		 timei = gettemp(); //		 led = ON;
	     if ( timei < setsubtemp ) 	led = OFF;
       }
	  	if(keysubsub==0)	 {	  //检测按键--按下 --------   -------
  	        delay(); 
	        if(keysubsub==0)  {	 //再次判断按键是否按下
			  if ( setsubtemp != 0 )  setsubtemp--;
			  disp[6]=smgduan[setsubtemp/10];//10位
			  disp[7]=smgduan[setsubtemp%10%10];//个位位
	        }
     	   while(!keysubsub);	 //检测按键是否松开
		   At24c02Write(3,setsubtemp); 
//		  timei=gettemp(); //		 led = ON;
//	     if ( timei > setsubtemp ) 	led = ON;
       }
 jmpkey:
   	if(keyfun==0)	 {	  //检测按键功能键按下
  	        delay(); 
	    if(keyfun==0)  {
			 tfun++;	
		  timei =  tfun&0x0f;
	      switch (timei)
			{
				case 1: relay=0; 	disp0(timei);	  //关继电器
						break;
				case 2:	 relay=1;  disp0(timei);
					break;

				default:  tfun = tfun&0xf0;
					  disp[0]=smgduan[setaddtemp/10];//10位
					  disp[1]=smgduan[setaddtemp%10%10];//个位位
					  disp[6]=smgduan[setsubtemp/10];//10位
					  disp[7]=smgduan[setsubtemp%10%10];//个位位
					break;
			}
         }
     	   while(!keyfun);	 //检测按键是否松开
		   At24c02Write(4,tfun); 
       }
	   	
    	timei = gettemp();			//换算温度    
		currenttemp == tfun&0x0f ;
	   if (	 currenttemp  == 0 ) {
			if( (timei < setsubtemp) ) {   //打开继电器
			      led=ON;
			} 
			else if (timei > setaddtemp) {
				led = OFF;
			} 
 		}
		timei =  tfun&0x0f;	  //功能 1-2 
		led2=OFF;
		if (  timei != 0) 	{ led2=ON;  led =OFF;  }
		if (  timei == 2) 	 led =ON;

     }
}

