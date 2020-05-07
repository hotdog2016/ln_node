/*****************************************************************
                           TELE_T_R.C  file

��Ҫ���ܣ������ĵķ��ͺͽ��գ����м�⣬��ͻ���⼰CRCЧ��
          ���൱��OSI�ο�ģ���е�MAC�Ӳ��ʵ�֣�

����ʱ�䣺2013.7.30
*****************************************************************/
/*--------------------------------------------------------------*/
/*---------------------------ͷ�ļ�-----------------------------*/
/*--------------------------------------------------------------*/
#include "12C5A60S2.h"
#include "TELE_T_R.h"
#include "CONFIG.h"
#include "TELE_MANAGE.h"
#include "USB.h"
/*--------------------------------------------------------------*/

sbit a=P1^6;
sbit b=P1^7;

unsigned char data Tele_RT;                            //���Ľ���֡����
unsigned char data CRC_R;                              //���ձ��ĵ�CRCЧ����

bit           Tele_Ring;                               //�������ڽ��ձ�־λ
unsigned char data r[15];                              //��ʱ��Ž��յı���

unsigned char data Tele_TT;                            //���ͱ���֡����
bit           Tele_TF;									//���ķ��ͱ�־λ
unsigned char data CRC_T;                              //���ͱ��ĵ�CRCЧ����

unsigned char data t[15];                              //��ʱ�����Ҫ���͵ı���
unsigned char data Frame_Num_T;                        //�����ͱ��ĵ�֡��Ŀ
unsigned char data Retry_Time;                         //���ķ������Դ���
bit           data Tele_Second;                        //���͵ڶ�֡���ݱ�־λ


bit           Idle_Cheak;                         //��־��ʱ��0����;�����м��
unsigned char data Idle_Cheak_Counter;                 //���м�������

bit           GZC_Control;                        //��־��ʱ��0����;��GZC����
unsigned char data GZC_Control_Counter;                //GZC���Ƽ�����


bit           Back_Read_Cheak;                    //��־��ʱ��0����;���ض�����
unsigned char data Back_Read_Counter;                  //�ض����Ƽ�����

bit           Over_Time_Cheak;                    //��־��ʱ��0����;����ʱ���
unsigned char data Over_Time_Counter;                  //��ʱ��������

bit           Ack_Cheak;                          //��־��ʱ��0����;��ȷ�ϳ�ʱ���
unsigned char data Ack_Cheak_Counter;                  //ȷ�ϳ�ʱ��������

bit           Ack_Flag;                           //����Ӧ���־λ

unsigned char TMD;                                 //���ڼ�����ձ���֡����






//CRCЧ�����
unsigned char code CRC_Table[]=
{
  0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
  0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
  0x23,0x7D,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
  0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
  0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
  0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
  0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
  0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
  0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
  0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
  0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
  0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
  0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
  0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
  0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
  0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35
};
/*--------------------------------------------------------------*/
/*------------------���ķ��������ģ���ʼ��-------------------*/
/*--------------------------------------------------------------*/
void init_tele_t_r()             
{
	PDI=0;                                             //����������Ϊ����
	Tele_RT=0;                                         //���Ľ���֡����
	CRC_R=0;                                           //���ձ��ĵ�CRCЧ����
	
	Tele_Ring=0;                             
	
	Tele_TT=0;                                         //���ͱ���֡����
	Tele_TF=0;
	CRC_T=0;                                           //���ͱ��ĵ�CRCЧ����
	
	Retry_Time=0;
	
	Tele_Second=0;                                     //���͵ڶ�֡���ݱ�־λ
	
	Idle_Cheak=0;                                      //��־��ʱ��0����;�����м��
	Idle_Cheak_Counter=0;                              //���м�������
	
	GZC_Control=0;                                     //��־��ʱ��0����;��GZC����
	GZC_Control_Counter=0;                             //GZC���Ƽ�����
	
	Back_Read_Cheak=0;                                 //��־��ʱ��0����;���ض�����
	Back_Read_Counter=0;                               //�ض����Ƽ�����
	
	Over_Time_Cheak=0;                                 //��־��ʱ��0����;����ʱ���
	Over_Time_Counter=0;                               //��ʱ��������
	
	Ack_Cheak=0;                                       //��־��ʱ��0����;��Ӧ����
	Ack_Cheak_Counter=0;                               //Ӧ���������
	
	Ack_Flag=0;                                        //Ӧ���־λ
	
	RDI=1;                                             //RDIΪ����ڣ�Ҫ��д��1
	
	TMOD=0x22;                                         //T0��T1������ģʽ2
	TH1=0xF8; TL1=0xF8;                                //���ò�����Ϊ9600
	//TR1=1;                                             //������ʱ��1
	
	TH0=0x80;TL0=0x80;                                 //��ʱ��0��Ϊ���ķ��Ϳ��ƵĶ�ʱ��׼����ʱʱ��Ϊһλ���ݳ��ȵ�һ��,52us
	ET0=1;                     
	TR0=1;                                             //������ʱ��0
	
	SCON=0xE0;                                         //���ô��ڹ�����ģʽ3��SM2��1��REN��0  SCON =SM0 SM1 SM2 REN TB8 RB8 TI RI
	
	S2CON=0x50;                                       //����2�����ڷ�ʽ1
	BRT=0xF8;	                                        //���ò�����Ϊ9600
	AUXR=0x11;                                        //BRTR=1 ���������ʿ�ʼ�������Ҵ���1���ö��������ʷ�����
	
	IP=0x12;                                           //���ô��ڼ���ʱ��0Ϊ�����ȼ��ж�

}

