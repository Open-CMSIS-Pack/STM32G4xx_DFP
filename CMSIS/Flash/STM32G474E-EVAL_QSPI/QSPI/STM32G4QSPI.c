/**
  ******************************************************************************
  * @file    STM32G4QSPI.c
  * @author  MCD Application Team
  * @brief   This file defines the operations of the external loader for
  *          mt25ql512abb QSPI memory of STM32G474E-EVAL.
  *           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */



#include "STM32G4QSPI.h"
#include <string.h>

 #include "./mt25ql512abb/mt25ql512abb.h"

/* Private variables ---------------------------------------------------------*/
BSP_QSPI_Init_t Flash;
/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32G474E_EVAL_QSPI_Private_Functions Private Functions
  * @{
  */

void HAL_Delay(uint32_t Delay)
{
  int i=0;
  for (i=0; i<0x1000; i++);
}
/** @defgroup STM32G474E-EVAL_QSPI_Exported_Functions Exported Functions
  * @{
  */

 /**
  * @brief  Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @param  Init       QSPI Init structure
  * @retval BSP status
  */
 HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{ 
  return HAL_OK;
}
/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Init_QSPI()
{   /*  init system*/
   SystemInit(); 
   HAL_Init();  

  /*  define InterfaceMode and transferRate*/
  Flash.InterfaceMode = BSP_QSPI_QPI_MODE; 
  Flash.TransferRate  = BSP_QSPI_DTR_TRANSFER; 
  Flash.DualFlashMode=BSP_QSPI_DUALFLASH_ENABLE;
  
  /* Configure the system clock  */
   SystemClock_Config();
   
   /*Initialaize QSPI*/
   if(BSP_QSPI_Init(0,&Flash) !=0)
    return 0;
  /*Configure the QSPI in memory-mapped mode*/ 
if(BSP_QSPI_EnableMemoryMappedMode(0)!=0)
	return 0;
  
   return 1;
}

/**
  * @brief   erase memory.
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */

 int MassErase (void){
/*Initialaize QSPI*/
   if(BSP_QSPI_DeInit(0) !=0)
    return 0;
   if(BSP_QSPI_Init(0,&Flash) !=0)
    return 0; 
	 
  /*Erases the entire QSPI memory*/
	 if(BSP_QSPI_EraseChip(0)!=0)
		 return 0;

  /*Reads current status of the QSPI memory*/
	while (BSP_QSPI_GetStatus(0)!=BSP_ERROR_NONE){};

	 return 1;
 
 }
 
 /**
  * @brief   Program memory.
  * @param   Address: page address
  * @param   Size   : size of data
  * @param   buffer : pointer to data buffer
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
 int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
  
/*Initialaize QSPI*/
   if(BSP_QSPI_DeInit(0) !=0)
    return 0;
   if(BSP_QSPI_Init(0,&Flash) !=0)
    return 0; 
    Address = Address & 0x0fffffff;
    /*Writes an amount of data to the QSPI memory.*/
   if( BSP_QSPI_Write(0,buffer,Address, Size)!=0)
     return 0;
     
   return 1;
}


/**
  * @brief   Sector erase.
  * @param   EraseStartAddress :  erase start address
  * @param   EraseEndAddress   :  erase end address
  * @retval  None
  */
 int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
uint32_t BlockAddr;
  EraseStartAddress &= 0x0FFFFFFF;  
  EraseEndAddress &= 0x0FFFFFFF;
  EraseStartAddress = EraseStartAddress -  EraseStartAddress % 0x20000;
/*Initialaize QSPI*/
   if(BSP_QSPI_DeInit(0) !=0)
    return 0;
   if(BSP_QSPI_Init(0,&Flash) !=0)
    return 0; 

  while (EraseEndAddress>=EraseStartAddress)
  {
    BlockAddr = EraseStartAddress;
  /*Erases the specified block of the QSPI memory*/
    if(BSP_QSPI_EraseBlock(0,BlockAddr,  MT25QL512ABB_ERASE_64K)!=0)
			return 0;
     /*Reads current status of the QSPI memory*/
    while (BSP_QSPI_GetStatus(0)!=0){};
      EraseStartAddress+=0x20000;
      
  }
    /*Configure the QSPI in memory-mapped mode*/ 
if(BSP_QSPI_EnableMemoryMappedMode(0)!=0)
	return 0;

  return 1;	
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 170000000
  *            HCLK(Hz)                       = 170000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 4
  *            PLL_N                          = 85
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 8
  * @param  None
  * @retval None
  */
static int SystemClock_Config(void)
{
 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    while(1);
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
  PeriphClkInit.QspiClockSelection = RCC_QSPICLKSOURCE_SYSCLK;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    while(1);
  }

  return 1;
}






/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
