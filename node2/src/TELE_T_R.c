/*****************************************************************
                           TELE_T_R.C  file

主要功能：负责报文的发送和接收，空闲检测，冲突避免及CRC效验
          （相当于OSI参考模型中的MAC子层的实现）

创建时间：2013.7.30
*****************************************************************/
/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include "12C5A60S2.h"
#include "TELE_T_R.h"
#include "CONFIG.h"
#include "TELE_MANAGE.h"
#include "USB.h"
/*--------------------------------------------------------------*/

sbit a=P1^6;
sbit b=P1^7;

unsigned char data Tele_RT;                            //报文接收帧次数
unsigned char data CRC_R;                              //接收报文的CRC效验码

bit           Tele_Ring;                               //报文正在接收标志位
unsigned char data r[15];                              //暂时存放接收的报文

unsigned char data Tele_TT;                            //发送报文帧次数
bit           Tele_TF;									//报文发送标志位
unsigned char data CRC_T;                              //发送报文的CRC效验码

unsigned char data t[15];                              //暂时存放需要发送的报文
unsigned char data Frame_Num_T;                        //待发送报文的帧数目
unsigned char data Retry_Time;                         //报文发送重试次数
bit           data Tele_Second;                        //发送第二帧数据标志位


bit           Idle_Cheak;                         //标志定时器0的用途，空闲检测
unsigned char data Idle_Cheak_Counter;                 //空闲检测计数器

bit           GZC_Control;                        //标志定时器0的用途，GZC控制
unsigned char data GZC_Control_Counter;                //GZC控制计数器


bit           Back_Read_Cheak;                    //标志定时器0的用途，回读控制
unsigned char data Back_Read_Counter;                  //回读控制计数器

bit           Over_Time_Cheak;                    //标志定时器0的用途，超时检测
unsigned char data Over_Time_Counter;                  //超时检测计数器

bit           Ack_Cheak;                          //标志定时器0的用途，确认超时检测
unsigned char data Ack_Cheak_Counter;                  //确认超时检测计数器

bit           Ack_Flag;                           //发送应答标志位

unsigned char TMD;                                 //用于计算接收报文帧数量






//CRC效验码表
unsigned char code CRC_Table[]=
{
  0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
  0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
  0x23,0x7D,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
  0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
  0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
  0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
  0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
  0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
  0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
  0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
  0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
  0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
  0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
  0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
  0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
  0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35
};
/*--------------------------------------------------------------*/
/*------------------报文发送与接收模块初始化-------------------*/
/*--------------------------------------------------------------*/
void init_tele_t_r()             
{
	PDI=0;                                             //将总线设置为高阻
	Tele_RT=0;                                         //报文接收帧次数
	CRC_R=0;                                           //接收报文的CRC效验码
	
	Tele_Ring=0;                             
	
	Tele_TT=0;                                         //发送报文帧次数
	Tele_TF=0;
	CRC_T=0;                                           //发送报文的CRC效验码
	
	Retry_Time=0;
	
	Tele_Second=0;                                     //发送第二帧数据标志位
	
	Idle_Cheak=0;                                      //标志定时器0的用途，空闲检测
	Idle_Cheak_Counter=0;                              //空闲检测计数器
	
	GZC_Control=0;                                     //标志定时器0的用途，GZC控制
	GZC_Control_Counter=0;                             //GZC控制计数器
	
	Back_Read_Cheak=0;                                 //标志定时器0的用途，回读控制
	Back_Read_Counter=0;                               //回读控制计数器
	
	Over_Time_Cheak=0;                                 //标志定时器0的用途，超时检测
	Over_Time_Counter=0;                               //超时检测计数器
	
	Ack_Cheak=0;                                       //标志定时器0的用途，应答检测
	Ack_Cheak_Counter=0;                               //应答检测计数器
	
	Ack_Flag=0;                                        //应答标志位
	
	RDI=1;                                             //RDI为输入口，要先写入1
	
	TMOD=0x22;                                         //T0和T1工作于模式2
	TH1=0xF8; TL1=0xF8;                                //设置波特率为9600
	//TR1=1;                                             //启动定时器1
	
	TH0=0x80;TL0=0x80;                                 //定时器0作为报文发送控制的定时基准，定时时间为一位数据长度的一半,52us
	ET0=1;                     
	TR0=1;                                             //启动定时器0
	
	SCON=0xE0;                                         //设置串口工作于模式3，SM2置1，REN置0  SCON =SM0 SM1 SM2 REN TB8 RB8 TI RI
	
	S2CON=0x50;                                       //串口2工作于方式1
	BRT=0xF8;	                                        //设置波特率为9600
	AUXR=0x11;                                        //BRTR=1 独立波特率开始计数并且串口1采用独立波特率发生器
	
	IP=0x12;                                           //设置串口及定时器0为高优先级中断

}

