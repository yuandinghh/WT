#include "ds18b20.h"
#include "delay.h"

//IO��������
#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
//IO��������											   
#define	DS18B20_DQ_OUT PGout(11) //���ݶ˿�	PG11 
#define	DS18B20_DQ_IN  PGin(11)  //���ݶ˿�	PG11
  
//��λDS18B20
void DS18B20_Rst()	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; //����DQ
    delay_us(750);    //����750us
    DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check() 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit() 			 // read one bit
{
    u8 data;
	
	DS18B20_IO_OUT();//SET PG11 OUTPUT    
	DS18B20_DQ_OUT=0; 
	delay_us(2);
    DS18B20_DQ_OUT=1;	
	DS18B20_IO_IN();//SET PG11 INPUT
	
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte()    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}

//��ʼ�¶�ת��
void DS18B20_Start()// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init()
{
	RCC->APB2ENR|=1<<8;    //ʹ��PORTG��ʱ�� 
	GPIOG->CRH&=0XFFFF0FFF;//PORTG.11 �������
	GPIOG->CRH|=0X00003000;
	GPIOG->ODR|=1<<11;      //���1
	DS18B20_Rst();
	return DS18B20_Check();
}  

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-55.0~125.0�� 
float DS18B20_Get_Temp()
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  	    	  
    if(TH>7) //�¶�Ϊ��
    {
        TH=~TH;
        TL=~TL; 
        temp=0;  
    }
	else //�¶�Ϊ��
	{
		temp=1;
	}	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(float)tem*0.625;//ת��     
	if(temp)return tem/10.0; //�����¶�ֵ
	else return -tem/10.0;    
}
