/*****************************************************************
                           USB.C  file

��Ҫ���ܣ�����USB�����ӣ����ݷ��ͺͽ���

����ʱ�䣺2013.11.21
*****************************************************************/
/*--------------------------------------------------------------*/
/*---------------------------ͷ�ļ�-----------------------------*/
/*--------------------------------------------------------------*/
#include <intrins.h>
#include "12C5A60S2.h"
#include "USB.h"
#include "TELE_MANAGE.h"
#include "DELAY.h"
#define D12_DATA  P2 //usbоƬ���ݶ˿�
/*--------------------------------------------------------------*/




sbit D12_COMMAND_DATA=P3^4;
sbit D12_WR=P3^6;
sbit D12_RD=P3^7;

unsigned char Buffer[16];             //���˵�0�õĻ�����

unsigned char bmRequestType;                //USB��׼����ĸ����ֶ�
unsigned char bRequest;
unsigned int wValue;
unsigned int wIndex;
unsigned int wLength;

unsigned char *pSendData;                   //��ǰ�������ݵ�λ��
unsigned int SendLength;                    //��Ҫ�������ݵĳ���
bit    NeedZeroPacket;                      //�Ƿ񷵻�0�������ݰ��ı�־λ
unsigned char ConfigValue;                  //��ǰ������ֵ��ֻ�������÷�0���ú�
bit    Ep1InIsBusy;                         //�˵�1�����Ƿ�æ�ı�־������������������ʱ���ñ�־Ϊ�档
                                            //���������п���ʱ���ñ�־Ϊ�١�
unsigned char InBuf[count];                     //���뱨�滺����
unsigned char OutBuf[count];                    //������滺����
bit    UsbOutFlag;                           //Usb�����־λ��1Ϊusb�������������Ҫ��������


code unsigned char DeviceDescriptor[0x12]=  //usb�豸������18�ֽ�
{
 0x12,                                      //bLength�ֶΡ��豸�������ĳ���Ϊ18(0x12)�ֽ�
 0x01,                                      //bDescriptorType�ֶΡ��豸�������ı��Ϊ0x01
 0x10,                                      //bcdUSB�ֶΡ��������ð汾ΪUSB1.1����0x0110��
 0x01,                                      //����С�˸�ʽ��0x10��ǰ��0x01�ں�
 0x00,                                      //bDeviceClass�ֶΡ����ǲ����豸�������ж����豸�࣬���ڽӿ��������ж����豸�࣬���Ը��ֶε�ֵΪ0��
 0x00,                                      //bDeviceSubClass�ֶΡ�bDeviceClass�ֶ�Ϊ0ʱ�����ֶ�ҲΪ0��
 0x00,                                      //bDeviceProtocol�ֶΡ�bDeviceClass�ֶ�Ϊ0ʱ�����ֶ�ҲΪ0��
 0x10,                                      //bMaxPacketSize0�ֶΡ�PDIUSBD12�Ķ˵�0��С��16�ֽڡ�
 0x88,0x88,                                 //idVender�ֶΡ�����ID�ţ���������ȡ0x8888������ʵ���á�ʵ�ʲ�Ʒ�������ʹ�ó���ID�ţ������USBЭ�����볧��ID�š�
 0x06,0x00,                                 //idProduct�ֶΡ���ƷID�ţ������ǵ�һ��ʵ�飬��������ȡ0x0006������С�˸�ʽ  
 0x00,0x01,                                 //bcdDevice�ֶΡ��������USB���տ�ʼ�����ͽ���1.0��ɣ���0x0100��С��ģʽ�����ֽ����ȡ�
 0x01,                                      //iManufacturer�ֶΡ������ַ���������ֵ��Ϊ�˷������͹������ַ��������ʹ�1��ʼ�ɡ�
 0x02,                                      //iProduct�ֶΡ���Ʒ�ַ���������ֵ���ո�����1�������ȡ2�ɡ�ע���ַ�������ֵ��Ҫʹ����ͬ��ֵ��
 0x03,                                      //iSerialNumber�ֶΡ��豸�����к��ַ�������ֵ������ȡ3�Ϳ����ˡ�
 0x01                                       //bNumConfigurations�ֶΡ����豸�����е�������������ֻ��Ҫһ�����þ����ˣ���˸�ֵ����Ϊ1��
};

