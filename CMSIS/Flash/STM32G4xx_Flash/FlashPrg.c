/* -----------------------------------------------------------------------------
 * Copyright (c) 2014 - 2021 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        13. July 2021
 * $Revision:    V1.2.0
 *
 * Project:      Flash Programming Functions for ST STM32G4xx Flash
 * --------------------------------------------------------------------------- */

/* History:
 *  Version 1.2.0
 *    Added Option Byte handling
 *  Version 1.1.0
 *    Changed to 4K Sector size (logical)
 *  Version 1.0.0
 *    Initial release
 */

/* Note:
   Flash category 2 devices have 2K sector size.
   Flash category 3 devices DualBank configuration have 2K sector size.
   Flash category 3 devices SingleBank configuration have 4K sector size.
   We use 4K sector size for all. In case of 2K physical size two contiguous sectores are deleted. */
/* Note:
   define FLASH_SB is used for Flash Category 2, 4
   define FLASH_DB is used for Flash Category 3    */


#include "..\FlashOS.h"        /* FlashOS Structures */

typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;

#define M32(adr) (*((vu32 *) (adr)))

/* Peripheral Memory Map */
#define IWDG_BASE         (0x40003000)
#define FLASH_BASE        (0x40022000)
#define DBGMCU_BASE       (0xE0042000)
#define FLASHSIZE_BASE    (0x1FFF75E0)

#define IWDG            ((IWDG_TypeDef   *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef  *) FLASH_BASE)
#define DBGMCU          ((DBGMCU_TypeDef *) DBGMCU_BASE)

/* Debug MCU */
typedef struct {
  vu32 IDCODE;
} DBGMCU_TypeDef;

/* Independent WATCHDOG */
typedef struct {
  vu32 KR;
  vu32 PR;
  vu32 RLR;
  vu32 SR;
} IWDG_TypeDef;

/* Flash Registers */
typedef struct {
  vu32 ACR;              /* Offset: 0x00  Access Control Register */
  vu32 PDKEYR;           /* Offset: 0x04  Power Down Key Register */
  vu32 KEYR;             /* Offset: 0x08  Key Register */
  vu32 OPTKEYR;          /* Offset: 0x0C  Option Key Register */
  vu32 SR;               /* Offset: 0x10  Status Register */
  vu32 CR;               /* Offset: 0x14  Control Register */
  vu32 ECCR;             /* Offset: 0x18  ECC Register */
  vu32 RESERVED0;
  vu32 OPTR;             /* Offset: 0x20  Option Register */
  vu32 PCROP1SR;         /* Offset: 0x24  Bank1 PCROP Start Address Register */
  vu32 PCROP1ER;         /* Offset: 0x28  Bank1 PCROP End Address Register */
  vu32 WRP1AR;           /* Offset: 0x2C  Bank1 WRP Area A Address Register */
  vu32 WRP1BR;           /* Offset: 0x30  Bank1 WRP Area B Address Register */
  vu32 RESERVED1[4];
  vu32 PCROP2SR;         /* Offset: 0x44  Bank2 PCROP Start Address Register */
  vu32 PCROP2ER;         /* Offset: 0x48  Bank2 PCROP End Address Register */
  vu32 WRP2AR;           /* Offset: 0x4C  Bank2 WRP Area A Address Register */
  vu32 WRP2BR;           /* Offset: 0x50  Bank2 WRP Area B Address Register */
  vu32 RESERVED2[7];
  vu32 SEC1R;            /* Offset: 0x70  Securable Memory Register Bank1 */
  vu32 SEC2R;            /* Offset: 0x74  Securable Memory Register Bank2 */
} FLASH_TypeDef;


/* Flash Keys */
#define FLASH_KEY1               0x45670123
#define FLASH_KEY2               0xCDEF89AB
#define FLASH_OPTKEY1            0x08192A3B
#define FLASH_OPTKEY2            0x4C5D6E7F

