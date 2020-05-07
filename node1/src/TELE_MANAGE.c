/*****************************************************************
                        TELE_MANAGE.c  file

��Ҫ���ܣ�����ѭ��������е�ʵ�������

����ʱ�䣺2013.11.21
*****************************************************************/
/*--------------------------------------------------------------*/
/*----------------------------ͷ�ļ�----------------------------*/
/*--------------------------------------------------------------*/
#include "TELE_MANAGE.h"
#include "CONFIG.h"
#include "tele_t_r.h"
#include "USB.h"
#include "WIFI.h"


unsigned char  Tele_T[QUEUE_LENGTH];               //���ͱ��Ļ�����
unsigned char  Tele_R[QUEUE_LENGTH];               //���ձ��Ļ�����
unsigned char  Tele_Buf[15];
unsigned char *Head_Index_T;                      //���ķ��Ͷ���ͷ��ַ
unsigned char *Tail_Index_T;                      //���ķ��Ͷ���β��ַ
unsigned char *Head_Index_R;                      //���Ľ��ն���ͷ��ַ
unsigned char *Tail_Index_R;                      //���Ľ��ն���β��ַ

bit           Queue_Empty_T;                      //���ķ��Ͷ��пձ�־λ
bit           Queue_Empty_R;                      //���Ľ��ն��пձ�־λ


bit           Tele_Cheak_T;                       //���ڱ궨�Ƿ���Ҫ���з��ͻ�����м�飬���������Ƿ��б���
bit           Tele_Cheak_R;                       //���ڱ궨�Ƿ���Ҫ���з��ͻ�����м�飬���������Ƿ��б���
bit           Tele_Handle_R;                      //���ջ��������Ĵ����־λ


bit Change_Flag;
/*--------------------------------------------------------------*/
/*------------------------���Ĺ����ʼ��-----------------------*/
/*--------------------------------------------------------------*/
void init_tele_manage()
{
	Head_Index_T=Tele_T;                               //��ʼ�����ķ��Ͷ���ͷ��ַ
	Tail_Index_T=Tele_T;                               //��ʼ�����ķ��Ͷ���β��ַ
	Head_Index_R=Tele_R;                               //��ʼ�����Ľ��ն���ͷ��ַ
	Tail_Index_R=Tele_R;                               //��ʼ�����Ľ��ն���β��ַ
	
	Queue_Empty_T=1;                                   //���ķ��Ͷ���Ϊ��
  Queue_Empty_R=1;                                   //���Ľ��ն���Ϊ��
	
	Tele_Cheak_T=1;
	Tele_Cheak_R=1;
}

/*--------------------------------------------------------------*/
/*---------------------д���ķ��ͻ������----------------------*/
/*--------------------------------------------------------------*/
unsigned char write_tele_t(unsigned char date[],unsigned char n)
{
	unsigned char i;

		if((date[0]&0x0F)!=n-3)                            //��д�����ݸ�ʽ���м�飬��Ա��Ļ���������ǳ���Ҫ
			return 0;

	if(Head_Index_T==Tail_Index_T)                     //���ͻ������Ϊ�ջ�����
	{
		if(Queue_Empty_T)                                //����Ϊ��
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
			Queue_Empty_T=0;                              //���зǿ�
		}
		else                                            //����Ϊ��
			return 0;                                     
	}
	
	else if(Head_Index_T>Tail_Index_T)
	{
		if((QUEUE_LENGTH+Tail_Index_T)>=(n+Head_Index_T))  //����ʣ��ռ�����������±���
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
		}
		else
			return 0;
	}
	
	else
	{
		if((Tail_Index_T-Head_Index_T)>=n)              //����ʣ��ռ�����������±���
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;                             //�˴β���Ҫ����ͷ��ַ����ж�
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
		}
		else
			return 0;
	}
	
	return 1;                                        //���ݳɹ�д��
}

