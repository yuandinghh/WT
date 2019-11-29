//2019-11-27 8051������  Ԭ��
#include "lcd.h"
#include <reg51.h>			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include "RF2401.h"	
#include "ds1302.h"
#include <intrins.h>
#include "temp.h"
#include <string.h>
//#include <stdlib.h>	   #include <stdio.h>
  
uchar CNCHAR[6] = "���϶�";
//void LcdDisplaytemp(int);
//void UsartConfiguration();
//typedef unsigned int u16;
uchar  sta,i;    //  ״̬����
#define RX_DR  (sta & 0x40)  // ���ճɹ��жϱ�־
#define TX_DS  (sta & 0x20)  // ����ɹ��жϱ�־
#define MAX_RT (sta & 0x10)  // �ط�����жϱ�־
#define sbuflen	 50   //���н��ܳ���
#define	  delayus(int)  _delay_us(int)
#define	 delayms(int)	  _delay_ms(int)
bit sbufoverflow = 0 ;	  //keil����λ����
u8  sbufc=0;
unsigned char idata sbuf[sbuflen];	  //������idata 
 static char  idata  itoap[7];      //����Ϊstatic������������ȫ�ֱ���

unsigned char SetState,SetPlace;
sbit CE=P1^0;
sbit IRQ=P3^3;
sbit CSN=P3^7;
sbit MOSI=P2^2;
sbit MISO=P2^0;
sbit SCK=P3^6;
sbit led=P2^0;  sbit LED=P2^0;       //sbit LED=P0^0;
sbit led1=P2^1;	sbit led2=P2^2;  sbit led3=P2^3;  sbit led4=P2^4;  sbit led5=P2^4;  sbit led6=P2^6;
sbit k1 = P3^1 ;    sbit  k2=  P3^0; sbit  k3 = P3^2; sbit  k4 = P3^3;  //key
uchar code TX_Addr[]={0x34,0x43,0x10,0x10,0x01};
uchar code TX_Buffer[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
uchar RX_Buffer[RX_DATA_WITDH];

unsigned char SetState,SetPlace;
//void Delay10ms(void);   //��� 0us
	
void _delay_us(uint x)	   {		 //��ʱ ����
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

void sbufsend(uchar c) {	 //���пڷ���һ���ַ�
	SBUF=c;				//�����յ������ݷ��뵽���ͼĴ���
	while(!TI);						 //�ȴ������������
	TI=0;  	 _delay_ms(10)	;
}
void sbufsendstr(char str[]) {  //�����ַ���
	uchar len,i;   len = strlen(str);
  	for(i=0;i<len;i++) sbufsend(str[i]);  }

void LcdDisplaytemp(int temp) {	 //lcd��ʾ	 �¶�ֵ	  LCD��ʾ��ȡ�����¶�    ����   : v  �¶�ֵ
  unsigned char  datas[] = {0, 0, 0, 0, 0}; //��������
  float tp;  
	if(temp< 0)				//���¶�ֵΪ����
  	{
	  	LcdWriteCom(0x80);		//д��ַ 80��ʾ��ʼ��ַ
//		SBUF='-';	while(!TI); 	TI=0;	//�����յ������ݷ��뵽���ͼĴ���
	    LcdWriteData('-');  		//��ʾ��
		//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
		temp=temp-1;   		temp=~temp;	  		tp=temp;
		temp=tp*0.0625*100+0.5; //������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�����?0.5��������С������档
  	}
 	else   	{			
	  	LcdWriteCom(0x80);		//д��ַ 80��ʾ��ʼ��ַ
	    LcdWriteData('+'); 		//��ʾ��
//		SBUF='+';	 		while(!TI);			TI=0;		 //���������ɱ�־λ
		tp=temp;//��Ϊ���ݴ�����С�������Խ��¶ȸ���һ�������ͱ�������¶���������ô����ô������ԭ����ǲ���������
		temp=tp*0.0625*100+0.5;	//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
	}
	datas[0] = temp / 10000;  	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;  	datas[3] = temp % 100 / 10;	  	datas[4] = temp % 10;

	LcdWriteCom(0x82);		  //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[0]); //��λ 
//	SBUF = '0'+datas[0];while (!TI); 	TI = 0;			//�����յ������ݷ��뵽���ͼĴ���
	LcdWriteCom(0x83);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[1]); //ʮλ
//	SBUF = '0'+datas[1];		while (!TI);  	TI = 0;
	LcdWriteCom(0x84);		//д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[2]); //��λ 
//	SBUF = '0'+datas[2];   	while (!TI);  TI = 0;
	LcdWriteCom(0x85);		//д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('.'); 		//��ʾ ��.��		SBUF = '.';	while (!TI); TI = 0;
	LcdWriteCom(0x86);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[3]); //��ʾС����  	SBUF = '0'+datas[3]; 	while (!TI);  	TI = 0;
	LcdWriteCom(0x87);		 //д��ַ 80��ʾ��ʼ��ַ
	LcdWriteData('0'+datas[4]); //��ʾС����  	SBUF = '0'+datas[4];//�����յ������ݷ��뵽���ͼĴ���  	while (!-TI);  	TI = 0;	  
//	for(i=0; i<6; i++)	 	{  	 	SBUF = CNCHAR[i]; 		while (!TI);		TI = 0;	   	}
}
void nRF24L01_Init(void)	   //nRF24L01��ʼ��
{
 CE=0;//����ģʽ��
 CSN=1;	  SCK=0; IRQ=1;   }