/* Flash Control Register definitions */
#define FLASH_CR_PG             ((u32)(  1U      ))
#define FLASH_CR_PER            ((u32)(  1U <<  1))
#define FLASH_CR_MER1           ((u32)(  1U <<  2))
#define FLASH_CR_PNB_MSK        ((u32)(0x7F <<  3))
#define FLASH_CR_BKER           ((u32)(  1U << 11))
#define FLASH_CR_MER2           ((u32)(  1U << 15))
#define FLASH_CR_STRT           ((u32)(  1U << 16))
#define FLASH_CR_OPTSTRT        ((u32)(  1U << 17))
#define FLASH_CR_FSTPG          ((u32)(  1U << 18))
#define FLASH_CR_OBL_LAUNCH     ((u32)(  1U << 27))
#define FLASH_CR_OPTLOCK        ((u32)(  1U << 30))
#define FLASH_CR_LOCK           ((u32)(  1U << 31))


/* Flash Status Register definitions */
#define FLASH_SR_EOP            ((u32)(  1U      ))
#define FLASH_SR_OPERR          ((u32)(  1U <<  1))
#define FLASH_SR_PROGERR        ((u32)(  1U <<  3))
#define FLASH_SR_WRPERR         ((u32)(  1U <<  4))
#define FLASH_SR_PGAERR         ((u32)(  1U <<  5))
#define FLASH_SR_SIZERR         ((u32)(  1U <<  6))
#define FLASH_SR_PGSERR         ((u32)(  1U <<  7))
#define FLASH_SR_MISSERR        ((u32)(  1U <<  8))
#define FLASH_SR_FASTERR        ((u32)(  1U <<  9))
#define FLASH_SR_RDERR          ((u32)(  1U << 14))
#define FLASH_SR_OPTVERR        ((u32)(  1U << 16))
#define FLASH_SR_BSY            ((u32)(  1U << 16))

#define FLASH_PGERR             (FLASH_SR_OPERR   | FLASH_SR_PROGERR | FLASH_SR_WRPERR  | \
                                 FLASH_SR_PGAERR  | FLASH_SR_SIZERR  | FLASH_SR_PGSERR  | \
                                 FLASH_SR_MISSERR | FLASH_SR_FASTERR | FLASH_SR_RDERR   | FLASH_SR_OPTVERR )


/* Flash option register definitions */
#define FLASH_OPTR_RDP          ((u32)(0xFF      ))
#define FLASH_OPTR_RDP_NO       ((u32)(0xAA      ))
#define FLASH_OPTR_DBANK        ((u32)(  1U << 22))


u32 flashBase;                   /* Flash base address */
u32 flashSize;                   /* Flash size in bytes */
u32 flashBankSize;               /* Flash bank size in bytes */
u32 flashPageSize;               /* Flash page size in bytes */

static void DSB(void) {
    __asm("DSB");
}


/*
 * Get Flash Type
 *    Return Value:   0 = Single-Bank flash
 *                    1 = Dual-Bank Flash (configurable)
 */

#if defined FLASH_MEM
static u32 GetFlashType (void) {
  u32 flashType;

  switch ((DBGMCU->IDCODE & 0xFFFU)) {
    case 0x468:             /* Flash Category 2 devices, 2k sectors */
    case 0x479:             /* Flash Category 4 devices, 2k sectors */
                            /* devices have only a singe bank flash */
      flashType = 0U;       /* Single-Bank Flash type */
    break;

    case 0x469:             /* Flash Category 3 devices, 2k or 4k sectors */
    default:                /* devices have a dual bank flash, configurable via FLASH_OPTR.DBANK */
      flashType = 1U;       /* Dual-Bank Flash type */
    break;
  }

  return (flashType);
}
#endif /* FLASH_MEM */


/*
 * Get Flash Bank Mode
 *    Return Value:   0 = Single-Bank mode
 *                    1 = Dual-Bank mode
 */

