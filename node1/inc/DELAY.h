/*--------------------------------------------------------------*/
//File:  	DELAY.h
//Date:  	13:12 2012-6-5
/*--------------------------------------------------------------*/
//晶振频率须设置为32.256MHz，每个机器周期为0.3us

#ifndef __DELAY_H__
#define __DELAY_H__

/*--------------------------------------------------------------*/
//总的延时时间=0.3ms*mmtime+1.2us，函数调用和返回各两个机器周期

void delayms(int mtime)	//延时0.3ms
{
	int m, n;
	for(m = mtime; m > 0; m--)
		for(n =125; n > 0; n--);
}

/*--------------------------------------------------------------*/
//总的延时时间=0.6us*utime+1.2us，函数调用和返回各两个机器周期

/*void delayus(char utime)//延时0.6us
{
	while(--utime);
}
*/
#endif