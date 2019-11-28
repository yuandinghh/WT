typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;


#define TX_ADDR_WITDH 5//���͵�ַ��������Ϊ5���ֽ�
#define RX_ADDR_WITDH 5//���յ�ַ��������Ϊ5���ֽ�
#define TX_DATA_WITDH 8//
#define RX_DATA_WITDH 8

#define R_REGISTER    0x00  // ���Ĵ���
#define W_REGISTER    0x20  // д�Ĵ���
#define R_RX_PLOAD    0x61  // ��RX FIFO��Ч���ݣ�1-32�ֽڣ�����������ɺ����ݱ������Ӧ���ڽ���ģʽ
#define W_TX_PLOAD    0xA0  // дTX FIFO��Ч���ݣ�1-32�ֽڣ�д�������ֽ�0��ʼ��Ӧ���ڷ���ģʽ
#define FLUSH_TX    0xE1  // ���TX FIFO�Ĵ�����Ӧ���ڷ���ģʽ
#define FLUSH_RX    0xE2  // ���RX FIFO�Ĵ�����Ӧ���ڽ���ģʽ
#define REUSE_TX_PL 0xE3  // ����ʹ����һ����Ч���ݣ���CEΪ�߹����У����ݰ������ϵ����·���
#define NOP         0xFF  // �ղ���������������״̬�Ĵ���

#define CONFIG      0x00  // ���üĴ���
#define EN_AA       0x01  // ���Զ�Ӧ�𡱹��ܼĴ���
#define EN_RX_ADDR  0x02  // ����ͨ��ʹ�ܼĴ���
#define SETUP_AW    0x03  // ��ַ�������üĴ���
#define SETUP_RETR  0x04  // �Զ��ط����üĴ���
#define RF_CH       0x05  // ��Ƶͨ��Ƶ�����üĴ���
#define RF_SETUP    0x06  // ��Ƶ���üĴ���
#define STATUS      0x07  // ״̬�Ĵ���
#define OBSERVE_TX  0x08  // ���ͼ��Ĵ���
#define CD          0x09  // �ز����Ĵ���
#define RX_ADDR_P0  0x0A  // ����ͨ��0���յ�ַ�Ĵ���
#define RX_ADDR_P1  0x0B  // ����ͨ��1���յ�ַ�Ĵ���
#define RX_ADDR_P2  0x0C  // ����ͨ��2���յ�ַ�Ĵ���
#define RX_ADDR_P3  0x0D  // ����ͨ��3���յ�ַ�Ĵ���
#define RX_ADDR_P4  0x0E  // ����ͨ��4���յ�ַ�Ĵ���
#define RX_ADDR_P5  0x0F  // ����ͨ��5���յ�ַ�Ĵ���
#define TX_ADDR     0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0    0x11  // ����ͨ��0��Ч���ݿ������üĴ���
#define RX_PW_P1    0x12  // ����ͨ��1��Ч���ݿ������üĴ���
#define RX_PW_P2    0x13  // ����ͨ��2��Ч���ݿ������üĴ���
#define RX_PW_P3    0x14  // ����ͨ��3��Ч���ݿ������üĴ���
#define RX_PW_P4    0x15  // ����ͨ��4��Ч���ݿ������üĴ���
#define RX_PW_P5    0x16  // ����ͨ��5��Ч���ݿ������üĴ���
#define FIFO_STATUS 0x17  // FIFO״̬�Ĵ���