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
/*-----------------------����2�жϳ���--------------------------*/
/*------------���ܣ���WIFIģ�鷢�ͺͽ��յĴ���------------------*/
/*--------------------------------------------------------------*/
void UART_2Interrupt(void) interrupt 8
{
	if(S2CON&0x01)								//S2RI  �����жϱ�־λ
	{							
//		if(!UsbOutFlag)			/*���ڴ˴�����USB�жϣ�UsbOutFlag=1������ֹWIFI��������*/
		{	
			S2CON=S2CON&0xFE;							//S2RI=0�����㴮��2�����жϱ�־λ
			WIFI_R[WIFI_RT]=S2BUF;
			WIFI_RT++;
			if(WIFI_RT==((WIFI_R[0]&0x0F)+3))
			{				
				write_tele_t(WIFI_R,WIFI_RT);
				WIFI_RT=0;
			}
		}
	}
	else				//�����ж�
	{
			S2CON=S2CON&0xFD;	                      //S2TI=0�����㴮��2�����жϱ�־λ
			WIFI_TT1++;
			if(WIFI_TT1<WIFI_TT)						//�����Ľ��ջ������
				S2BUF=WIFI_T[WIFI_TT1];					//���ķ��ͻ���
			else
			{
				Tele_Cheak_R=1;
				WIFI_TT1=0;
			}

	}
}