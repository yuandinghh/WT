//2019-11-27 8051造轮子  袁丁
#include "lcd.h"
#include "reg51.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include "RF2401.h"	
#include"ds1302.h"
#include <intrins.h>

uchar  sta,i;    //  状态变量
#define RX_DR  (sta & 0x40)  // 接收成功中断标志
#define TX_DS  (sta & 0x20)  // 发射成功中断标志
#define MAX_RT (sta & 0x10)  // 重发溢出中断标志
#define sbuflen	 50   //串行接受长度

bit sbufoverflow = 0 ;	  //keil定义位变量
u8 sbuf[sbuflen], sbufc=0;

sbit CE=P1^5;
sbit IRQ=P1^0;
sbit CSN=P1^4;
sbit MOSI=P1^2;
sbit MISO=P1^1;
sbit SCK=P1^3;
sbit led=P2^0;  sbit LED=P2^0;       //sbit LED=P0^0;
sbit led1=P2^1;	sbit led2=P2^2;  sbit led3=P2^3;
sbit k1 = P3^1 ;    sbit  k2=  P3^0; sbit  k3 = P3^2; sbit  k4 = P3^3;  //key
uchar code TX_Addr[]={0x34,0x43,0x10,0x10,0x01};
uchar code TX_Buffer[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
uchar RX_Buffer[RX_DATA_WITDH];

void _delay_us(uint x)	   {		 //延时 纳秒
 uint i,j;
 for (j=0;j<x;j++)
  for (i=0;i<12;i++);
}
void _delay_ms(uint x)
{
 uint i,j;
 for (j=0;j<x;j++)
  for (i=0;i<120;i++);
}
//nRF24L01初始化
void nRF24L01_Init(void)
{
 CE=0;//待机模式Ⅰ
 CSN=1;
 SCK=0;
 IRQ=1;
}
uchar SPI_RW(uchar byte)   //SPI时序函数
{
 uchar i;
 for(i=0;i<8;i++)//一字节8位循环8次写入
 {
  if(byte&0x80)//如果数据最高位是1
   MOSI=1;//向NRF24L01写1
  else //否则写0
   MOSI=0;
  byte<<=1;//低一位移到最高位
  SCK=1;//SCK拉高，写入一位数据，同时读取一位数据
  if(MISO)
   byte|=0x01;
  SCK=0;//SCK拉低
 }
 return byte;//返回读取一字节
}
//SPI写寄存器一字节函数
//reg:寄存器地址
//value:一字节（值）
uchar SPI_W_Reg(uchar reg,uchar value)
{
 uchar status;//返回状态
 CSN=0;//SPI片选
 status=SPI_RW(reg);//写入寄存器地址，同时读取状态
 SPI_RW(value);//写入一字节
 CSN=1;//
 return status;//返回状态
}
uchar SPI_R_byte(uchar reg)	   //SPI读一字节
{
 uchar reg_value;
 CSN=0;//SPI片选
 SPI_RW(reg);//写入地址
 reg_value=SPI_RW(0);//读取寄存器的值
 CSN=1;
 return reg_value;//返回读取的值
}
//SPI读取RXFIFO寄存器数据
//reg:寄存器地址
//Dat_Buffer:用来存读取的数据
//DLen:数据长度
uchar SPI_R_DBuffer(uchar reg,uchar *Dat_Buffer,uchar Dlen)
{
 uchar status,i;
 CSN=0;//SPI片选
 status=SPI_RW(reg);//写入寄存器地址，同时状态
 for(i=0;i<Dlen;i++)
 {
  Dat_Buffer[i]=SPI_RW(0);//存储数据
 }
 CSN=1;
 return status;
}
//SPI向TXFIFO寄存器写入数据
//reg:写入寄存器地址
//TX_Dat_Buffer:存放需要发送的数据
//Dlen:数据长度
uchar SPI_W_DBuffer(uchar reg,uchar *TX_Dat_Buffer,uchar Dlen)
{
 uchar status,i;
 CSN=0;//SPI片选，启动时序
 status=SPI_RW(reg);
 for(i=0;i<Dlen;i++)
 {
  SPI_RW(TX_Dat_Buffer[i]);//发送数据
 }
 CSN=1;
 return status; 
}
void nRF24L01_Set_TX_Mode(uchar *TX_Data)	   //设置发送模式
{
 CE=0;//待机（写寄存器之前一定要进入待机模式或掉电模式）
 SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);//写寄存器指令+接收节点地址+地址宽度
 SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);//为了接收设备应答信号，接收通道0地址与发送地址相同
 SPI_W_DBuffer(W_TX_PLOAD,TX_Data,TX_DATA_WITDH);//写有效数据地址+有效数据+有效数据宽度
 SPI_W_Reg(W_REGISTER+EN_AA,0x01);//接收通道0自动应答
 SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);//使能接收通道0
 SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);//自动重发延时250US+86US，重发10次
 SPI_W_Reg(W_REGISTER+RF_CH,0);//2.4GHZ
 SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//1Mbps速率,发射功率:0DBM,低噪声放大器增益
 SPI_W_Reg(W_REGISTER+CONFIG,0x0e);//发送模式,上电,16位CRC校验,CRC使能
 CE=1;//启动发射
 _delay_ms(5);//CE高电平持续时间最少10US以上
}
uchar Check_Ack(void)	    //检测应答信号
{
 sta=SPI_R_byte(R_REGISTER+STATUS);//读取寄存状态
 if(TX_DS||MAX_RT)//如果TX_DS或MAX_RT为1,则清除中断和清除TX_FIFO寄存器的值
 {
  SPI_W_Reg(W_REGISTER+STATUS,0xff);
  CSN=0;
  SPI_RW(FLUSH_TX);//如果没有这一句只能发一次数据，大家要注意
  CSN=1;
  return 0;
 }
 else
  return 1;
}