#if defined FLASH_MEM
static u32 GetFlashBankMode (void) {
  u32 flashBankMode;

  flashBankMode = (FLASH->OPTR & FLASH_OPTR_DBANK) ? 1U : 0U;

  return (flashBankMode);
}
#endif /* FLASH_MEM */


/*
 * Get Flash Bank Number
 *    Parameter:      adr:  Sector Address
 *    Return Value:   Bank Number (0..1)
 */

#if defined FLASH_MEM
static u32 GetFlashBankNum(u32 adr) {
  u32 flashBankNum;

  if (GetFlashType() == 1U) {
    /* Dual-Bank Flash */
    if (GetFlashBankMode() == 1U) {
      /* Dual-Bank Flash configured as Dual-Bank */
      if (adr >= (flashBase + flashBankSize)) {
        flashBankNum = 1U;
      }
      else {
        flashBankNum = 0U;
      }
    }
    else {
      /* Dual-Bank Flash configured as Single-Bank */
      flashBankNum = 0U;
    }
  }
  else {
    /* Single-Bank Flash */
    flashBankNum = 0u;
  }

  return (flashBankNum);
}
#endif /* FLASH_MEM */


/*
 * Get Flash Page Number
 *    Parameter:      adr:  Page Address
 *    Return Value:   Page Number (0..127)
 */

#if defined FLASH_MEM
static u32 GetFlashPageNum (unsigned long adr) {
  u32 flashPageNum;

  if (GetFlashType() == 1U) {
    /* Dual-Bank Flash */
    if (GetFlashBankMode() == 1U) {
      /* Dual-Bank Flash configured as Dual-Bank */
      flashPageNum = (((adr & (flashBankSize - 1U)) ) >> 11); /* 2K sector size */
    }
    else {
      /* Dual-Bank Flash configured as Single-Bank */
      flashPageNum = (((adr & (flashSize     - 1U)) ) >> 12); /* 4K sector size */
    }
  }
  else {
      /* Single-Bank Flash */
      flashPageNum = (((adr & (flashSize     - 1U)) ) >> 11); /* 2K sector size */
  }

  return (flashPageNum);
}
#endif /* FLASH_MEM */


/*
 * Get Flash Page Size
 *    Return Value:   flash page size (in Bytes)
 */

#if defined FLASH_MEM
static u32 GetFlashPageSize (void) {
  u32 flashPageSize;

  if (GetFlashType() == 1U) {
    /* Dual-Bank Flash */
    if (GetFlashBankMode() == 1U) {
      /* Dual-Bank Flash configured as Dual-Bank */
      flashPageSize = 0x0800;                            /* 2K sector size */
    }
    else {
      /* Dual-Bank Flash configured as Single-Bank */
      flashPageSize = 0x1000;                            /* 4K sector size */
    }
  }
  else {
      /* Single-Bank Flash */
      flashPageSize = 0x0800;                            /* 2K sector size */
  }

  return (flashPageSize);
}
#endif /* FLASH_MEM */


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  FLASH->KEYR = FLASH_KEY1;                              /* Unlock Flash operation */
  FLASH->KEYR = FLASH_KEY2;

#ifdef FLASH_OPT
  FLASH->OPTKEYR  = FLASH_OPTKEY1;                       /* Unlock Option Bytes operation */
  FLASH->OPTKEYR  = FLASH_OPTKEY2;
#endif /* FLASH_OPT */

  /* Wait until the flash is ready */
  while (FLASH->SR & FLASH_SR_BSY);

#if defined FLASH_MEM
  flashBase = adr;
  flashSize = ((*((u32 *)FLASHSIZE_BASE)) & 0x0000FFFF) << 10;
  flashBankSize = flashSize >> 1;
  flashPageSize = GetFlashPageSize();