/*--------------------------------------------------------------*/
/*------------------------报文发送程序--------------------------*/
/*--------------------------------------------------------------*/
void telegram_t()
{
	if(Tele_Ring==0)                                       //检测是否有报文正在接收，如果有则不能进行报文发送
	{
		
		Tele_TF=0;
		Tele_TT=0;                                           //发送报文帧次数初始化为0
		Tele_RT=0;                                           //接收报文帧次数初始化为0，用于回读接收
		Tele_Second=0;                                       //第二帧报文标志位初始化为0
		
		CRC_T=0;                                             //清零CRC的值
		
		CRC_R=0;
		
		Idle_Cheak=1;                                        //启动空闲检测
	}
}

/*--------------------------------------------------------------*/
/*-----CRC计算函数（每发送或者接收一帧报文都需要计算一次）------*/
/*--------------------------------------------------------------*/
/*unsigned char Get_CRC(unsigned char p, unsigned char crc)
{      
		crc=CRC_Table[crc^p]; 
    return crc;
}*/

/*--------------------------------------------------------------*/
/*----------------------定时器0中断服务程序---------------------*/
/*--------------------------------------------------------------*/
void interrupt_timer0() interrupt 1                    //定时器0的中断号为1
{
	/*---------------------------GZC控制--------------------------*/
	if(GZC_Control)                                      //GZC控制
	{
		GZC_Control_Counter++;                             //GZC控制计数器加1
		if(GZC_Control_Counter%2==0)
			GZC=!GZC;
		if(GZC_Control_Counter==20)                        //GZC控制结束
		{
			GZC=1;
			GZC_Control_Counter=0;                           //清空GZC控制计数器
			GZC_Control=0;                                   //停止GZC控制	
		}
	}
/*--------------------------------------------------------------*/	

/*---------------------------回读控制---------------------------*/	
	if(Back_Read_Cheak)                                  //回读控制
	{
		Back_Read_Counter++;                               //回读计数器加1
		switch(Back_Read_Counter)                 
		{
			case 5: Temp1_1=RDI;
								if(PDI==1)                             //PDI不等于0的情况下进行回读比较，如果回读到的数值不等于发送的数据，则将PDI置为0
									if(RDI!=TDX)  
										PDI=0;
							break;
			case 9: Temp1_3=RDI;
								if(PDI==1)                             //PDI不等于0的情况下进行回读比较，如果回读到的数值不等于发送的数据，则将PDI置为0
									if(RDI!=TDX)   
										PDI=0;
							break;
			case 13: Temp1_5=RDI;
								if(PDI==1)                             //PDI不等于0的情况下进行回读比较，如果回读到的数值不等于发送的数据，则将PDI置为0
									if(RDI!=TDX)  
										PDI=0;
							 break;
			case 17: Temp1_7=RDI;
								if(PDI==1)                             //PDI不等于0的情况下进行回读比较，如果回读到的数值不等于发送的数据，则将PDI置为0
									if(RDI!=TDX)  
										PDI=0;
							 Back_Read_Cheak=0;                      //停止回读
							 Back_Read_Counter=0;                    //清空回读计数器，为下一次回读检测做准备
							 r[Tele_RT]=Temp1;
               Tele_RT++;
							 CRC_R=CRC_Table[CRC_R^Temp1];			     //进行CRC计算
							 break;					 	
		}		
	}
/*--------------------------------------------------------------*/
	
/*-----------------------------空闲检测-------------------------*/
	if(Idle_Cheak)                                       //空闲检测，连续IDLE_TIME位数据为1，则认为总线空闲
	{
		if(RDI==1)                   					//回读信号为1，代表线空闲
			Idle_Cheak_Counter++;
		else
			Idle_Cheak_Counter=0;
		if(Idle_Cheak_Counter>IDLE_TIME)                  //总线空闲，发送第一帧数据
		{
			GZC=0;                                           //置位GZC的初始状态                    
			TB8=1;                                           //设置第八位发送数据为1
			PDI=1;                                           //取消高阻状态，允许发送数据
			REN=0;                                           //禁止接收数据
			SBUF=ID1;                                        //第一帧写SBUF
			while(TDX)
				PDI=RDI;
			TL0=0x80;                                        //重新对准定时器
			GZC_Control=1;                                   //开始GZC控制
		//  Back_Read_Cheak=1;                               //开始回读检测
			Tele_Second=1;	                                 //第二帧数据发送标志位置1
			Temp1=0xFF;                                      //将位变量置为FF，以便于回读数据的保存
			Idle_Cheak=0;                                    //停止空闲检测
			Idle_Cheak_Counter=0;                            //空闲检测计数器清零
			CRC_T=CRC_Table[CRC_T^ID1];
			
			Tele_TT++;
		}
	}
/*--------------------------------------------------------------*/	
	
/*----------------------------超时检测--------------------------*/	
	if(Over_Time_Cheak)                                  //超时检测
	{
		Over_Time_Counter++;                               //超时检测计数器加1
		if(Over_Time_Counter==OVER_TIME)                   //报文接收超时
		{
			a=~a;b=~b;
			Over_Time_Cheak=0;                               //停止超时检测
			Over_Time_Counter=0;                             //清零超时计数器
			Tele_Ring=0;
			SM2=1;
			Tele_RT=0;                                       //清零Tele_RT
			CRC_R=0;                                         //清零CRC_R的值，放弃之前计算的值	
			
		}
	}	
/*--------------------------------------------------------------*/	
	
/*----------------------------确认检测-------------------------*/
	if(Ack_Cheak)
	{
		Ack_Cheak_Counter++;
		if(Ack_Cheak_Counter==ACK_TIME)
		{
			
			Ack_Cheak=0;
			Ack_Cheak_Counter=0;
			Tele_TF=1;
			Retry_Time++;
			if(Retry_Time==3)
			{
				Tele_TF=0;
				Tele_Cheak_T=1;
				Retry_Time=0;
			}
			SM2=1;
		}
	}
}

