
/*****************************************************************
                          TELE_MANAGE.h  file

主要功能：与报文循环缓冲队列相关的变量定义及函数声明

创建时间：2013.7.31
*****************************************************************/
#ifndef __TELE_MANAGE_H__
#define __TELE_MANAGE_H__
/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include "CONFIG.h"

/*--------------------------------------------------------------*/
/*-------------------------全局变量-----------------------------*/
/*--------------------------------------------------------------*/
extern unsigned char   Tele_T[QUEUE_LENGTH];               //发送报文缓冲区
extern unsigned char   Tele_R[QUEUE_LENGTH];               //接收报文缓冲区
extern unsigned char *Head_Index_T;                      //报文发送队列头地址
extern unsigned char *Tail_Index_T;                      //报文发送队列尾地址
extern unsigned char *Head_Index_R;                      //报文接收队列头地址
extern unsigned char *Tail_Index_R;                      //报文接收队列尾地址
extern bit           Queue_Empty_T;                      //报文发送队列空标志位
extern bit           Queue_Empty_R;                      //报文接收队列空标志位

extern bit           Tele_Cheak_T;                       //用于标定是否需要进行发送缓冲队列检查，检查队列中是否有报文
extern bit           Tele_Cheak_R;                       //用于标定是否需要进行发送缓冲队列检查，检查队列中是否有报文
extern bit           Tele_Handle_R;                      //接收缓冲区报文处理标志位

/*--------------------------------------------------------------*/
/*--------------------------函数声明----------------------------*/
/*--------------------------------------------------------------*/
void init_tele_manage();                          //报文队列管理初始化

unsigned char write_tele_t(unsigned char date[],unsigned char n);
unsigned char read_tele_t(unsigned char date[]);

unsigned char write_tele_r(unsigned char date[],unsigned char n);
unsigned char read_tele_r(unsigned char date[]);

void tele_cheakt();
void tele_cheakr();


#endif