#endif /* FLASH_MEM */

  if ((FLASH->OPTR & 0x10000) == 0x00000) {              /* Test if IWDG is running (IWDG in HW mode) */
    /* Set IWDG time out to ~32.768 second */
    IWDG->KR  = 0x5555;                                  /* Enable write access to IWDG_PR and IWDG_RLR */
    IWDG->PR  = 0x06;                                    /* Set prescaler to 256 */
    IWDG->RLR = 4095;                                    /* Set reload value to 4095 */
  }

  return (0);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  FLASH->CR |= FLASH_CR_LOCK;                            /* Lock Flash operation */
  DSB();

#ifdef FLASH_OPT
  FLASH->CR  = FLASH_CR_OBL_LAUNCH;                         /* Load option bytes */
  DSB();
  while (FLASH->CR & FLASH_CR_OBL_LAUNCH);

  FLASH->CR = FLASH_CR_OPTLOCK;                          /* Lock option bytes operation */
  DSB();
#endif /* FLASH_OPT */

  return (0);
}


/*
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat) {
  /* force erase even if the content is 'Initial Content of Erased Memory'.
     Only a erased sector can be programmed. I think this is because of ECC */
  return (1);
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

#if defined FLASH_MEM
int EraseChip (void) {

  FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

  FLASH->CR  = (FLASH_CR_MER1 | FLASH_CR_MER2);          /* Bank A/B mass erase enabled */
  FLASH->CR |=  FLASH_CR_STRT;                           /* Start erase */
  DSB();

  while (FLASH->SR & FLASH_SR_BSY);

  return (0);                                            /* Done */
}
#endif /* FLASH_MEM */

#ifdef FLASH_OPT
int EraseChip (void) {

  FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

  FLASH->OPTR     = 0xFFEFF8AA;
  FLASH->PCROP1SR = 0xFFFFFFFF;
  FLASH->PCROP1ER = 0x7FFF0000;
  FLASH->WRP1AR   = 0xFF00FFFF;
  FLASH->WRP1BR   = 0xFF00FFFF;
  FLASH->SEC1R    = 0xFFFEFE00;

#if defined FLASH_DB
  FLASH->PCROP2SR = 0xFFFFFFFF;
  FLASH->PCROP2ER = 0x7FFF8000;
  FLASH->WRP2AR   = 0xFF80FFFF;
  FLASH->WRP2BR   = 0xFF80FFFF;
  FLASH->SEC2R    = 0xFFFEFF00;
#endif

  FLASH->CR       = FLASH_CR_OPTSTRT;                    /* Program values */
  DSB();

  while (FLASH->SR & FLASH_SR_BSY);

  if (FLASH->SR & FLASH_PGERR) {                         /* Check for Error */
    FLASH->SR  = FLASH_PGERR;                            /* Reset Error Flags */
    return (1);                                          /* Failed */
  }

  return (0);                                            /* Done */
}
#endif /* FLASH_OPT */


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

#if defined FLASH_MEM
int EraseSector (unsigned long adr) {
  u32 b, p;

  b = GetFlashBankNum(adr);                              /* Get Bank Number 0..1  */
  p = GetFlashPageNum(adr);                              /* Get Page Number 0..127 */

  FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

  FLASH->CR  = (FLASH_CR_PER |                           /* Page Erase Enabled */
                (p <<  3)    |                           /* page Number. 0 to 127 for each bank */
                (b << 11)     );
  FLASH->CR |=  FLASH_CR_STRT;                           /* Start Erase */
  DSB();

  while (FLASH->SR & FLASH_SR_BSY);

  if (FLASH->SR & FLASH_PGERR) {                         /* Check for Error */
    FLASH->SR  = FLASH_PGERR;                            /* Reset Error Flags */
    return (1);                                          /* Failed */
  }

  return (0);                                            /* Done */
}
#endif /* FLASH_MEM */

