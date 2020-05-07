/*****************************************************************
                          CONFIG.h  file

主要功能：ID地址及重要系统参数宏定义

创建时间：2013.7.30
*****************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__ 



/*--------------------------------------------------------------*/
/*--------------报文发送和接收模块相关参数的宏定义--------------*/
/*--------------------------------------------------------------*/
#define IDLE_TIME  64                           //认定总线空闲的时间，32*52us（16位数据长度，波特率9600）
#define OVER_TIME  32                           //报文接收超时时间，16*52us，即8位数据长度
#define ACK_TIME   40                           //等待确认报文的时间，30*52us，即15位数据长度

/*--------------------------------------------------------------*/
/*-----------------报文管理模块相关参数的宏定义-----------------*/
/*--------------------------------------------------------------*/
#define QUEUE_LENGTH  60                        //报文发送缓冲区和接受缓冲区的队列长度，40Byte

/*--------------------------------------------------------------*/
/*--------------------------系统变量声明-----------------------*/
/*--------------------------------------------------------------*/
#define ID  04                                  //网关模块ID地址

extern unsigned char ID1;                       //用于ID转换的中间变量1
extern unsigned char ID2;                       //用于ID转换的中间变量2

extern unsigned char REID1;                       //用于ID转换的中间变量1
extern unsigned char REID2;                       //用于ID转换的中间变量2
extern unsigned char REID;
extern unsigned char bdata Temp1;               //用于位运算的中间变量1
extern unsigned char bdata Temp2;               //用于位运算的中间变量2

extern  bit Temp1_0;                            //位变量定义
extern	bit Temp1_1;
extern	bit Temp1_2;
extern	bit Temp1_3;
extern	bit Temp1_4;
extern	bit Temp1_5;
extern	bit Temp1_6;
extern	bit Temp1_7;
extern	bit Temp2_0;
extern	bit Temp2_1;
extern	bit Temp2_2;
extern	bit Temp2_3;
extern	bit Temp2_4;
extern	bit Temp2_5;
extern	bit Temp2_6;
extern	bit Temp2_7;

/*--------------------------------------------------------------*/
/*---------------------------函数声明---------------------------*/
/*--------------------------------------------------------------*/

void id_chang();
void id_rechange();
#endif