uchar SPI_RW(uchar byte)   //SPIʱ����
{
 uchar i;
 for(i=0;i<8;i++)//һ�ֽ�8λѭ��8��д��
 {
  if(byte&0x80)//����������λ��1
   MOSI=1;//��NRF24L01д1
  else //����д0
   MOSI=0;
  byte<<=1;//��һλ�Ƶ����λ
  SCK=1;//SCK���ߣ�д��һλ���ݣ�ͬʱ��ȡһλ����
  if(MISO)
   byte|=0x01;
  SCK=0;//SCK����
 }
 return byte;//���ض�ȡһ�ֽ�
}
//SPIд�Ĵ���һ�ֽں���
//reg:�Ĵ�����ַ
//value:һ�ֽڣ�ֵ��
uchar SPI_W_Reg(uchar reg,uchar value)
{
 uchar status;//����״̬
 CSN=0;//SPIƬѡ
 status=SPI_RW(reg);//д��Ĵ�����ַ��ͬʱ��ȡ״̬
 SPI_RW(value);//д��һ�ֽ�
 CSN=1;//
 return status;//����״̬
}
uchar SPI_R_byte(uchar reg)	   //SPI��һ�ֽ�
{
 uchar reg_value;
 CSN=0;//SPIƬѡ
 SPI_RW(reg);//д���ַ
 reg_value=SPI_RW(0);//��ȡ�Ĵ�����ֵ
 CSN=1;
 return reg_value;//���ض�ȡ��ֵ
}
//SPI��ȡRXFIFO�Ĵ�������
//reg:�Ĵ�����ַ
//Dat_Buffer:�������ȡ������
//DLen:���ݳ���
uchar SPI_R_DBuffer(uchar reg,uchar *Dat_Buffer,uchar Dlen)
{
 uchar status,i;
 CSN=0;//SPIƬѡ
 status=SPI_RW(reg);//д��Ĵ�����ַ��ͬʱ״̬
 for(i=0;i<Dlen;i++)
 {
  Dat_Buffer[i]=SPI_RW(0);//�洢����
 }
 CSN=1;
 return status;
}
//SPI��TXFIFO�Ĵ���д������		 reg:д��Ĵ�����ַ
//TX_Dat_Buffer:�����Ҫ���͵�����	 Dlen:���ݳ���
uchar SPI_W_DBuffer(uchar reg,uchar *TX_Dat_Buffer,uchar Dlen)	 {
 uchar status,i;
 CSN=0;							//SPIƬѡ������ʱ��
 status=SPI_RW(reg);
 for(i=0;i<Dlen;i++)
 {
  SPI_RW(TX_Dat_Buffer[i]);//��������
 }
 CSN=1;
 return status; 
}
void nRF24L01_Set_TX_Mode(uchar *TX_Data)	   //���÷���ģʽ
{
 CE=0;//������д�Ĵ���֮ǰһ��Ҫ�������ģʽ�����ģʽ��
 SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);//д�Ĵ���ָ��+���սڵ��ַ+��ַ���
 SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);//Ϊ�˽����豸Ӧ���źţ�����ͨ��0��ַ�뷢�͵�ַ��ͬ
 SPI_W_DBuffer(W_TX_PLOAD,TX_Data,TX_DATA_WITDH);//д��Ч���ݵ�ַ+��Ч����+��Ч���ݿ��
 SPI_W_Reg(W_REGISTER+EN_AA,0x01);//����ͨ��0�Զ�Ӧ��
 SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);//ʹ�ܽ���ͨ��0
 SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);//�Զ��ط���ʱ250US+86US���ط�10��
 SPI_W_Reg(W_REGISTER+RF_CH,0);//2.4GHZ
 SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//1Mbps����,���书��:0DBM,�������Ŵ�������
 SPI_W_Reg(W_REGISTER+CONFIG,0x0e);//����ģʽ,�ϵ�,16λCRCУ��,CRCʹ��
 CE=1;//��������
 delayms(5);//CE�ߵ�ƽ����ʱ������10US����
}
uchar Check_Ack(void)	    //���Ӧ���ź�
{
 sta=SPI_R_byte(R_REGISTER+STATUS);//��ȡ�Ĵ�״̬
 if(TX_DS||MAX_RT)//���TX_DS��MAX_RTΪ1,������жϺ����TX_FIFO�Ĵ�����ֵ
 {
  SPI_W_Reg(W_REGISTER+STATUS,0xff);
  CSN=0;
  SPI_RW(FLUSH_TX);//���û����һ��ֻ�ܷ�һ�����ݣ����Ҫע��
  CSN=1;
  return 0;
 }
 else
  return 1;
}