/*--------------------------------------------------------------*/
/*---------------------�����ķ��ͻ������----------------------*/
/*--���ܣ���data[]�������ֵ���ݵ�*Tail_T,���ر��ĳ�ID1ID2CRC�����֡��--*/
/*--------------------------------------------------------------*/
unsigned char read_tele_t(unsigned char date[])
{		//date[0]��������֡��
	unsigned char i,n;
	n=0;										//������֡����
	
	if(Head_Index_T==Tail_Index_T)                   //���ͻ������Ϊ�ջ�����
	{
		if(Queue_Empty_T)                              //���п�
			return 0;
		else
		{
			date[0]=*Tail_Index_T;
			Tail_Index_T++;
			
			n=date[0]&0x0F;							//��ȡ����������֡����

			for(i=1;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //β��ַ���
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
		}
	}
	
	else if(Head_Index_T>Tail_Index_T)
	{
		n=*Tail_Index_T&0x0F;

		if((Head_Index_T-Tail_Index_T)<(n+3))          //������ʣ��֡��ĿС�ڱ����ĸ��е�֡��Ŀ(�������ڵķ��ͱ���֡��Ŀ�ȱ����������֡��ĿС3)
		{                                              //����ID1֡��ID2֡��CRC֡
			Tail_Index_T=Head_Index_T;                   //�����������д��������������
			Queue_Empty_T=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //β��ַ���,,,�˴�����Ҫ���
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
			if(Head_Index_T==Tail_Index_T)
				Queue_Empty_T=1;
		}
	}
	
	else
	{
		n=*Tail_Index_T&0x0F;
	
		if((Head_Index_T-Tail_Index_T+QUEUE_LENGTH)<(n+3))
		{
			Tail_Index_T=Head_Index_T;                   //�����������д��������������
			Queue_Empty_T=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //β��ַ���
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
			if(Head_Index_T==Tail_Index_T)
				Queue_Empty_T=1;
		}
	}
	
	return (n+3);									//���س�ID1��ID2��CRC֡���������֡
}


/*--------------------------------------------------------------*/
/*---------------------д���Ľ��ջ������----------------------*/
/*--------------------------------------------------------------*/
unsigned char write_tele_r(unsigned char date[],unsigned char n)
{
	unsigned char i;
//	if((date[2]&0xC0)==0x80)                             //��������
//	{
//		if((date[2]&0x0F)!=(n+2-6))                            //��д�����ݸ�ʽ���м�飬��Ա��Ļ���������ǳ���Ҫ
//			return 0;
//	}
//	else
//	{
		if((date[2]&0x0F)!=(n+3-6))                            //��д�����ݸ�ʽ���м�飬��Ա��Ļ���������ǳ���Ҫ
			return 0;
//	}
	if(Head_Index_R==Tail_Index_R)                     //���ջ������Ϊ�ջ�����
	{
		if(Queue_Empty_R)                                //����Ϊ��
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
			Queue_Empty_R=0;                              //���зǿ�
		}
		else                                            //����Ϊ��
			return 0;                                     
	}
	
	else if(Head_Index_R>Tail_Index_R)
	{
		if((QUEUE_LENGTH+Tail_Index_R)>=(n+Head_Index_R))  //����ʣ��ռ�����������±���
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
		}
		else
			return 0;
	}
	
	else
	{
		if((Tail_Index_R-Head_Index_R)>=n)             //����ʣ��ռ�����������±���
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;                            //�˴β���Ҫ����ͷ��ַ����ж�
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //ͷ��ַ���
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //ͷ��ַ������С��ַ��ʵ��ѭ������
			}
		}
		else
			return 0;
	}
	
	return 1;                                        //���ݳɹ�д��
}

