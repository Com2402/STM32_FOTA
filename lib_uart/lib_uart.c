#include "lib_uart.h"

static uint8_t rxBuffer[RX_BUFFER_SIZE];
static volatile uint16_t rxHead = 0;
static volatile uint16_t rxTail = 0;

void UART_Init(void)
{
    // Enable USART clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    // Configure UART pins (PA9 TX, PA10 RX)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    // PA9 TX
    GPIOA->MODER &= ~GPIO_MODER_MODER9;
    GPIOA->MODER |= GPIO_MODER_MODER9_1;  // Alternate function
    GPIOA->AFR[1] |= (1 << 4);  // AF1 for USART1
    
    // PA10 RX
    GPIOA->MODER &= ~GPIO_MODER_MODER10;
    GPIOA->MODER |= GPIO_MODER_MODER10_1;  // Alternate function
    GPIOA->AFR[1] |= (1 << 8);  // AF1 for USART1
    
    // Configure UART
    USART1->BRR = SystemCoreClock/115200;  // 115200 baud
    USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_UE;
    
    // Enable UART interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}

void UART_IRQHandler(void)
{
    if(USART1->ISR & USART_ISR_RXNE)
    {
        uint8_t data = USART1->RDR;
        uint16_t nextHead = (rxHead + 1) % RX_BUFFER_SIZE;
        
        if(nextHead != rxTail)
        {
            rxBuffer[rxHead] = data;
            rxHead = nextHead;
        }
    }
}

uint8_t UART_Available(void)
{
    return (uint16_t)(RX_BUFFER_SIZE + rxHead - rxTail) % RX_BUFFER_SIZE;
}

uint8_t UART_Receive(void)
{
    if(rxHead == rxTail)
        return 0;
        
    uint8_t data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    return data;
}

void UART_Transmit(uint8_t data)
{
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = data;
}