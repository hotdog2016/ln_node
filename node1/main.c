/*
 *节点1
 功能：电脑发给串口2,然后串口2再发给串口1,通过串口1发送出去。
 *
 * */


#include "intrins.h"
#include "12C5A60S2.h"
#include "usart.h"
//#include "TELE_MANAGE.h"
 



/*串口2的发送函数，发送一个缓冲区
 *从发送缓冲区里读数据，根据第一个字节的数据来确定发向哪里
 */
int main()
{

	init();
	init_uart();


	while(1)
	{
		usart_test();
	}
}