void nRF24L01_Set_RX_Mode(void)
{
 CE=0;//??
 SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
 SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
 SPI_W_Reg(W_REGISTER+EN_AA,0x01);//auot ack
 SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
 SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
 SPI_W_Reg(W_REGISTER+RX_PW_P0,RX_DATA_WITDH);
 SPI_W_Reg(W_REGISTER+RF_CH,0);
 SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//0db,lna
 SPI_W_Reg(W_REGISTER+CONFIG,0x0f);
 
 CE=1;
 _delay_ms(5);
}

uchar nRF24L01_RX_Data(void)	 {
 sta=SPI_R_byte(R_REGISTER+STATUS);
 if(RX_DR)	 {
  CE=0;
  SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
  SPI_W_Reg(W_REGISTER+STATUS,0xff);
  CSN=0;
  SPI_RW(FLUSH_RX);
  CSN=1;
  return 1;
 }
 else	  return 0;
}

void UsartInit()	 //系统初始化
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X21;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF3;		TL1=0XF3; 		//计数器初始值设置，0XF3波特率是4800的	 th1=0xfd:19200      
	ES=1;	EA=1;	//打开总中断		//打开接收中断
	TR1=1;					//打开计数器 	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0=0XFC;	TL0=0X18;   	//给定时器赋初值，定时1ms
	ET0=1;		TR0=1;			//打开定时器0中断允许   //打开定时器
//	 P0=0xff;  	 P1=0xff;	 P2=0xff;	 P3=0xff;
		
}

void delay1u(u16 i)
{
	while(i--);	
}

u8 keyk1_4()
{
	if(k1==0) {		  //检测按键K1是否按下
		delay1u(1000);   //消除抖动 一般大约10ms
		if(k1==0) {	 //再次判断按键是否按下
		  	led=~led;	return 1;  //led状态取反
		}
		while(!k1);	 //检测按键是否松开
	}	
	if(k2==0) {		  //检测按键K1是否按下
	delay1u(1000);   //消除抖动 一般大约10ms
	if(k2==0) {	 //再次判断按键是否按下
		led1=~led1;	return 2; 	  //led状态取反
	}
	while(!k1);	 //检测按键是否松开
	}
	if(k3==0) {		  //检测按键K1是否按下
		delay1u(1000);   //消除抖动 一般大约10ms
		if(k3==0) {	 //再次判断按键是否按下
			led2=~led2;	return 3; 	  //led状态取反
	}
	while(!k1);	 //检测按键是否松开
	}	
	if(k4==0) {		  //检测按键K1是否按下
		delay1u(1000);   //消除抖动 一般大约10ms
		if(k4==0) {	 //再次判断按键是否按下
			led3=~led3;	return 4; 	  //led状态取反
	}
	while(!k1);	 //检测按键是否松开
	}											
}
void main()	   {
u8 Disp[]=" Pechin Science ";	
	UsartInit();  //	串口初始化
	 _delay_us(100);
	  LcdInit();
	for(i=0;i<16;i++)
	{
		LcdWriteData(Disp[i]);	
	}
	   while(1)	{
	   		keyk1_4();	
	   } ;
	 _delay_us(1000);
	 nRF24L01_Init();
	 while(1)
	 {
	  nRF24L01_Set_RX_Mode();
	  _delay_ms(100);
	  if(nRF24L01_RX_Data())
	  {
	   LED=0;//?????????
	  }
	  else//????
	   LED=1;
	 }
 while(1)   {
  for(i=0;i<TX_DATA_WITDH-1;i++)		//发送7次数据
  {
   nRF24L01_Set_TX_Mode(&TX_Buffer[i]);//发送数据
   while(Check_Ack());		//等待发送完成
    LED=~LED;
  }
 }

}

/*******************************************************************************
* 函数名         : Usart() interrupt 4
* 函数功能		  : 串口通信中断函数
*******************************************************************************/
void Usart() interrupt 4
{
	u8 receiveData;		receiveData=SBUF;			//出去接收到的数据
	RI = 0;						//清除接收中断标志位
	sbuf[sbufc]	 = receiveData;
	 sbufc++; if (sbufc >  sbuflen)   sbufoverflow=1;
	SBUF=receiveData;				//将接收到的数据放入到发送寄存器
	while(!TI);						 //等待发送数据完成
	TI=0;							//清除发送完成标志位
}

void Timer0() interrupt 1
{
	static u16 i;
	TH0=0XFC;	TL0=0X18; 	//给定时器赋初值，定时1ms
	i++;
	if(i==1000)	{	i=0; 	led=~led;  	}	  			//一秒 
}
