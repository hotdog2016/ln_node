#include "intrins.h"
#include "12C5A60S2.h"

#define	BUF_LENTH       100			//���崮�ڽ��ջ��峤��

#define MAIN_Fosc		11059200L	//define main clock

/*
 *define the baudrate,
 *12T mode: 600~115200 for 22.1184MHZ, 
 *					300~57600  for 11.0592MHZ
 */
#define Baudrate		9600		
									
//Calculate BRT reload value
#define BRT_Reload		(256 - MAIN_Fosc / 12 / 32 / Baudrate)		


sbit PDI=P1^0;                                   //�������
sbit RDI=P1^4;                                   //�ض�
sbit GZC=P1^1;                                   //��1��2֡����
sbit TDX=P3^1;                                   //�������ݶ�

unsigned char data receive_couts=0;
unsigned char data data_lenth=0;
unsigned char 	uart1_wr = 0;		//дָ��
unsigned char 	uart1_rd = 0;		//��ָ��
unsigned char 	xdata RX1_Buffer[BUF_LENTH];

unsigned char 	uart2_wr = 0;		//дָ��
unsigned char 	uart2_rd = 0;		//��ָ��
unsigned char 	xdata RX2_Buffer[BUF_LENTH];
unsigned char r[20];
unsigned char t[20];
unsigned char r1[20];

bit UART1SendFlag;
bit UART2SendFlag;

bit UART1RevFlag;
bit UART2RevFlag;

bit UART1_Over;
bit UART2_Over;

unsigned char temp=0;
//����1��ʼ��

void init_uart(void)
{
	// ����1   SM2=1 SM1=1 SM0=1   ������ģʽ3��������SM2=1	
	SCON |= 0xe0;
	//����1ʹ�ö��������ʷ�����
	AUXR |= 1;
	
	//ʹ�ܴ���1����
	SCON |= 1<<4;

	//����2������ģʽ1 
	S2CON |= 0x40;
	//ʹ�ܴ���2����
	S2CON |= 1<<4;

	

	//���������� 9600
	BRT=BRT_Reload;	                                        //���ò�����Ϊ9600
	//ʹ�ܶ��������ʷ�����
	AUXR |= 1<<4;

	//�崮��1�жϱ�־λ
	SCON &= ~0x03;
	//�崮��2�жϱ�־λ
	S2CON &= ~0x03;
	//������1�ж�
	IE |= 1<<4;
	//������2�ж�
	IE2 |= 1; 
	//�����ж�
	IE |= 1<<7;
}

void uart1_TxByte(unsigned char dat)
{
	UART1_Over = 0;
	TB8=1;
	SBUF = dat;
	while(!UART1_Over);
	UART1_Over = 0;
}

void uart2_TxByte(unsigned char dat)
{
	UART2_Over = 0;
	S2BUF = dat;
	while(!UART2_Over);
	UART2_Over = 0;

}
//����һ���ַ���
void write_uart1rx_buffer(unsigned char tmp[],unsigned char len)		
{
	unsigned char i;
  for (i=0; i<len; i++) 
	{
		RX1_Buffer[uart1_wr]=tmp[i];
		if(++uart1_wr >= BUF_LENTH) uart1_wr = 0;
	}
}
//����һ���ַ���

void write_uart2rx_buffer(unsigned char tmp[],unsigned char len)		
{
	unsigned char i;
    for (i=0; i<len; i++) 
	{
		RX2_Buffer[uart2_wr]=tmp[i];
		if(++uart2_wr >= BUF_LENTH) uart2_wr = 0;
	}
}

//void read_uart1rx_buffer(unsigned char tmp[],unsigned char len)
//{
//
//}
//void read_uart2rx_buffer(unsigned char tmp[],unsigned char len)
//{
//
//}
/*�ڴ���1�������ݵ�ʱ��Ҫע������TB8*/

/*����2�Ľ��մ�����
 *�жϽ��յ������Ǵ���������ģ�Ȼ�󽫵�һ���ֽ��滻��������µı��ġ�
 */

/*����1�ķ��ͺ���������һ��������
 *���ݵ�һ���ֽڵ�������ȷ����������
 */
