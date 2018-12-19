#include "stm32f10x.h"

/*define*/
#define ADC1_DR_Address    ((u32)0x4001244C)
#define ADC_Channel_Count 5
#define ADC_Sample_Times 8

/*global variable definition*/
u32 ADC_ConvertedValue[ADC_Sample_Times][ADC_Channel_Count];
u32 ADC_Filtered_Value[ADC_Channel_Count];
const u16 CCR4 = 75;//5% duty
const u16 CCR3 = 75;//5% duty
u16 Next_Boost = 0;

int Ek = 0;
int Ek_1 = 0;
int EkL = 0;
int EkL_1 = 0;

/*function declararion*/
void DMA_Config(void);
void ADC1_Config(void);
void TIM4_Config(void);
void GPIOB_Config(void);
void GPIOC_Config(void);
void TIM1_PWM_Init(void);
void ADC_Value_Init(void);

void filter(void);
void delay_ms(uint16_t time);

int main()
{
	SystemInit();
	ADC_Value_Init();
	delay_ms(1000);
	
	DMA_Config();	
	ADC1_Config();	
	
	GPIOB_Config();	
  GPIOC_Config();
  TIM4_Config();
//  TIM1_PWM_Init();	
	GPIO_SetBits(GPIOC, GPIO_Pin_13);

	delay_ms(300);
	filter();
  EkL_1 = EkL;
	EkL = (int)(ADC_Filtered_Value[0] - 1363);
	
	delay_ms(300);
	filter();
	EkL_1 = EkL;
	EkL = (int)(ADC_Filtered_Value[0] - 1363);	
	delay_ms(300);
	
	while(1)
	{
		filter();
/*		
		if( (ADC_Filtered_Value[0] < 800) || (ADC_Filtered_Value[0] > 1600) )//BoostL Low/High input protection
		{
			TIM4->CCR3 = 1;
			break;
		}
*/		

		//BuckL
		EkL_1 = EkL;
		EkL = (int)(ADC_Filtered_Value[0] - 1363);				
    if (ADC_Filtered_Value[0] > 1363)//14V
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			if(ADC_Filtered_Value[0] > 1454)//16V
			{
				TIM4->CCR3 = 100;
			}
			else
			{
				if((TIM4->CCR3 - EkL/80 + (EkL - EkL_1)/20) < 75)
				{
					TIM4->CCR3 = 75;
				}
				else
				{
					TIM4->CCR3 = TIM4->CCR3 - EkL/80 + (EkL - EkL_1)/20;
				}	
			}
		}

		else
    {			
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
			if((TIM4->CCR3 - EkL/80 - (EkL - EkL_1)/20) > 400)
			{
				TIM4->CCR3 = 400;
			}
			else
			{
				TIM4->CCR3 = TIM4->CCR3 -EkL/80 - (EkL - EkL_1)/20;
			}
		}	
		
	}
	return 0;
	
}


/*DMA Setting*/
void DMA_Config()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //enable DMA1 clock
  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(DMA1_Channel1);  //Setting DMA channels
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//Setting DMA peripheral address
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue[0][0];	//DMA address, access ADC converting result directly
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
  DMA_InitStructure.DMA_BufferSize = (ADC_Sample_Times * ADC_Channel_Count);	//DMA buffer setting£»
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);  //Enable DMA channer
	
//	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//Enable DMA interupt
	
//	ADC_SoftwareStartConvCmd(ADC1, DISABLE);  //Begin converting
}

/*DMA interuption service function
void DMA1_Channel1_IRQHandler()
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
			ADValue = (u32)(3300000/4096*ADC_ConvertedValue[1]/1000);
			DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}
*/

/*ADC1 Setting*/
void ADC1_Config(void)
{	
	/*GPIO£¬PA0 1 2 3 4  as analog input*/   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 
	GPIO_InitTypeDef GPIO_InitStructure; 	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		    //GPIO as analog input
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
	
	/*ADC work mode setting*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
  ADC_InitTypeDef ADC_InitStructure;	
	
	ADC_DeInit(ADC1); 
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 5; 
 	ADC_Init(ADC1, &ADC_InitStructure);
	
  /*Establish ADC1 converting groups*/ 	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);

  /*binding ADC1 and DMA channel 1*/
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE); //Enable ADC1
		
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	
	
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
}

void GPIOC_Config(void)
{
	//Open clock first
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	
	//PC13 as pull-push output
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}


//PWM
void GPIOB_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM4_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//Freq = 72M / 1500 / 1 = 48KHz
	TIM_TimeBaseStructure.TIM_Period = (1500 - 1);
	TIM_TimeBaseStructure.TIM_Prescaler = (1 - 1);
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	//Duty = CCR1 / ARR = 500 / 1000 = 50% 
	//u16 CCR1 = 500;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM4,ENABLE);
}

