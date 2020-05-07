
/*****************************************************************
                        TELE_MANAGE.c  file

主要功能：报文循环缓冲队列的实现与管理

创建时间：2013.7.31
*****************************************************************/
/*--------------------------------------------------------------*/
/*----------------------------头文件----------------------------*/
/*--------------------------------------------------------------*/
#include "TELE_MANAGE.h"


unsigned char  Tele_T[QUEUE_LENGTH];               //发送报文缓冲区
unsigned char  Tele_R[QUEUE_LENGTH];               //接收报文缓冲区

unsigned char *Head_Index_T;                      //报文发送队列头地址
unsigned char *Tail_Index_T;                      //报文发送队列尾地址
unsigned char *Head_Index_R;                      //报文接收队列头地址
unsigned char *Tail_Index_R;                      //报文接收队列尾地址

bit           Queue_Empty_T;                      //报文发送队列空标志位
bit           Queue_Empty_R;                      //报文接收队列空标志位


bit           Tele_Cheak_T;        //用于标定是否需要进行发送缓冲队列检查，检查队列中是否有报文
bit           Tele_Cheak_R;        //用于标定是否需要进行接收缓冲队列检查，检查队列中是否有报文
bit           Tele_Handle_R;       //接收缓冲区报文处理标志位

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
	
	Tele_Cheak_T=1;								//报文发送缓存队列检查位，初始化置1，对报文缓存区进行检查
	Tele_Cheak_R=1;								//报文接收缓存队列检查位，初始化置1，对报文缓存区进行检查
}

