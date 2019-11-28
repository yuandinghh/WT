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

	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);	  	//��ʱ��ʼ��
	uart_init(72,9600);	//����1��ʼ��
	uart2_init(36,9600);//����2��ʼ��
	LCD12864_init();    //��ʼ��LCD12864
	RTC_Init();			//��ʼ��RTC
	DS18B20_Init();
	welcome();
	lcm_clr();        //��������
	Clean_12864_GDRAM();
	while(1)
	{
		if(t!=calendar.sec)	//1��ʱ�䵽
		{
			t=calendar.sec;
			displaydate();	 //��ʾ����
			displayxq();     //��ʾ����
			displaytime();	 //��ʾʱ��
			disptemp();      //��ʾ�¶�
			displaynl();	 //��ʾũ��
		}
		if(USART2_RX_STA&0X8000)//����2�յ�������
		{
			rxlen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
			if(rxlen==16) //���յ�������Ϊ16λ
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
			USART2_RX_STA=0; //������һ�ν���
		}
	}	 
}

/*
//����תũ������ 
void Conversion(u8 c,u8 year,u8 month,u8 day) 
{                          
    u8 temp1,temp2,temp3,month_p; 
    uint temp4,table_addr; 
    u8 flag2,flag_y; 
//��λ���ݱ��ַ 
    if(c==0) { table_addr=(year+100-1)*3; } 
    	else { table_addr=(year-1)*3;     } 
//ȡ���괺�����ڵĹ����·� 
temp1=year_code[table_addr+2]&0x60;  
    temp1=_cror_(temp1,5); 
//ȡ���괺�����ڵĹ����� 
temp2=year_code[table_addr+2]&0x1f;  
    if(temp1==0x1) 	{ temp3=temp2-1; }   
    	else  	{ temp3=temp2+31-1; } 
    if(month<10)	{ temp4=day_code1[month-1]+day-1; } 
    	else     	{ temp4=day_code2[month-10]+day-1;} 
    if ((month>2)&&(year%4==0)&&(year!=0)) 
       temp4+=1; //��������´���2�²��Ҹ����2��Ϊ����,������1 
    if (temp4>=temp3)//Ԫ���ڴ�����ǰ��Ԫ�����Ǵ��� 
    {  
        temp4-=temp3; month=1; 
        month_p=1;  //month_pΪ�·�ָ��,Ԫ���ڴ���ǰ����Ǵ��ڵ���month_pָ������ 
        flag2=GetMoonDay(month_p,table_addr); //����ũ����Ϊ��С����С��,���·���1,С�·���0 
        flag_y=0; 
        if(flag2==0)temp1=29;    //С��29�� 
        	else    temp1=30;    //��С30�� 
        temp2=year_code[table_addr]&0xf0; 
        temp2=_cror_(temp2,4);  //�����ݱ���ȡ����������·�,��Ϊ0����������� 
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
    else //Ԫ���ڴ����Ժ� 
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