#if defined FLASH_OPT || defined FLASH_OTP
int EraseSector (unsigned long adr) {
  /* erase sector is not needed for
     - Flash Option bytes
     - Flash One Time Programmable bytes
  */
  return (0);                                            /* Done */
}
#endif /* FLASH_OPT || defined FLASH_OTP */


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

#if defined FLASH_MEM
int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

  sz = (sz + 7) & ~7;                                    /* Adjust size for two words */

  FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

  FLASH->CR = FLASH_CR_PG;                               /* Programming Enabled */

  while (sz) {
    M32(adr    ) = *((u32 *)(buf + 0));                  /* Program the first word of the Double Word */
    M32(adr + 4) = *((u32 *)(buf + 4));                  /* Program the second word of the Double Word */
    DSB();

    while (FLASH->SR & FLASH_SR_BSY);

    if (FLASH->SR & FLASH_PGERR) {                       /* Check for Error */
      FLASH->SR  = FLASH_PGERR;                          /* Reset Error Flags */
      return (1);                                        /* Failed */
    }

    adr += 8;                                            /* Go to next DoubleWord */
    buf += 8;
    sz  -= 8;
  }

  FLASH->CR &= ~(FLASH_CR_PG) ;                          /* Reset CR */

  return (0);
}
#endif /* FLASH_MEM */

#ifdef FLASH_OPT
int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

  u32 optr;
  u32 pcrop1sr;
  u32 pcrop1er;
  u32 wrp1ar;
  u32 wrp1br;
  u32 sec1r;
#if defined FLASH_DB
  u32 pcrop2sr;
  u32 pcrop2er;
  u32 wrp2ar;
  u32 wrp2br;
  u32 sec2r;
#endif

  optr     = *((u32 *)(buf +  0));
  pcrop1sr = *((u32 *)(buf +  4));
  pcrop1er = *((u32 *)(buf +  8));
  wrp1ar   = *((u32 *)(buf + 12));
  wrp1br   = *((u32 *)(buf + 16));
  sec1r    = *((u32 *)(buf + 20));
#if defined FLASH_DB
  pcrop2sr = *((u32 *)(buf + 24));
  pcrop2er = *((u32 *)(buf + 28));
  wrp2ar   = *((u32 *)(buf + 32));
  wrp2br   = *((u32 *)(buf + 36));
  sec2r    = *((u32 *)(buf + 40));
#endif

  FLASH->SR  = FLASH_PGERR;                             /* Reset Error Flags */

  FLASH->OPTR     = (optr     & 0xFFFFFFFF);            /* Write OPTR value */
  FLASH->PCROP1SR = (pcrop1sr & 0x0000FFFF);            /* Write PCROP1SR value */
  FLASH->PCROP1ER = (pcrop1er & 0x8000FFFF);            /* Write PCROP1ER value */
  FLASH->WRP1AR   = (wrp1ar   & 0x00FF00FF);            /* Write WRP1AR value */
  FLASH->WRP1BR   = (wrp1br   & 0x00FF00FF);            /* Write WRP1BR value */
  FLASH->SEC1R    = (sec1r    & 0x000101FF);            /* Write SEC1R value */
#if defined FLASH_DB
  FLASH->PCROP2SR = (pcrop2sr & 0x0000FFFF);            /* Write PCROP2SR value */
  FLASH->PCROP2ER = (pcrop2er & 0x0000FFFF);            /* Write PCROP2ER value */
  FLASH->WRP2AR   = (wrp2ar   & 0x00FF00FF);            /* Write WRP2AR value */
  FLASH->WRP2BR   = (wrp2br   & 0x00FF00FF);            /* Write WRP2BR value */
  FLASH->SEC2R    = (sec2r    & 0x000100FF);            /* Write SEC1R value */