/*--------------------------------------------------------------*/
/*------------------------���ķ��ͳ���--------------------------*/
/*--------------------------------------------------------------*/
void telegram_t()
{
	if(Tele_Ring==0)                                       //����Ƿ��б������ڽ��գ���������ܽ��б��ķ���
	{
		
		Tele_TF=0;
		Tele_TT=0;                                           //���ͱ���֡������ʼ��Ϊ0
		Tele_RT=0;                                           //���ձ���֡������ʼ��Ϊ0�����ڻض�����
		Tele_Second=0;                                       //�ڶ�֡���ı�־λ��ʼ��Ϊ0
		
		CRC_T=0;                                             //����CRC��ֵ
		
		CRC_R=0;
		
		Idle_Cheak=1;                                        //�������м��
	}
}

/*--------------------------------------------------------------*/
/*-----CRC���㺯����ÿ���ͻ��߽���һ֡���Ķ���Ҫ����һ�Σ�------*/
/*--------------------------------------------------------------*/
/*unsigned char Get_CRC(unsigned char p, unsigned char crc)
{      
		crc=CRC_Table[crc^p]; 
    return crc;
}*/

/*--------------------------------------------------------------*/
/*----------------------��ʱ��0�жϷ������---------------------*/
/*--------------------------------------------------------------*/
void interrupt_timer0() interrupt 1                    //��ʱ��0���жϺ�Ϊ1
{
	/*---------------------------GZC����--------------------------*/
	if(GZC_Control)                                      //GZC����
	{
		GZC_Control_Counter++;                             //GZC���Ƽ�������1
		if(GZC_Control_Counter%2==0)
			GZC=!GZC;
		if(GZC_Control_Counter==20)                        //GZC���ƽ���
		{
			GZC=1;
			GZC_Control_Counter=0;                           //���GZC���Ƽ�����
			GZC_Control=0;                                   //ֹͣGZC����	
		}
	}
/*--------------------------------------------------------------*/	

/*---------------------------�ض�����---------------------------*/	
	if(Back_Read_Cheak)                                  //�ض�����
	{
		Back_Read_Counter++;                               //�ض���������1
		switch(Back_Read_Counter)                 
		{
			case 5: Temp1_1=RDI;
								if(PDI==1)                             //PDI������0������½��лض��Ƚϣ�����ض�������ֵ�����ڷ��͵����ݣ���PDI��Ϊ0
									if(RDI!=TDX)  
										PDI=0;
							break;
			case 9: Temp1_3=RDI;
								if(PDI==1)                             //PDI������0������½��лض��Ƚϣ�����ض�������ֵ�����ڷ��͵����ݣ���PDI��Ϊ0
									if(RDI!=TDX)   
										PDI=0;
							break;
			case 13: Temp1_5=RDI;
								if(PDI==1)                             //PDI������0������½��лض��Ƚϣ�����ض�������ֵ�����ڷ��͵����ݣ���PDI��Ϊ0
									if(RDI!=TDX)  
										PDI=0;
							 break;
			case 17: Temp1_7=RDI;
								if(PDI==1)                             //PDI������0������½��лض��Ƚϣ�����ض�������ֵ�����ڷ��͵����ݣ���PDI��Ϊ0
									if(RDI!=TDX)  
										PDI=0;
							 Back_Read_Cheak=0;                      //ֹͣ�ض�
							 Back_Read_Counter=0;                    //��ջض���������Ϊ��һ�λض������׼��
							 r[Tele_RT]=Temp1;
               Tele_RT++;
							 CRC_R=CRC_Table[CRC_R^Temp1];			     //����CRC����
							 break;					 	
		}		
	}
/*--------------------------------------------------------------*/
	
/*-----------------------------���м��-------------------------*/
	if(Idle_Cheak)                                       //���м�⣬����IDLE_TIMEλ����Ϊ1������Ϊ���߿���
	{
		if(RDI==1)                   					//�ض��ź�Ϊ1�������߿���
			Idle_Cheak_Counter++;
		else
			Idle_Cheak_Counter=0;
		if(Idle_Cheak_Counter>IDLE_TIME)                  //���߿��У����͵�һ֡����
		{
			GZC=0;                                           //��λGZC�ĳ�ʼ״̬                    
			TB8=1;                                           //���õڰ�λ��������Ϊ1
			PDI=1;                                           //ȡ������״̬������������
			REN=0;                                           //��ֹ��������
			SBUF=ID1;                                        //��һ֡дSBUF
			while(TDX)
				PDI=RDI;
			TL0=0x80;                                        //���¶�׼��ʱ��
			GZC_Control=1;                                   //��ʼGZC����
		//  Back_Read_Cheak=1;                               //��ʼ�ض����
			Tele_Second=1;	                                 //�ڶ�֡���ݷ��ͱ�־λ��1
			Temp1=0xFF;                                      //��λ������ΪFF���Ա��ڻض����ݵı���
			Idle_Cheak=0;                                    //ֹͣ���м��
			Idle_Cheak_Counter=0;                            //���м�����������
			CRC_T=CRC_Table[CRC_T^ID1];
			
			Tele_TT++;
		}
	}
/*--------------------------------------------------------------*/	
	
/*----------------------------��ʱ���--------------------------*/	
	if(Over_Time_Cheak)                                  //��ʱ���
	{
		Over_Time_Counter++;                               //��ʱ����������1
		if(Over_Time_Counter==OVER_TIME)                   //���Ľ��ճ�ʱ
		{
			a=~a;b=~b;
			Over_Time_Cheak=0;                               //ֹͣ��ʱ���
			Over_Time_Counter=0;                             //���㳬ʱ������
			Tele_Ring=0;
			SM2=1;
			Tele_RT=0;                                       //����Tele_RT
			CRC_R=0;                                         //����CRC_R��ֵ������֮ǰ�����ֵ	
			
		}
	}	
/*--------------------------------------------------------------*/	
	
/*----------------------------ȷ�ϼ��-------------------------*/
	if(Ack_Cheak)
	{
		Ack_Cheak_Counter++;
		if(Ack_Cheak_Counter==ACK_TIME)
		{
			
			Ack_Cheak=0;
			Ack_Cheak_Counter=0;
			Tele_TF=1;
			Retry_Time++;
			if(Retry_Time==3)
			{
				Tele_TF=0;
				Tele_Cheak_T=1;
				Retry_Time=0;
			}
			SM2=1;
		}
	}
}

