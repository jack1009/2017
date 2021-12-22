#include <stm32f10x_lib.h>
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 	 	 
//Mini STM32开发板范例代码1
//跑马灯实验
//正点原子@ALIENTEK
//2010.5.27	   
int main(void)
{				  
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化
	LED_Init();		  	 //初始化与LED连接的硬件接口
	while(1)
	{
		LED0=0;
		LED1=1;
		delay_ms(300);
		LED0=1;
		LED1=0;
		delay_ms(300);
	}	 
}

























