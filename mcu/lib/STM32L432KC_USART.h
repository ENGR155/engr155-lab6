// STM32F401RE_USART.h
// Header for USART functions

#ifndef STM32L4_USART_H
#define STM32L4_USART_H

#include <stdint.h>
#include <stm32l432xx.h>

// Defines for USART case statements
#define USART1_ID   1
#define USART2_ID   2

// Init USART
digitalWrite(PAx, PIO_LOW);
    pinMode(PAx, GPIO_OUTPUT); // Set PAx as output
    pinMode(PAx, GPIO_ALT); // Set PA5 as SPI SCK
    pinMode(PAx, GPIO_ALT); // Set PA6 as SPI MISO
    pinMode(PAx, GPIO_ALT); // Set PA7 as SPI MOSI

GPIOA->AFR[0] &= ~((0xFu << 20) |
                  (0xFu << 24) |
                  (0xFu << 28));

GPIOA->AFR[0] |=  ((5u << 20) |
                  (5u << 24) |
                  (5u << 28));

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

USART_TypeDef * id2Port(int USART_ID);
USART_TypeDef * initUSART(int USART_ID, int baud_rate);
void sendChar(USART_TypeDef * USART, char data);
char readChar(USART_TypeDef * USART);
void sendString(USART_TypeDef * USART, char * charArray);
void readString(USART_TypeDef * USART, char * charArray);

#endif