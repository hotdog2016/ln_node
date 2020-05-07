#include "intrins.h"
#include "12C5A60S2.h"

#define	BUF_LENTH       100			//定义串口接收缓冲长度

#define MAIN_Fosc		11059200L	//define main clock

/*
 *define the baudrate,
 *12T mode: 600~115200 for 22.1184MHZ, 
 *					300~57600  for 11.0592MHZ
 */
#define Baudrate		9600		
									
//Calculate BRT reload value
#define BRT_Reload		(256 - MAIN_Fosc / 12 / 32 / Baudrate)		


sbit PDI=P1^0;                                   //高阻控制
sbit RDI=P1^4;                                   //回读
sbit GZC=P1^1;                                   //第1、2帧控制
sbit TDX=P3^1;                                   //发送数据端

unsigned char data receive_couts=0;
unsigned char data data_lenth=0;
unsigned char 	uart1_wr = 0;		//写指针
unsigned char 	uart1_rd = 0;		//读指针
unsigned char 	xdata RX1_Buffer[BUF_LENTH];

unsigned char 	uart2_wr = 0;		//写指针
unsigned char 	uart2_rd = 0;		//读指针
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
//串口1初始化

void init_uart(void)
{
	// 串口1   SM2=1 SM1=1 SM0=1   工作在模式3并且设置SM2=1	
	SCON |= 0xe0;
	//串口1使用独立波特率发生器
	AUXR |= 1;
	
	//使能串口1接收
	SCON |= 1<<4;

	//串口2工作在模式1 
	S2CON |= 0x40;
	//使能串口2接收
	S2CON |= 1<<4;

	

	//波特率设置 9600
	BRT=BRT_Reload;	                                        //设置波特率为9600
	//使能独立波特率发生器
	AUXR |= 1<<4;

	//清串口1中断标志位
	SCON &= ~0x03;
	//清串口2中断标志位
	S2CON &= ~0x03;
	//开串口1中断
	IE |= 1<<4;
	//开串口2中断
	IE2 |= 1; 
	//开总中断
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
//发送一串字符串
void write_uart1rx_buffer(unsigned char tmp[],unsigned char len)		
{
	unsigned char i;
  for (i=0; i<len; i++) 
	{
		RX1_Buffer[uart1_wr]=tmp[i];
		if(++uart1_wr >= BUF_LENTH) uart1_wr = 0;
	}
}
//发送一串字符串

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
/*在串口1发送数据的时候，要注意设置TB8*/

/*串口2的接收处理函数
 *判断接收的数据是从哪里过来的，然后将第一个字节替换掉，组成新的报文。
 */

/*串口1的发送函数，发送一个缓冲区
 *根据第一个字节的数据来确定发向哪里
 */
void uart1_handle()
{
	unsigned char n;
	int i = 0;
	unsigned char xdata msg[20];
	if(uart1_wr!=uart1_rd)
	{	
		//获取接收到的报文数量
		n = RX1_Buffer[uart1_rd+1]+2;
		for(i=0;i<n;i++)
		{
			uart1_TxByte(RX1_Buffer[uart1_rd]);
			msg[i] = RX1_Buffer[uart1_rd];
			if(++uart1_rd >= BUF_LENTH)	uart1_rd = 0;
		}
		//0a串口1
		//0b串口2
		//0cLN
		//0d电脑
		//如果1收到消息，要么是LN，要么是串口2
			//如果收到的第一字节是0c，说明是LN传来的数据，将第一字节改成0a转发给串口2
			//如果收到的第一字节是0b,则是串口2发来数据，通过ln转发出去。先将这个数据的第一字节改成01
		//如果2收到消息，要么是串口1,要么是电脑
			//如果第一字节是0a,则说明是串口1发来的数据，应该发给电脑
			//如果第一字节是0d,则说明是电脑发来的数据，第一字节换成0b，发送给串口1
		if(0x0c==msg[0])
		{
			//如果收到的第一字节是0c，说明是LN传来的数据，将第一字节改成0a转发给串口2
			
		}
		else if(0x0b==msg[0])
		{
			//如果收到的第一字节是0b,则是串口2发来数据，通过ln转发出去。先将这个数据的第一字节改成01
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
		//获取接收到的报文数量
		n = RX2_Buffer[uart2_rd+1]+2;
		for(i=0;i < n;i++ )
		{
			uart2_TxByte(RX2_Buffer[uart2_rd]);
			msg[i] = RX2_Buffer[uart2_rd];
			if(++uart2_rd >= BUF_LENTH)	uart2_rd = 0;
		}

		//0a串口1
		//0b串口2
		//0cLN
		//0d电脑
		if(0x0a==msg[0])
		{
			//如果收到的第一字节是0a，说明是串口1传来的数据，将第一字节改成0a转发给串口2
			
		}
		else if(0x0d==msg[0])
		{
			//如果收到的第一字节是0d,则是电脑发来数据，发给串口1
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
//串口1中断
void interrupt_uart1(void) interrupt 4
{
	if(1==RI)
	{
		RI=0;
		temp = SBUF;
		//接收到了发送过来的数据，先放到缓冲区r1[];
		if(receive_couts<1)
		{
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts==1)  //接测第二个字节的数据，这个数字代表后面数据的数量
		{
			data_lenth = temp; 
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts<data_lenth+2)
		{
			r[receive_couts] = temp;
			receive_couts++;
			if((data_lenth+2)==receive_couts)//数据已经接收完，开始传送并且设置相应的标志位
			{
				receive_couts = 0;
				write_uart1rx_buffer(r,data_lenth+2);
				data_lenth=0;
				//设置发送标志位
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

//串口2中断
void interrupt_uart2(void) interrupt 8
{
		
	if(0x01&S2CON) //接收中断标志位
	{
		//清中断2接受标志位
		S2CON &= ~0x01;//1111 1110 
		temp = S2BUF;
		//接收到了发送过来的数据，先放到缓冲区r1[];
		if(receive_couts<1)
		{
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts==1)  //接测第二个字节的数据，这个数字代表后面数据的数量
		{
			data_lenth = temp; 
			r[receive_couts] = temp;
			receive_couts++;
		}
		else if(receive_couts<data_lenth+2)
		{
			r[receive_couts] = temp;
			receive_couts++;
			if((data_lenth+2)==receive_couts)//数据已经接收完，开始传送并且设置相应的标志位
			{
				receive_couts = 0;
				write_uart2rx_buffer(r,data_lenth+2);
				data_lenth=0;
				//设置发送标志位
				UART2RevFlag=1;
			}
		}
	}
	if(0x02&S2CON)//发送完成中断标志位
	{
		S2CON &= ~0x02; //1111 1101
		UART2_Over =1;
	}
}