/*--------------------------------------------------------------*/
/*-----------------------�����жϳ���---------------------------*/
/*--------------------------------------------------------------*/
void interrupt_uart()    interrupt 4                  //�жϺ�Ϊ4
{
/*-----------------------���Ľ��մ������-----------------------*/	
	if(RI==1)                         
	{
		
		RI=0;                                            //������ձ�־λRI
		Temp2=SBUF;                                      //��һʱ���ȡSBUF
		Over_Time_Cheak=0;                               //ֹͣ��ʱ���
		Over_Time_Counter=0;                             //��ճ�ʱ��������
		if(Ack_Cheak==0)
		{
			Tele_Ring=1;                                     //���ڽ��ձ���
			if(Tele_RT<2)
			{
				
				r[Tele_RT]=Temp2;			//ID1,ID2
				Tele_RT++;
				
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
				
			}
			else if(Tele_RT==2)						//��������֡
			{
				r[Tele_RT]=Temp2;
				Tele_RT++;
					
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
								
				TMD=Temp2&0x0F;						
			}
			else if(Tele_RT<4)							//ȺID
			{
				
				Tele_RT++;
				
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
			}
			else if(Tele_RT==4)							//ģ��ID
			{

				if(Temp2==ID)//�����ͬ����˵���˱����Ƿ��͸���ģ���	//ʹSM2λΪ0���Խ��պ�������
				{
					SM2=0;										//�ж�IDƥ�䣬����һ֡����
					Tele_RT++;                                   //����֡������1
				                              
					CRC_R=CRC_Table[CRC_R^Temp2];
					Over_Time_Cheak=1;                           //��ʱ��2��;��־λ��1���������ĳ�ʱ���
						
					
				} 				
				else
				{					
					Tele_RT=0;                //�˴α��Ĳ��Ƿ��͸���ģ��ģ�����Tele_RT,����ǰ������������� 					
					CRC_R=0; 
					Tele_Ring=0;
				}
			}
			
						
			else if(Tele_RT<(TMD+6-1)) 
			{
				
				r[Tele_RT-2]=Temp2;                       //�����ݱ��浽r��
				Tele_RT++;                                   //����֡������1
				                            
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;                
			}
			else// if(Tele_RT>=(TMD+6-1))						//���ķ�����֡����
			{
							
				if(Temp2==CRC_R)
				{

					if((r[2]&0xC0)!=0x80)                    //�������Ĳ���Ҫȷ��֡
					{
						PDI=1;
						TB8=0;
						REN=0;
						SBUF=0xAA;
						Ack_Flag=1;
						write_tele_r(r,(Tele_RT-2));		//�Ƿ��������ٴ洢2֡
					}
					else
						write_tele_r(r,(Tele_RT-2));		//��������Ҳ�ٴ洢2֡
				}
				
				
				CRC_R=0;                                   //����CRC_R��ֵ��׼���´ν��ձ��ļ���
				Tele_Ring=0;
				SM2=1;                                     //��λSM2��׼�������µı���
				Tele_RT=0;                                 //�ָ�Tele_RT,׼�������µı���
			}
		}
		else
		{
			Ack_Cheak=0;
			Ack_Cheak_Counter=0;
			
			if(1)
			//if(Temp2==0xAA)
			{
				
				
				Tele_Cheak_T=1;
				Retry_Time=0;
			}
			else
			{
				Tele_TF=1;
				Retry_Time++;
				if(Retry_Time==3)
				{
					Tele_TF=0;
					Tele_Cheak_T=1;
					Retry_Time=0;
				}
			}
			SM2=1;
		}
	}	
/*--------------------------------------------------------------*/
	
/*-----------------------���ķ��ʹ������-----------------------*/	
	else if(Ack_Flag)
	{
		
		
		TI=0;
		PDI=0;
		Ack_Flag=0;
		REN=1;
	}
	else if(PDI==0)                                    //δȡ�����߿���Ȩ 
	{
		
		
		REN=1;                                           //�ָ����Ľ���
		Tele_Ring=1;                                     //���ڽ��ձ���
		TI=0;                                            //��շ��ͱ�־λ
		Tele_TF=1;                                       //����δ�ɹ����ͣ���λ���ķ��ͱ�־λ
		Retry_Time++;                                    //���Դ�����1
		Over_Time_Cheak=1;                               //�������ĳ�ʱ���
		CRC_T=0;                                         //����֮ǰ����ķ���CRC
		
		if(Retry_Time==10)                               //����10�ζ�û�з��ͳ�ȥ���������������
		{
			Tele_TF=0;
			Tele_Cheak_T=1;
			Retry_Time=0;
		}
	}
	
	else if(Tele_Second)                               //���еڶ�֡���ݷ���
	{
		TI=0;                                            //��һʱ�����㷢���жϱ�־λ
		GZC=0;                                           //�ڶ�֡���ݣ������ø������1                     
		TB8=1;                                           //���õڰ�λ��������Ϊ1
		SBUF=ID2;                                        //�ڶ�֡����д��SBUF
	  while(TDX)
			PDI=RDI;		
		TL0=0x80;                                        //���¶�׼��ʱ��
		GZC_Control=1;                                   //����GZC����
	//	Back_Read_Cheak=1;                               //�����ض����
		Tele_Second=0;                                   //�ڶ�֡��־λ����
		Tele_TT++;                                       //��������֡������1
		CRC_T=CRC_Table[CRC_T^ID2];
		
	}
	
	else                                               //�����������ݷ���
	{
		TI=0;                                            //��һʱ�����㷢���жϱ�־λ
		Tele_RT=0;                                       //����Tele_RT�������ض�ʱ���������
		CRC_R=0;                                         //����CRC_R��ֵ������֮ǰ�����ֵ
		
		Tele_TT++;

		if(Tele_TT>5)                       
			TB8=0;                     
		else
			TB8=1;			
	
		
		if(Tele_TT<(Frame_Num_T+3))            //�������֡�Ƿ�����//����(��ID1ID2CRC)+3=�ܳ���
		{
			SBUF=t[Tele_TT-3];
			CRC_T=CRC_Table[CRC_T^t[Tele_TT-3]];
		
		}
		else if(Tele_TT==(Frame_Num_T+3))       //�����ܳ������һλ           //����Ƿ���CRCЧ����
			SBUF=CRC_T;                                   //����CRCЧ����
		else                                             //����֡������
		{
			
			REN=1;                                         //�ָ���������
			PDI=0;                                         //�������߸���
			if((t[0]&0xC0)!=0x80)                          //�������Ĳ���Ҫȷ��֡
			{
				SM2=0;                                       //׼������ȷ��֡
				Ack_Cheak=1;                                 //����ȷ�ϳ�ʱ��⣬�ڹ涨��ʱ���ڱ��Ľ��սڵ�Ҫ����ȷ��֡
			}
			else                                           //����Ƿ������ģ�ֱ����λ���ķ��ͼ���־λ
				Tele_Cheak_T=1;
		}			
	}
}
