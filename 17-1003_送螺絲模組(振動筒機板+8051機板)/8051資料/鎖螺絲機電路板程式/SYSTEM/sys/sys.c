#include "stm32f10x.h"
#include <stm32f10x.h>		 
#include "sys.h"

void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);
}

void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;
	temp1<<=8;
	temp=SCB->AIRCR;
	temp&=0X0000F8FF;
	temp|=0X05FA0000; 
	temp|=temp1;	   
	SCB->AIRCR=temp; 	  				   
}

// void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
// { 
// 	u32 temp;	
// 	u8 IPRADDR=NVIC_Channel/4; 
// 	u8 IPROFFSET=NVIC_Channel%4;
// 	IPROFFSET=IPROFFSET*8+4;    
// 	MY_NVIC_PriorityGroupConfig(NVIC_Group);
// 	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
// 	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
// 	temp&=0xf;

// 	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;
// 	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);    
// 	NVIC->IPR[IPRADDR]|=temp<<IPROFFSET;  	  				   
// }

void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;
	EXTOFFSET=(BITx%4)*4;

	RCC->APB2ENR|=0x01;

	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;
	
	EXTI->IMR|=1<<BITx;
	//EXTI->EMR|=1<<BITx;
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;
} 


void MYRCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000; 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR = 0x00000014; 
  	RCC->APB2ENR = 0x00000000;   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;    													 
	RCC->CFGR &= 0xF8FF0000;   
	RCC->CR &= 0xFEF6FFFF;    
	RCC->CR &= 0xFFFBFFFF;    	  
	RCC->CFGR &= 0xFF80FFFF;  
	RCC->CIR = 0x00000000;   

	#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else   
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

__asm void WFI_SET(void)
{
	WFI;    
}

void Sys_Standby(void)
{
	SCB->SCR|=1<<2; 
  	RCC->APB1ENR|=1<<28;  
 	PWR->CSR|=1<<8;         
	PWR->CR|=1<<2;         
	PWR->CR|=1<<1;       
	WFI_SET();				
}	  

//void BKP_Write(u8 reg,u16 dat)
//{
//  RCC->APB1ENR|=1<<28;     
//	RCC->APB1ENR|=1<<27;       
//	PWR->CR|=1<<8;          
//	switch(reg)
//	{
//		case 1:
//			BKP->DR1=dat;
//			break;
//		case 2:
//			BKP->DR2=dat;
//			break;
//		case 3:
//			BKP->DR3=dat;
//			break; 
//		case 4:
//			BKP->DR4=dat;
//			break;
//		case 5:
//			BKP->DR5=dat;
//			break;
//		case 6:
//			BKP->DR6=dat;
//			break;
//		case 7:
//			BKP->DR7=dat;
//			break;
//		case 8:
//			BKP->DR8=dat;
//			break;
//		case 9:
//			BKP->DR9=dat;
//			break;
//		case 10:
//			BKP->DR10=dat;
//			break;
//	} 
//}	    

void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 

void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;       
	AFIO->MAPR&=0XF8FFFFFF; 
	AFIO->MAPR|=temp;      
} 

void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		 
	RCC->CR|=0x00010000;  
	while(!(RCC->CR>>17));
	RCC->CFGR=0X00000400; 
	PLL-=2;
	RCC->CFGR|=PLL<<18;   
	RCC->CFGR|=1<<16;	  
	FLASH->ACR|=0x32;	 

	RCC->CR|=0x01000000; 
	while(!(RCC->CR>>25));
	RCC->CFGR|=0x00000002;
	while(temp!=0x02)    
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}    
}		    




























