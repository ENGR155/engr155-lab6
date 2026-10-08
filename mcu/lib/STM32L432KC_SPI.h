// STM32L432KC_SPI.h
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
void initSPI(int br, int cpol, int cpha) {

    digitalWrite(PA4, PIO_LOW);
    pinMode(PA4, GPIO_OUTPUT); // Set PA4 as output for SPI Chip Enable
    pinMode(PA5, GPIO_ALT); // Set PA5 as SPI SCK
    pinMode(PA6, GPIO_ALT); // Set PA6 as SPI MISO
    pinMode(PA7, GPIO_ALT); // Set PA7 as SPI MOSI

    GPIOA->AFR[0] &= ~((0xFu << 20) |
                  (0xFu << 24) |
                  (0xFu << 28));

    GPIOA->AFR[0] |=  ((5u << 20) |
                  (5u << 24) |
                  (5u << 28));

    initTIM(TIM15); // Initialize TIM15 for SPI timing
    // Clear baud rate bits
    SPI1->CR1 &= ~SPI_CR1_BR_Msk;
    // Set baud rate bits
    SPI1->CR1 |= (br << SPI_CR1_BR_Pos);
    // Set clock polarity and phase
    if (cpol) {
        SPI1->CR1 |= SPI_CR1_CPOL;
    } else {
        SPI1->CR1 &= ~SPI_CR1_CPOL;
    }
    if (cpha) {
        SPI1->CR1 |= SPI_CR1_CPHA;
    } else {
        SPI1->CR1 &= ~SPI_CR1_CPHA;
    }
};

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
char spiSendReceive(char send) {
    // Wait until the transmit buffer is empty
    while (!(SPI1->SR & SPI_SR_TXE));
    // Send the character
    *((__IO uint8_t *)&SPI1->DR) = send;
    // Wait until a character is received
    while (!(SPI1->SR & SPI_SR_RXNE));
    // Return the received character
    return *((__IO uint8_t *)&SPI1->DR);
};

#endif