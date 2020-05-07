/*****************************************************************
                           USB.C  file

主要功能：负责USB的连接，数据发送和接收

创建时间：2013.11.21
*****************************************************************/
/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include <intrins.h>
#include "12C5A60S2.h"
#include "USB.h"
#include "TELE_MANAGE.h"
#include "DELAY.h"
#define D12_DATA  P2 //usb芯片数据端口
/*--------------------------------------------------------------*/




sbit D12_COMMAND_DATA=P3^4;
sbit D12_WR=P3^6;
sbit D12_RD=P3^7;

unsigned char Buffer[16];             //读端点0用的缓冲区

unsigned char bmRequestType;                //USB标准请求的各个字段
unsigned char bRequest;
unsigned int wValue;
unsigned int wIndex;
unsigned int wLength;

unsigned char *pSendData;                   //当前发送数据的位置
unsigned int SendLength;                    //需要发送数据的长度
bit    NeedZeroPacket;                      //是否返回0长度数据包的标志位
unsigned char ConfigValue;                  //当前的配置值。只有在设置非0配置后
bit    Ep1InIsBusy;                         //端点1缓冲是否忙的标志。当缓冲区中有数据时，该标志为真。
                                            //当缓冲区中空闲时，该标志为假。
unsigned char InBuf[count];                     //输入报告缓冲区
unsigned char OutBuf[count];                    //输出报告缓冲区
bit    UsbOutFlag;                           //Usb输出标志位，1为usb有数据输出，需要处理数据


code unsigned char DeviceDescriptor[0x12]=  //usb设备描述，18字节
{
 0x12,                                      //bLength字段。设备描述符的长度为18(0x12)字节
 0x01,                                      //bDescriptorType字段。设备描述符的编号为0x01
 0x10,                                      //bcdUSB字段。这里设置版本为USB1.1，即0x0110。
 0x01,                                      //采用小端格式，0x10在前，0x01在后
 0x00,                                      //bDeviceClass字段。我们不在设备描述符中定义设备类，而在接口描述符中定义设备类，所以该字段的值为0。
 0x00,                                      //bDeviceSubClass字段。bDeviceClass字段为0时，该字段也为0。
 0x00,                                      //bDeviceProtocol字段。bDeviceClass字段为0时，该字段也为0。
 0x10,                                      //bMaxPacketSize0字段。PDIUSBD12的端点0大小的16字节。
 0x88,0x88,                                 //idVender字段。厂商ID号，我们这里取0x8888，仅供实验用。实际产品不能随便使用厂商ID号，必须跟USB协会申请厂商ID号。
 0x06,0x00,                                 //idProduct字段。产品ID号，由于是第一个实验，我们这里取0x0006，采用小端格式  
 0x00,0x01,                                 //bcdDevice字段。我们这个USB鼠标刚开始做，就叫它1.0版吧，即0x0100。小端模式，低字节在先。
 0x01,                                      //iManufacturer字段。厂商字符串的索引值，为了方便记忆和管理，字符串索引就从1开始吧。
 0x02,                                      //iProduct字段。产品字符串的索引值。刚刚用了1，这里就取2吧。注意字符串索引值不要使用相同的值。
 0x03,                                      //iSerialNumber字段。设备的序列号字符串索引值。这里取3就可以了。
 0x01                                       //bNumConfigurations字段。该设备所具有的配置数。我们只需要一种配置就行了，因此该值设置为1。
};

code unsigned char ReportDescriptor[27]=    //USB报告描述符
{
 0x05, 0x01,                                //USAGE_PAGE (Generic Desktop)
 0x09, 0x00,                                //USAGE (0)
 0xa1, 0x01,                                //COLLECTION (Application)
 0x15, 0x00,                                //LOGICAL_MINIMUM (0)
 0x25, 0xff,                                //LOGICAL_MAXIMUM (255)
 0x19, 0x01,                                //USAGE_MINIMUM (1)
 0x29, count,                                //USAGE_MAXIMUM (11) 
 0x95, count,                                //REPORT_COUNT (11)
 //0x29, 0x08,                                //USAGE_MAXIMUM (11) 
 //0x95, 0x08,                                //REPORT_COUNT (11)
 0x75, 0x08,                                //REPORT_SIZE (11)
 0x81, 0x02,                                //INPUT (Data,Var,Abs)
 0x19, 0x01,                                //USAGE_MINIMUM (1)
 0x29, count,                                //USAGE_MAXIMUM (8)
 //0x29, 0x08,                                //USAGE_MAXIMUM (8) 
 0x91, 0x02,                                //OUTPUT (Data,Var,Abs)
 0xc0                                       //END_COLLECTION
};

