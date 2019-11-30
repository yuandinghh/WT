#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
 
/************************************************
 ALIENTEK战舰STM32开发板实验33
 无线通信 实验
************************************************/

 int main(void)
 {	 
	u8 key,mode;
	u16 t=0;			 
	u8 tmp_buf[33];		    
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
 	NRF24L01_Init();    		//初始化NRF24L01 
 	POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,50,200,24,24,"WarShip STM32");	
	LCD_ShowString(30,80,200,24,24,"NRF24L01 TEST");	
	LCD_ShowString(30,110,200,24,24,"ATOM@ALIENTEK");
	LCD_ShowString(30,140,200,24,24,"2015/1/17"); 
	while(NRF24L01_Check())    	{
		LCD_ShowString(30,180,200,24,24,"NRF24L01 Error");
		delay_ms(200);    LCD_Fill(30,180,239,130+24,WHITE);    delay_ms(200);
	}
	LCD_ShowString(30,170,200,24,24,"NRF24L01 OK"); 	 
 	while(1) {	
		key=KEY_Scan(0);
		if(key==KEY0_PRES) 	{
			mode=0;   	LCD_ShowString(30,500,200,24,24,"NRF24L01 RX_Mode");		break;
		}else if(key==KEY1_PRES) 		{
			mode=1;  LCD_ShowString(30,500,200,24,24,"NRF24L01 TX_Mode");			break;
		}
		t++;
		if(t==100)LCD_ShowString(10,200,230,24,24,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
 		if(t==200)
		{	
			LCD_Fill(10,230,230,150+24,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	LCD_Fill(10,230,240,166,WHITE);//清空上面的显示		  
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	if(mode==0) 	{                          //RX模式
		LCD_ShowString(30,260,200,24,24,"NRF24L01 RX_Mode");	
		LCD_ShowString(30,290,200,24,24,"Received DATA:");	
		NRF24L01_RX_Mode();	      t=0;
		while(1)  		{	  		    		    				 
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{
				tmp_buf[32]=0;//加入字符串结束符
				LCD_ShowString(0,270,lcddev.width-1,32,24,tmp_buf);    
			}else delay_us(100);	   
			t++;
			if(t == 10000)             //大约1s钟改变一次状态
			{
				  t=0; LED1=!LED1;
			} 
         //   else {      break;   }     //error
		};	
	} else  {                 //TX模式
		LCD_ShowString(30,320,200,24,24,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();   	mode=' ';//从空格键开始  
		while(1)
		{	  		   				 
			if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
			{
				LCD_ShowString(30,330,239,32,24,"Sended DATA:");	
				LCD_ShowString(0,360,lcddev.width-1,32,24,tmp_buf); 
				key=mode;
				for(t=0;t<32;t++)
				{
					key++;
					if(key>('~'))key=' ';
					tmp_buf[t]=key;	
				}
				mode++; 
				if(mode>'~')mode=' ';  	  
				tmp_buf[32]=0;//加入结束符		   
			}else
			{										   	
 				LCD_Fill(0,360,lcddev.width,170+24*3,WHITE);//清空显示			   
				LCD_ShowString(30,360,lcddev.width-1,32,24,"Send Failed "); 
			};
			LED1=!LED1;
			delay_ms(1500);				    
		};
	} 
}


