


void _delay_us(uint x)
{
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

void nRF24L01_Init(void)
{
 _delay_us(2000);
 CE=0;
 CSN=1;
 SCK=0;
 IRQ=1;
}


   
uchar SPI_RW(uchar byte)
{
 uchar i;
 for(i=0;i<8;i++)
 {
  if(byte&0x80)
   MOSI=1;
  else
   MOSI=0;
  byte<<=1;
  SCK=1;
  if(MISO)
   byte|=0x01;
  SCK=0;
 }
 return byte;
}

//uchar SPI_W_Reg(uchar reg,uchar value)
//{
// uchar status;
// CSN=0;
// status=SPI_RW(reg);
// SPI_RW(value);
// CSN=1;
// return status;
//}

//uchar SPI_R_byte(uchar reg)
//{
// uchar status;
// CSN=0;
// SPI_RW(reg);
// status=SPI_RW(0);
// CSN=1;
// return status;
//}

//uchar SPI_R_DBuffer(uchar reg,uchar *Dat_Buffer,uchar Dlen)
//{
// uchar reg_value,i;
// CSN=0;
// reg_value=SPI_RW(reg);
// for(i=0;i<Dlen;i++)
// {
//  Dat_Buffer[i]=SPI_RW(0);
// }
// CSN=1;
// return reg_value;
//}

//uchar SPI_W_DBuffer(uchar reg,uchar *TX_Dat_Buffer,uchar Dlen)
//{
// uchar reg_value,i;
// CSN=0;
// reg_value=SPI_RW(reg);
// for(i=0;i<Dlen;i++)
// {
//  SPI_RW(TX_Dat_Buffer[i]);
// }
// CSN=1;
// return reg_value; 
//}