code unsigned char ConfigurationDescriptor[41]=//USB配置描述符，配置描述符总长度为9+9+9+7+7字节
{
 /******************配置描述符***********************/
 0x09,                                      //bLength字段。配置描述符的长度为9字节。
 0x02,                                      //bDescriptorType字段。配置描述符编号为0x02。
 0x29,                                      //wTotalLength字段。配置描述符集合的总长度,低字节
 0x00,                                      //wTotalLength字段。配置描述符集合的总长度,高字节
 0x01,                                      //bNumInterfaces字段。该配置包含的接口数，只有一个接口。
 0x01,                                      //bConfiguration字段。该配置的值为1。
 0x00,                                      //iConfigurationz字段，该配置的字符串索引。这里没有，为0。
 0x80,                                      //bmAttributes字段，该设备的属性
 0xFA,                                      //bMaxPower字段，该设备需要的最大电流量，这里设置500mA
 
 /*******************接口描述符*********************/
 0x09,                                      //bLength字段。接口描述符的长度为9字节。
 0x04,                                      //bDescriptorType字段。接口描述符的编号为0x04。
 0x00,                                      //bInterfaceNumber字段。该接口的编号，第一个接口，编号为0。
 0x00,                                      //bAlternateSetting字段。该接口的备用编号，为0。
 0x02,                                      //bNumEndpoints字段。非0端点的数目。需要两个中断端点，一个输入，一个输出
 0x03,                                      //bInterfaceClass字段。该接口所使用的类。自定义HID设备使用HID类，为0x03
 0x00,                                      //bInterfaceSubClass字段。该接口所使用的子类。在HID1.1协议中，自定义设备子类代码为0x00
 0x00,                                      //bInterfaceProtocol字段。这里为0x00
 0x00,                                      //iConfiguration字段。该接口的字符串索引值。这里没有，为0。
 
 /******************HID描述符************************/
 0x09,                                      //bLength字段。本HID描述符下只有一个下级描述符。所以长度为9字节。
 0x21,                                      //bDescriptorType字段。HID描述符的编号为0x21。
 0x10,0x01,                                 //bcdHID字段。本协议使用的HID1.1协议。注意低字节在先。
 0x21,                                      //bCountyCode字段。设备适用的国家代码，这里选择为美国，代码0x21。
 0x01,                                      //bNumDescriptors字段。下级描述符的数目。我们只有一个报告描述符。
 0x22,                                      //bDescriptorType字段。下级描述符的类型，为报告描述符，编号为0x22。
 0x1B,0x00,                                 //bDescriptorLength字段。下级描述符的长度。下级描述符为报告描述符。
 
 /*******************输入端点描述符******************/
 0x07,                                      //bLength字段。端点描述符长度为7字节。
 0x05,                                      //bDescriptorType字段。端点描述符编号为0x05。
 0x81,                                      //bEndpointAddress字段。端点的地址。我们使用D12的输入端点1，D7位表示数据方向，输入端点D7为1。所以输入端点1的地址为0x81。
 0x03,                                      //bmAttributes字段。D1~D0为端点传输类型选择，该端点为中断端点。中断端点的编号为3。其它位保留为0。
 0x10,0x00,                                 //wMaxPacketSize字段。该端点的最大包长。端点1的最大包长为16字节，注意低字节在先。
 0x01,                                      //bInterval字段。端点查询的时间，我们设置为1个帧时间，即1ms。
 
 /*******************输出端点描述符******************/
 0x07,                                      //bLength字段。端点描述符长度为7字节。
 0x05,                                      //bDescriptorType字段。端点描述符编号为0x05。
 0x01,                                      //bEndpointAddress字段。端点的地址。我们使用D12的输出端点1，D7位表示数据方向，输入端点D7为1。所以输入端点1的地址为0x81。
 0x03,                                      //bmAttributes字段。D1~D0为端点传输类型选择，该端点为中断端点。中断端点的编号为3。其它位保留为0。
 0x10,0x00,                                 //wMaxPacketSize字段。该端点的最大包长。端点1的最大包长为16字节，注意低字节在先。
 0x01                                       //bInterval字段。端点查询的时间，我们设置为1个帧时间，即1ms。
};

