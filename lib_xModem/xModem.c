#include "xmodem.h"
#include "lib_uart.h"

static uint8_t Xmodem_CalcChecksum(uint8_t *data, uint16_t length)
{
    uint8_t checksum = 0;
    while(length--)
        checksum += *data++;
    return checksum;
}

HAL_StatusTypeDef XMODEM_Receive(void (*save_data)(uint8_t*, uint32_t))
{
    uint8_t packet_number = 1;
    uint8_t buffer[PACKET_SIZE + 4];
    uint32_t firmware_size = 0;
    uint8_t nak = NAK;
    uint8_t ack = ACK;
    uint32_t timeout = 0;
    
    // Send NAK to start reception
    UART_Transmit(nak);
    
    while(1)
    {
        if(UART_Available())
        {
            uint8_t data = UART_Receive();
            
            if(data == SOH)
            {
                // Receive packet header and data
                for(int i = 1; i < PACKET_SIZE + 4; i++)
                {
                    timeout = 0;
                    while(!UART_Available())
                    {
                        timeout++;
                        if(timeout > 100000) return HAL_ERROR;
                    }
                    buffer[i] = UART_Receive();
                }
                
                // Check packet number
                if(buffer[1] != packet_number) {
                    UART_Transmit(nak);
                    continue;
                }
                
                // Check complement
                if(buffer[2] != (uint8_t)(~buffer[1])) {
                    UART_Transmit(nak);
                    continue;
                }
                
                // Verify checksum
                uint8_t calc_checksum = Xmodem_CalcChecksum(&buffer[3], PACKET_SIZE);
                if(calc_checksum != buffer[PACKET_SIZE + 3]) {
                    UART_Transmit(nak);
                    continue;
                }
                
                // Save data
                save_data(&buffer[3], firmware_size);
                firmware_size += PACKET_SIZE;
                
                UART_Transmit(ack);
                packet_number++;
            }
            else if(data == EOT)
            {
                UART_Transmit(ack);
                return HAL_OK;
            }
        }
        
        timeout++;
        if(timeout > 1000000) return HAL_ERROR;
    }
    return HAL_ERROR;
}