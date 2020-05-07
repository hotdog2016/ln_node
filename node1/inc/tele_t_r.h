/******************************************************************
                         TELE_T_R.H  file

��Ҫ���ܣ��ӿڶ��塢ȫ�ֱ������弰��������

����ʱ�䣺2013.7.30
*******************************************************************/

#ifndef __TELE_T_R_H__
#define __TELE_T_R_H__
/*--------------------------------------------------------------*/
/*---------------------------ͷ�ļ�-----------------------------*/
/*--------------------------------------------------------------*/
#include "12C5A60S2.h"

sbit PDI=P1^0;                                   //�������
sbit RDI=P3^0;                                   //�ض�
sbit GZC=P1^1;                                   //��1��2֡����
sbit TDX=P3^1;                                   //�������ݶ�

/*--------------------------------------------------------------*/
/*-------------------------ȫ�ֱ�������-------------------------*/
/*--------------------------------------------------------------*/




extern unsigned char data Tele_RT;                            //���Ľ���֡����

extern unsigned char data CRC_R;                              //���ձ��ĵ�CRCЧ����

extern unsigned char data r[15];                              //��ʱ��Ž��յı���

extern unsigned char data Tele_TT;                            //���ͱ���֡����
extern bit           Tele_TF;
extern unsigned char data CRC_T;                              //���ͱ��ĵ�CRCЧ����

extern unsigned char data t[15];                              //��ʱ�����Ҫ���͵ı���
extern unsigned char data Frame_Num_T;                        //�����ͱ���֡��Ŀ
extern unsigned char data Retry_Time;                         //���ķ������Դ���
extern bit           Tele_Second;                        //���͵ڶ�֡���ݱ�־λ



extern bit           Idle_Cheak;                         //��־��ʱ��0����;�����м��
extern unsigned char data Idle_Cheak_Counter;                 //���м�������

extern bit           GZC_Control;                        //��־��ʱ��0����;��GZC����
extern unsigned char data GZC_Control_Counter;                //GZC���Ƽ�����


extern bit           Back_Read_Cheak;                    //��־��ʱ��0����;���ض�����
extern unsigned char data Back_Read_Counter;                  //�ض����Ƽ�����

extern bit           Over_Time_Cheak;                    //��־��ʱ��0����;����ʱ���
extern unsigned char data Over_Time_Counter;                  //��ʱ��������

extern bit           Ack_Cheak;                          //��־��ʱ��0����;��ȷ�ϳ�ʱ���
extern unsigned char data Ack_Cheak_Counter;                  //ȷ�ϳ�ʱ��������

extern bit           Ack_Flag;                           //����Ӧ���־λ

extern unsigned char TMD;










/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*---------------------------��������---------------------------*/
/*--------------------------------------------------------------*/
void init_tele_t_r();
void telegram_t();

/*--------------------------------------------------------------*/
#endif