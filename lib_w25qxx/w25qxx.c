#include "w25qxx.h"

void W25QXX_Init(void)
{
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;
    uint8_t cmd[4] = {0x90, 0x00, 0x00, 0x00};
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    HAL_SPI_Receive(&hspi1, (uint8_t*)&Temp, 2, 100);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    
    return Temp;
}

void W25QXX_WriteEnable(void)
{
    uint8_t cmd = W25X_WriteEnable;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

void W25QXX_WaitForWriteEnd(void)
{
    uint8_t cmd = W25X_ReadStatusReg;
    uint8_t status = 0;
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    
    do {
        HAL_SPI_Receive(&hspi1, &status, 1, 100);
    } while((status & 0x01) == 0x01);
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

void W25QXX_EraseSector(uint32_t SectorAddr)
{
    W25QXX_WriteEnable();
    
    uint8_t cmd[4];
    cmd[0] = W25X_SectorErase;
    cmd[1] = (SectorAddr >> 16) & 0xFF;
    cmd[2] = (SectorAddr >> 8) & 0xFF;
    cmd[3] = SectorAddr & 0xFF;
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    
    W25QXX_WaitForWriteEnd();
}

void W25QXX_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    W25QXX_WriteEnable();
    
    uint8_t cmd[4];
    cmd[0] = W25X_PageProgram;
    cmd[1] = (WriteAddr >> 16) & 0xFF;
    cmd[2] = (WriteAddr >> 8) & 0xFF;
    cmd[3] = WriteAddr & 0xFF;
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    HAL_SPI_Transmit(&hspi1, pBuffer, NumByteToWrite, 100);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    
    W25QXX_WaitForWriteEnd();
}

void W25QXX_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t pageRemain;
    pageRemain = 256 - WriteAddr % 256;
    
    if(NumByteToWrite <= pageRemain) 
        pageRemain = NumByteToWrite;
    
    while(1)
    {
        W25QXX_WritePage(pBuffer, WriteAddr, pageRemain);
        
        if(NumByteToWrite == pageRemain)
            break;
        else
        {
            pBuffer += pageRemain;
            WriteAddr += pageRemain;
            NumByteToWrite -= pageRemain;
            
            if(NumByteToWrite > 256)
                pageRemain = 256;
            else
                pageRemain = NumByteToWrite;
        }
    }
}

void W25QXX_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint8_t cmd[4];
    cmd[0] = W25X_ReadData;
    cmd[1] = (ReadAddr >> 16) & 0xFF;
    cmd[2] = (ReadAddr >> 8) & 0xFF;
    cmd[3] = ReadAddr & 0xFF;
    
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    HAL_SPI_Receive(&hspi1, pBuffer, NumByteToRead, 2000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}
