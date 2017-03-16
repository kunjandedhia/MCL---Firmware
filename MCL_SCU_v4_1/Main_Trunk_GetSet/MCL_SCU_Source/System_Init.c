/******************************************************************************
**@file     :    System_Init.c
* 
**@brief    :   Handles System Initalization.
******************************************************************************
*/
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Config.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/

/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARITION
* ----------------------------------------------------------------------------
*/
/*****************************************************************************
**@Function     : fnSystem_Init
**@Descriptions : Initialize all peripherals of system
**@parameters   : None
**@return       : None
*****************************************************************************/
void fnSystem_Init(void)
{
  //	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  /********************Reciever Relay Interface Init*********/
  fnReciever_Config();
  /*********Initilize System Tick for ETH Delay*************/
  fnSysTick_Init();
  /*************UART VCM,HCM Communication Init*************/
  fnUSART_Config();
  /***************Serial Debug USB-USART Init***************/
  DebugComPort_Init();
  
#ifdef DBG
  printf("\n\rSCU START\r\n");
#endif
  /********************CAN Init******************************/
  fnCAN_INIT();
  /********************NVSRAM Init***************************/
  fnNVSRAM_Config();  /*initialize NVSRAM peripheral*/
  /********************EEPROM Init***************************/
  fnEEPROM_Config();  /*initialize EEPROM peripheral*/
  /********************SPI Init ***************************/
  fnSPI_Config(); /* Called here since common for EEPROM AND NVSRAM*/
  /********************Hooter Relay Init*********************/
  fnHooter_Config();
  /********************I2C1 Config***************************/
  fnI2C_Configuration();
  /********************Timer Initialization******************/
  fnTIM_Configuration();
  /****configure ethernet (GPIOs, clocks, MAC, DMA)**********/
  // fnETH_Init();
  /**********************Initilize wifi usart****************/
  fnWifiInit(115200);
  /********************DAC Initialization***************/
  fnDAC_Config();
  /*******************ADC Initialization***************/
  fnADC_Config();
  
  /*Reset the FIFO of all the Modules*/
  fnFifo_Reset(&stFifo_HMI);
  fnFifo_Reset(&stFifo_OPMM);
  fnFifo_Reset(&stFifo_VCM);
  fnFifo_Reset(&stFifo_HCM);
  StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
  
}


/*****************************************************************************
**@Function 			: fnPressure_Regulator_Config
**@Descriptions	: Initialize  Pressre Regulation Required Peripherals
**@parameters		: None
**@return				: None
*****************************************************************************/

void fnPressure_Regulator_Config(void)
{
  /*DAC Initialization*/
  fnDAC_Config();
  /*ADC Initialization*/
  fnADC_Config();
  
}


/*****************************************************************************
**@Function 			: fnCAN_INIT()
**@Descriptions	: Initialize  Internal CAN
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnCAN_INIT(void)
{
  /*initialize internal CAN1 peripheral*/
  fnCAN_Config(CAN1);  
  /*initialize internal CAN2 peripheral*/
  fnCAN_Config(CAN2);  
  
}	

/*****************************************************************************
**@Function 			: fnNVSRAM_Config()
**@Descriptions	: Initialize  NVSRAM Required Peripherals
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnNVSRAM_Config(void)
{
  /*initialize GPIOS Required for NVSRAM driving used for Datalogging*/
  fnDataLogging_Init(); 
  /*initialize internal RTC Used for Time stamp during Datalogging*/
  fnRTC_Init(); 
}	

/*****************************************************************************
**@Function 			: fnEEPROM_Config()
**@Descriptions	: Initialize  EEPROM Required Peripherals
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnEEPROM_Config(void)
{
  /*GPIO Initialization Structure variable*/
  GPIO_InitTypeDef  stGPIO_InitStructure; 
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | 
                             RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | 
                               RCC_AHB1Periph_GPIOI, ENABLE);
  stGPIO_InitStructure.GPIO_Pin 	= EEPROM_CHIP_SELECT;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EEPROM_CHIP_SELECT_PORT, &stGPIO_InitStructure);
  GPIO_SetBits(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT);
}	



