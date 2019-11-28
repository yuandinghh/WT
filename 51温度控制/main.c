/**************************************************************************************
*	2019-4-15 �¶ȿ���   Ԭ��				***/
#include "REG52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include "XPT2046.h"
#include "I2C.h"
#define TIMESEC 1000   //��ʱ��ʾ ��ǰ�¶�
typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;  
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))	  //ȡ�� ��8λ
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))	   //ȡ ��8λ
#define ON   0    //����led��
#define OFF   1   //��led��
 sbit keyincadd = P1^0;	 //����P10����	�����¶ȵ� �� +
sbit keyincsub = P1^1;	 //	 	���¶ȵ� �� -
sbit keysubadd = P1^2;	 //����P10����	�����¶ȵ� �� +
sbit keysubsub = P1^3;	 //	 	���¶ȵ� �� -
sbit keyfun =    P1^4;	 //	 ���ܼ�

sbit led=P2^7;	 //����P20����led
sbit led2=P2^6;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4 ;
u8 data   time100ms;	 //��ʱ 100ms
u8 data	currenttemp;   //��ǰ�¶�
u8 data	setaddtemp;   //���õ����ӣ��رյ��¶ȣ� �¶�
u8 data	setsubtemp;   //���õļ��٣���ʼ�򿪵������¶�
bdata  tfun ;      // ���ܼ� +   ������־λ   00000XXX  ;XXX Ϊ���ܼ��� ����λ�ֽ�
sbit relay=tfun^4 ;		//��ǰ �̵�����״̬ 

u8  data  timei	; //��ʱ����
u16 data timesec ;  //1 �� ��ʱ
u16 data  temp;		//��ǰ�¶�
u8  data  disp[8] ;		 //��ǰ �¶�����
u8 code smgduan[17]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,
						0x5e,0x79,0x71,0x40};   //16 Ϊ  - 

/***�� �� ��         : delay	   1ms		  ***/
void delay()
{
   	   time100ms = 3;
	   while(1) {
	   	if (  time100ms == 0)  return;
		}
}

/** ������   :DigDisplay()  * ��������		 :�������ʾ����   ****/
void DigDisplay()  {
    u8 i;
	timei =  tfun&0x0f;
//	if ( timei != 0 ) return ;
	    for(i=0; i<8; i++)
	    {
	       switch(i) {	 //λѡ��ѡ�����������ܣ�
		        case(0):
		            LSA=0;  LSB=0; LSC=0;	              break;//��ʾ��0λ
		        case(1):
		            LSA=1; LSB=0;    LSC=0;	              break;//��ʾ��1λ
		        case(2):
		            LSA=0;     LSB=1;  LSC=0;             break;//��ʾ��2λ
		        case(3):
		            LSA=1;	 LSB=1;  LSC=0;	              break;//��ʾ��3λ
				case(4):
					LSA=0;LSB=0;LSC=1; break;//��ʾ��4λ
				case(5):
					LSA=1;LSB=0;LSC=1; break;//��ʾ��5λ
				case(6):
					LSA=0;LSB=1;LSC=1; break;//��ʾ��6λ
				case(7):
					LSA=1;LSB=1;LSC=1; break;//��ʾ��7λ	
	        }
	        P0=disp[i];//��������
	        delay(); //���һ��ʱ��ɨ��
	    //    P0=0x00;//����
	    }
}
 /*******************************************************************************
* �� �� ��         : void Timer0() interrupt 1
* ��������		   : ��ʱ��0�жϺ���
*******************************************************************************/
void Timer0() interrupt 1	{
    static u16 ini;
    TH0=0XFC;	//����ʱ������ֵ����ʱ1ms
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
		temp = Read_AD_Data(0x94);		//   AIN0 ��λ��
//	}
//	i++;
	timei =  tfun&0x0f;
	if ( timei != 0 ) return ;	   		 //����ʾ�¶�  ���빦�ܼ�״̬
	disp[2]=smgduan[temp/1000];			//ǧλ
	timei = smgduan[temp%1000/100];	
	disp[3]=timei|0x80;;		//��λ
 	disp[4]=smgduan[temp%1000%100/10];//��λ
	disp[5]=smgduan[temp%1000%100%10];		
}
void disp0(u8 fun) {
  	disp[0]=smgduan[0];//ǧλ
	disp[1]=smgduan[0];//��λ
  	disp[2]=smgduan[15];//ǧλ
	disp[3]=smgduan[16];//��λ
	disp[4]=smgduan[fun];//��λ
	disp[5]=smgduan[0xff];
	disp[6]=smgduan[0];//ǧλ
	disp[7]=smgduan[0];//��λ	
		
 }
