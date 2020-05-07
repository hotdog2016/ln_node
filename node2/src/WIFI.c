#include "12C5A60S2.h"
#include "WIFI.h"
#include "usb.h"
#include "TELE_T_R.h"
#include "TELE_MANAGE.h"





unsigned char WIFI_R[15];
unsigned char WIFI_T[15];
unsigned char WIFI_RT;
unsigned char WIFI_TT;
unsigned char WIFI_TT1;




void init_WIFI()
{
	WIFI_RT=0;
	WIFI_TT=0;
	WIFI_TT1=0;
}


/*--------------------------------------------------------------*/
/*-----------------------串口2中断程序--------------------------*/
/*------------功能：对WIFI模块发送和接收的处理------------------*/
/*--------------------------------------------------------------*/
void UART_2Interrupt(void) interrupt 8
{
	if(S2CON&0x01)								//S2RI  接收中断标志位
	{							
//		if(!UsbOutFlag)			/*可在此处增加USB判断，UsbOutFlag=1，则阻止WIFI发送数据*/
		{	
			S2CON=S2CON&0xFE;							//S2RI=0，清零串口2接收中断标志位
			WIFI_R[WIFI_RT]=S2BUF;
			WIFI_RT++;
			if(WIFI_RT==((WIFI_R[0]&0x0F)+3))
			{				
				write_tele_t(WIFI_R,WIFI_RT);
				WIFI_RT=0;
			}
		}
	}
	else				//发送中断
	{
			S2CON=S2CON&0xFD;	                      //S2TI=0，清零串口2发送中断标志位
			WIFI_TT1++;
			if(WIFI_TT1<WIFI_TT)						//读报文接收缓存队列
				S2BUF=WIFI_T[WIFI_TT1];					//报文发送缓存
			else
			{
				Tele_Cheak_R=1;
				WIFI_TT1=0;
			}

	}
}