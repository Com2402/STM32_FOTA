#ifndef __XMODEM_H
#define __XMODEM_H

#include "main.h"

#define SOH  0x01
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define PACKET_SIZE 128

HAL_StatusTypeDef XMODEM_Receive(void (*save_data)(uint8_t*, uint32_t));

#endif