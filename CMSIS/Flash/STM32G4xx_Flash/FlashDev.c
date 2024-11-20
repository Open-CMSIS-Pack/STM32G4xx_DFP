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
 * Project:      Flash Device Description for ST STM32G4xx Flash
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

#include "..\FlashOS.h"          // FlashOS Structures


#ifdef FLASH_MEM

#ifdef STM32G49x_512
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G49x-Ax 512 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00080000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif
#ifdef STM32G49x_256
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G49x-Ax 256 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00040000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif
#ifdef STM32G43x_128
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G43x-4x 128 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00020000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif
#ifdef STM32G43x_64
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G43x-4x 64 KB Flash", // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00010000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif
#ifdef STM32G43x_32
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G43x-4x 32 KB Flash", // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00008000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif

#ifdef STM32G47x_128
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G47x-8x 128 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00020000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif
#ifdef STM32G47x_256
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G47x-8x 256 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00040000,                 // Device Size in Bytes (256kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif

#ifdef STM32G47x_512
  struct FlashDevice const FlashDevice  =  {
     FLASH_DRV_VERS,             // Driver Version, do not modify!
     "STM32G47x-8x 512 KB Flash",   // Device Name
     ONCHIP,                     // Device Type
     0x08000000,                 // Device Start Address
     0x00080000,                 // Device Size in Bytes (512kB)
     1024,                       // Programming Page Size
     0,                          // Reserved, must be 0
     0xFF,                       // Initial Content of Erased Memory
     400,                        // Program Page Timeout 400 mSec
     400,                        // Erase Sector Timeout 400 mSec
     // Specify Size and Address of Sectors
     0x800, 0x000000,            // Sector Size  2kB
     SECTOR_END
  };
#endif

#endif // FLASH_MEM


#if defined FLASH_OPT

#if defined FLASH_SB
  struct FlashDevice const FlashDevice  =  {
      FLASH_DRV_VERS,            // Driver Version, do not modify!
      "STM32G4xx single bank Flash Options", // Device Name
      ONCHIP,                    // Device Type
      0x1FFF7800,                // Device Start Address
      0x00000018,                // Device Size in Bytes (20)
      1024,                      // Programming Page Size
      0,                         // Reserved, must be 0
      0xFF,                      // Initial Content of Erased Memory
      3000,                      // Program Page Timeout 3 Sec
      3000,                      // Erase Sector Timeout 3 Sec
      // Specify Size and Address of Sectors
      0x0018, 0x000000,          // Sector Size 20B
      SECTOR_END
  };
#endif // FLASH_SB
#if defined FLASH_DB
  struct FlashDevice const FlashDevice  =  {
      FLASH_DRV_VERS,            // Driver Version, do not modify!
      "STM32G4xx dual bank Flash Options", // Device Name
      ONCHIP,                    // Device Type
      0x1FFF7800,                // Device Start Address
      0x00000054,                // Device Size in Bytes (52)
      1024,                      // Programming Page Size
      0,                         // Reserved, must be 0
      0xFF,                      // Initial Content of Erased Memory
      3000,                      // Program Page Timeout 3 Sec
      3000,                      // Erase Sector Timeout 3 Sec
      // Specify Size and Address of Sectors
      0x0054, 0x000000,          // Sector Size 52B
      SECTOR_END
  };
#endif // FLASH_DB

#endif // FLASH_OPT