code unsigned char LanguageId[4]=           //语言ID的定义
{
 0x04,                                      //本描述符的长度
 0x03,                                      //字符串描述符
 0x09,0x04                                  //0x0409为美式英语的ID
};

code unsigned char ManufacturerStringDescriptor[18]=//厂商字符串
{
 0x12,                                        //该描述符的长度为18字节
 0x03,                                        //字符串描述符的类型编码为0x03
 0x7A,0x66,                                   //智
 0xFD,0x80,                                   //能
 0xFA,0x5E,                                   //建
 0x51,0x7B,                                   //筑
 0x35,0x75,                                   //电
 0x14,0x6C,                                   //气
 0xD1,0x76,                                   //监
 0xA7,0x63                                    //控
};

code unsigned char ProductStringDescriptor[10]=//产品字符串
{
 0x0A,                                        //该描述符的长度为10字节
 0x03,                                        //字符串描述符的类型编码为0x03
 0x51, 0x7F,                                  //网
 0x73, 0x51,                                  //关
 0x21, 0x6A,                                  //模
 0x57, 0x57                                   //块
};

code unsigned char  SerialNumberStringDescriptor[22]=//产品序列号字符串
{
 0x16,                                        //该描述符的长度为22字节
 0x03,                                        //字符串描述符的类型编码为0x03
 0x32, 0x00,                                  //2
 0x30, 0x00,                                  //0
 0x31, 0x00,                                  //1
 0x33, 0x00,                                  //3
 0x2E, 0x00,                                  //.
 0x31, 0x00,                                  //1
 0x31, 0x00,                                  //1
 0x2E, 0x00,                                  //.
 0x32, 0x00,                                  //2
 0x31, 0x00                                   //1
};

/*--------------------------------------------------------------*/
/*-------------------------USB初始化函数------------------------*/
/*--------------------------------------------------------------*/
void UsbInit(void)
{
	P2M1=0xFF;
	P2M0=0xFF;
	D12_DATA=0xFF;                           //将P2端口初始化为输入状态
	ConfigValue=0;                           //当前的配置值初始化为0
  Ep1InIsBusy=0;                           //端点1缓冲空闲 
	NeedZeroPacket=0;
	UsbOutFlag=0;                            //Usb输出标志位初始化为0，没有输出报告需要处理
	IE0=0;                                   //开外部中断之前，先清空一下其中断标志位
  EA=1;                                    //开总中断	
	EX0=1;                                   //允许外部中断0产生中断
	IT0=0;                                   //外部中断0为电平触发方式	
}
/*--------------------------------------------------------------*/
/*---------------------------D12写命令--------------------------*/
/*--------------------------------------------------------------*/
void D12WriteCommand(unsigned char Command)
{
	P2M1=0x00;
	_nop_();
	_nop_();
	D12_COMMAND_DATA=1;                     //选择写命令
	_nop_();
	_nop_();
	D12_WR=0;                               //写控制端口置0
	_nop_();
	_nop_();
	D12_DATA=Command;                       //将命令输出到数据总线上
	_nop_();
	_nop_();
	D12_WR=1;                               //写控制端口拉高
	_nop_();
	_nop_();
	P2M1=0xFF;
	_nop_();
	_nop_();
	D12_DATA=0xFF;                          //将总线端口恢复到输入状态，备用
	_nop_();
	_nop_();
}

