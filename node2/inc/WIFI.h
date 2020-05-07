/*****************************************************************
                          CONFIG.h  file

主要功能：ID地址及重要系统参数宏定义

创建时间：2013.7.30
*****************************************************************/
#ifndef __WIFI_H__
#define __WIFI_H__ 

#define S2RI 0x01	//用于获取接收中断请求位
#define S2TI 0x02	//用于获取发送中断请求位

extern unsigned char WIFI_R[15];
extern unsigned char WIFI_T[15];
extern unsigned char WIFI_RT;
extern unsigned char WIFI_TT;
extern unsigned char WIFI_TT1;

void init_WIFI();


#endif