void nRF24L01_Set_RX_Mode(void)
{
 CE=0;				//??
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

u8 keyk1_4()   {
	if(k1==0) {		  //��ⰴ��K1�Ƿ���
		_delay_ms(10);   //�������� һ���Լ10ms
		if(k1==0) {	 //�ٴ��жϰ����Ƿ���
		  	led=~led;	return 1;  //led״̬ȡ��
		}
		while(!k1);	 //��ⰴ���Ƿ��ɿ�
	}	
	if(k2==0) {		  //��ⰴ��K1�Ƿ���
	_delay_ms(10);  //�������� һ���Լ10ms
	if(k2==0) {	 //�ٴ��жϰ����Ƿ���
		led1=~led1;	return 2; 	  //led״̬ȡ��
	}
	while(!k3);	 //��ⰴ���Ƿ��ɿ�
	}
	if(k3==0) {		  //��ⰴ��K1�Ƿ���
		_delay_ms(10);   //�������� һ���Լ10ms
		if(k3==0) {	 //�ٴ��жϰ����Ƿ���
			led2=~led2;	return 3; 	  //led״̬ȡ��
	}
	while(!k4);	 //��ⰴ���Ƿ��ɿ�
	}	
	if(k4==0) {		  //��ⰴ��K1�Ƿ���
		_delay_ms(10);  //�������� һ���Լ10ms
		if(k4==0) {	 //�ٴ��жϰ����Ƿ���
			led3=~led3;	return 4; 	  //led״̬ȡ��
		}
	}  	return 0;		 }


/*******************************************************************************
* ������         : Usart() interrupt 4
* ��������		  : ����ͨ���жϺ���
*******************************************************************************/
void Usart() interrupt 4		{   //���п��ж�
	RI = 0;						//��������жϱ�־λ
	sbuf[sbufc]	 = SBUF;   	//��ȥ���յ�������
	 sbufc++; if (sbufc >  sbuflen)   sbufoverflow=1;
//	SBUF=receiveData;				//�����յ������ݷ��뵽���ͼĴ���
//	while(!TI);						 //�ȴ������������
//	TI=0;							//���������ɱ�־λ
}
void Timer0() interrupt 1	{	 //time0 
	static u16 i;
	TH0=0XFC;	TL0=0X18; 	//����ʱ������ֵ����ʱ1ms
	i++;
	if(i==1000)	{	i=0; 	led=~led;  	}	  			//һ�� 
}

// ��������		   : �ⲿ�ж�0 �жϺ���
void Int0() interrupt 0	{
	_delay_ms(10);
	if(k3==0)
	{
		SetState=~SetState;
		SetPlace=0;
		Ds1302Init();	
	}
	led5 = ~led5;
sbufsendstr("int0");
}
void disptime() {
		if(SetState==0)	{ Ds1302ReadTime();	}	//��ʾʱ��
		else	{
			if(keyk1_4() == 1) {		//��ⰴ��K1�Ƿ���
				SetPlace++;	  _delay_us(1000);  sbufsend('B');
				if(SetPlace>=7)	SetPlace=0;					
			}
			if(keyk1_4() == 2) {		//��ⰴ��K2�Ƿ���
					TIME[SetPlace]++;
					if((TIME[SetPlace]&0x0f)>9)					 //����BCD�롣
					{
						TIME[SetPlace]=TIME[SetPlace]+6;
					}
					if((TIME[SetPlace]>=0x60)&&(SetPlace<2))		//����ֻ�ܵ�59
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x24)&&(SetPlace==2))		//Сʱֻ�ܵ�23
					{
						TIME[SetPlace]=0;
					}
					if((TIME[SetPlace]>=0x32)&&(SetPlace==3))		//��ֻ�ܵ�31
					{
						TIME[SetPlace]=0;	
					}
					if((TIME[SetPlace]>=0x13)&&(SetPlace==4))		//��ֻ�ܵ�12
					{
						TIME[SetPlace]=0;
					}	
					if((TIME[SetPlace]>=0x7)&&(SetPlace==5)) {		//��ֻ�ܵ�7
						TIME[SetPlace]=1;
					}		
			}						
		}
		LcdDisplaytime();	
}

