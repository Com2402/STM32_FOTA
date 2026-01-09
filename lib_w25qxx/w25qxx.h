#ifndef __W25QXX_H
#define __W25QXX_H

#include "main.h"
#include "stm32f0xx_hal.h"
/* W25Qxx Commands */
#define W25X_WriteEnable        0x06 
#define W25X_WriteDisable       0x04 
#define W25X_ReadStatusReg      0x05 
#define W25X_WriteStatusReg     0x01 
#define W25X_ReadData          0x03 
#define W25X_FastReadData       0x0B 
#define W25X_FastReadDual       0x3B 
#define W25X_PageProgram        0x02 
#define W25X_BlockErase         0xD8 
#define W25X_SectorErase        0x20 
#define W25X_ChipErase         0xC7 
#define W25X_PowerDown         0xB9 
#define W25X_ReleasePowerDown   0xAB 
#define W25X_DeviceID          0xAB 
#define W25X_ManufactDeviceID   0x90 
#define W25X_JedecDeviceID      0x9F 
// Thêm vào w25qxx.h
#define SPI1_CS_GPIO_Port    GPIOA
#define SPI1_CS_Pin         GPIO_PIN_4
extern SPI_HandleTypeDef hspi1;
void W25QXX_Init(void);
uint16_t W25QXX_ReadID(void);
void W25QXX_WriteEnable(void);
void W25QXX_WaitForWriteEnd(void);
void W25QXX_EraseSector(uint32_t SectorAddr);
void W25QXX_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#endif