u8 gettemp() {
		timei = (temp/1000);			//�����¶�    
		timei = timei*10 + (temp%1000/100);	
		return 	timei;
}
/*******************************************************************************
* �� �� ��       : main
* ��������		 : ������
*******************************************************************************/
void main()
{		//	timesec	= 0xaa55;  		timei= BYTE0(timesec) ;		timei= BYTE1(timesec) ;
    TMOD|=0X01;//Timer0Init()    ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
    TH0=0XFC;	//����ʱ������ֵ����ʱ1ms
    TL0=0X18;
    ET0=1;//�򿪶�ʱ��0�ж�����
    EA=1;//�����ж�
    TR0=1;//�򿪶�ʱ��
	timei = At24c02Read(0);	  currenttemp = At24c02Read(1);
	if ( timei != 0x55 && currenttemp != 0XAA) {	   //�Ƿ��Ѿ� ��������
		setaddtemp =25;  //�ر��¶�
		At24c02Write(0,0x55); 	At24c02Write(1,0xAA); At24c02Write(2,25); 
     	 At24c02Write(3,18);   	 At24c02Write(4,0);  
	 	setsubtemp =18	;	  //�����¶�
 	}
	else {		 //�Ѿ���ʼ��
		setaddtemp =  At24c02Read(2);	setsubtemp =  At24c02Read(3);
		tfun =  At24c02Read(4);
	}
	disp[0]=smgduan[setaddtemp/10];//10λ
	disp[1]=smgduan[setaddtemp%10%10];//��λλ
    disp[6]=smgduan[setsubtemp/10];//10λ
	disp[7]=smgduan[setsubtemp%10%10];//��λλ

      while(1)   {	   //��ѭ��
	   datapros();	 	DigDisplay() ; 	    //��ʾ��ǰ�¶�
	   	timei =  tfun&0x0f;
     	if ( timei != 0 ) goto jmpkey ;	  //���ܼ�1-2  �� 0 
	 
	   if(keyincadd==0)	 {	  //��ⰴ�����Ӱ���
  	        delay();   //�������� һ���Լ10ms
	        if(keyincadd==0)	 //�ٴ��жϰ����Ƿ���
	        {
	          setaddtemp++;
			  if (setaddtemp > 99 )  { setaddtemp--; }
			  disp[0]=smgduan[setaddtemp/10];//10λ
			  disp[1]=smgduan[setaddtemp%10%10];//��λ
	        }
       	 while(!keyincadd);	 //��ⰴ���Ƿ��ɿ�
		  At24c02Write(2,setaddtemp); 
		  	timei=gettemp(); 
			if ( timei < setaddtemp )  	led = ON;	
       }
	  	if(keyincsub==0)	 {	  //��ⰴ��--����	+++++++------------
  	        delay(); 
	        if(keyincsub==0)  {	 //�ٴ��жϰ����Ƿ���
			  if ( setaddtemp != 0 )  setaddtemp--;
			  disp[0]=smgduan[setaddtemp/10];//10λ
			  disp[1]=smgduan[setaddtemp%10%10];//��λλ
	        }
     	   while(!keyincsub);	 //��ⰴ���Ƿ��ɿ�
		   At24c02Write(2,setaddtemp); 
		   	timei=gettemp();
			if ( timei > setaddtemp )  	led = OFF;
       }
 	   	   if(keysubadd==0)	 {	  //SUB ���-- ���Ӱ�  -----------++++++++++
  	        delay();  
	        if(keysubadd == 0)	 //�ٴ��жϰ����Ƿ���
	        {
	          setsubtemp++;
			  if (setsubtemp > 99 )  { setsubtemp--; }
			  disp[6]=smgduan[setsubtemp/10];//10λ
			  disp[7]=smgduan[setsubtemp%10%10];//��λλ
	        }
       	 while(!keysubadd);	 //��ⰴ���Ƿ��ɿ�
		 At24c02Write(3,setsubtemp); 
		 timei = gettemp(); //		 led = ON;
	     if ( timei < setsubtemp ) 	led = OFF;
       }
	  	if(keysubsub==0)	 {	  //��ⰴ��--���� --------   -------
  	        delay(); 
	        if(keysubsub==0)  {	 //�ٴ��жϰ����Ƿ���
			  if ( setsubtemp != 0 )  setsubtemp--;
			  disp[6]=smgduan[setsubtemp/10];//10λ
			  disp[7]=smgduan[setsubtemp%10%10];//��λλ
	        }
     	   while(!keysubsub);	 //��ⰴ���Ƿ��ɿ�
		   At24c02Write(3,setsubtemp); 
//		  timei=gettemp(); //		 led = ON;
//	     if ( timei > setsubtemp ) 	led = ON;
       }
 jmpkey:
   	if(keyfun==0)	 {	  //��ⰴ�����ܼ�����
  	        delay(); 
	    if(keyfun==0)  {
			 tfun++;	
		  timei =  tfun&0x0f;
	      switch (timei)
			{
				case 1: relay=0; 	disp0(timei);	  //�ؼ̵���
						break;
				case 2:	 relay=1;  disp0(timei);
					break;

				default:  tfun = tfun&0xf0;
					  disp[0]=smgduan[setaddtemp/10];//10λ
					  disp[1]=smgduan[setaddtemp%10%10];//��λλ
					  disp[6]=smgduan[setsubtemp/10];//10λ
					  disp[7]=smgduan[setsubtemp%10%10];//��λλ
					break;
			}
         }
     	   while(!keyfun);	 //��ⰴ���Ƿ��ɿ�
		   At24c02Write(4,tfun); 
       }
	   	
    	timei = gettemp();			//�����¶�    
		currenttemp == tfun&0x0f ;
	   if (	 currenttemp  == 0 ) {
			if( (timei < setsubtemp) ) {   //�򿪼̵���
			      led=ON;
			} 
			else if (timei > setaddtemp) {
				led = OFF;
			} 
 		}
		timei =  tfun&0x0f;	  //���� 1-2 
		led2=OFF;
		if (  timei != 0) 	{ led2=ON;  led =OFF;  }
		if (  timei == 2) 	 led =ON;

     }
}

