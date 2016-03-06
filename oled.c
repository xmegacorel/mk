//#define OLED_ADDR (0x78 <<1)
#define OLED_ADDR 0x78
#define LIS_LONG_TIMEOUT 0xffff

#include "stm32f0xx.h"
#include "delay.h"
#include "codetab.h"




void InitI2CBus()
{
	
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA/*RCC_APB2Periph_GPIOA*/,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	
	
}

uint16_t LIS_ReadReg(uint8_t RegName)
{
	

	uint16_t tmp = 0;
	uint32_t LIS_Timeout;

	/* Test on BUSY Flag */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET)
	{
		if((LIS_Timeout--) == 0) return 1;
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C1, OLED_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		if((LIS_Timeout--) == 0) return 2;
	}

	/* Send Register address */
	I2C_SendData(I2C1, (uint8_t)RegName);

	/* Wait until TC flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC) == RESET)
	{
	if((LIS_Timeout--) == 0) return 3;
	} 
}

uint8_t LIS_WriteReg(uint8_t RegName, uint16_t RegValue)
{

	uint32_t LIS_Timeout;

	/* Test on BUSY Flag */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET)
	{
		if((LIS_Timeout--) == 0) return 1;
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C1, OLED_ADDR, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		if((LIS_Timeout--) == 0) return 2;
	}

	/* Send Register address */
	I2C_SendData(I2C1, (uint8_t)RegName);

	/* Wait until TCR flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TCR) == RESET)
	{
		if((LIS_Timeout--) == 0) return 3;
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C1, OLED_ADDR, 1, I2C_AutoEnd_Mode, I2C_No_StartStop); // 1 ??? 2 ????


	/* Wait until TXIS flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT; ;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		if((LIS_Timeout--) == 0) return 4;
	}

	/* Write data to TXDR */
	I2C_SendData(I2C1, RegValue);

	/* Wait until STOPF flag is set */
	LIS_Timeout = LIS_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET)
	{
	if((LIS_Timeout--) == 0) return 5;
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);

	return 10;
	
}


void I2C_WriteByte(uint8_t addr,uint8_t data)
{
	
		LIS_WriteReg(addr, data);
	
		return;
		
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
		I2C_TransferHandling(I2C1, OLED_ADDR, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

		
	  while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET);
	
		I2C_SendData(I2C1, addr);
		
		while(I2C_GetFlagStatus(I2C1, I2C_ISR_TCR) == RESET);
		
		I2C_TransferHandling(I2C1, OLED_ADDR, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);


		while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET);


		I2C_SendData(I2C1, data);


		while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET);


		I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}


void WriteCmd(unsigned char I2C_Command)//??????
{
	I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(unsigned char I2C_Data)//??????
{
	I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
	DelayMs(100); //????µ????±??????

	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //??¶?µ??? 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y) //????????µ???±?
{
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//?«??????
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//????
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : ?«OLED????????»???
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //????µ???±?
	WriteCmd(0X14);  //????µ???±?
	WriteCmd(0XAF);  //OLED»???
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          :
// Parameters     : x,y -- ????µ???±?(x:0~127, y:0~7); ch[] -- ??????µ???·??®; TextSize -- ??·?????(1:6*8 ; 2:8*16)
// Description    : ????codetab.h??µ?ASCII??·?,??6*8??8*16??????
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : ??OLED???? -- ??????????,OLED?¦???»µ?10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //????µ???±?
	WriteCmd(0X10);  //??±?µ???±?
	WriteCmd(0XAE);  //OLED????
}
