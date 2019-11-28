#include "delay.h"
#include "sys.h"
#include "rtc.h"
#include "usart.h"
#include "usart2.h"
#include "LCD12864.h"
#include "Display.h"
#include "ds18b20.h"

#define uchar unsigned char
#define uint  unsigned int

int main()
{
	u8 t=0;
	u8 rxlen=0;
	u16 syear;
	u8 smon,sday,hour,min,sec;

	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);	  	//延时初始化
	uart_init(72,9600);	//串口1初始化
	uart2_init(36,9600);//串口2初始化
	LCD12864_init();    //初始化LCD12864
	RTC_Init();			//初始化RTC
	DS18B20_Init();
	welcome();
	lcm_clr();        //清屏函数
	Clean_12864_GDRAM();
	while(1)
	{
		if(t!=calendar.sec)	//1秒时间到
		{
			t=calendar.sec;
			displaydate();	 //显示日期
			displayxq();     //显示星期
			displaytime();	 //显示时间
			disptemp();      //显示温度
			displaynl();	 //显示农历
		}
		if(USART2_RX_STA&0X8000)//串口2收到数据了
		{
			rxlen=USART2_RX_STA&0X7FFF;	//得到数据长度
			if(rxlen==16) //接收到的数据为16位
			{
				rxlen=0;
				syear=(USART2_RX_BUF[0]-'0')*1000+(USART2_RX_BUF[1]-'0')*100+(USART2_RX_BUF[2]-'0')*10+(USART2_RX_BUF[3]-'0');
				smon=(USART2_RX_BUF[4]-'0')*10+(USART2_RX_BUF[5]-'0');
				sday=(USART2_RX_BUF[6]-'0')*10+(USART2_RX_BUF[7]-'0');
				hour=(USART2_RX_BUF[8]-'0')*10+(USART2_RX_BUF[9]-'0');
				min=(USART2_RX_BUF[10]-'0')*10+(USART2_RX_BUF[11]-'0');
				sec=(USART2_RX_BUF[12]-'0')*10+(USART2_RX_BUF[13]-'0');
				RTC_Set(syear,smon,sday,hour,min,sec);
			}
			USART2_RX_STA=0; //启动下一次接收
		}
	}	 
}

/*
//公历转农历函数 
void Conversion(u8 c,u8 year,u8 month,u8 day) 
{                          
    u8 temp1,temp2,temp3,month_p; 
    uint temp4,table_addr; 
    u8 flag2,flag_y; 
//定位数据表地址 
    if(c==0) { table_addr=(year+100-1)*3; } 
    	else { table_addr=(year-1)*3;     } 
//取当年春节所在的公历月份 
temp1=year_code[table_addr+2]&0x60;  
    temp1=_cror_(temp1,5); 
//取当年春节所在的公历日 
temp2=year_code[table_addr+2]&0x1f;  
    if(temp1==0x1) 	{ temp3=temp2-1; }   
    	else  	{ temp3=temp2+31-1; } 
    if(month<10)	{ temp4=day_code1[month-1]+day-1; } 
    	else     	{ temp4=day_code2[month-10]+day-1;} 
    if ((month>2)&&(year%4==0)&&(year!=0)) 
       temp4+=1; //如果公历月大于2月并且该年的2月为闰月,天数加1 
    if (temp4>=temp3)//元旦在春节以前或元旦就是春节 
    {  
        temp4-=temp3; month=1; 
        month_p=1;  //month_p为月份指向,元旦在春节前或就是春节当日month_p指向首月 
        flag2=GetMoonDay(month_p,table_addr); //检查该农历月为大小还是小月,大月返回1,小月返回0 
        flag_y=0; 
        if(flag2==0)temp1=29;    //小月29天 
        	else    temp1=30;    //大小30天 
        temp2=year_code[table_addr]&0xf0; 
        temp2=_cror_(temp2,4);  //从数据表中取该年的闰月月份,如为0则该年无闰月 
        while(temp4>=temp1) 
        { 
            temp4-=temp1; month_p+=1; 
            if(month==temp2) 
    	{ 
              flag_y=~flag_y; 
              if(flag_y==0)month+=1; 
            } 
            	else month+=1; 
            flag2=GetMoonDay(month_p,table_addr); 
            if(flag2==0)temp1=29; 
            	else    temp1=30; 
        } 
        day=temp4+1; 
    } 
    else //元旦在春节以后 
    {   
        temp3-=temp4; 
        if (year==0){year=99;c=1;} 
        	else	   year-=1; 
        table_addr-=3; 
        month=12; 
        temp2=year_code[table_addr]&0xf0; 
        temp2=_cror_(temp2,4); 
        if (temp2==0)month_p=12;  
        	else     month_p=13;  
        flag_y=0; 
        flag2=GetMoonDay(month_p,table_addr); 
        if(flag2==0)temp1=29; 
        	else    temp1=30; 
        while(temp3>temp1) 
        { 
            temp3-=temp1; 
            month_p-=1; 
            if(flag_y==0)month-=1; 
            if(month==temp2)flag_y=~flag_y; 
            flag2=GetMoonDay(month_p,table_addr); 
            if(flag2==0)temp1=29; 
            	else    temp1=30; 
        } 
        day=temp1-temp3+1; 
    } 
    c_moon=c; 
    year_moon=year; 
    month_moon=month; 
    day_moon=day; 
}
*/