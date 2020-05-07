/*****************************************************************
                        TELE_MANAGE.c  file

主要功能：报文循环缓冲队列的实现与管理

创建时间：2013.11.21
*****************************************************************/
/*--------------------------------------------------------------*/
/*----------------------------头文件----------------------------*/
/*--------------------------------------------------------------*/
#include "TELE_MANAGE.h"
#include "CONFIG.h"
#include "tele_t_r.h"
#include "USB.h"
#include "WIFI.h"


unsigned char  Tele_T[QUEUE_LENGTH];               //发送报文缓冲区
unsigned char  Tele_R[QUEUE_LENGTH];               //接收报文缓冲区
unsigned char  Tele_Buf[15];
unsigned char *Head_Index_T;                      //报文发送队列头地址
unsigned char *Tail_Index_T;                      //报文发送队列尾地址
unsigned char *Head_Index_R;                      //报文接收队列头地址
unsigned char *Tail_Index_R;                      //报文接收队列尾地址

bit           Queue_Empty_T;                      //报文发送队列空标志位
bit           Queue_Empty_R;                      //报文接收队列空标志位


bit           Tele_Cheak_T;                       //用于标定是否需要进行发送缓冲队列检查，检查队列中是否有报文
bit           Tele_Cheak_R;                       //用于标定是否需要进行发送缓冲队列检查，检查队列中是否有报文
bit           Tele_Handle_R;                      //接收缓冲区报文处理标志位


bit Change_Flag;
/*--------------------------------------------------------------*/
/*------------------------报文管理初始化-----------------------*/
/*--------------------------------------------------------------*/
void init_tele_manage()
{
	Head_Index_T=Tele_T;                               //初始化报文发送队列头地址
	Tail_Index_T=Tele_T;                               //初始化报文发送队列尾地址
	Head_Index_R=Tele_R;                               //初始化报文接收队列头地址
	Tail_Index_R=Tele_R;                               //初始化报文接收队列尾地址
	
	Queue_Empty_T=1;                                   //报文发送队列为空
  Queue_Empty_R=1;                                   //报文接收队列为空
	
	Tele_Cheak_T=1;
	Tele_Cheak_R=1;
}

/*--------------------------------------------------------------*/
/*---------------------写报文发送缓冲队列----------------------*/
/*--------------------------------------------------------------*/
unsigned char write_tele_t(unsigned char date[],unsigned char n)
{
	unsigned char i;

		if((date[0]&0x0F)!=n-3)                            //对写入数据格式进行检查，这对报文缓冲区管理非常重要
			return 0;

	if(Head_Index_T==Tail_Index_T)                     //发送缓冲队列为空或者满
	{
		if(Queue_Empty_T)                                //队列为空
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
			Queue_Empty_T=0;                              //队列非空
		}
		else                                            //队列为满
			return 0;                                     
	}
	
	else if(Head_Index_T>Tail_Index_T)
	{
		if((QUEUE_LENGTH+Tail_Index_T)>=(n+Head_Index_T))  //队列剩余空间可以容纳下新报文
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
		}
		else
			return 0;
	}
	
	else
	{
		if((Tail_Index_T-Head_Index_T)>=n)              //队列剩余空间可以容纳下新报文
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_T=date[i];
				Head_Index_T++;                             //此次不需要进行头地址溢出判断
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
		}
		else
			return 0;
	}
	
	return 1;                                        //数据成功写入
}

/*--------------------------------------------------------------*/
/*---------------------读报文发送缓冲队列----------------------*/
/*--功能：将data[]里面的数值传递到*Tail_T,返回报文除ID1ID2CRC以外的帧数--*/
/*--------------------------------------------------------------*/
unsigned char read_tele_t(unsigned char date[])
{		//date[0]报文属性帧，
	unsigned char i,n;
	n=0;										//报文总帧数量
	
	if(Head_Index_T==Tail_Index_T)                   //发送缓冲队列为空或者满
	{
		if(Queue_Empty_T)                              //队列空
			return 0;
		else
		{
			date[0]=*Tail_Index_T;
			Tail_Index_T++;
			
			n=date[0]&0x0F;							//获取报文数据域帧数量

			for(i=1;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //尾地址溢出
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
			}
		}
	}
	
	else if(Head_Index_T>Tail_Index_T)
	{
		n=*Tail_Index_T&0x0F;

		if((Head_Index_T-Tail_Index_T)<(n+3))          //缓冲区剩余帧数目小于本报文该有的帧数目(缓冲区内的发送报文帧数目比报文属性里的帧数目小3)
		{                                              //少了ID1帧、ID2帧、CRC帧
			Tail_Index_T=Head_Index_T;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_T=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //尾地址溢出,,,此处不需要检查
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
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
			Tail_Index_T=Head_Index_T;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_T=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
			{
				date[i]=*Tail_Index_T;
				Tail_Index_T++;
				if((Tail_Index_T-Tele_T)==QUEUE_LENGTH)     //尾地址溢出
					Tail_Index_T=Tail_Index_T-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
			}
			if(Head_Index_T==Tail_Index_T)
				Queue_Empty_T=1;
		}
	}
	
	return (n+3);									//返回除ID1，ID2，CRC帧以外的其他帧
}