/*-------------------------------------------------------------*/
/*---------------------写报文发送缓冲队列----------------------*/
/*-------------------------------------------------------------*/
unsigned char write_tele_t(unsigned char date[],unsigned char n)//不包括ID1，ID2和CRC  3帧
{											
	unsigned char i;			
										//新版协议上bit3-bit0代表数据域长度
	if((date[0]&0x0f)!=n-3)             //对写入数据格式进行检查，这对报文缓冲区管理非常重要
		return 0;				//判断数据域帧数量，即封装报文减去了【报文属性，群ID，模块ID3帧】

	if(Head_Index_T==Tail_Index_T)                     //发送缓冲队列为空或者满
	{
		if(Queue_Empty_T)                                //队列为空
		{
			for(i=0;i<n;i++)					//将n个数据写入到报文发送缓存区【不带有ID1,ID2和CRC】
			{
				*Head_Index_T=date[i];				//将date[]传递给指针变量*Head_Index
				Head_Index_T++;
				if((Head_Index_T-Tele_T)==QUEUE_LENGTH)     //头地址溢出   40
					Head_Index_T=Head_Index_T-QUEUE_LENGTH;   //头地址返回最小地址，实现循环缓冲
			}
			Queue_Empty_T=0;                              //队列非空，存有数据
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
/*---------------------读报文发送缓冲队列-----------------------*/
/*------------功能：返回缓存报文的长度--------------------------*/
/*--------------------------------------------------------------*/
unsigned char read_tele_t(unsigned char date[])
{
	unsigned char i,n;
	n=0;											//存储报文总帧数量
	
	if(Head_Index_T==Tail_Index_T)                   //发送缓冲队列为空或者满
	{
		if(Queue_Empty_T)                              //队列为空,无报文
			return 0;
		else											//队列非空
		{
			date[0]=*Tail_Index_T;				//从写完的尾指针开始读
			Tail_Index_T++;
			n=date[0]&0x0F;					//判断报文中数据域长度
	
			for(i=1;i<n+3;i++)					//n+3加上了【报文属性，群ID，接收ID】不包括ID1，ID2，CRC
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
              
		n=*Tail_Index_T&0x0F;						//由报文属性帧获取报文数据域数目		                     
		
		if((Head_Index_T-Tail_Index_T)<(n+3)) //缓冲区剩余帧数目小于本报文该有的帧数目(缓冲区内的发送报文帧数目比报文属性里的帧数目小3)
		{                                      //n+3加上了【报文属性，群ID，接收ID】不包括ID1，ID2，CRC
			Tail_Index_T=Head_Index_T;       //缓冲区数据有错误，清空所有数据
			Queue_Empty_T=1;
			return 0;
		}
		else									//缓存区能够容纳报文的帧数
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
    
		n=*Tail_Index_T&0x0F;								//由报文属性帧获取数据域数目
		
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
	
	return (n+3);					//n+3加上了【报文属性，群ID，接收ID】不包括ID1，ID2，CRC
}


/*--------------------------------------------------------------*/
/*---------------------写报文接收缓冲队列----------------------*/
/*--------------------------------------------------------------*/
unsigned char write_tele_r(unsigned char date[],unsigned char n)
{
	unsigned char i;
										      //n中含有【ID1,ID2,属性,数据域】，缺少【群ID，接收模块,CRC 3帧】
//	if((date[2]&0x0F)!=n-3)                            //对写入数据格式进行检查，这对报文缓冲区管理非常重要
//		return 0;

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
/*---------------------读报文接收缓冲队列-----------------------*/
/*                 功能:返回取出接收报文的个数值                */
/*--------------------------------------------------------------*/
unsigned char read_tele_r(unsigned char date[])
{
	unsigned char i,n;
	n=0;
	
	if(Head_Index_R==Tail_Index_R)                   //发送缓冲队列为空或者满
	{
		if(Queue_Empty_R)                              //队列空
			return 0;
		else										   //队列非空
		{
			if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)			//
			{
					n=*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F;				//n为报文数据域数量
			}
			else
			{
					n=*(Tail_Index_R+2)&0x0F;
			}
			for(i=0;i<n+3;i++)						//数据域+【ID1,ID2,属性】
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
		if(Tail_Index_R-Tele_R>=QUEUE_LENGTH-2)         
		{
				n=*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F;		  //由报文属性帧获取报文数据域数目
		}
		else
		{
				n=*(Tail_Index_R+2)&0x0F;					//由报文属性帧获取报文数据域数目
		}             
		
		if((Head_Index_R-Tail_Index_R)<(n+3))//缓冲区剩余帧数目小于本报文该有的帧数目(缓冲区内的发送报文帧数目比报文属性里的帧数目小3)
		{                                              //少了群ID、接收模块ID、CRC帧
			Tail_Index_R=Head_Index_R;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
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
				n=*(Tail_Index_R+2-QUEUE_LENGTH)&0x0F;
		}
		else
		{
				n=*(Tail_Index_R+2)&0x0F;
		}                      
		
		if((Head_Index_R-Tail_Index_R+QUEUE_LENGTH)<(n+3))
		{
			Tail_Index_R=Head_Index_R;                   //缓冲区数据有错误，清空所有数据
			Queue_Empty_R=1;
			return 0;
		}
		else
		{
			for(i=0;i<n+3;i++)
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
	
	return (n+3);							//返回【ID1，ID2，属性，数据域】不含有【群ID，接收模块ID，CRC】
}

/*-------------------------------------------------------------*/
/*----------------------报文缓冲队列检查-----------------------*/
/*-------------------------------------------------------------*/
void tele_cheakt()
{
	unsigned char i;
	if(Queue_Empty_T==0)								//队列非空，存有数据
	{
		Frame_Num_T=read_tele_t(t); //将缓存区的报文读取到t[]，返回数据域+【报文属性，群ID，接收ID】不包括ID1，ID2，CRC
		if(Frame_Num_T)							//
		{		
			if(t[2]==ID)
			{
				if(t[0]&0x10)
					  Group_Operate_Flag=1;
				else  Module_Operate_Flag=1;

				tele[2]=t[0];
				for(i=3;i<10;i++)
					tele[i]=t[i];				
				Tele_Handle_R=1;
//				led0=~led0;  	
			}
			else 
			{
				Tele_TF=1;							//有报文要发送置1--->>主函数调取报文发送函数
				Tele_Cheak_T=0;                                        //停止发送缓冲队列检测
			}
		}
	}
}

void tele_cheakr()
{
	if(Queue_Empty_R==0)							////队列非空，存有数据
		Tele_Handle_R=1;				    //接收缓冲区报文处理标志位置1--->>主函数进行报文解析
}
