#include "main.h"

unsigned char buffer [14];
int g_counter = 0;
int min = 10000, max = 0;
int g_adc_counter = 0;

void I2C_Config(void)
{
	
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);

	// i2C ??????
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = 0x10;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing = 0x0010020A;//0x00201D2B;// now 100kHz 0x0010020A; 400kHz

	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

void ADC_Config()
{
	 ADC_InitTypeDef     ADC_InitStructure;
   GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  ADC_DeInit(ADC1);
  
  ADC_StructInit(&ADC_InitStructure);
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles);
  ADC_GetCalibrationFactor(ADC1);
  ADC_Cmd(ADC1, ENABLE);     
  
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
	ADC_StartOfConversion(ADC1);
  
}





void Timer_Config()
{
		TIM_TimeBaseInitTypeDef TTB;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		int Period = (SystemCoreClock / 1000);
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		TTB.TIM_ClockDivision = 0;
		TTB.TIM_CounterMode = TIM_CounterMode_Up;
		TTB.TIM_Period = Period - 1;
		TTB.TIM_Prescaler = 0;
		TTB.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM1, &TTB);
	
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
		TIM_Cmd(TIM1, ENABLE);
	
		/* Interrupt Timer1 */
    NVIC_ClearPendingIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void OutputPin_Config()
{
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		GPIO_InitTypeDef Gp;
	
		Gp.GPIO_Pin = GPIO_Pin_1; //Set pins inside the struct
		Gp.GPIO_Mode = GPIO_Mode_OUT; //Set GPIO pins as output
		Gp.GPIO_OType = GPIO_OType_PP; //Ensure output is push-pull vs open drain
		Gp.GPIO_PuPd = GPIO_PuPd_NOPULL; //No internal pullup resistors required
		Gp.GPIO_Speed = GPIO_Speed_Level_1; //Set GPIO speed to lowest
		GPIO_Init(GPIOA, &Gp); //Assign struct to LED_GPIO
}


void printf1(int i)
{
	int index = 0;
	while(i >= 10)
	{
		  int p = i / 10;
		  int ost = i - p * 10;
		  if (i == 10)
			{
				buffer[index++] = '0';
				buffer[index++] = '1';
				buffer[index] = 0;
				goto as;
				return;
			}
			else
				buffer[index++] = '0' + ost;
			
			i = p;
	 }
	if (i  < 10 && i != 0 )
		buffer[index++] = '0' + i;
	 
	 buffer[index] = 0;
	 
as:
	 int max = 0;
	 if (index >= 4)
		 max = 2;
	 else 
		 max = 1;
	 for (int j = 0; j < max; j ++)
	 {
		 int m = buffer[j];
		 buffer[j] = buffer[index - j - 1];
		 buffer[index - j - 1] = m;
	 }
}


void ProcessAdcValue()
{
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		unsigned int ADCValue = ADC_GetConversionValue(ADC1);

		int realVoltage = ((3300 * ADCValue) / 4096);
		sprintf((char *)buffer, "%05d", realVoltage);
		
		OLED_ShowStr(0,1, buffer ,1);
			
		if (min > realVoltage)
			 min = realVoltage;
		
		sprintf((char *)buffer, "min - %05d", min);
		OLED_ShowStr(0,2, buffer ,1);
			
		if (max < realVoltage)
			max = realVoltage;
			
		sprintf((char *)buffer, "max - %05d", max);
		OLED_ShowStr(0,3, buffer ,1);
		
		printf1(realVoltage);
		OLED_ShowStr(0,4, buffer ,1);
		realVoltage = (realVoltage < 180) ? 0 : realVoltage - 180;
			
		printf1(realVoltage);
		sprintf((char *)buffer, "real voltage - %d", realVoltage);
		OLED_ShowStr(0,5, buffer ,1);
}





void TIM1_BRK_UP_TRG_COM_IRQHandler (void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
			if (g_counter % 2 == 0)
					GPIO_SetBits(GPIOA, GPIO_Pin_1);
			else
					GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		
			g_counter++;
			
//		  if (g_adc_counter++ > 1000)
//			{
//				OLED_CLS();
//				ProcessAdcValue();
//				g_adc_counter = 0;
//			}
			
		
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


int main()
{
		DelayInit();
		I2C_Config();
		OLED_Init();
	
		ADC_Config();
		Timer_Config();
	  OutputPin_Config();
	
    while(1);
}