code unsigned char ReportDescriptor[27]=    //USB����������
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

code unsigned char ConfigurationDescriptor[41]=//USB�����������������������ܳ���Ϊ9+9+9+7+7�ֽ�
{
 /******************����������***********************/
 0x09,                                      //bLength�ֶΡ������������ĳ���Ϊ9�ֽڡ�
 0x02,                                      //bDescriptorType�ֶΡ��������������Ϊ0x02��
 0x29,                                      //wTotalLength�ֶΡ��������������ϵ��ܳ���,���ֽ�
 0x00,                                      //wTotalLength�ֶΡ��������������ϵ��ܳ���,���ֽ�
 0x01,                                      //bNumInterfaces�ֶΡ������ð����Ľӿ�����ֻ��һ���ӿڡ�
 0x01,                                      //bConfiguration�ֶΡ������õ�ֵΪ1��
 0x00,                                      //iConfigurationz�ֶΣ������õ��ַ�������������û�У�Ϊ0��
 0x80,                                      //bmAttributes�ֶΣ����豸������
 0xFA,                                      //bMaxPower�ֶΣ����豸��Ҫ��������������������500mA
 
 /*******************�ӿ�������*********************/
 0x09,                                      //bLength�ֶΡ��ӿ��������ĳ���Ϊ9�ֽڡ�
 0x04,                                      //bDescriptorType�ֶΡ��ӿ��������ı��Ϊ0x04��
 0x00,                                      //bInterfaceNumber�ֶΡ��ýӿڵı�ţ���һ���ӿڣ����Ϊ0��
 0x00,                                      //bAlternateSetting�ֶΡ��ýӿڵı��ñ�ţ�Ϊ0��
 0x02,                                      //bNumEndpoints�ֶΡ���0�˵����Ŀ����Ҫ�����ж϶˵㣬һ�����룬һ�����
 0x03,                                      //bInterfaceClass�ֶΡ��ýӿ���ʹ�õ��ࡣ�Զ���HID�豸ʹ��HID�࣬Ϊ0x03
 0x00,                                      //bInterfaceSubClass�ֶΡ��ýӿ���ʹ�õ����ࡣ��HID1.1Э���У��Զ����豸�������Ϊ0x00
 0x00,                                      //bInterfaceProtocol�ֶΡ�����Ϊ0x00
 0x00,                                      //iConfiguration�ֶΡ��ýӿڵ��ַ�������ֵ������û�У�Ϊ0��
 
 /******************HID������************************/
 0x09,                                      //bLength�ֶΡ���HID��������ֻ��һ���¼������������Գ���Ϊ9�ֽڡ�
 0x21,                                      //bDescriptorType�ֶΡ�HID�������ı��Ϊ0x21��
 0x10,0x01,                                 //bcdHID�ֶΡ���Э��ʹ�õ�HID1.1Э�顣ע����ֽ����ȡ�
 0x21,                                      //bCountyCode�ֶΡ��豸���õĹ��Ҵ��룬����ѡ��Ϊ����������0x21��
 0x01,                                      //bNumDescriptors�ֶΡ��¼�����������Ŀ������ֻ��һ��������������
 0x22,                                      //bDescriptorType�ֶΡ��¼������������ͣ�Ϊ���������������Ϊ0x22��
 0x1B,0x00,                                 //bDescriptorLength�ֶΡ��¼��������ĳ��ȡ��¼�������Ϊ������������
 
 /*******************����˵�������******************/
 0x07,                                      //bLength�ֶΡ��˵�����������Ϊ7�ֽڡ�
 0x05,                                      //bDescriptorType�ֶΡ��˵����������Ϊ0x05��
 0x81,                                      //bEndpointAddress�ֶΡ��˵�ĵ�ַ������ʹ��D12������˵�1��D7λ��ʾ���ݷ�������˵�D7Ϊ1����������˵�1�ĵ�ַΪ0x81��
 0x03,                                      //bmAttributes�ֶΡ�D1~D0Ϊ�˵㴫������ѡ�񣬸ö˵�Ϊ�ж϶˵㡣�ж϶˵�ı��Ϊ3������λ����Ϊ0��
 0x10,0x00,                                 //wMaxPacketSize�ֶΡ��ö˵�����������˵�1��������Ϊ16�ֽڣ�ע����ֽ����ȡ�
 0x01,                                      //bInterval�ֶΡ��˵��ѯ��ʱ�䣬��������Ϊ1��֡ʱ�䣬��1ms��
 
 /*******************����˵�������******************/
 0x07,                                      //bLength�ֶΡ��˵�����������Ϊ7�ֽڡ�
 0x05,                                      //bDescriptorType�ֶΡ��˵����������Ϊ0x05��
 0x01,                                      //bEndpointAddress�ֶΡ��˵�ĵ�ַ������ʹ��D12������˵�1��D7λ��ʾ���ݷ�������˵�D7Ϊ1����������˵�1�ĵ�ַΪ0x81��
 0x03,                                      //bmAttributes�ֶΡ�D1~D0Ϊ�˵㴫������ѡ�񣬸ö˵�Ϊ�ж϶˵㡣�ж϶˵�ı��Ϊ3������λ����Ϊ0��
 0x10,0x00,                                 //wMaxPacketSize�ֶΡ��ö˵�����������˵�1��������Ϊ16�ֽڣ�ע����ֽ����ȡ�
 0x01                                       //bInterval�ֶΡ��˵��ѯ��ʱ�䣬��������Ϊ1��֡ʱ�䣬��1ms��
};