/*--------------------------------------------------------------*/
/*-----------------------从D12读一字节数据----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12ReadByte(void)
{
	unsigned char temp;
	D12_COMMAND_DATA=0;                     //地址位选择数据
	_nop_();
	_nop_();
	D12_RD=0;                               //读控制端口置0
	_nop_();
	_nop_();
	temp=D12_DATA;                          //从总线上读取数据
	_nop_();
	_nop_();
	D12_RD=1;                               //读控制端口拉高
	return temp;                            //返回读取的一字节数据
}

/*--------------------------------------------------------------*/
/*-----------------------向D12写一字节数据----------------------*/
/*--------------------------------------------------------------*/
void D12WriteByte(unsigned char Data)
{
	P2M1=0x00;
	_nop_();
	_nop_();
	D12_COMMAND_DATA=0;                     //地址位选择数据
	_nop_();
	_nop_();
	D12_WR=0;
	_nop_();
	_nop_();
	D12_DATA=Data;
	_nop_();
	_nop_();
	D12_WR=1;
	_nop_();
	_nop_();
	P2M1=0xFF;
	_nop_();
	_nop_();
	D12_DATA=0xFF;                          //总线恢复为输入状态，备用
	_nop_();
	_nop_();
}

/*--------------------------------------------------------------*/
/*--------------------------USB断开连接-------------------------*/
/*--------------------------------------------------------------*/
void UsbDisconnect(void)
{
	D12WriteCommand(0xF3);                  //写设置模式命令
	D12WriteByte(0x06);                     //写设置模式的第一字节数据
	D12WriteByte(0x47);                     //写设置模式的第二字节数据
	delayms(10000);                         //延时1s，以便主机确认USB以断开
}

/*--------------------------------------------------------------*/
/*----------------------------USB连接---------------------------*/
/*--------------------------------------------------------------*/
void UsbConnect(void)
{
	D12WriteCommand(0xF3);                  //写设置模式命令
	D12WriteByte(0x16);                     //写设置模式的第一字节数据
	D12WriteByte(0x47);                     //写设置模式的第二字节数据
}

/*--------------------------------------------------------------*/
/*------------------------读端点缓冲区函数----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12ReadEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)
{
	unsigned char i,j;
	D12WriteCommand(Endp);                  //选择要操作的端点缓冲区
	D12WriteCommand(0xF0);                  //写读缓冲区命令
	D12ReadByte();                          //缓冲区的第一个字节是保留的，无用
	j=D12ReadByte();                        //第二字节代表缓冲区数据长度
	if(j>Len)                               //如果缓冲区字节比读取时指定的字节长，则只读取指定字节长度的数据
		j=Len;
	for(i=0;i<j;i++)                        //读取缓冲区字节数据
	{
		D12_RD=0;
		_nop_();
		_nop_();
		*(Buf+i)=D12_DATA;
		_nop_();
	  _nop_();
		D12_RD=1;
		_nop_();
	  _nop_();
	}
	return j;                               //返回实际读取的字节数
}

/*--------------------------------------------------------------*/
/*------------------------写端点缓冲区函数----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12WriteEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)
{
	unsigned char i;
	D12WriteCommand(Endp);                  //选择要操作的端点缓冲区
	D12WriteCommand(0xF0);                  //写写缓冲区命令（与读缓冲区命令一样，都是0xF0）
	D12WriteByte(0x00);                     //第一个字节要写入0
	D12WriteByte(Len);                      //第二个字节写入数据长度
	P2M1=0x00;
	_nop_();
	_nop_();
	for(i=0;i<Len;i++)                      //向缓冲区写数据
	{
		D12_WR=0;
		_nop_();
		_nop_();
		D12_DATA=(*(Buf+i));
		_nop_();
		_nop_();
		D12_WR=1;
		_nop_();
		_nop_();
	}
	P2M1=0xFF;
	_nop_();
	_nop_();
	D12_DATA=0xFF;                          //总线端口恢复为输入状态，备用
	_nop_();
	_nop_();
	D12WriteCommand(0xFA);                  //使端点缓冲区数据有效
	return Len;
}

/*--------------------------------------------------------------*/
/*---------根据pData和SendLength将数据发送到端点0的函数---------*/
/*--------------------------------------------------------------*/
void UsbEp0SendData(void)
{
 if(SendLength>0x10)                     //判断要发送的数据是否大于端点0的最大包长，
 {
  D12WriteEndpointBuffer(1,16,pSendData);//按最大包长度发送
  SendLength-=0x10;                      //发送后剩余字节数减少最大包长
  pSendData+=0x10;                       //发送一次后指针位置要调整
 }
 else
 {
  if(SendLength!=0)
  {
   D12WriteEndpointBuffer(1,SendLength,pSendData);//不够最大包长，可以直接发送
   SendLength=0;                         //发送完毕后，SendLength长度变为0
  }
  else if(NeedZeroPacket==1)             //如果需要发送0长度数据
  {
   D12WriteEndpointBuffer(1,0,pSendData);//发送0长度数据包
   NeedZeroPacket=0;                     //清需要发送0长度数据包标志
  }
 }
}