/*  ���ܣ�����ת��Ϊ�ַ���
 * char s[] �������Ǵ洢������ÿһλ  */

char *itoa(int n) {
    int i = 0,isNegative = 0;
    if((isNegative = n) < 0) //����Ǹ�������תΪ����
    {
        n = -n;
    }
    do      //�Ӹ�λ��ʼ��Ϊ�ַ���ֱ�����λ�����Ӧ�÷�ת
    {
        itoap[i++] = n%10 + '0';
        n = n/10;
    }while(n > 0);
    if(isNegative < 0)   //����Ǹ��������ϸ���
    {
        itoap[i++] = '-';
    }
    itoap[i] = '\0';    //�������ַ���������
    return itoap  ;   //reverse(s);
}



void UsartInit()	 //ϵͳ��ʼ��
{
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X21;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XF3;		TL1=0XF3; 		//��������ʼֵ���ã�0XF3��������4800��	 th1=0xfd:19200      
	ES=1;	EA=1;	//�����ж�		//�򿪽����ж�
	TR1=1;					//�򿪼����� 	TMOD|=0X01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
	TH0=0XFC;	TL0=0X18;   	//����ʱ������ֵ����ʱ1ms
//	ET0=1;		TR0=1;			//�򿪶�ʱ��0�ж�����   //�򿪶�ʱ��
//	IT0=1;//�����س�����ʽ���½��أ�   	//����INT0
//	EX0=1;//��INT0���ж�����//	EA=1;//�����ж�	
//	 P0=0xff;  	 P1=0xff;	 P2=0xff;	 P3=0xff;
	
}


void main()	   {
  	UsartInit();  //	���ڳ�ʼ��
   _delay_us(100);
	LcdInit();	//	Ds1302Init(); //	LcdDisplay1("Disp",2);LcdDisplay2("Yuanding",3);	  //��һ�� �� x λ����ʾ
//  	for(i=0;i<16;i++) 	{ 		_delay_ms(60)	;		}		LcdInit();		//���� all ����
// int t16 ;	char str[5];
 //  	LcdWriteCom(0x88);	LcdWriteData('C');			//д��ַ 80��ʾ��ʼ��ַ
//	t16 =  Ds18b20ReadTemp();	//	itoa(t16);		  sbufsendstr("1CDDDDCC2 "); 
 //   LcdDisplaytemp(Ds18b20ReadTemp());
   	 _delay_us(100);
	 nRF24L01_Init();
    while(1)	{	   //
	 sbufsend('A'); 
	  nRF24L01_Set_RX_Mode();
	  _delay_ms(100);
	  if(nRF24L01_RX_Data())  	  {
	   led=0;							//?????????
	  	 sbufsendstr(" led0 "); 
	  }
	  else			//????
	   led=1;   	 sbufsendstr(" led1 ");   //�յ�����
	 }

 while(1)   {
  for(i=0;i<TX_DATA_WITDH-1;i++)		//����7������
  {
   nRF24L01_Set_TX_Mode(&TX_Buffer[i]);//��������
   while(Check_Ack());		//�ȴ��������
    LED=~LED;
  }
 }

}
