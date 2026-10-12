// STM32L432KC_SPI.c
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "STM32L432KC_SPI.h"
#include "STM32L432KC_GPIO.h"

void initSPI(int br, int cpol, int cpha) {
    // Clock enable
    gpioEnable(GPIO_PORT_A);
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    (void)RCC->APB2ENR;

    // Set everything to 0
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;

    digitalWrite(PA4, PIO_LOW);
    pinMode(PA4, GPIO_OUTPUT); // Set PA4 as output for SPI Chip Enable
    pinMode(PA5, GPIO_ALT); // Set PA5 as SPI SCK
    pinMode(PA6, GPIO_ALT); // Set PA6 as SPI MISO
    pinMode(PA7, GPIO_ALT); // Set PA7 as SPI MOSI

    // Clear fields and set AF5 (SPI) for pins
    GPIOA->AFR[0] &= ~((0xFu << 20) | (0xFu << 24) | (0xFu << 28));
    GPIOA->AFR[0] |= (5u << 20) | (5u << 24) | (5u << 28);

    // Initialize TIM15 for SPI timing

    // Clear baud rate bits
    SPI1->CR1 &= ~SPI_CR1_BR_Msk;
    // Set baud rate bits
    SPI1->CR1 |= ((uint32_t)br << SPI_CR1_BR_Pos);
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

    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    SPI1->CR2 |= (7u << SPI_CR2_DS_Pos);
    SPI1->CR2 |= SPI_CR2_FRXTH;
    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

char spiSendReceive(char send) {
    // Wait until the transmit buffer is empty
    while (!(SPI1->SR & SPI_SR_TXE));
    // Send the character

    *((volatile char *)&SPI1->DR) = send;
    // Wait until a character is received
    while (!(SPI1->SR & SPI_SR_RXNE));
    // Read byte and wait
    char received = *((volatile char *)&SPI1->DR);
    while (SPI1->SR & SPI_SR_BSY);
    // Return the received character
    return received;
}
