/*****************************************************************
                          CONFIG.h  file

��Ҫ���ܣ�ID��ַ����Ҫϵͳ�����궨��

����ʱ�䣺2013.7.30
*****************************************************************/
#ifndef __WIFI_H__
#define __WIFI_H__ 

#define S2RI 0x01	//���ڻ�ȡ�����ж�����λ
#define S2TI 0x02	//���ڻ�ȡ�����ж�����λ

extern unsigned char WIFI_R[15];
extern unsigned char WIFI_T[15];
extern unsigned char WIFI_RT;
extern unsigned char WIFI_TT;
extern unsigned char WIFI_TT1;

void init_WIFI();


#endif
