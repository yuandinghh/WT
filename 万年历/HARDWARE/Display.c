#include "Display.h"
#include "LCD12864.h"
#include "rtc.h"
#include "ds18b20.h"
#include "Lunar.h"

void welcome(void) //显示欢迎信息
{								  
	write_com(0x80); lcm_w_word("  高精度万年历  ");delay_ms(500); 
	write_com(0x90); lcm_w_word("设计:           ");delay_ms(500); 	
	write_com(0x88); lcm_w_word("硬件检测        ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测.       ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测..      ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测...     ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测....    ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测.....   ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测......  ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测......O ");delay_ms(50);
	write_com(0x88); lcm_w_word("硬件检测......OK");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载        ");delay_ms(50); 	
	write_com(0x98); lcm_w_word("程序加载.       ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载..      ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载...     ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载....    ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载.....   ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载......  ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载......O ");delay_ms(50);
	write_com(0x98); lcm_w_word("程序加载......OK");delay_ms(50);	
}

void displaydate()	 //显示日期
{
	u16 year=0;
	u8 month=0,date=0;	

	write_com(0x30);
	write_com(0x06); //光标右移,DDRAM位址计数器(AC)加1,不整屏移动

	//往液晶屏填写 年 数据
	year=calendar.w_year;
	lcm_w_test(0,0x80);	
	write_data(year/1000+'0');
	write_data((year%1000)/100+'0');	
	write_data((year%100)/10+'0');
	write_data((year%10)/1+'0');
	
	//往液晶屏填写 月 数据
	month=calendar.w_month;
	lcm_w_word("-");	
	write_data(month/10+0x30);
	write_data((month%10)+0x30);	

	//往液晶屏填写 日 数据
	date=calendar.w_date;
	lcm_w_word("-");		 
	write_data(date/10+0x30);
	write_data(date%10+0x30);
}
void displayxq()     //显示星期
{
	u8 day=calendar.week;
	write_com(0x86);	  
	lcm_w_word("周");
 	write_com(0x87);	  
	if(day==0)  {lcm_w_word("日");}
	//if(day==7)  {lcm_w_word("日");}    
  	if(day==6)  {lcm_w_word("六");}    
  	if(day==5)  {lcm_w_word("五");}    
  	if(day==4)  {lcm_w_word("四");}   
//	if(day==3)  {lcm_w_word("三");}	 //此指令不能正确显示，需安装补丁程序才能显示。
  	if(day==3)  {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //此指令等于lcm_w_word("三"); 因为“三”的内码失效。
  	if(day==2)  {lcm_w_word("二");}    
  	if(day==1)  {lcm_w_word("一");}
}
void displaytime()	 //显示时间
{
	u8 hour=0,min=0,sec=0;
	//往液晶屏填写 小时 数据
	hour=calendar.hour;                        //读取小时
	write1632GDRAM(1,2,num1632[hour/10]);
	write1632GDRAM(2,2,num1632[hour%10]);
	write1632GDRAM(3,2,num1632[10]);        //时钟分隔符“:”

	//往液晶屏填写 分钟 数据
	min=calendar.min;                         //读取分				
	write1632GDRAM(4,2,num1632[min/10]);
	write1632GDRAM(5,2,num1632[min%10]);
	write1632GDRAM(6,2,num1632[10]);        //时钟分隔符“:”

	//往液晶屏填写 秒钟 数据
	sec=calendar.sec;                         //读取秒
	write1632GDRAM(7,2,num1632[sec/10]);
	write1632GDRAM(8,2,num1632[sec%10]);
}
void disptemp()      //显示温度
{
	u8 disdata[5];
	u16  tvalue;
	float temp;
	temp = DS18B20_Get_Temp();
	if(temp>=0)//温度在0摄氏度以上
	{
		set1616pic(1,4,0,1); //显示"温度计图标"		
	}
	else //温度在0摄氏度以下
	{
		temp=-temp;
		set1616pic(1,4,0,2); //显示"负温度图标"
	}
	temp*=10;
	tvalue=(u16)temp;
	disdata[0]=tvalue/100+0x30;         //十位数
	disdata[1]=tvalue/10%10+0x30;           //个位数
	disdata[2]=tvalue%10+0x30;               //小数位    
	if(disdata[0]==0x30) //如果十位为0，不显示
	{
		disdata[0]=0x20;
	}
	write_com(0x30); 
	write_com(0x06);	
	write_com(0x99);           //在液晶上显示温度起始位置："28.8°C"
	write_data(disdata[0]);    //显示十位 	
	write_data(disdata[1]);    //显示个位 	
	write_data(0x2e);          //显示小数点 	
	write_data(disdata[2]);    //显示小数位
	set1616pic(4,4,0,0);	   //在第5列第4行不反白的°C图标
}
void displaynl()	 //显示农历
{
	Conversion(century,calendar.w_year,calendar.w_month,calendar.w_date);	//公历转农历
	write_com(0x30); write_com(0x06); write_com(0x9c);
//	if (month_moon==1) {lcm_w_word("正");}
	if (month_moon==1) {lcm_w_test(1,0xd5);lcm_w_test(1,0xfd);}
	if (month_moon==2) {lcm_w_word("二");}
//	if (month_moon==2) {lcm_w_word("三");}
	if (month_moon==3) {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);}
	if (month_moon==4) {lcm_w_word("四");}
	if (month_moon==5) {lcm_w_word("五");}
	if (month_moon==6) {lcm_w_word("六");}
	if (month_moon==7) {lcm_w_word("七");}
	if (month_moon==8) {lcm_w_word("八");}
	if (month_moon==9) {lcm_w_word("九");}
	if (month_moon==10){lcm_w_word("十");}
	if (month_moon==11){lcm_w_word("冬");}
	if (month_moon==12){lcm_w_word("腊");}
                        lcm_w_word("月");

	if (day_moon==1)   {lcm_w_word("初一");}
	if (day_moon==2)   {lcm_w_word("初二");}
	if (day_moon==3)   {lcm_w_word("初");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //“三”的代码
	if (day_moon==4)   {lcm_w_word("初四");}
	if (day_moon==5)   {lcm_w_word("初五");}
	if (day_moon==6)   {lcm_w_word("初六");}
	if (day_moon==7)   {lcm_w_word("初七");}
	if (day_moon==8)   {lcm_w_word("初八");}
	if (day_moon==9)   {lcm_w_word("初九");}
	if (day_moon==10)  {lcm_w_word("初十");}
	if (day_moon==11)  {lcm_w_word("十一");}
	if (day_moon==12)  {lcm_w_word("十二");}
	if (day_moon==13)  {lcm_w_word("十");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //十三
	if (day_moon==14)  {lcm_w_word("十四");}
	if (day_moon==15)  {lcm_w_word("十五");}
	if (day_moon==16)  {lcm_w_word("十六");}
	if (day_moon==17)  {lcm_w_word("十七");}
	if (day_moon==18)  {lcm_w_word("十八");}
	if (day_moon==19)  {lcm_w_word("十九");}
	if (day_moon==20)  {lcm_w_word("二十");}
	if (day_moon==21)  {lcm_w_word("廿一");}
	if (day_moon==22)  {lcm_w_word("廿二");}
	if (day_moon==23)  {lcm_w_word("廿");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);}	//廿三
	if (day_moon==24)  {lcm_w_word("廿四");}
	if (day_moon==25)  {lcm_w_word("廿五");}
	if (day_moon==26)  {lcm_w_word("廿六");}
	if (day_moon==27)  {lcm_w_word("廿七");}
	if (day_moon==28)  {lcm_w_word("廿八");}
	if (day_moon==29)  {lcm_w_word("廿九");}
	if (day_moon==30)  {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);lcm_w_word("十");}	//三十
}