void delay_ms(uint16_t time)
{    
    uint16_t i = 0; 

    while(time--)
    {
        i = 10000;
        while(i--);    
    }
}

void ADC_Value_Init(void)
{
	for(int i = 0; i< ADC_Channel_Count;i++)
	{
		 ADC_Filtered_Value[i] = 0;
		for(int j = 0; j < ADC_Sample_Times; j++)
		{
			ADC_ConvertedValue[j][i] = 0;
		}
	}
}

void filter(void)
{
	u32 sum = 0;
	u32 temp = 0;
	
	/*Sorting the data, large in the bottom*/
	for(int i = 0; i< ADC_Channel_Count;i++)//Sorting accoring to column
	{
		for(int k = 0; k < ADC_Sample_Times - 1; k++)//Sorting each column
		{
			for(int j = 0; j < ADC_Sample_Times - 1 - k; j++)
			{
				if(ADC_ConvertedValue[j][i] > ADC_ConvertedValue[j+1][i])
				{
					temp = ADC_ConvertedValue[j][i];
					ADC_ConvertedValue[j+1][i] = ADC_ConvertedValue[j][i];//Swap
					ADC_ConvertedValue[j][i] = temp;				
				}
			}
		}
	}

	/*Get average*/
	for(int i = 0; i< ADC_Channel_Count;i++)
	{
		for(int j = 2; j <= ADC_Sample_Times - 2 ; j++)//remove extreme datas
		{
			sum += ADC_ConvertedValue[j][i];
		}	
		ADC_Filtered_Value[i] = sum / (ADC_Sample_Times - 4)* 805 / 1000; //converting to mV
		sum = 0;
	}
	
}
/* ADC_ConvertedValue[ADC_Sample_Times][ADC_Channel_Count];
 ADC_Filtered_Value[ADC_Channel_Count];
void filter()
{
	for(int i = 0; i < 24 ; i++)
	{
		if(i%3 == 0)
		sum += ADC_ConvertedValue[i];
	}
	FilteredValue[0] = sum/8;
	FilteredValue[0]  = 3300000/4096*FilteredValue[0]/1000;
}

void Init_ConvertedValue()
{
	for(int i = 0; i < 24; i++)
	{
		ADC_ConvertedValue[i] = 0;
	}
}
*/

//PWM
void TIM1_4_PWMShiftInit(void)  
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;  
    GPIO_InitTypeDef  GPIO_InitStruct;  
    TIM_OCInitTypeDef TIM_OCInitStruct;  
 
    /**********************TIM1 4 GPIO Setting*****************************/ 
		//TIM4  CH3 = PB8 = BuckL   CH4 = PB9	= BoostL
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);  
      
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  
  
    GPIO_Init(GPIOB,&GPIO_InitStruct);  
	
	  //TIM1  CH1 = PA8 = BuckL   CH2 = PA9 = BoostH
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; 
    GPIO_Init(GPIOA,&GPIO_InitStruct);  
      
    /**********************Initializing TimBase*************************/  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);	
      
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;  
    TIM_TimeBaseInitStruct.TIM_Period    = 800;       //frequency = 72000000/PSC/(ARR+1) = 90KHz 
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;       
      
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);  
		
    /**********************Initializing TIM4 OC*************************/  
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;  
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCInitStruct.TIM_Pulse = 20;                 //TIM4 CH3 duty£º375/£¨ARR+1£©=25%
    TIM_OC3Init(TIM4,&TIM_OCInitStruct);  
		
    //TIM1 CH1pwm output 
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;  		
    TIM_OCInitStruct.TIM_Pulse=20;                   //TIM1 CH1 duty£º375/£¨ARR+1£©=25%
    TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;                      
    TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Disable;// Never rewrite this line!               
    TIM_OC1Init(TIM1,&TIM_OCInitStruct);//TIM1  CH1 = PA8 = BuckL         
  
    /**************************Config trigger mode**************************/
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;  
    TIM_OCInitStruct.TIM_Pulse = 400-2;             //phase offset£º360*750/(ARR+2) = 180 degree
    TIM_OC4Init(TIM1,&TIM_OCInitStruct);          
  
    /**************************Config Master-Slave mode*************************/
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC4Ref);  //TIM1 OC4 trigger
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable); 
 
    TIM_SelectInputTrigger(TIM4, TIM_TS_ITR0);      //ITRO trigger
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Trigger);   
	
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);               //Enable output
}