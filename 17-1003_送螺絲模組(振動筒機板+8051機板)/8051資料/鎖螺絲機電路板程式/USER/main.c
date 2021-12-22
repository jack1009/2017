/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stm32f10x.h>
#include <stdio.h>
#include "sys.h"
#include "delay.h"	
#include "led.h" 	
#include "usart.h"		

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
u8 autoFlow=0;						//自動流程 
u8 manualToAuto=0;				//手動=>自動旗標
u8 autoToManual=0;				//自動=>手動旗標
u8 keyup_upDown_pb=1;		//鎖螺絲按鍵旗標
u8 keyup_withstand_pb=1;	//頂起按鍵旗標
u8 keyup_sendScrew_pb=1;	//送螺絲按鍵旗標
u8 keyup_screwStart_pb=1;	//起子啟動按鍵旗標
u8 errorStatus=0;					//異常狀態
u8 initFlow=0;						//????????
u8 isFloatCheckOK=0;			//????????
u8 homeFlag=0;						//??????
u8 finishFlag=0;					//??????
u8 isScrewInTube=0;				//???????
u8 isScrewInClip=0;				//????????
u8 sendScrewFlag=0;				//?????
u8 errorLackOccurFlag=0;	//???????
u8 lackCounter=0;  				//??????????,?????????.???????????????.

u8 error_slip=0;					//????
u8 error_float=0;					//????
u8 error_up_sol=0;				//???????
u8 error_down_sol=0;			//???????
u8 error_full_snr=0;			//????
u8 error_lack_snr=0;			//?????

u16 AD_value;							//ADC??????
vu16 ADC_ConvertedValue;	//ADC??????

u16 count_f3=0;						//????autoFlow==3
u16 count_f4_slipNG=0;		//????autoFlow==4,????NG
u16 count_adc_show=0;			//ADC???
u16 count_f4_timed=0;     //???????,?????

u8 errorCode[8];					//??????
u16 count_up_sol_error=0;				//?????????
u16 count_down_sol_error=0;			//?????????
u16 count_full_snr_error=0;			//??????
u16 count_errorStatus=0;	//??????

#define ADC1_DR_Address    ((u32)0x4001244C)