void uart1_handle()
{
	unsigned char n;
	int i = 0;
	unsigned char xdata msg[20];
	if(uart1_wr!=uart1_rd)
	{	
		//��ȡ���յ��ı�������
		n = RX1_Buffer[uart1_rd+1]+2;
		for(i=0;i<n;i++)
		{
			uart1_TxByte(RX1_Buffer[uart1_rd]);
			msg[i] = RX1_Buffer[uart1_rd];
			if(++uart1_rd >= BUF_LENTH)	uart1_rd = 0;
		}
		//0a����1
		//0b����2
		//0cLN
		//0d����
		//���1�յ���Ϣ��Ҫô��LN��Ҫô�Ǵ���2
			//����յ��ĵ�һ�ֽ���0c��˵����LN���������ݣ�����һ�ֽڸĳ�0aת��������2
			//����յ��ĵ�һ�ֽ���0b,���Ǵ���2�������ݣ�ͨ��lnת����ȥ���Ƚ�������ݵĵ�һ�ֽڸĳ�01
		//���2�յ���Ϣ��Ҫô�Ǵ���1,Ҫô�ǵ���
			//�����һ�ֽ���0a,��˵���Ǵ���1���������ݣ�Ӧ�÷�������
			//�����һ�ֽ���0d,��˵���ǵ��Է��������ݣ���һ�ֽڻ���0b�����͸�����1
		if(0x0c==msg[0])
		{
			//����յ��ĵ�һ�ֽ���0c��˵����LN���������ݣ�����һ�ֽڸĳ�0aת��������2
			
		}
		else if(0x0b==msg[0])
		{
			//����յ��ĵ�һ�ֽ���0b,���Ǵ���2�������ݣ�ͨ��lnת����ȥ���Ƚ�������ݵĵ�һ�ֽڸĳ�01
		}
	}
}

void uart2_handle()
{
	unsigned char n;
	unsigned char xdata msg[20];
	int i;
	if(uart2_wr!=uart2_rd)
	{
		//��ȡ���յ��ı�������
		n = RX2_Buffer[uart2_rd+1]+2;
		for(i=0;i < n;i++ )
		{
			uart2_TxByte(RX2_Buffer[uart2_rd]);
			msg[i] = RX2_Buffer[uart2_rd];
			if(++uart2_rd >= BUF_LENTH)	uart2_rd = 0;
		}

		//0a����1
		//0b����2
		//0cLN
		//0d����
		if(0x0a==msg[0])
		{
			//����յ��ĵ�һ�ֽ���0a��˵���Ǵ���1���������ݣ�����һ�ֽڸĳ�0aת��������2
			
		}
		else if(0x0d==msg[0])
		{
			//����յ��ĵ�һ�ֽ���0d,���ǵ��Է������ݣ���������1
		}	
	}
}

void usart_test()
{
	uart1_handle();
	uart2_handle();
}


void init()
{
	PDI = 1;
	GZC = 0;
	RDI = 1;
}
//����1�ж�
void interrupt_uart1(void) interrupt 4
{
	if(1==RI)
	{
		RI=0;
		temp = SBUF;
		//���յ��˷��͹��������ݣ��ȷŵ�������r1[];
		if(receive_couts<1)
		{
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts==1)  //�Ӳ�ڶ����ֽڵ����ݣ�������ִ���������ݵ�����
		{
			data_lenth = temp; 
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts<data_lenth+2)
		{
			r[receive_couts] = temp;
			receive_couts++;
			if((data_lenth+2)==receive_couts)//�����Ѿ������꣬��ʼ���Ͳ���������Ӧ�ı�־λ
			{
				receive_couts = 0;
				write_uart1rx_buffer(r,data_lenth+2);
				data_lenth=0;
				//���÷��ͱ�־λ
				UART1RevFlag=1;
			}
		}
	}
	if(1==TI)
	{
		TI=0;
		UART1_Over =1;
	}
}

//����2�ж�
void interrupt_uart2(void) interrupt 8
{
		
	if(0x01&S2CON) //�����жϱ�־λ
	{
		//���ж�2���ܱ�־λ
		S2CON &= ~0x01;//1111 1110 
		temp = S2BUF;
		//���յ��˷��͹��������ݣ��ȷŵ�������r1[];
		if(receive_couts<1)
		{
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts==1)  //�Ӳ�ڶ����ֽڵ����ݣ�������ִ���������ݵ�����
		{
			data_lenth = temp; 
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts<data_lenth+2)
		{
			r[receive_couts] = temp;
			receive_couts++;
			if((data_lenth+2)==receive_couts)//�����Ѿ������꣬��ʼ���Ͳ���������Ӧ�ı�־λ
			{
				receive_couts = 0;
				write_uart2rx_buffer(r,data_lenth+2);
				data_lenth=0;
				//���÷��ͱ�־λ
				UART2RevFlag=1;
			}
		}
	}
	if(0x02&S2CON)//��������жϱ�־λ
	{
		S2CON &= ~0x02; //1111 1101
		UART2_Over =1;
	}
}

