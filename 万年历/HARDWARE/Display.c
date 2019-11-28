#include "Display.h"
#include "LCD12864.h"
#include "rtc.h"
#include "ds18b20.h"
#include "Lunar.h"

void welcome(void) //��ʾ��ӭ��Ϣ
{								  
	write_com(0x80); lcm_w_word("  �߾���������  ");delay_ms(500); 
	write_com(0x90); lcm_w_word("���:           ");delay_ms(500); 	
	write_com(0x88); lcm_w_word("Ӳ�����        ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����.       ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����..      ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����...     ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����....    ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����.....   ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����......  ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����......O ");delay_ms(50);
	write_com(0x88); lcm_w_word("Ӳ�����......OK");delay_ms(50);
	write_com(0x98); lcm_w_word("�������        ");delay_ms(50); 	
	write_com(0x98); lcm_w_word("�������.       ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������..      ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������...     ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������....    ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������.....   ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������......  ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������......O ");delay_ms(50);
	write_com(0x98); lcm_w_word("�������......OK");delay_ms(50);	
}

void displaydate()	 //��ʾ����
{
	u16 year=0;
	u8 month=0,date=0;	

	write_com(0x30);
	write_com(0x06); //�������,DDRAMλַ������(AC)��1,�������ƶ�

	//��Һ������д �� ����
	year=calendar.w_year;
	lcm_w_test(0,0x80);	
	write_data(year/1000+'0');
	write_data((year%1000)/100+'0');	
	write_data((year%100)/10+'0');
	write_data((year%10)/1+'0');
	
	//��Һ������д �� ����
	month=calendar.w_month;
	lcm_w_word("-");	
	write_data(month/10+0x30);
	write_data((month%10)+0x30);	

	//��Һ������д �� ����
	date=calendar.w_date;
	lcm_w_word("-");		 
	write_data(date/10+0x30);
	write_data(date%10+0x30);
}
void displayxq()     //��ʾ����
{
	u8 day=calendar.week;
	write_com(0x86);	  
	lcm_w_word("��");
 	write_com(0x87);	  
	if(day==0)  {lcm_w_word("��");}
	//if(day==7)  {lcm_w_word("��");}    
  	if(day==6)  {lcm_w_word("��");}    
  	if(day==5)  {lcm_w_word("��");}    
  	if(day==4)  {lcm_w_word("��");}   
//	if(day==3)  {lcm_w_word("��");}	 //��ָ�����ȷ��ʾ���谲װ�������������ʾ��
  	if(day==3)  {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //��ָ�����lcm_w_word("��"); ��Ϊ������������ʧЧ��
  	if(day==2)  {lcm_w_word("��");}    
  	if(day==1)  {lcm_w_word("һ");}
}
void displaytime()	 //��ʾʱ��
{
	u8 hour=0,min=0,sec=0;
	//��Һ������д Сʱ ����
	hour=calendar.hour;                        //��ȡСʱ
	write1632GDRAM(1,2,num1632[hour/10]);
	write1632GDRAM(2,2,num1632[hour%10]);
	write1632GDRAM(3,2,num1632[10]);        //ʱ�ӷָ�����:��

	//��Һ������д ���� ����
	min=calendar.min;                         //��ȡ��				
	write1632GDRAM(4,2,num1632[min/10]);
	write1632GDRAM(5,2,num1632[min%10]);
	write1632GDRAM(6,2,num1632[10]);        //ʱ�ӷָ�����:��

	//��Һ������д ���� ����
	sec=calendar.sec;                         //��ȡ��
	write1632GDRAM(7,2,num1632[sec/10]);
	write1632GDRAM(8,2,num1632[sec%10]);
}
void disptemp()      //��ʾ�¶�
{
	u8 disdata[5];
	u16  tvalue;
	float temp;
	temp = DS18B20_Get_Temp();
	if(temp>=0)//�¶���0���϶�����
	{
		set1616pic(1,4,0,1); //��ʾ"�¶ȼ�ͼ��"		
	}
	else //�¶���0���϶�����
	{
		temp=-temp;
		set1616pic(1,4,0,2); //��ʾ"���¶�ͼ��"
	}
	temp*=10;
	tvalue=(u16)temp;
	disdata[0]=tvalue/100+0x30;         //ʮλ��
	disdata[1]=tvalue/10%10+0x30;           //��λ��
	disdata[2]=tvalue%10+0x30;               //С��λ    
	if(disdata[0]==0x30) //���ʮλΪ0������ʾ
	{
		disdata[0]=0x20;
	}
	write_com(0x30); 
	write_com(0x06);	
	write_com(0x99);           //��Һ������ʾ�¶���ʼλ�ã�"28.8��C"
	write_data(disdata[0]);    //��ʾʮλ 	
	write_data(disdata[1]);    //��ʾ��λ 	
	write_data(0x2e);          //��ʾС���� 	
	write_data(disdata[2]);    //��ʾС��λ
	set1616pic(4,4,0,0);	   //�ڵ�5�е�4�в����׵ġ�Cͼ��
}
void displaynl()	 //��ʾũ��
{
	Conversion(century,calendar.w_year,calendar.w_month,calendar.w_date);	//����תũ��
	write_com(0x30); write_com(0x06); write_com(0x9c);
//	if (month_moon==1) {lcm_w_word("��");}
	if (month_moon==1) {lcm_w_test(1,0xd5);lcm_w_test(1,0xfd);}
	if (month_moon==2) {lcm_w_word("��");}
//	if (month_moon==2) {lcm_w_word("��");}
	if (month_moon==3) {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);}
	if (month_moon==4) {lcm_w_word("��");}
	if (month_moon==5) {lcm_w_word("��");}
	if (month_moon==6) {lcm_w_word("��");}
	if (month_moon==7) {lcm_w_word("��");}
	if (month_moon==8) {lcm_w_word("��");}
	if (month_moon==9) {lcm_w_word("��");}
	if (month_moon==10){lcm_w_word("ʮ");}
	if (month_moon==11){lcm_w_word("��");}
	if (month_moon==12){lcm_w_word("��");}
                        lcm_w_word("��");

	if (day_moon==1)   {lcm_w_word("��һ");}
	if (day_moon==2)   {lcm_w_word("����");}
	if (day_moon==3)   {lcm_w_word("��");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //�������Ĵ���
	if (day_moon==4)   {lcm_w_word("����");}
	if (day_moon==5)   {lcm_w_word("����");}
	if (day_moon==6)   {lcm_w_word("����");}
	if (day_moon==7)   {lcm_w_word("����");}
	if (day_moon==8)   {lcm_w_word("����");}
	if (day_moon==9)   {lcm_w_word("����");}
	if (day_moon==10)  {lcm_w_word("��ʮ");}
	if (day_moon==11)  {lcm_w_word("ʮһ");}
	if (day_moon==12)  {lcm_w_word("ʮ��");}
	if (day_moon==13)  {lcm_w_word("ʮ");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);} //ʮ��
	if (day_moon==14)  {lcm_w_word("ʮ��");}
	if (day_moon==15)  {lcm_w_word("ʮ��");}
	if (day_moon==16)  {lcm_w_word("ʮ��");}
	if (day_moon==17)  {lcm_w_word("ʮ��");}
	if (day_moon==18)  {lcm_w_word("ʮ��");}
	if (day_moon==19)  {lcm_w_word("ʮ��");}
	if (day_moon==20)  {lcm_w_word("��ʮ");}
	if (day_moon==21)  {lcm_w_word("إһ");}
	if (day_moon==22)  {lcm_w_word("إ��");}
	if (day_moon==23)  {lcm_w_word("إ");lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);}	//إ��
	if (day_moon==24)  {lcm_w_word("إ��");}
	if (day_moon==25)  {lcm_w_word("إ��");}
	if (day_moon==26)  {lcm_w_word("إ��");}
	if (day_moon==27)  {lcm_w_word("إ��");}
	if (day_moon==28)  {lcm_w_word("إ��");}
	if (day_moon==29)  {lcm_w_word("إ��");}
	if (day_moon==30)  {lcm_w_test(1,0xc8);lcm_w_test(1,0xfd);lcm_w_word("ʮ");}	//��ʮ
}