void GPIO_Configuration(void);
void USART_Configuration(void);
void ADC_Configuration(void);
void machine_init(void);
void dataChange(void);
u8 floatCheck(void);
u8 valueToBit(u8 inValue,u8 inBit);

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
	/*機板初始化*/
	Stm32_Clock_Init(9);
	delay_init(72);
	GPIO_Configuration();
	USART_Configuration();
	ADC_Configuration();
	
 	 printf("*****************************************************************\r\n");
     printf("*                                                               *\r\n");
     printf("*  Thank you for using HY-RedBull V3.0 Development Board ! ^_^  *\r\n");
     printf("*                                                               *\r\n");
     printf("*****************************************************************\r\n");

	/*主程式*/
	while(1)
	{
		/*ADC秀值*/
//		count_adc_show++;
//		if(count_adc_show<1000)
//		{
//			count_adc_show++;
//			delay_ms(1);
//		}
//		else
//		{
//			count_adc_show=0;
//			AD_value = (u16)ADC_ConvertedValue;
// 			printf("The current AD value = %4d \r\n", AD_value);
//			printf("up/down relay= %4d \r\n",upDown_sol);
//		}
	
		/*異常*/
		if(error_slip)
			errorStatus=1;
		else if(error_float)
			errorStatus=2;
		else if(error_lack_snr)
			errorStatus=4;
		else if(error_full_snr)
			errorStatus=5;
		else if(error_up_sol)
			errorStatus=6;
		else if(error_down_sol)
			errorStatus=7;
		else
			errorStatus=0;
		
		/*異常輸出*/
		if(errorStatus)
		{
			int i;
			count_errorStatus++;
			for(i=0; i<8; i++)
			{
				errorCode[i]=valueToBit(errorStatus,i);
			}
			if(errorCode[0])
				errorCode0_ry=1;
			if(errorCode[1])
				errorCode1_ry=1;
			if(errorCode[2])
				errorCode2_ry=1;
		}
		else
		{
				errorCode0_ry=0;
				errorCode1_ry=0;
				errorCode2_ry=0;
			  count_errorStatus=0;
		}
		
		/*缺螺絲檢查*/
 		if(lack_snr==0)//當檢知動作時
 			isScrewInTube=1;
//			isScrewInClip=1;//不使用缺螺絲檢查時
		
		/*滿料檢查*/
		if(full_snr)
		{
			count_full_snr_error++;
			if(count_full_snr_error>10000)
			{
				error_full_snr=1;
			}
			delay_ms(1);
		}
		else
		{
			count_full_snr_error=0;
			error_full_snr=0;
		}
		
		/*完成訊號*/
		if(finishFlag)
			finishOutput_ry=1;
		else
			finishOutput_ry=0;
		
		/*異常清除*/
		if(stop_pb==1)
		{
			error_slip=0;
			error_float=0;
			error_up_sol=0;
			error_down_sol=0;
			error_lack_snr=0;
			isScrewInClip=1;
		}
		
		/*原點輸出*/
		if(up_ls==0 && withstand_sol==0 && sendScrew_sol==0 && screwStart_ry==0)
		{
			homeOutput_ry=1;
			homeFlag=1;
		}
		else
		{
			homeOutput_ry=0;
			homeFlag=0;
		}
		
		/***************************************************************************
		**手動程序
		****************************************************************************/
		if(autoManual_pb)
		{
			autoFlow=0;
			manualToAuto=0;
			
			/*自動=>手動*/
			if(autoToManual<1)
			{
				machine_init();
				autoToManual++;
			}			
			/*鎖螺絲*/
			if(keyup_upDown_pb && upDown_pb==0)
			{
				delay_ms(10);
				keyup_upDown_pb=0;
				upDown_sol=!upDown_sol;
				screwStart_ry=!screwStart_ry;
			}
			if(upDown_pb)
				{
					delay_ms(10);
					keyup_upDown_pb=1;
				}
				
				/*頂起*/
				if(keyup_withstand_pb && withstand_pb==0)
			{
				delay_ms(10);
				keyup_withstand_pb=0;
				withstand_sol=!withstand_sol;
			}
			if(withstand_pb)
				{
					delay_ms(10);
					keyup_withstand_pb=1;
				}
				
				/*送螺絲*/
				if(reset_pb && keyup_sendScrew_pb && sendScrew_pb==0)
			{
				delay_ms(10);
				keyup_sendScrew_pb=0;
				sendScrew_sol=1;
				dataChange();
				delay_ms(250);
				sendScrew_sol=0;
			}
			if(sendScrew_pb)
			{
				delay_ms(10);
				keyup_sendScrew_pb=1;
			}
			
				/*起子啟動*/
//				if(keyup_screwStart_pb && screwStart_pb==0)
//			{
//				delay_ms(10);
//				keyup_screwStart_pb=0;
//				screwStart_ry=!screwStart_ry;
//			}
//			if(screwStart_pb)
//				{
//					delay_ms(10);
//					keyup_screwStart_pb=1;
//				}

		}
		
		/**********************************************************
		**自動
		***********************************************************/
		else
		{
			autoToManual=0;
			
			/*急停和異常時機器復歸*/
			if((errorStatus!=0 && errorStatus!=5) || stop_pb  )
				machine_init();

			/*上下磁簧狀態*/
			if(down_ls==0)
				sendScrew_sol=1;
			if(up_ls==0)
				sendScrew_sol=0;
			
			/*手動=>自動*/
			if(manualToAuto<1)
			{
 				machine_init();
				manualToAuto++;
			}
			
			/*上磁簧異常*/
			if(upDown_sol==0 && up_ls==1)
			{
				count_up_sol_error++;
				if(count_up_sol_error>3000)
				{
					error_up_sol=1;
				}
				delay_ms(1);
			}
			else
			{
				count_up_sol_error=0;
			}
			/*下磁簧異常*/
			if(upDown_sol==1 && down_ls==1)
			{
				count_down_sol_error++;
				if(count_down_sol_error>3000)
				{
					error_down_sol=1;
					
				}
				delay_ms(1);
			}
			else
			{
				count_down_sol_error=0;
			}
			
		/*缺螺絲異常*/
		if(isScrewInClip==0 && errorLackOccurFlag==1)
		{
			errorLackOccurFlag=0;
			lackCounter++;
			if(lackCounter>1)
			{
				error_lack_snr=1;
			}
		}
		if(isScrewInClip)
		{
			errorLackOccurFlag=1;
		}
			
			/*******************************************************************
			**自動主程式
			********************************************************************/
			/*power on*/
			if(autoFlow==0 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				count_up_sol_error=0;
				count_down_sol_error=0;
				count_f3=0;
				count_f4_slipNG=0;
				count_f4_timed=0;
				autoFlow=1;
				sendScrewFlag=0;
			}
			
			/*idle*/
			if(autoFlow==1 && stop_pb==0 &&(errorStatus==0 | errorStatus==5))
			{
				if(sendScrewFlag)
				{
					dataChange();
					sendScrewFlag=0;
				}
				if(start_pb==0 && reset_pb==1 && up_ls==0 && homeFlag==1)
				{
					delay_ms(10);
					autoFlow=2;
				}
			}
			
			/*Down*/
			if(autoFlow==2 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				finishFlag=0;
				upDown_sol=1;
				if(down_ls==0)
				{
					autoFlow=3;
				}
			}
			
			/*Clamp ON*/
			if(autoFlow==3 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				screwStart_ry=1;
				sendScrewFlag=1;
				withstand_sol=1;
				if(count_f3<300)
				{
					count_f3++;
					delay_ms(1);
				}
				else
				{
					autoFlow=4;
					count_f3=0;
				}
			}
			
			/*SlipCheck*/
			if(autoFlow==4 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				/*Have Slip SNR*/
				if(slip_snr==1)
				{
					autoFlow=5;
					count_f4_slipNG=0;
				}
				else
				{
					count_f4_slipNG++;
					if(count_f4_slipNG >= 2000)
					{
						error_slip=1;
						count_f4_slipNG=0;
					}
					delay_ms(1);
				}
				/*Have Slip SNR END*/
				
				/*Have NOT Slip SNR*/
				
// 					if(count_f4_timed<AD_value)
// 					{
// 						count_f4_timed++;
// 					}
// 					else
// 					{
// 						count_f4_timed=0;
// 						autoFlow=5;
// 					}
				/*Have NOT Slip SNR END*/

			}
			
			/*Float Check*/
			if(autoFlow==5 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				//isFloatCheckOK=floatCheck();  //?????
 				isFloatCheckOK=1;						//??????
				if(isFloatCheckOK==0)
				{
					error_float=1;
				}
				else
				{
					delay_ms(100);
					autoFlow=6;
				}
			}
			
			/*Up*/
			if(autoFlow==6 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				screwStart_ry=0;
				upDown_sol=0;
				if(up_ls==0)
				{
					autoFlow=7;
					finishFlag=1;					
				}
			}
			
			/*Clamp Off*/
			if(autoFlow==7 && stop_pb==0 && (errorStatus==0 | errorStatus==5))
			{
				withstand_sol=0;
				if(reset_pb==0)
				{
					autoFlow=1;
				}
			}
		}
	}	 
}