#endif

  FLASH->CR  = FLASH_CR_OPTSTRT;                        /* Program values */
  DSB();

  while (FLASH->SR & FLASH_SR_BSY) {
    IWDG->KR = 0xAAAA;                                  /* Reload IWDG */
  }

  if (FLASH->SR & FLASH_PGERR) {                        /* Check for Error */
    FLASH->SR |= FLASH_PGERR;                           /* Reset Error Flags */
    return (1);                                         /* Failed */
  }

  return (0);                                           /* Done */
}
#endif /* FLASH_OPT */


/*
 *  Verify Flash Contents
 *    Parameter:      adr:  Start Address
 *                    sz:   Size (in bytes)
 *                    buf:  Data
 *    Return Value:   (adr+sz) - OK, Failed Address
 */

#ifdef FLASH_OPT
unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf) {
  u32 optr;
  u32 pcrop1sr;
  u32 pcrop1er;
  u32 wrp1ar;
  u32 wrp1br;
  u32 sec1r;
#if defined FLASH_DB
  u32 pcrop2sr;
  u32 pcrop2er;
  u32 wrp2ar;
  u32 wrp2br;
  u32 sec2r;
#endif

  optr     = *((u32 *)(buf +  0));
  pcrop1sr = *((u32 *)(buf +  4));
  pcrop1er = *((u32 *)(buf +  8));
  wrp1ar   = *((u32 *)(buf + 12));
  wrp1br   = *((u32 *)(buf + 16));
  sec1r    = *((u32 *)(buf + 20));
#if defined FLASH_DB
  pcrop2sr = *((u32 *)(buf + 24));
  pcrop2er = *((u32 *)(buf + 28));
  wrp2ar   = *((u32 *)(buf + 32));
  wrp2br   = *((u32 *)(buf + 36));
  sec2r    = *((u32 *)(buf + 40));
#endif


  if ((FLASH->OPTR     & 0xFFFFFFFF) != (optr     & 0xFFFFFFFF)) {    /* Check OPTR values */
    return (adr + 0);
  }

  if ((FLASH->PCROP1SR & 0x0000FFFF) != (pcrop1sr & 0x0000FFFF)) {    /* Check PCROP1SR values */
    return (adr + 1);
  }

  if ((FLASH->PCROP1ER & 0x8000FFFF) != (pcrop1er & 0x8000FFFF)) {    /* Check PCROP1ER values */
    return (adr + 2);
  }

  if ((FLASH->WRP1AR   & 0x00FF00FF) != (wrp1ar   & 0x00FF00FF)) {    /* Check WRP1AR values */
    return (adr + 3);
  }

  if ((FLASH->WRP1BR   & 0x00FF00FF) != (wrp1br   & 0x00FF00FF)) {    /* Check WRP1BR values */
    return (adr + 4);
  }

  if ((FLASH->SEC1R    & 0x000101FF) != (sec1r    & 0x000101FF)) {    /* Check SEC1R value */
    return (adr + 5);
  }
#if defined FLASH_DB
  if ((FLASH->PCROP2SR & 0x0000FFFF) != (pcrop2sr & 0x0000FFFF)) {    /* Check PCROP2SR values */
    return (adr + 6);
  }

  if ((FLASH->PCROP2ER & 0x0000FFFF) != (pcrop2er & 0x0000FFFF)) {    /* Check PCROP2ER values */
    return (adr + 7);
  }

  if ((FLASH->WRP2AR   & 0x00FF00FF) != (wrp2ar   & 0x00FF00FF)) {    /* Check WRP2AR values */
    return (adr + 8);
  }

  if ((FLASH->WRP2BR   & 0x00FF00FF) != (wrp2br   & 0x00FF00FF)) {    /* Check WRP2BR values */
    return (adr + 9);
  }

  if ((FLASH->SEC2R    & 0x000000FF) != (sec2r    & 0x000000FF)) {    /* Check SEC2R value */
    return (adr + 10);
  }
#endif

  return (adr + sz);
}
#endif /* FLASH_OPT */