/*--------------------------------------------------------------*/
/*-------------------------设置地址函数-------------------------*/
/*--------------------------------------------------------------*/
void D12SetAddress(unsigned char Addr)
{
	D12WriteCommand(0xD0);                 //0xD0为设置地址命令
	D12WriteByte(0x80|Addr);               //写入一字节的地址值，并使能地址，D7为地址使能位
}

/*--------------------------------------------------------------*/
/*------------------------使能非零端点函数----------------------*/
/*--------------------------------------------------------------*/
void D12SetEndpointEnable(unsigned char Enable)
{
	D12WriteCommand(0xD8);                 //0xD8,端点使能命令
	if(Enable!=0)                          //非0端点使能
		D12WriteByte(0x01);
	else
		D12WriteByte(0x00);                  
}

/*--------------------------------------------------------------*/
/*---------------------端点0输出中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp0Out(void)
{
	unsigned char LastStatus;
	D12WriteCommand(0x40);                  //读取端点0最后状态的命令，清除端点中断标志
	LastStatus=D12ReadByte();
	if(LastStatus&0x20)                     //判定是否为建立包
	{
		D12ReadEndpointBuffer(0,16,Buffer);   //读端点0输出缓冲区
		D12WriteCommand(0x01);                //选择端点0输入
		D12WriteCommand(0xF1);                //发送应答设置
		D12WriteCommand(0x00);                //选择端点0输出
		D12WriteCommand(0xF1);                //发送应答设置
		D12WriteCommand(0xF2);                //清空端点缓冲区
		bmRequestType=Buffer[0];              //将缓冲数据填到设备请求的各字段中
		bRequest=Buffer[1];
		wValue=Buffer[2]+(((unsigned int)Buffer[3])<<8);
		wIndex=Buffer[4]+(((unsigned int)Buffer[5])<<8);
		wLength=Buffer[6]+(((unsigned int)Buffer[7])<<8);
		if((bmRequestType&0x80)==0x80)        //下面的代码判断具体的请求，并根据不同的请求进行相关操作如果D7位为1，则说明是输入请求
		{
			switch((bmRequestType>>5)&0x03)     //根据bmRequestType的D6~5位散转，D6~5位表示请求的类型，0为标准请求，1为类请求，2为厂商请求。
			{
				case 0:                           //标准请求
				switch(bRequest)
				{
					case 8: 
					break;                          //获取配置
					case 6:                         //获取描述符
					switch((wValue>>8)&0xFF)        //对描述符类型进行散转，对于全速设备，标准请求只支持发送到设备的设备、配置、字符串三种描述符
					{
						case 1:                       //设备描述符
						pSendData=DeviceDescriptor;   //需要发送的数据
						if(wLength>0x12)              //判断请求的字节数是否比实际需要发送的字节数多，如果请求的比实际的长，那么只返回实际长度的数据
						{
							SendLength=0x12;            //这里设备描述符的长度为18字节
							if(SendLength%0x10==0)      //判读需要发送设备描述符的长度是否是端点0包长度的整数倍
								NeedZeroPacket=1;         //需要返回0长度的数据包      
						}							
						else
              SendLength=wLength;
						UsbEp0SendData();             //将数据通过EP0返回
						break;
						
						case 2:                       //配置描述符
						pSendData=ConfigurationDescriptor; //需要发送的数据为配置描述符
						SendLength=0x29;              //配置描述符集合长度为41个字节
						if(wLength>SendLength)        //判断请求的长度是否比实际长度大，如果是，则只返回实际长度
						{
							if(SendLength%0x10==0)      //判读需要发送设备描述符的长度是否是端点0包长度的整数倍
								NeedZeroPacket=1;         //如果是，需要返回0长度的数据包
						}
						else	
							SendLength=wLength;         //如果不是，则返回请求的长度
						UsbEp0SendData();             //将数据通过EP0返回
						break;
         
						case 3:                       //字符串描述符
						switch(wValue&0xFF)           //根据wValue的低字节（索引值）散转
						{
							case 0:                     //获取语言ID
							pSendData=LanguageId;
							SendLength=0x04;            //语言ID的长度为4个字节
							break;
							case 1:                     //厂商字符串
							pSendData=ManufacturerStringDescriptor;
							SendLength=0x12;            //厂商字符串长度为18字节
							break;
							case 2:                     //产品字符串
							pSendData=ProductStringDescriptor;
							SendLength=0x0A;            //产品字符串长度为10字节
							break;
							case 3:                     //产品序列号
							pSendData=SerialNumberStringDescriptor;
							SendLength=0x16;            //产品序列号为22字节
							break;
							default:                    //对于未知索引值的请求，返回一个0长度的包
							SendLength=0;
							NeedZeroPacket=1;
							break;
						}
						if(wLength>SendLength)        //判断请求的字节数是否比实际需要发送的字节数多，如果是，那么只返回实际长度的数据
						{
							if(SendLength%0x10==0)      //判读需要发送设备描述符的长度是否是端点0包长度的整数倍
								NeedZeroPacket=1;         //需要返回0长度的数据包
						}
						else
							SendLength=wLength;         //如果不是，则返回请求的长度
						UsbEp0SendData();             //将数据通过EP0返回 
						break;
						
						case 0x22:                    //报告描述符
						pSendData=ReportDescriptor;   //需要发送的数据为报告描述符
						SendLength=0x1B;              //需要返回的数据长度         
						if(wLength>SendLength)        //判断请求的字节数是否比实际需要发送的字节数多，如果是，那么只返回实际长度的数据
						{
							if(SendLength%0x10==0)      //判读需要发送设备描述符的长度是否是端点0包长度的整数倍
								NeedZeroPacket=1;         //需要返回0长度的数据包
						}
						else
							SendLength=wLength;         //如果不是，则返回请求的长度
						UsbEp0SendData();             //将数据通过EP0返回
						break;
                 
						default:                      //其它描述符
						break;
					}
					break;
					case 10:                        //获取接口
					break;
					case 0:                         //获取状态
					break;
					case 12:                        //同步帧
					break;
					default:                        //未定义的标准请求
					break;
				}
				break;
    
				case 1:                           //类请求
				break;
    
				case 2:                           //厂商请求
				break;
    
				default:                          //未定义的请求。这里只显示一个报错信息。
				break;
			}
		}
		
		else                                  //if(bmRequestType&0x80==0x80)之else，否则说明是输出请求
		{
			switch((bmRequestType>>5)&0x03)     //根据bmRequestType的D6~5位散转，D6~5位表示请求的类型，0为标准请求，1为类请求，2为厂商请求。
			{
				case 0:                           //标准请求，USB协议定义了几个标准输出请求，请求的代码在bRequest中，对不同的请求代码进行散转
				switch(bRequest)
				{
					case 1:                         //清除特性
					break;
					case 5:                         //设置地址
					D12SetAddress(wValue&0xFF);     //wValue中的低字节是设置的地址值
					SendLength=0;                   //设置地址没有数据过程，直接进入到状态过程，返回一个0长度的数据包
					NeedZeroPacket=1;
					UsbEp0SendData();               //将数据通过EP0返回
					break;
					case 9:                         //设置配置
					ConfigValue=wValue&0xFF;        //保存当前配置值，只有当wValue的低字节为非0值时，才能使能非零端点
					D12SetEndpointEnable(ConfigValue);
					SendLength=0;                   //返回一个0长度的状态数据包
					NeedZeroPacket=1;
					UsbEp0SendData();               //将数据通过EP0返回
					break;
					case 7:                         //设置描述符
					break;
					case 3:                         //设置特性
					break;
					case 11:                        //设置接口
					break;      
					default:                        //未定义的标准请求     
					break;
				}
				break;
    
				case 1:                           //类请求
				switch(bRequest)
				{
					case 0x0A:
					SendLength=0;                   //只需要返回一个0长度的数据包即可
					NeedZeroPacket=1;
					UsbEp0SendData();               //将数据通过EP0返回
					break;
					default:
					break;
				}
				break;
    
				case 2:                           //厂商请求
				break;
    
				default:                          //未定义的请求。这里只显示一个报错信息。
				break;
			}
		}
	}
	else
	{
		D12ReadEndpointBuffer(0,16,Buffer);   //读端点0输出缓冲区
		D12WriteCommand(0xF2);                //清空端点缓冲区
	}
}

/*--------------------------------------------------------------*/
/*---------------------端点0输入中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp0In(void)
{
	D12WriteCommand(0x41);                  //读取端点0最后状态的命令，清除端点中断标志
	D12ReadByte();
	UsbEp0SendData();
}

/*--------------------------------------------------------------*/
/*---------------------端点1输出中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp1Out(void)
{
	
	D12WriteCommand(0x42);                  //读取端点1输出中断最后状态的命令，清除端点中断标志
	D12ReadByte();
	D12ReadEndpointBuffer(2,count,OutBuf);      //读取端点1输出缓冲区数据
	//D12ReadEndpointBuffer(2,8,OutBuf);      //读取端点1输出缓冲区数据
	D12WriteCommand(0xF2);                  //清空端点1输出缓冲区
	UsbOutFlag=1;                           //usb输出标志位置1，告知主函数有来自usb的数据需要处理
}

/*--------------------------------------------------------------*/
/*---------------------端点1输入中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp1In(void)
{
	D12WriteCommand(0x43);                  //读取端点0最后状态的命令，清除端点中断标志
	D12ReadByte();
	Ep1InIsBusy=0;                          //端点1输入忙标志位清零
}

/*--------------------------------------------------------------*/
/*---------------------端点2输出中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp2Out(void)
{
}

/*--------------------------------------------------------------*/
/*---------------------端点2输入中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbEp2In(void)
{
}

/*--------------------------------------------------------------*/
/*----------------------usb挂起中断处理函数---------------------*/
/*--------------------------------------------------------------*/
void UsbBusSuspend(void)
{
}