/*--------------------------------------------------------------*/
/*-----------------------串口中断程序---------------------------*/
/*--------------------------------------------------------------*/
void interrupt_uart()    interrupt 4                  //中断号为4
{
/*-----------------------报文接收处理程序-----------------------*/	
	if(RI==1)                         
	{
		
		RI=0;                                            //清零接收标志位RI
		Temp2=SBUF;                                      //第一时间读取SBUF
		Over_Time_Cheak=0;                               //停止超时检测
		Over_Time_Counter=0;                             //清空超时检测计数器
		if(Ack_Cheak==0)
		{
			Tele_Ring=1;                                     //正在接收报文
			if(Tele_RT<2)
			{
				
				r[Tele_RT]=Temp2;			//ID1,ID2
				Tele_RT++;
				
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
				
			}
			else if(Tele_RT==2)						//报文属性帧
			{
				r[Tele_RT]=Temp2;
				Tele_RT++;
					
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
								
				TMD=Temp2&0x0F;						
			}
			else if(Tele_RT<4)							//群ID
			{
				
				Tele_RT++;
				
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;
			}
			else if(Tele_RT==4)							//模块ID
			{

				if(Temp2==ID)//如果相同，则说明此报文是发送给本模块的	//使SM2位为0，以接收后续数据
				{
					SM2=0;										//判断ID匹配，从下一帧接收
					Tele_RT++;                                   //接受帧次数加1
				                              
					CRC_R=CRC_Table[CRC_R^Temp2];
					Over_Time_Cheak=1;                           //定时器2用途标志位置1，启动报文超时检测
						
					
				} 				
				else
				{					
					Tele_RT=0;                //此次报文不是发送给本模块的，清零Tele_RT,放弃前面所保存的数据 					
					CRC_R=0; 
					Tele_Ring=0;
				}
			}
			
						
			else if(Tele_RT<(TMD+6-1)) 
			{
				
				r[Tele_RT-2]=Temp2;                       //将数据保存到r中
				Tele_RT++;                                   //接受帧次数加1
				                            
				CRC_R=CRC_Table[CRC_R^Temp2];
				Over_Time_Cheak=1;                
			}
			else// if(Tele_RT>=(TMD+6-1))						//报文发送总帧数量
			{
							
				if(Temp2==CRC_R)
				{

					if((r[2]&0xC0)!=0x80)                    //反馈报文不需要确认帧
					{
						PDI=1;
						TB8=0;
						REN=0;
						SBUF=0xAA;
						Ack_Flag=1;
						write_tele_r(r,(Tele_RT-2));		//非反馈报文少存储2帧
					}
					else
						write_tele_r(r,(Tele_RT-2));		//反馈报文也少存储2帧
				}
				
				
				CRC_R=0;                                   //清零CRC_R的值，准备下次接收报文计算
				Tele_Ring=0;
				SM2=1;                                     //置位SM2，准备接收新的报文
				Tele_RT=0;                                 //恢复Tele_RT,准备接收新的报文
			}
		}
		else
		{
			Ack_Cheak=0;
			Ack_Cheak_Counter=0;
			
			if(1)
			//if(Temp2==0xAA)
			{
				
				
				Tele_Cheak_T=1;
				Retry_Time=0;
			}
			else
			{
				Tele_TF=1;
				Retry_Time++;
				if(Retry_Time==3)
				{
					Tele_TF=0;
					Tele_Cheak_T=1;
					Retry_Time=0;
				}
			}
			SM2=1;
		}
	}	
/*--------------------------------------------------------------*/
	
/*-----------------------报文发送处理程序-----------------------*/	
	else if(Ack_Flag)
	{
		
		
		TI=0;
		PDI=0;
		Ack_Flag=0;
		REN=1;
	}
	else if(PDI==0)                                    //未取得总线控制权 
	{
		
		
		REN=1;                                           //恢复报文接收
		Tele_Ring=1;                                     //正在接收报文
		TI=0;                                            //清空发送标志位
		Tele_TF=1;                                       //报文未成功发送，置位报文发送标志位
		Retry_Time++;                                    //重试次数加1
		Over_Time_Cheak=1;                               //启动报文超时检测
		CRC_T=0;                                         //放弃之前计算的发送CRC
		
		if(Retry_Time==10)                               //重试10次都没有发送出去，则放弃此条报文
		{
			Tele_TF=0;
			Tele_Cheak_T=1;
			Retry_Time=0;
		}
	}
	
	else if(Tele_Second)                               //进行第二帧数据发送
	{
		TI=0;                                            //第一时间清零发送中断标志位
		GZC=0;                                           //第二帧数据，还是用高阻代表1                     
		TB8=1;                                           //设置第八位发送数据为1
		SBUF=ID2;                                        //第二帧数据写入SBUF
	  while(TDX)
			PDI=RDI;		
		TL0=0x80;                                        //重新对准定时器
		GZC_Control=1;                                   //启动GZC控制
	//	Back_Read_Cheak=1;                               //启动回读检测
		Tele_Second=0;                                   //第二帧标志位清零
		Tele_TT++;                                       //发送数据帧次数加1
		CRC_T=CRC_Table[CRC_T^ID2];
		
	}
	
	else                                               //进行正常数据发送
	{
		TI=0;                                            //第一时间清零发送中断标志位
		Tele_RT=0;                                       //清零Tele_RT，放弃回读时保存的数据
		CRC_R=0;                                         //清零CRC_R的值，放弃之前计算的值
		
		Tele_TT++;

		if(Tele_TT>5)                       
			TB8=0;                     
		else
			TB8=1;			
	
		
		if(Tele_TT<(Frame_Num_T+3))            //检测数据帧是否发送完//报文(除ID1ID2CRC)+3=总长度
		{
			SBUF=t[Tele_TT-3];
			CRC_T=CRC_Table[CRC_T^t[Tele_TT-3]];
		
		}
		else if(Tele_TT==(Frame_Num_T+3))       //报文总长度最后一位           //检测是否发送CRC效验码
			SBUF=CRC_T;                                   //发送CRC效验码
		else                                             //所有帧发送完
		{
			
			REN=1;                                         //恢复接收允许
			PDI=0;                                         //保持总线高阻
			if((t[0]&0xC0)!=0x80)                          //反馈报文不需要确认帧
			{
				SM2=0;                                       //准备接收确认帧
				Ack_Cheak=1;                                 //启动确认超时检测，在规定的时间内报文接收节点要返回确认帧
			}
			else                                           //如果是反馈报文，直接置位报文发送检测标志位
				Tele_Cheak_T=1;
		}			
	}
}