/*--------------------------------------------------------------*/
/*---------------------写报文接收缓冲队列----------------------*/
/*--------------------------------------------------------------*/
unsigned char write_tele_r(unsigned char date[],unsigned char n)
{
	unsigned char i;
//	if((date[2]&0xC0)==0x80)                             //反馈报文
//	{
//		if((date[2]&0x0F)!=(n+2-6))                            //对写入数据格式进行检查，这对报文缓冲区管理非常重要
//			return 0;
//	}
//	else
//	{
		if((date[2]&0x0F)!=(n+3-6))                            //对写入数据格式进行检查，这对报文缓冲区管理非常重要
			return 0;
//	}
	if(Head_Index_R==Tail_Index_R)                     //接收缓冲队列为空或者满
	{
		if(Queue_Empty_R)                                //队列为空
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
			Queue_Empty_R=0;                              //队列非空
		}
		else                                            //队列为满
			return 0;                                     
	}
	
	else if(Head_Index_R>Tail_Index_R)
	{
		if((QUEUE_LENGTH+Tail_Index_R)>=(n+Head_Index_R))  //队列剩余空间可以容纳下新报文
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
		}
		else
			return 0;
	}
	
	else
	{
		if((Tail_Index_R-Head_Index_R)>=n)             //队列剩余空间可以容纳下新报文
		{
			for(i=0;i<n;i++)
			{
				*Head_Index_R=date[i];
				Head_Index_R++;                            //此次不需要进行头地址溢出判断
				if((Head_Index_R-Tele_R)==QUEUE_LENGTH)     //头地址溢出
					Head_Index_R=Head_Index_R-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
		}
		else
			return 0;
	}
	
	return 1;                                        //数据成功写入
}

/*--------------------------------------------------------------*/
/*---------------------读报文接收缓冲队列----------------------*/
/*--------------------------------------------------------------*/
unsigned char read_tele_r(unsigned char date[])
{
	unsigned char i,n;
	n=0;
	
	if(Head_Index_R==Tail_Index_R)                   //发送缓冲队列为空或者满
	{
		if(Queue_Empty_R)                              //队列空
			return 0;
		else
		{
			if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)
			{
					n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F)+3;			//数据域+ID1,ID2+属性
			}
			else
			{
					n=(*(Tail_Index_R+2)&0x0F)+3;
			}
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //尾地址溢出
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
			}
		}
	}
	
	else if(Head_Index_R>Tail_Index_R)
	{
		if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)         //由报文属性帧获取报文帧数目
		{
				n=(*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F)+3;
		}
		else
		{
				n=(*(Tail_Index_R+2)&0x0F)+3;
		}             
		
		if((Head_Index_R-Tail_Index_R)<n)          //缓冲区剩余帧数目小于本报文该有的帧数目(缓冲区内的发送报文帧数目比报文属性里的帧数目小3)
		{                                              //少了ID1帧、ID2帧、CRC帧
			Tail_Index_R=Head_Index_R;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //尾地址溢出,,,此处不需要检查
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
			}
			if(Head_Index_R==Tail_Index_R)
				Queue_Empty_R=1;
		}
	}
	
	else
	{
		if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)         //由报文属性帧获取报文帧数目
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
			Tail_Index_R=Head_Index_R;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n;i++)
			{
				date[i]=*Tail_Index_R;
				Tail_Index_R++;
				if((Tail_Index_R-Tele_R)==QUEUE_LENGTH)     //尾地址溢出
					Tail_Index_R=Tail_Index_R-QUEUE_LENGTH;   //尾地址返回最小地址，实现循环缓冲
			}
			if(Head_Index_R==Tail_Index_R)
				Queue_Empty_R=1;
		}
	}
	
	return n;
}

/*--------------------------------------------------------------*/
/*----------------------报文缓冲队列检查-----------------------*/
/*--------------------------------------------------------------*/
void tele_cheakt()
{
	if(Queue_Empty_T==0)
	{
		Frame_Num_T=read_tele_t(t);						//返回值为除ID1,ID2,CRC以外的帧数量
		if(Frame_Num_T)
		{		
			Tele_TF=1;
			Tele_Cheak_T=0;                                        //停止发送缓冲队列检测
		}
	}
}

void tele_cheakr()
{
	unsigned char i;
	if(Queue_Empty_R==0)
	{
		WIFI_TT=read_tele_r(Tele_Buf);					//返回【ID1，ID2，属性+数据域】
		REID1=Tele_Buf[0];
		REID2=Tele_Buf[1];
		id_rechange();					
		WIFI_T[0]=REID;
		if(((Tele_Buf[2]&0x10)==0x10))			//若为组反馈
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
		else if((Tele_Buf[2]&0x10)==0x00)							//若为模块反馈
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
				InBuf[i]=WIFI_T[i];				//接收到的报文，写入到接收报文缓存区
				
			Tele_Cheak_R=0;
			Tele_Handle_R=1;
		}
	}
}

