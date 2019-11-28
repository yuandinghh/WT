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
//#include "mem.h" 

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
��X = 10��Y=10 ��ʼ��ʾ24λ ���� ʱ�� ---------X------>

************************************************/
#define HZSIZE	 	24    //ѡ����size
#define DATEX	 	10			//��ʼX ����
#define DATEY	 	10        //��ʼY ����
#define DateX 190		 //��ʾ���ڵ� X ����
#define DispStartX	  30   //���KEY0��������   X
#define DispStartY    30 
#define ProgramX    30 	
#define	 ProgramY 55
#define		SystemX  30
#define		SystemY  80  //ProgramY+25

 int main(void)
 {	  //	u32 fontcnt;		  
	u8 i;    //	u8 fontx[2];//gbk��
	u8 key;   	    
	_calendar_obj nowtime;    //���� ����ʱ��
	 
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	LED_Init();
  KEY_Init();	
	LCD_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�  	 //��ʼ������						//��ʼ��LCD   
 	RTC_Init();	
	nowtime = calendar;  //RTC��ʼ�� //�ṹ����	//memcpy(void *nowtime, const void *calendar,  sizeof(calendar)));
	W25QXX_Init();
  my_mem_init(SRAMIN);			//��ʼ��W25Q128      //��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	 
//		u32 fontcnt;		  
//	u8 i,j;
//	u8 fontx[2];//gbk��
//	u8 key,t;   	    

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD   
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH. 
	 
	RTC_Init();	
	nowtime = calendar;  //RTC��ʼ�� //�ṹ����	//memcpy(void *nowtime, const void *calendar,  sizeof(calendar))); 
	while(font_init()) 			//����ֿ�
	{
//UPD:    
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
	POINT_COLOR=RED;       
	Show_Str(30,50,200,16,"ս��STM32F103������",16,0);				    	 
	Show_Str(30,70,200,16,"GBK�ֿ���Գ���",16,0);				    	 
	Show_Str(30,90,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,110,200,16,"2015��1��20��",16,0);
	Show_Str(30,130,200,16,"��KEY0,�����ֿ�",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,150,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,170,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,190,200,16,"���ּ�����:",16,0);

	Show_Str(30,220,200,24,"��Ӧ����Ϊ:",24,0); 
	Show_Str(30,244,200,16,"��Ӧ����(16*16)Ϊ:",16,0);			 
	Show_Str(30,260,200,12,"��Ӧ����(12*12)Ϊ:",12,0);		
	
//#define	  HZSIZE 24
//#define DispStartX	  30   //���KEY0��������   X
//#define DispStartY    300 
//#define ProgramX    30 	
//#define	 ProgramY 325
//#define		SystemX  30
//#define		SystemY  350  //ProgramY+25

//  	Show_Str(DispStartX,DispStartY,200,HZSIZE,"���KEY0������������",24,0);
//		Show_Str(ProgramX,ProgramY,200,HZSIZE,"���KEY2������ʼ���ò���",24,0);
//		Show_Str(SystemX,SystemY,200,HZSIZE,"�����ɫ����ϵͳ����",24,0);
//֧���Զ�����  (x,y):��ʼ����  width,height:����  str  :�ַ���  size :�����С mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ
//	Show_Str(30,30,200,24,"��ǰʱ��:",24,0);			



	
	  firsttimedisplay();   //��һ����ʾ ʱ��  2019-11-1
//  	Show_Str(DispStartX,DispStartY,200,HZSIZE,"���KEY0������������",24,0);
//		Show_Str(ProgramX,ProgramY,200,HZSIZE,"���KEY2������ʼ���ò���",24,0);
//		Show_Str(SystemX,SystemY,200,HZSIZE,"�����ɫ����ϵͳ����",24,0);
	
		while(1) {
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
					case 0:				
						Show_Str(DateX,DATEY,72,24,"������",24,0);		
					break;
					case 1:				
						Show_Str(DateX,DATEY,72,24,"���һ",24,0);				
					break;
					case 2:			
						Show_Str(DateX,DATEY,72,24,"��ݶ�",24,0);						break;
					case 3:						Show_Str(DateX,DATEY,72,24,"�����",24,0);		
					break;
					case 4:				
						Show_Str(DateX,DATEY,72,24,"�����",24,0);						break;
					case 5:		
						Show_Str(DateX,DATEY,72,24,"�����",24,0);	
					break;
					case 6:			
						Show_Str(DateX,DATEY,72,24,"�����",24,0);				
					break;  
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
	
			
				for(i=0;i<10;i++) {
					delay_ms(100);	
				}
				LED1=!LED1;
	}
		
		
		
		

//				while(t--)//��ʱ,ͬʱɨ�谴��
//				{
//					delay_ms(20);
//					key=KEY_Scan(0);
////					if(key==KEY0_PRES)goto UPD;
//				}
	
	
}

















