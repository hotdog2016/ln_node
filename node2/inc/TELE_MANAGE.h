/*****************************************************************
                          TELE_MANAGE.h  file

��Ҫ���ܣ��뱨��ѭ�����������صı������弰��������

����ʱ�䣺2013.11.21
*****************************************************************/
#ifndef __TELE_MANAGE_H__
#define __TELE_MANAGE_H__
/*--------------------------------------------------------------*/
/*---------------------------ͷ�ļ�-----------------------------*/
/*--------------------------------------------------------------*/
#include "CONFIG.h"

/*--------------------------------------------------------------*/
/*-------------------------ȫ�ֱ���-----------------------------*/
/*--------------------------------------------------------------*/
extern unsigned char   Tele_T[QUEUE_LENGTH];               //���ͱ��Ļ�����
extern unsigned char   Tele_R[QUEUE_LENGTH];               //���ձ��Ļ�����

extern unsigned char  Tele_Buf[15];

extern unsigned char *Head_Index_T;                      //���ķ��Ͷ���ͷ��ַ
extern unsigned char *Tail_Index_T;                      //���ķ��Ͷ���β��ַ
extern unsigned char *Head_Index_R;                      //���Ľ��ն���ͷ��ַ
extern unsigned char *Tail_Index_R;                      //���Ľ��ն���β��ַ
extern bit           Queue_Empty_T;                      //���ķ��Ͷ��пձ�־λ
extern bit           Queue_Empty_R;                      //���Ľ��ն��пձ�־λ

extern bit           Tele_Cheak_T;                       //���ڱ궨�Ƿ���Ҫ���з��ͻ�����м�飬���������Ƿ��б���
extern bit           Tele_Cheak_R;                       //���ڱ궨�Ƿ���Ҫ���з��ͻ�����м�飬���������Ƿ��б���
extern bit           Tele_Handle_R;                      //���ջ��������Ĵ����־λ

/*--------------------------------------------------------------*/
/*--------------------------��������----------------------------*/
/*--------------------------------------------------------------*/
void init_tele_manage();                          //���Ķ��й����ʼ��

unsigned char write_tele_t(unsigned char date[],unsigned char n);
unsigned char read_tele_t(unsigned char date[]);

unsigned char write_tele_r(unsigned char date[],unsigned char n);
unsigned char read_tele_r(unsigned char date[]);

void tele_cheakt();
void tele_cheakr();


#endif