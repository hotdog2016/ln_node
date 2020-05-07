/*****************************************************************
                           CONFIG.C  file

主要功能：网关模块ID地址分配及重要参数定义

创建时间：2013.11.21
*****************************************************************/
/*--------------------------------------------------------------*/
/*-----------------------------头文件---------------------------*/
/*--------------------------------------------------------------*/
#include "CONFIG.h"


unsigned char bdata Temp1;
unsigned char bdata Temp2;

sbit Temp1_0=Temp1^0;
sbit Temp1_1=Temp1^1;
sbit Temp1_2=Temp1^2;
sbit Temp1_3=Temp1^3;
sbit Temp1_4=Temp1^4;
sbit Temp1_5=Temp1^5;
sbit Temp1_6=Temp1^6;
sbit Temp1_7=Temp1^7;
sbit Temp2_0=Temp2^0;
sbit Temp2_1=Temp2^1;
sbit Temp2_2=Temp2^2;
sbit Temp2_3=Temp2^3;
sbit Temp2_4=Temp2^4;
sbit Temp2_5=Temp2^5;
sbit Temp2_6=Temp2^6;
sbit Temp2_7=Temp2^7;


unsigned char bdata Temp3;
unsigned char bdata Temp4;

sbit Temp3_0=Temp3^0;
sbit Temp3_1=Temp3^1;
sbit Temp3_2=Temp3^2;
sbit Temp3_3=Temp3^3;
sbit Temp3_4=Temp3^4;
sbit Temp3_5=Temp3^5;
sbit Temp3_6=Temp3^6;
sbit Temp3_7=Temp3^7;
sbit Temp4_0=Temp4^0;
sbit Temp4_1=Temp4^1;
sbit Temp4_2=Temp4^2;
sbit Temp4_3=Temp4^3;
sbit Temp4_4=Temp4^4;
sbit Temp4_5=Temp4^5;
sbit Temp4_6=Temp4^6;
sbit Temp4_7=Temp4^7;

unsigned char ID1,ID2;
unsigned char REID=0,REID1=0,REID2=0;


/*--------------------------------------------------------------*/
/*--------------------------ID转换程序--------------------------*/
/*--------------------------------------------------------------*/
void id_chang()
{
	Temp1=ID;
	Temp2_0=Temp1_7;
	Temp2_1=Temp1_6;
	Temp2_2=Temp1_5;
	Temp2_3=Temp1_4;
	Temp2_4=Temp1_3;
	Temp2_5=Temp1_2;
	Temp2_6=Temp1_1;
	Temp2_7=Temp1_0;
	Temp1=Temp2;
	Temp2=0xFF;
	Temp2_7=Temp1_7;
	Temp2_5=Temp1_6;
	Temp2_3=Temp1_5;
	Temp2_1=Temp1_4;
	ID2=Temp2;
	Temp2=0xFF;
	Temp2_7=Temp1_3;
	Temp2_5=Temp1_2;
	Temp2_3=Temp1_1;
	Temp2_1=Temp1_0;
	ID1=Temp2;	
}
/*--------------------------------------------------------------*/
/*-------------------------ID反转换程序-------------------------*/
/*--------------------------------------------------------------*/
void id_rechange()
{
	Temp3=REID2;
	Temp4_7=Temp3_7;
	Temp4_6=Temp3_5;
	Temp4_5=Temp3_3;
	Temp4_4=Temp3_1;
	Temp3=REID1;
	Temp4_3=Temp3_7;
	Temp4_2=Temp3_5;
	Temp4_1=Temp3_3;
	Temp4_0=Temp3_1;
	Temp3=Temp4;
	Temp4_0=Temp3_7;
	Temp4_1=Temp3_6;
	Temp4_2=Temp3_5;
	Temp4_3=Temp3_4;
	Temp4_4=Temp3_3;
	Temp4_5=Temp3_2;
	Temp4_6=Temp3_1;
	Temp4_7=Temp3_0;
	REID=Temp4;
}