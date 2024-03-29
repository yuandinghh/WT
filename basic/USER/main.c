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
STM32开发
 魏涛 工业过程控制
作者：袁丁 2019-10-19 1:19
统一设定好时间继电器的开停时间。主要注意要设定为先关,就是合上空开,时间继电
器为关,计时如:第一个关 180秒(三分钟),然后开5秒。第二个时间继电器设定为关
186秒,开5秒,第三个继电器设定为关 192秒,开5秒。以此类推。直到第5 个继电器
每组最多带5 个时间继电器。共4组
特别注意时间继电器关的时间要不相同,否则会同时开,5 组同时开电流很大。空开会
过流跳,保险丝会经常断。
从X = 10，Y=10 开始显示24位 日期 时间 ---------X------>

************************************************/
#define HZSIZE	 	24    //选择汉字size
#define DATEX	 	10			//起始X 坐标
#define DATEY	 	10        //起始Y 坐标
#define DateX 190		 //显示星期的 X 坐标
#define DispStartX	  30   //点击KEY0按键启动   X
#define DispStartY    30 
#define ProgramX    30 	
#define	 ProgramY 55
#define		SystemX  30
#define		SystemY  80  //ProgramY+25

 int main(void)
 {	  //	u32 fontcnt;		  
	u8 i;    //	u8 fontx[2];//gbk码
	u8 key;   	    
	_calendar_obj nowtime;    //备用 日期时间
	 
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
 	LED_Init();
  KEY_Init();	
	LCD_Init();				//初始化与LED连接的硬件接口  	 //初始化按键						//初始化LCD   
 	RTC_Init();	
	nowtime = calendar;  //RTC初始化 //结构复制	//memcpy(void *nowtime, const void *calendar,  sizeof(calendar)));
	W25QXX_Init();
  my_mem_init(SRAMIN);			//初始化W25Q128      //初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	 
//		u32 fontcnt;		  
//	u8 i,j;
//	u8 fontx[2];//gbk码
//	u8 key,t;   	    

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH. 
	 
	RTC_Init();	
	nowtime = calendar;  //RTC初始化 //结构复制	//memcpy(void *nowtime, const void *calendar,  sizeof(calendar))); 
	while(font_init()) 			//检查字库
	{
//UPD:    
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(30,50,200,16,16,"WarShip STM32");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}  
	POINT_COLOR=RED;       
	Show_Str(30,50,200,16,"战舰STM32F103开发板",16,0);				    	 
	Show_Str(30,70,200,16,"GBK字库测试程序",16,0);				    	 
	Show_Str(30,90,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,110,200,16,"2015年1月20日",16,0);
	Show_Str(30,130,200,16,"按KEY0,更新字库",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,150,200,16,"内码高字节:",16,0);				    	 
	Show_Str(30,170,200,16,"内码低字节:",16,0);				    	 
	Show_Str(30,190,200,16,"汉字计数器:",16,0);

	Show_Str(30,220,200,24,"对应汉字为:",24,0); 
	Show_Str(30,244,200,16,"对应汉字(16*16)为:",16,0);			 
	Show_Str(30,260,200,12,"对应汉字(12*12)为:",12,0);		
	
//#define	  HZSIZE 24
//#define DispStartX	  30   //点击KEY0按键启动   X
//#define DispStartY    300 
//#define ProgramX    30 	
//#define	 ProgramY 325
//#define		SystemX  30
//#define		SystemY  350  //ProgramY+25

//  	Show_Str(DispStartX,DispStartY,200,HZSIZE,"点击KEY0按键启动运行",24,0);
//		Show_Str(ProgramX,ProgramY,200,HZSIZE,"点击KEY2按键开始设置参数",24,0);
//		Show_Str(SystemX,SystemY,200,HZSIZE,"点击红色按键系统重启",24,0);
//支持自动换行  (x,y):起始坐标  width,height:区域  str  :字符串  size :字体大小 mode:0,非叠加方式;1,叠加方式
//	Show_Str(30,30,200,24,"当前时间:",24,0);			



	
	  firsttimedisplay();   //第一次显示 时间  2019-11-1
//  	Show_Str(DispStartX,DispStartY,200,HZSIZE,"点击KEY0按键启动运行",24,0);
//		Show_Str(ProgramX,ProgramY,200,HZSIZE,"点击KEY2按键开始设置参数",24,0);
//		Show_Str(SystemX,SystemY,200,HZSIZE,"点击红色按键系统重启",24,0);
	
		while(1) {
	  POINT_COLOR=BLUE;    //	Show_Str(30,230,200,24,"对应汉字为:",24,0);   //显示汉字的函数
		if (calendar.w_year != nowtime.w_year) {
				nowtime.w_year = calendar.w_year;		
				LCD_ShowNum(DATEX,DATEY,calendar.w_year,4,24);	
		}  //				Show_Str(60,DATEY,24,24,"年",24,0);		 							  
		if (calendar.w_month != nowtime.w_month) {
			nowtime.w_month = calendar.w_month;		
			LCD_ShowNum(84,DATEY,calendar.w_month,2,24);	
		}   //				Show_Str(108,DATEY,24,24,"月",24,1);	
		if (calendar.w_date != nowtime.w_date) {
				nowtime.w_date = calendar.w_date;		
				LCD_ShowNum(134,DATEY,calendar.w_date,2,24);	  //+2空隙
		} //				Show_Str(160,DATEY,24,24,"日",24,1);				//+5 空隙	 	
				if (calendar.week != nowtime.week) {
					nowtime.week = calendar.week;
				switch(calendar.week)				{
					case 0:				
						Show_Str(DateX,DATEY,72,24,"星期天",24,0);		
					break;
					case 1:				
						Show_Str(DateX,DATEY,72,24,"礼拜一",24,0);				
					break;
					case 2:			
						Show_Str(DateX,DATEY,72,24,"礼拜二",24,0);						break;
					case 3:						Show_Str(DateX,DATEY,72,24,"礼拜三",24,0);		
					break;
					case 4:				
						Show_Str(DateX,DATEY,72,24,"礼拜四",24,0);						break;
					case 5:		
						Show_Str(DateX,DATEY,72,24,"礼拜五",24,0);	
					break;
					case 6:			
						Show_Str(DateX,DATEY,72,24,"礼拜六",24,0);				
					break;  
				}
			}
		POINT_COLOR=RED;   //	POINT_COLOR=CYAN;  颜色太暗	
		if (calendar.hour != nowtime.hour) {
				nowtime.hour = calendar.hour;		
				LCD_ShowNum(282,DATEY,calendar.hour,2,24);  //	Show_Str(308,DATEY,24,24,"：",24,0);  //显示时间
		}
		if (calendar.min != nowtime.min) {
				nowtime.min = calendar.min;			
				LCD_ShowNum(326,DATEY,calendar.min,2,24);  //	Show_Str(352,DATEY,24,24,"：",24,0);
		}
		LCD_ShowNum(370,DATEY,calendar.sec,2,24);
	
			
				for(i=0;i<10;i++) {
					delay_ms(100);	
				}
				LED1=!LED1;
	}
		
		
		
		

//				while(t--)//延时,同时扫描按键
//				{
//					delay_ms(20);
//					key=KEY_Scan(0);
////					if(key==KEY0_PRES)goto UPD;
//				}
	
	
}

