/*--------------------------------------------------------------*/
/*-----------------------usb复位中断处理函数--------------------*/
/*--------------------------------------------------------------*/
void UsbBusReset(void)
{
	Ep1InIsBusy=0;                           //复位后端点1输入缓冲区空闲。
}

/*--------------------------------------------------------------*/
/*-------------------------报告发送函数-------------------------*/
/*---------功能：USB从下位机取出的数据上传到主机----------------*/
/*--------------------------------------------------------------*/
void SendReport(void)
{
	if(ConfigValue!=0)
		if(!Ep1InIsBusy)			//端点1缓冲是否忙的标志。当缓冲区中有数据时，该标志为1
		{
			D12WriteEndpointBuffer(3,count,InBuf);		//输入报告缓存区
			Ep1InIsBusy=1;							//端点1缓冲是否忙的标志
		}
}
/*--------------------------------------------------------------*/
/*-----------------------报告接收处理函数-----------------------*/
/*---------------功能：USB模块发送数据到STC---------------------*/
/*--------------------------------------------------------------*/
void RecieveReport(void)
{
	unsigned char i,j;
	i=OutBuf[0]&0x0F;							//usb模块缓存的指令报文数据域数量
	j=write_tele_t(OutBuf,i+3);					//将USB输出报告缓冲区写入到报文发送缓存区
	
	if(j)
	{
		UsbOutFlag=0;							//写入成功，标志位清空
		
	}
}

/*--------------------------------------------------------------*/
/*-------------------------外部中断0程序------------------------*/
/*--------------------------------------------------------------*/
void interrupt_INT0() interrupt 0          //外部中断0的中断号为0
{
	unsigned char InterruptSource;
	D12WriteCommand(0xF4);                   //读取中断寄存器
	InterruptSource=D12ReadByte();		   //从总线上读取数据
  if(InterruptSource&0x80)UsbBusSuspend(); //总线挂起中断处理
  if(InterruptSource&0x40)UsbBusReset();   //总线复位中断处理
  
  if(InterruptSource&0x01)UsbEp0Out();     //端点0输出中断处理
  if(InterruptSource&0x02)UsbEp0In();      //端点0输入中断处理
  
  if(InterruptSource&0x04)UsbEp1Out();     //端点1输出中断处理
  if(InterruptSource&0x08)UsbEp1In();      //端点1输入中断处理
  
  if(InterruptSource&0x10)UsbEp2Out();     //端点2输出中断处理
  if(InterruptSource&0x20)UsbEp2In();      //端点2输入中断处理
}