code unsigned char LanguageId[4]=           //����ID�Ķ���
{
 0x04,                                      //���������ĳ���
 0x03,                                      //�ַ���������
 0x09,0x04                                  //0x0409Ϊ��ʽӢ���ID
};

code unsigned char ManufacturerStringDescriptor[18]=//�����ַ���
{
 0x12,                                        //���������ĳ���Ϊ18�ֽ�
 0x03,                                        //�ַ��������������ͱ���Ϊ0x03
 0x7A,0x66,                                   //��
 0xFD,0x80,                                   //��
 0xFA,0x5E,                                   //��
 0x51,0x7B,                                   //��
 0x35,0x75,                                   //��
 0x14,0x6C,                                   //��
 0xD1,0x76,                                   //��
 0xA7,0x63                                    //��
};

code unsigned char ProductStringDescriptor[10]=//��Ʒ�ַ���
{
 0x0A,                                        //���������ĳ���Ϊ10�ֽ�
 0x03,                                        //�ַ��������������ͱ���Ϊ0x03
 0x51, 0x7F,                                  //��
 0x73, 0x51,                                  //��
 0x21, 0x6A,                                  //ģ
 0x57, 0x57                                   //��
};

code unsigned char  SerialNumberStringDescriptor[22]=//��Ʒ���к��ַ���
{
 0x16,                                        //���������ĳ���Ϊ22�ֽ�
 0x03,                                        //�ַ��������������ͱ���Ϊ0x03
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
/*-------------------------USB��ʼ������------------------------*/
/*--------------------------------------------------------------*/
void UsbInit(void)
{
	P2M1=0xFF;
	P2M0=0xFF;
	D12_DATA=0xFF;                           //��P2�˿ڳ�ʼ��Ϊ����״̬
	ConfigValue=0;                           //��ǰ������ֵ��ʼ��Ϊ0
  Ep1InIsBusy=0;                           //�˵�1������� 
	NeedZeroPacket=0;
	UsbOutFlag=0;                            //Usb�����־λ��ʼ��Ϊ0��û�����������Ҫ����
	IE0=0;                                   //���ⲿ�ж�֮ǰ�������һ�����жϱ�־λ
  EA=1;                                    //�����ж�	
	EX0=1;                                   //�����ⲿ�ж�0�����ж�
	IT0=0;                                   //�ⲿ�ж�0Ϊ��ƽ������ʽ	
}
/*--------------------------------------------------------------*/
/*---------------------------D12д����--------------------------*/
/*--------------------------------------------------------------*/
void D12WriteCommand(unsigned char Command)
{
	P2M1=0x00;
	_nop_();
	_nop_();
	D12_COMMAND_DATA=1;                     //ѡ��д����
	_nop_();
	_nop_();
	D12_WR=0;                               //д���ƶ˿���0
	_nop_();
	_nop_();
	D12_DATA=Command;                       //���������������������
	_nop_();
	_nop_();
	D12_WR=1;                               //д���ƶ˿�����
	_nop_();
	_nop_();
	P2M1=0xFF;
	_nop_();
	_nop_();
	D12_DATA=0xFF;                          //�����߶˿ڻָ�������״̬������
	_nop_();
	_nop_();
}

/*--------------------------------------------------------------*/
/*-----------------------��D12��һ�ֽ�����----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12ReadByte(void)
{
	unsigned char temp;
	D12_COMMAND_DATA=0;                     //��ַλѡ������
	_nop_();
	_nop_();
	D12_RD=0;                               //�����ƶ˿���0
	_nop_();
	_nop_();
	temp=D12_DATA;                          //�������϶�ȡ����
	_nop_();
	_nop_();
	D12_RD=1;                               //�����ƶ˿�����
	return temp;                            //���ض�ȡ��һ�ֽ�����
}

/*--------------------------------------------------------------*/
/*-----------------------��D12дһ�ֽ�����----------------------*/
/*--------------------------------------------------------------*/
void D12WriteByte(unsigned char Data)
{
	P2M1=0x00;
	_nop_();
	_nop_();
	D12_COMMAND_DATA=0;                     //��ַλѡ������
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
	D12_DATA=0xFF;                          //���߻ָ�Ϊ����״̬������
	_nop_();
	_nop_();
}

/*--------------------------------------------------------------*/
/*--------------------------USB�Ͽ�����-------------------------*/
/*--------------------------------------------------------------*/
void UsbDisconnect(void)
{
	D12WriteCommand(0xF3);                  //д����ģʽ����
	D12WriteByte(0x06);                     //д����ģʽ�ĵ�һ�ֽ�����
	D12WriteByte(0x47);                     //д����ģʽ�ĵڶ��ֽ�����
	delayms(10000);                         //��ʱ1s���Ա�����ȷ��USB�ԶϿ�
}

/*--------------------------------------------------------------*/
/*----------------------------USB����---------------------------*/
/*--------------------------------------------------------------*/
void UsbConnect(void)
{
	D12WriteCommand(0xF3);                  //д����ģʽ����
	D12WriteByte(0x16);                     //д����ģʽ�ĵ�һ�ֽ�����
	D12WriteByte(0x47);                     //д����ģʽ�ĵڶ��ֽ�����
}

/*--------------------------------------------------------------*/
/*------------------------���˵㻺��������----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12ReadEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)
{
	unsigned char i,j;
	D12WriteCommand(Endp);                  //ѡ��Ҫ�����Ķ˵㻺����
	D12WriteCommand(0xF0);                  //д������������
	D12ReadByte();                          //�������ĵ�һ���ֽ��Ǳ����ģ�����
	j=D12ReadByte();                        //�ڶ��ֽڴ������������ݳ���
	if(j>Len)                               //����������ֽڱȶ�ȡʱָ�����ֽڳ�����ֻ��ȡָ���ֽڳ��ȵ�����
		j=Len;
	for(i=0;i<j;i++)                        //��ȡ�������ֽ�����
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
	return j;                               //����ʵ�ʶ�ȡ���ֽ���
}

/*--------------------------------------------------------------*/
/*------------------------д�˵㻺��������----------------------*/
/*--------------------------------------------------------------*/
unsigned char D12WriteEndpointBuffer(unsigned char Endp, unsigned char Len, unsigned char *Buf)
{
	unsigned char i;
	D12WriteCommand(Endp);                  //ѡ��Ҫ�����Ķ˵㻺����
	D12WriteCommand(0xF0);                  //дд����������������������һ��������0xF0��
	D12WriteByte(0x00);                     //��һ���ֽ�Ҫд��0
	D12WriteByte(Len);                      //�ڶ����ֽ�д�����ݳ���
	P2M1=0x00;
	_nop_();
	_nop_();
	for(i=0;i<Len;i++)                      //�򻺳���д����
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
	D12_DATA=0xFF;                          //���߶˿ڻָ�Ϊ����״̬������
	_nop_();
	_nop_();
	D12WriteCommand(0xFA);                  //ʹ�˵㻺����������Ч
	return Len;
}

/*--------------------------------------------------------------*/
/*---------����pData��SendLength�����ݷ��͵��˵�0�ĺ���---------*/
/*--------------------------------------------------------------*/
void UsbEp0SendData(void)
{
 if(SendLength>0x10)                     //�ж�Ҫ���͵������Ƿ���ڶ˵�0����������
 {
  D12WriteEndpointBuffer(1,16,pSendData);//���������ȷ���
  SendLength-=0x10;                      //���ͺ�ʣ���ֽ�������������
  pSendData+=0x10;                       //����һ�κ�ָ��λ��Ҫ����
 }
 else
 {
  if(SendLength!=0)
  {
   D12WriteEndpointBuffer(1,SendLength,pSendData);//����������������ֱ�ӷ���
   SendLength=0;                         //������Ϻ�SendLength���ȱ�Ϊ0
  }
  else if(NeedZeroPacket==1)             //�����Ҫ����0��������
  {
   D12WriteEndpointBuffer(1,0,pSendData);//����0�������ݰ�
   NeedZeroPacket=0;                     //����Ҫ����0�������ݰ���־
  }
 }
}

/*--------------------------------------------------------------*/
/*-------------------------���õ�ַ����-------------------------*/
/*--------------------------------------------------------------*/
void D12SetAddress(unsigned char Addr)
{
	D12WriteCommand(0xD0);                 //0xD0Ϊ���õ�ַ����
	D12WriteByte(0x80|Addr);               //д��һ�ֽڵĵ�ֵַ����ʹ�ܵ�ַ��D7Ϊ��ַʹ��λ
}

/*--------------------------------------------------------------*/
/*------------------------ʹ�ܷ���˵㺯��----------------------*/
/*--------------------------------------------------------------*/
void D12SetEndpointEnable(unsigned char Enable)
{
	D12WriteCommand(0xD8);                 //0xD8,�˵�ʹ������
	if(Enable!=0)                          //��0�˵�ʹ��
		D12WriteByte(0x01);
	else
		D12WriteByte(0x00);                  
}

/*--------------------------------------------------------------*/
/*---------------------�˵�0����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp0Out(void)
{
	unsigned char LastStatus;
	D12WriteCommand(0x40);                  //��ȡ�˵�0���״̬���������˵��жϱ�־
	LastStatus=D12ReadByte();
	if(LastStatus&0x20)                     //�ж��Ƿ�Ϊ������
	{
		D12ReadEndpointBuffer(0,16,Buffer);   //���˵�0���������
		D12WriteCommand(0x01);                //ѡ��˵�0����
		D12WriteCommand(0xF1);                //����Ӧ������
		D12WriteCommand(0x00);                //ѡ��˵�0���
		D12WriteCommand(0xF1);                //����Ӧ������
		D12WriteCommand(0xF2);                //��ն˵㻺����
		bmRequestType=Buffer[0];              //������������豸����ĸ��ֶ���
		bRequest=Buffer[1];
		wValue=Buffer[2]+(((unsigned int)Buffer[3])<<8);
		wIndex=Buffer[4]+(((unsigned int)Buffer[5])<<8);
		wLength=Buffer[6]+(((unsigned int)Buffer[7])<<8);
		if((bmRequestType&0x80)==0x80)        //����Ĵ����жϾ�������󣬲����ݲ�ͬ�����������ز������D7λΪ1����˵������������
		{
			switch((bmRequestType>>5)&0x03)     //����bmRequestType��D6~5λɢת��D6~5λ��ʾ��������ͣ�0Ϊ��׼����1Ϊ������2Ϊ��������
			{
				case 0:                           //��׼����
				switch(bRequest)
				{
					case 8: 
					break;                          //��ȡ����
					case 6:                         //��ȡ������
					switch((wValue>>8)&0xFF)        //�����������ͽ���ɢת������ȫ���豸����׼����ֻ֧�ַ��͵��豸���豸�����á��ַ�������������
					{
						case 1:                       //�豸������
						pSendData=DeviceDescriptor;   //��Ҫ���͵�����
						if(wLength>0x12)              //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ����࣬�������ı�ʵ�ʵĳ�����ôֻ����ʵ�ʳ��ȵ�����
						{
							SendLength=0x12;            //�����豸�������ĳ���Ϊ18�ֽ�
							if(SendLength%0x10==0)      //�ж���Ҫ�����豸�������ĳ����Ƿ��Ƕ˵�0�����ȵ�������
								NeedZeroPacket=1;         //��Ҫ����0���ȵ����ݰ�      
						}							
						else
              SendLength=wLength;
						UsbEp0SendData();             //������ͨ��EP0����
						break;
						
						case 2:                       //����������
						pSendData=ConfigurationDescriptor; //��Ҫ���͵�����Ϊ����������
						SendLength=0x29;              //�������������ϳ���Ϊ41���ֽ�
						if(wLength>SendLength)        //�ж�����ĳ����Ƿ��ʵ�ʳ��ȴ�����ǣ���ֻ����ʵ�ʳ���
						{
							if(SendLength%0x10==0)      //�ж���Ҫ�����豸�������ĳ����Ƿ��Ƕ˵�0�����ȵ�������
								NeedZeroPacket=1;         //����ǣ���Ҫ����0���ȵ����ݰ�
						}
						else	
							SendLength=wLength;         //������ǣ��򷵻�����ĳ���
						UsbEp0SendData();             //������ͨ��EP0����
						break;
         
						case 3:                       //�ַ���������
						switch(wValue&0xFF)           //����wValue�ĵ��ֽڣ�����ֵ��ɢת
						{
							case 0:                     //��ȡ����ID
							pSendData=LanguageId;
							SendLength=0x04;            //����ID�ĳ���Ϊ4���ֽ�
							break;
							case 1:                     //�����ַ���
							pSendData=ManufacturerStringDescriptor;
							SendLength=0x12;            //�����ַ�������Ϊ18�ֽ�
							break;
							case 2:                     //��Ʒ�ַ���
							pSendData=ProductStringDescriptor;
							SendLength=0x0A;            //��Ʒ�ַ�������Ϊ10�ֽ�
							break;
							case 3:                     //��Ʒ���к�
							pSendData=SerialNumberStringDescriptor;
							SendLength=0x16;            //��Ʒ���к�Ϊ22�ֽ�
							break;
							default:                    //����δ֪����ֵ�����󣬷���һ��0���ȵİ�
							SendLength=0;
							NeedZeroPacket=1;
							break;
						}
						if(wLength>SendLength)        //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ����࣬����ǣ���ôֻ����ʵ�ʳ��ȵ�����
						{
							if(SendLength%0x10==0)      //�ж���Ҫ�����豸�������ĳ����Ƿ��Ƕ˵�0�����ȵ�������
								NeedZeroPacket=1;         //��Ҫ����0���ȵ����ݰ�
						}
						else
							SendLength=wLength;         //������ǣ��򷵻�����ĳ���
						UsbEp0SendData();             //������ͨ��EP0���� 
						break;
						
						case 0x22:                    //����������
						pSendData=ReportDescriptor;   //��Ҫ���͵�����Ϊ����������
						SendLength=0x1B;              //��Ҫ���ص����ݳ���         
						if(wLength>SendLength)        //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ����࣬����ǣ���ôֻ����ʵ�ʳ��ȵ�����
						{
							if(SendLength%0x10==0)      //�ж���Ҫ�����豸�������ĳ����Ƿ��Ƕ˵�0�����ȵ�������
								NeedZeroPacket=1;         //��Ҫ����0���ȵ����ݰ�
						}
						else
							SendLength=wLength;         //������ǣ��򷵻�����ĳ���
						UsbEp0SendData();             //������ͨ��EP0����
						break;
                 
						default:                      //����������
						break;
					}
					break;
					case 10:                        //��ȡ�ӿ�
					break;
					case 0:                         //��ȡ״̬
					break;
					case 12:                        //ͬ��֡
					break;
					default:                        //δ����ı�׼����
					break;
				}
				break;
    
				case 1:                           //������
				break;
    
				case 2:                           //��������
				break;
    
				default:                          //δ�������������ֻ��ʾһ��������Ϣ��
				break;
			}
		}
		
		else                                  //if(bmRequestType&0x80==0x80)֮else������˵�����������
		{
			switch((bmRequestType>>5)&0x03)     //����bmRequestType��D6~5λɢת��D6~5λ��ʾ��������ͣ�0Ϊ��׼����1Ϊ������2Ϊ��������
			{
				case 0:                           //��׼����USBЭ�鶨���˼�����׼�����������Ĵ�����bRequest�У��Բ�ͬ������������ɢת
				switch(bRequest)
				{
					case 1:                         //�������
					break;
					case 5:                         //���õ�ַ
					D12SetAddress(wValue&0xFF);     //wValue�еĵ��ֽ������õĵ�ֵַ
					SendLength=0;                   //���õ�ַû�����ݹ��̣�ֱ�ӽ��뵽״̬���̣�����һ��0���ȵ����ݰ�
					NeedZeroPacket=1;
					UsbEp0SendData();               //������ͨ��EP0����
					break;
					case 9:                         //��������
					ConfigValue=wValue&0xFF;        //���浱ǰ����ֵ��ֻ�е�wValue�ĵ��ֽ�Ϊ��0ֵʱ������ʹ�ܷ���˵�
					D12SetEndpointEnable(ConfigValue);
					SendLength=0;                   //����һ��0���ȵ�״̬���ݰ�
					NeedZeroPacket=1;
					UsbEp0SendData();               //������ͨ��EP0����
					break;
					case 7:                         //����������
					break;
					case 3:                         //��������
					break;
					case 11:                        //���ýӿ�
					break;      
					default:                        //δ����ı�׼����     
					break;
				}
				break;
    
				case 1:                           //������
				switch(bRequest)
				{
					case 0x0A:
					SendLength=0;                   //ֻ��Ҫ����һ��0���ȵ����ݰ�����
					NeedZeroPacket=1;
					UsbEp0SendData();               //������ͨ��EP0����
					break;
					default:
					break;
				}
				break;
    
				case 2:                           //��������
				break;
    
				default:                          //δ�������������ֻ��ʾһ��������Ϣ��
				break;
			}
		}
	}
	else
	{
		D12ReadEndpointBuffer(0,16,Buffer);   //���˵�0���������
		D12WriteCommand(0xF2);                //��ն˵㻺����
	}
}

/*--------------------------------------------------------------*/
/*---------------------�˵�0�����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp0In(void)
{
	D12WriteCommand(0x41);                  //��ȡ�˵�0���״̬���������˵��жϱ�־
	D12ReadByte();
	UsbEp0SendData();
}

/*--------------------------------------------------------------*/
/*---------------------�˵�1����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp1Out(void)
{
	
	D12WriteCommand(0x42);                  //��ȡ�˵�1����ж����״̬���������˵��жϱ�־
	D12ReadByte();
	D12ReadEndpointBuffer(2,count,OutBuf);      //��ȡ�˵�1�������������
	//D12ReadEndpointBuffer(2,8,OutBuf);      //��ȡ�˵�1�������������
	D12WriteCommand(0xF2);                  //��ն˵�1���������
	UsbOutFlag=1;                           //usb�����־λ��1����֪������������usb��������Ҫ����
}

/*--------------------------------------------------------------*/
/*---------------------�˵�1�����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp1In(void)
{
	D12WriteCommand(0x43);                  //��ȡ�˵�0���״̬���������˵��жϱ�־
	D12ReadByte();
	Ep1InIsBusy=0;                          //�˵�1����æ��־λ����
}

/*--------------------------------------------------------------*/
/*---------------------�˵�2����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp2Out(void)
{
}

/*--------------------------------------------------------------*/
/*---------------------�˵�2�����жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbEp2In(void)
{
}

/*--------------------------------------------------------------*/
/*----------------------usb�����жϴ�������---------------------*/
/*--------------------------------------------------------------*/
void UsbBusSuspend(void)
{
}

/*--------------------------------------------------------------*/
/*-----------------------usb��λ�жϴ�������--------------------*/
/*--------------------------------------------------------------*/
void UsbBusReset(void)
{
	Ep1InIsBusy=0;                           //��λ��˵�1���뻺�������С�
}

/*--------------------------------------------------------------*/
/*-------------------------���淢�ͺ���-------------------------*/
/*---------���ܣ�USB����λ��ȡ���������ϴ�������----------------*/
/*--------------------------------------------------------------*/
void SendReport(void)
{
	if(ConfigValue!=0)
		if(!Ep1InIsBusy)			//�˵�1�����Ƿ�æ�ı�־������������������ʱ���ñ�־Ϊ1
		{
			D12WriteEndpointBuffer(3,count,InBuf);		//���뱨�滺����
			Ep1InIsBusy=1;							//�˵�1�����Ƿ�æ�ı�־
		}
}
/*--------------------------------------------------------------*/
/*-----------------------������մ�������-----------------------*/
/*---------------���ܣ�USBģ�鷢�����ݵ�STC---------------------*/
/*--------------------------------------------------------------*/
void RecieveReport(void)
{
	unsigned char i,j;
	i=OutBuf[0]&0x0F;							//usbģ�黺���ָ�������������
	j=write_tele_t(OutBuf,i+3);					//��USB������滺����д�뵽���ķ��ͻ�����
	
	if(j)
	{
		UsbOutFlag=0;							//д��ɹ�����־λ���
		
	}
}

/*--------------------------------------------------------------*/
/*-------------------------�ⲿ�ж�0����------------------------*/
/*--------------------------------------------------------------*/
void interrupt_INT0() interrupt 0          //�ⲿ�ж�0���жϺ�Ϊ0
{
	unsigned char InterruptSource;
	D12WriteCommand(0xF4);                   //��ȡ�жϼĴ���
	InterruptSource=D12ReadByte();		   //�������϶�ȡ����
  if(InterruptSource&0x80)UsbBusSuspend(); //���߹����жϴ���
  if(InterruptSource&0x40)UsbBusReset();   //���߸�λ�жϴ���
  
  if(InterruptSource&0x01)UsbEp0Out();     //�˵�0����жϴ���
  if(InterruptSource&0x02)UsbEp0In();      //�˵�0�����жϴ���
  
  if(InterruptSource&0x04)UsbEp1Out();     //�˵�1����жϴ���
  if(InterruptSource&0x08)UsbEp1In();      //�˵�1�����жϴ���
  
  if(InterruptSource&0x10)UsbEp2Out();     //�˵�2����жϴ���
  if(InterruptSource&0x20)UsbEp2In();      //�˵�2�����жϴ���
}