/*****************************************************************************
**@Function 			: fnReciever_Config()
**@Descriptions	: Initialize  Remote Reciever GPIOs
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnReciever_Config(void)
{
  /*GPIO,EXTI,NVIC Initialization Structure variable*/
  GPIO_InitTypeDef  stGPIO_InitStructure; 
  EXTI_InitTypeDef stEXTI_InitStructure;
  NVIC_InitTypeDef stNVIC_InitStructure;
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | 
                             RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | 
                               RCC_AHB1Periph_GPIOI, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
  /*Prime Selector GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= PRIME_SELECT_PIN;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PRIME_SELECT_PORT, &stGPIO_InitStructure);
  
  
  
  /*Flush Selector GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= FLUSH_SELECT_PIN;
  GPIO_Init(FLUSH_SELECT_PORT, &stGPIO_InitStructure);
  
  /*Spray Selector GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= SPRAY_SELECT_PIN;
  GPIO_Init(SPRAY_SELECT_PORT, &stGPIO_InitStructure);
  
  /*Read Pipe Temperature GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= READ_PIPE_TEMP_SELECT_PIN;
  GPIO_Init(READ_PIPE_TEMP_SELECT_PORT, &stGPIO_InitStructure);
  
  /*START Process GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= START_SELECT_PIN;
  GPIO_Init(START_SELECT_PORT, &stGPIO_InitStructure);
  
  /*Flush Complete OR Auto Stall Start GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= FLUSH_COMPLETE_SELECT_PIN;
  GPIO_Init(FLUSH_COMPLETE_SELECT_PORT, &stGPIO_InitStructure);
  
  /*ESTOP Signal GPIO */
  stGPIO_InitStructure.GPIO_Pin 	= ESTOP_SIGNAL_SELECT_PIN;
  GPIO_Init(ESTOP_SIGNAL_SELECT_PORT, &stGPIO_InitStructure);
  
  /* Tell system that you will use PH4 for EXTI_Line4 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource4);
  
  /* PI0 is connected to EXTI_Line4 */
  stEXTI_InitStructure.EXTI_Line = EXTI_Line4;
  /* Enable interrupt */
  stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* Interrupt mode */
  stEXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* Triggers on rising and falling edge */
  stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* Add to EXTI */
  EXTI_Init(&stEXTI_InitStructure);
  
  /* Add IRQ vector to NVIC */
  /* PH4 is connected to EXTI_Line4, which has EXTI4_IRQn vector */
  stNVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  /* Set priority */
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  /* Set sub priority */
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  /* Enable interrupt */
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Add to NVIC */
  NVIC_Init(&stNVIC_InitStructure);
  
  /*START Signal GPIO */
  stGPIO_InitStructure.GPIO_Pin = START_SIGNAL_SELECT_PIN;
  GPIO_Init(START_SIGNAL_SELECT_PORT, &stGPIO_InitStructure);
  
  /* Tell system that you will use PI0 for EXTI_Line0 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource0);
  
  /* PI0 is connected to EXTI_Line0 */
  stEXTI_InitStructure.EXTI_Line = EXTI_Line0;
  /* Enable interrupt */
  stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* Interrupt mode */
  stEXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* Triggers on rising and falling edge */
  stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* Add to EXTI */
  EXTI_Init(&stEXTI_InitStructure);
  
  /* Add IRQ vector to NVIC */
  /* PI0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
  stNVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  /* Set priority */
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  /* Set sub priority */
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;
  /* Enable interrupt */
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Add to NVIC */
  NVIC_Init(&stNVIC_InitStructure);
  
}	


/*****************************************************************************
**@Function 			: fnHooter_Config()
**@Descriptions	: Initialize  HOOTER Required Peripherals
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnHooter_Config(void)
{
  /*GPIO Initialization Structure variable*/
  GPIO_InitTypeDef  stGPIO_InitStructure; 
  
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | 
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | 
                             RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | 
                               RCC_AHB1Periph_GPIOI, ENABLE);
  
  stGPIO_InitStructure.GPIO_Pin 	= HOOTER_SELECT;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HOOTER_SELECT_PORT, &stGPIO_InitStructure);
  GPIO_ResetBits(HOOTER_SELECT_PORT, HOOTER_SELECT);
}	

/*****************************************************************************
**@Function 			: fnSPI_Config
**@Descriptions	: Initialize  SPI2 Peripheral
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnSPI_Config(void)
{
  
  //	NVIC_InitTypeDef stNVIC_InitStructure;
  SPI_InitTypeDef  stSPI_InitStructure;
  GPIO_InitTypeDef stGPIO_InitStructure;
  /* Peripheral Clock Enable -----------------------------------------------*/
  /* Enable the SPI clock */
  SPIx_CLK_INIT(SPIx_CLK, ENABLE);
  
#ifdef GENERAL_DBG
  //printf("SPI clk \n\r");
#endif	
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);
  
#ifdef GENERAL_DBG
  //printf("GPIO cllk\n\r");
#endif	
  /* SPI GPIO Configuration --------------------------------------------------*/
  
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);    
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);
  
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  
  /* SPI SCK pin configuration */
  stGPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &stGPIO_InitStructure);
  
  /* SPI  MISO pin configuration */
  stGPIO_InitStructure.GPIO_Pin =  SPIx_MISO_PIN;
  GPIO_Init(SPIx_MISO_GPIO_PORT, &stGPIO_InitStructure);  
  
  /* SPI  MOSI pin configuration */
  stGPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &stGPIO_InitStructure);
  
#ifdef GENERAL_DBG
  printf("GPIO Init\n\r");
#endif
  
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIX_PERIPH);
  stSPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  stSPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  stSPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  stSPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  stSPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  stSPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  stSPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  stSPI_InitStructure.SPI_CRCPolynomial = 7;
  
  /* Configure the Priority Group to 1 bit */                
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  //	/* Configure the SPI interrupt priority */
  //	stNVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
  //	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //	stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //	stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //	NVIC_Init(&stNVIC_InitStructure);
  
  /*Configuring SPI as master*/
  
  stSPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPIX_PERIPH, &stSPI_InitStructure);
  
#ifdef GENERAL_DBG
  printf("SPI Init\n\r");
#endif	
  
  
  /* Enable the SPI peripheral */
  SPI_Cmd(SPIX_PERIPH, ENABLE);
#ifdef GENERAL_DBG
  printf("SPI Enable\n\r");
#endif	
  
  /* Disable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPIX_PERIPH, SPI_I2S_IT_RXNE, DISABLE);
  
  /* Disable the Tx empty interrupt */
  SPI_I2S_ITConfig(SPIX_PERIPH, SPI_I2S_IT_TXE, DISABLE);
  
  /* Disable the SPI peripheral */
  /* SPI_Cmd(SPIX_PERIPH, DISABLE); */
}