/*****************************************************************************
**Machine Init
******************************************************************************/
	
void machine_init(void)
{
	screwStart_ry=0;
	autoFlow=0;
	delay_ms(10);
	upDown_sol=0;
	while(up_ls)
	{}
		sendScrew_sol=0;
		withstand_sol=0;
		finishFlag=1;
}

/******************************************************************************
**Float Check
*******************************************************************************/
u8 floatCheck(void)
{
	u16 okCount=0;
	u16 ngCount=0;
	
	do
	{
		if(float_snr==0)
		{
			okCount++;
			ngCount=0;
		}
		else
		{
			ngCount++;
			okCount=0;
		}
		delay_ms(1);
	}
	while(okCount<300 && ngCount<300);
		if(okCount>=300)
		{return 1;}
		else
		{return 0;}
}

/************************************************************
**Feed Screw Check
*************************************************************/
void dataChange(void)
{
	isScrewInClip=isScrewInTube;
	isScrewInTube=0;
}

/************************************************************
**
*************************************************************/
u8 valueToBit(u8 inValue,u8 inBit)
{
	u8 i;
	u8 result;
	for(i=0 ; i<(inBit+1) ; i++)
	{
		result=inValue%2;
		inValue=inValue/2;
	}
	return result;
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG , ENABLE); 						 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE); 						 

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
  /*
  *  USART1_TX -> PA9 , USART1_RX -> PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TC);
  USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Configure the ADC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);   
   
  /* DMA channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
    
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
