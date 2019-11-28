#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "rtc.h" 
#include "timer.h"

#define	HZSIZE 24    //������ʾsize
#define DispStartX	  30   //���KEY0��������   X
#define DispStartY    30+20 
#define ProgramX    30 	
#define	ProgramY    55+20
#define	SystemX    30
#define	SystemY    75+20  //ProgramY+25
#define HZSIZE	 	24    //ѡ����size
#define DATEX	 	10			//��ʼX ����
#define DATEY	 	10        //��ʼY ����
#define DateX   180		 //��ʾ���ڵ� X ����

 void Sartrun(void);
 u8 runstate;   //����״̬  0 ������ɨ����̣�1 ��������ʱ�� ������2 ��� ״̬��
 //LED״̬���ú���
void led_set(u8 sta)
{
	LED0=sta;
} 
//�����������ò��Ժ���
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
} 
 
/************************************************
STM32����
 κ�� ��ҵ���̿���
���ߣ�Ԭ�� 2019-10-19 1:19
ͳһ�趨��ʱ��̵����Ŀ�ͣʱ�䡣��Ҫע��Ҫ�趨Ϊ�ȹ�,���Ǻ��Ͽտ�,ʱ��̵�
��Ϊ��,��ʱ��:��һ���� 180��(������),Ȼ��5�롣�ڶ���ʱ��̵����趨Ϊ��
186��,��5��,�������̵����趨Ϊ�� 192��,��5�롣�Դ����ơ�ֱ����5 ���̵���
ÿ������5 ��ʱ��̵�������4��
�ر�ע��ʱ��̵����ص�ʱ��Ҫ����ͬ,�����ͬʱ��,5 ��ͬʱ�������ܴ󡣿տ���
������,����˿�ᾭ���ϡ�
��X = 10��Y=10 ��ʼ��ʾ24λ ���� ʱ�� ---------X------>    */
  u16 outdat=0xFF00;
  u16 tmpdat,dd;

 int main(void)  {	 
//	u32 fontcnt;  	u8 i,j;  	u8 fontx[2];								//gbk��
	u8 key;  //,t;   	    
	_calendar_obj nowtime;  	  //���� ����ʱ��
	delay_init();	    				 //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 				//���ڳ�ʼ��Ϊ115200
 	usmart_dev.init(72);				//��ʼ��USMART		
 	LED_Init();		KEY_Init();		LCD_Init();	//��ʼ��LCD  //��ʼ����LED���ӵ�Ӳ���ӿ� //��ʼ������
	W25QXX_Init();	 	my_mem_init(SRAMIN);	exfuns_init();	//Ϊfatfs��ر��������ڴ� 	//��ʼ���ڲ��ڴ�� 		//��ʼ��W25Q128
 	f_mount(fs[0],"0:",1); 	f_mount(fs[1],"1:",1); 		//����SD��  		//����FLASH.
	RTC_Init();	 	nowtime = calendar;  //RTC��ʼ�� //�ṹ����	//memcpy(void *nowtime, const void *calendar,  sizeof(calendar))); 
	TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
    
	while(font_init()) 		{		//����ֿ�   UPD:    
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,50,200,16,16,"WarShip STM32");
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  
  	POINT_COLOR=BLUE; 	Show_Str(DispStartX,DispStartY,300,HZSIZE,"���KEY0������������",24,0);
	POINT_COLOR=GREEN; 	Show_Str(ProgramX,ProgramY,300,HZSIZE,"���KEY2������ʼ���ò���",24,0);
	POINT_COLOR=RED; 		Show_Str(SystemX,SystemY,300,HZSIZE,"�����ɫ����ϵͳ����",24,0);
	firsttimedisplay();   //��һ����ʾ ʱ��  2019-11-1
	while(1)	{     //main loop
		  POINT_COLOR=BLUE;    //	Show_Str(30,230,200,24,"��Ӧ����Ϊ:",24,0);   //��ʾ���ֵĺ���
		if (calendar.w_year != nowtime.w_year) {
				nowtime.w_year = calendar.w_year;		
				LCD_ShowNum(DATEX,DATEY,calendar.w_year,4,24);	
		}  //				Show_Str(60,DATEY,24,24,"��",24,0);		 							  
		if (calendar.w_month != nowtime.w_month) {
			nowtime.w_month = calendar.w_month;		
			LCD_ShowNum(84,DATEY,calendar.w_month,2,24);	
		}   //				Show_Str(108,DATEY,24,24,"��",24,1);	
		if (calendar.w_date != nowtime.w_date) {
				nowtime.w_date = calendar.w_date;		
				LCD_ShowNum(134,DATEY,calendar.w_date,2,24);	  //+2��϶
		} //				Show_Str(160,DATEY,24,24,"��",24,1);				//+5 ��϶	 	
				if (calendar.week != nowtime.week) {
					nowtime.week = calendar.week;
				switch(calendar.week)				{
					case 0:			Show_Str(DateX,DATEY,72,24,"������",24,0);		break;
					case 1:			Show_Str(DateX,DATEY,72,24,"���һ",24,0);		break;
					case 2:			Show_Str(DateX,DATEY,72,24,"��ݶ�",24,0);		break;
					case 3:			Show_Str(DateX,DATEY,72,24,"�����",24,0);		break;
					case 4:			Show_Str(DateX,DATEY,72,24,"�����",24,0);		break;
					case 5:			Show_Str(DateX,DATEY,72,24,"�����",24,0);		break;
					case 6:			Show_Str(DateX,DATEY,72,24,"�����",24,0);		break;  
				}
			}
		POINT_COLOR=RED;   //	POINT_COLOR=CYAN;  ��ɫ̫��	
		if (calendar.hour != nowtime.hour) {
				nowtime.hour = calendar.hour;		
				LCD_ShowNum(282,DATEY,calendar.hour,2,24);  //	Show_Str(308,DATEY,24,24,"��",24,0);  //��ʾʱ��
		}
		if (calendar.min != nowtime.min) {
				nowtime.min = calendar.min;			
				LCD_ShowNum(326,DATEY,calendar.min,2,24);  //	Show_Str(352,DATEY,24,24,"��",24,0);
		}
		LCD_ShowNum(370,DATEY,calendar.sec,2,24);

		if ( runstate == 0  ) {
					delay_ms(10);
					key=KEY_Scan(0);
					switch(key)			{
					case 1:	 Sartrun()	;		break;   //1��KEY0����   ���KEY0������������
					case 2:			;		break;   //2��KEY1����
					case 3:			;		break;   //3��KEY2����   ���KEY2������ʼ���ò���
					case 4:			;		break;   //4��KEY3����   WK_UP	
					}
		  }
tmpdat=GPIO_ReadOutputData(GPIOE);
//outdat|=(tmpdat&0x00FF);   //GPIO_Write(GPIOE,outdat);   //GPIO_Write(GPIOB,outdat);
		LED1=1;   		delay_ms(1500);
//GPIOE->ODR|=0<<5;   // GPIO_Write(GPIOE,0X0);     GPIO_Write(GPIOB,0X0);   // GPIOx->ODR = PortVal;
		LED1=0;
		delay_ms(1500);    
	}     //loop
} 
char startrun[4][5];
u8 group,num,i;              //���� ÿ�� 5�� 

void Sartrun(void) {  
    
	runstate=1;
	 group=0,num=0;          //���� ÿ�� 5�� 
    for( i=0; i<4;i++) {
        
    }

	ms500++ ;
	
}

void runreley()    {


}















