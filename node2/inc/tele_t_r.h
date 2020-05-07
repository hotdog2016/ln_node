/******************************************************************
                         TELE_T_R.H  file

主要功能：接口定义、全局变量定义及函数申明

创建时间：2013.7.30
*******************************************************************/

#ifndef __TELE_T_R_H__
#define __TELE_T_R_H__
/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include "12C5A60S2.h"

sbit PDI=P1^0;                                   //高阻控制
sbit RDI=P3^0;                                   //回读
sbit GZC=P1^1;                                   //第1、2帧控制
sbit TDX=P3^1;                                   //发送数据端

/*--------------------------------------------------------------*/
/*-------------------------全局变量定义-------------------------*/
/*--------------------------------------------------------------*/




extern unsigned char data Tele_RT;                            //报文接收帧次数

extern unsigned char data CRC_R;                              //接收报文的CRC效验码

extern unsigned char data r[15];                              //暂时存放接收的报文

extern unsigned char data Tele_TT;                            //发送报文帧次数
extern bit           Tele_TF;
extern unsigned char data CRC_T;                              //发送报文的CRC效验码

extern unsigned char data t[15];                              //暂时存放需要发送的报文
extern unsigned char data Frame_Num_T;                        //待发送报文帧数目
extern unsigned char data Retry_Time;                         //报文发送重试次数
extern bit           Tele_Second;                        //发送第二帧数据标志位



extern bit           Idle_Cheak;                         //标志定时器0的用途，空闲检测
extern unsigned char data Idle_Cheak_Counter;                 //空闲检测计数器

extern bit           GZC_Control;                        //标志定时器0的用途，GZC控制
extern unsigned char data GZC_Control_Counter;                //GZC控制计数器


extern bit           Back_Read_Cheak;                    //标志定时器0的用途，回读控制
extern unsigned char data Back_Read_Counter;                  //回读控制计数器

extern bit           Over_Time_Cheak;                    //标志定时器0的用途，超时检测
extern unsigned char data Over_Time_Counter;                  //超时检测计数器

extern bit           Ack_Cheak;                          //标志定时器0的用途，确认超时检测
extern unsigned char data Ack_Cheak_Counter;                  //确认超时检测计数器

extern bit           Ack_Flag;                           //发送应答标志位

extern unsigned char TMD;










/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*---------------------------函数声明---------------------------*/
/*--------------------------------------------------------------*/
void init_tele_t_r();
void telegram_t();

/*--------------------------------------------------------------*/
#endif