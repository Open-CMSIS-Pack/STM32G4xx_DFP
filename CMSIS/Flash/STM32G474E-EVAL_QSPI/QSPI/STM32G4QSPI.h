/**
  ******************************************************************************
  * @file    STM32G4QSPI.h
  * @author  MCD Application Team
  * @brief   Header file of STM32G4QSPI.c
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


#ifndef __STM32G4QSPI_H
#define __STM32G4QSPI_H
#include "stdint.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g474e_eval_qspi.h"


#define TIMEOUT 5000U


/* Private function prototypes -----------------------------------------------*/
int Init_QSPI(void);
int Write (uint32_t Address, uint32_t Size, uint8_t* buffer);
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
int MassErase ( void);
int SystemClock_Config(void);

#endif