/*--------------------------------------------------------------*/
/*---------------------�����Ľ��ջ������----------------------*/
/*--------------------------------------------------------------*/
unsigned char read_tele_r(unsigned char date[])
{
	unsigned char i,n;
	n=0;
	
	if(Head_Index_R==Tail_Index_R)                   //���ͻ������Ϊ�ջ�����
	{
		if(Queue_Empty_R)                              //���п�
			return 0;
		else
		{
			if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)
			{
					n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F)+3;			//������+ID1,ID2+����
			}
			else
			{
					n=(*(Tail_Index_R+2)&0x0F)+3;
			}
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //β��ַ���
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
		}
	}
	
	else if(Head_Index_R>Tail_Index_R)
	{
		if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)         //�ɱ�������֡��ȡ����֡��Ŀ
		{
				n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F)+3;
		}
		else
		{
				n=(*(Tail_Index_R+2)&0x0F)+3;
		}             
		
		if((Head_Index_R-Tail_Index_R)<n)          //������ʣ��֡��ĿС�ڱ����ĸ��е�֡��Ŀ(�������ڵķ��ͱ���֡��Ŀ�ȱ����������֡��ĿС3)
		{                                              //����ID1֡��ID2֡��CRC֡
			Tail_Index_R=Head_Index_R;                   //�����������д��������������
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //β��ַ���,,,�˴�����Ҫ���
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
			if(Head_Index_R==Tail_Index_R)
				Queue_Empty_R=1;
		}
	}
	
	else
	{
		if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)         //�ɱ�������֡��ȡ����֡��Ŀ
		{
//			if((*(Tail_Index_R+2-QUEUE_LENGTH)&0xC0)!=0x80)
				n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F)+3;
//			else
//				n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x07)-2;
		}
		else
		{
//			if((*(Tail_Index_R+2)&0xC0)!=0x80)
				n=(*(Tail_Index_R+2)&0x0F)+3;
//			else 
//				n=(*(Tail_Index_R+2)&0x07)-2;
		}                      
		
		if((Head_Index_R-Tail_Index_R+QUEUE_LENGTH)<n)
		{
			Tail_Index_R=Head_Index_R;                   //�����������д��������������
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //β��ַ���
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //β��ַ������С��ַ��ʵ��ѭ������
			}
			if(Head_Index_R==Tail_Index_R)
				Queue_Empty_R=1;
		}
	}
	
	return n;
}

/*--------------------------------------------------------------*/
/*----------------------���Ļ�����м��-----------------------*/
/*--------------------------------------------------------------*/
void tele_cheakt()
{
	if(Queue_Empty_T==0)
	{
		Frame_Num_T=read_tele_t(t);						//����ֵΪ��ID1,ID2,CRC�����֡����
		if(Frame_Num_T)
		{		
			Tele_TF=1;
			Tele_Cheak_T=0;                                        //ֹͣ���ͻ�����м��
		}
	}
}

void tele_cheakr()
{
	unsigned char i;
	if(Queue_Empty_R==0)
	{
		WIFI_TT=read_tele_r(Tele_Buf);					//���ء�ID1��ID2������+������
		REID1=Tele_Buf[0];
		REID2=Tele_Buf[1];
		id_rechange();					
		WIFI_T[0]=REID;
		if(((Tele_Buf[2]&0x10)==0x10))			//��Ϊ�鷴��
		{
			WIFI_T[0]=Tele_Buf[5];
			WIFI_T[1]=Tele_Buf[2];
			WIFI_T[2]=Tele_Buf[3];
			WIFI_T[3]=Tele_Buf[4];
			for(i=4;i<(WIFI_TT-2);i++)
			{
				WIFI_T[i]=Tele_Buf[i+2];
			}
			Change_Flag=1;
		}
		else if((Tele_Buf[2]&0x10)==0x00)							//��Ϊģ�鷴��
		{
			for(i=1;i<(WIFI_TT-1);i++)
			{
				WIFI_T[i]=Tele_Buf[i+1];
			}
			Change_Flag=1;
		}
		
		if(Change_Flag)
		{
			Change_Flag=0;
			for(i=0;i<(WIFI_TT-1);i++)
				InBuf[i]=WIFI_T[i];				//���յ��ı��ģ�д�뵽���ձ��Ļ�����
				
			Tele_Cheak_R=0;
			Tele_Handle_R=1;
		}
	}
}

