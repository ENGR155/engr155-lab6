// DS1722.c
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "DS1722.h"
#include "STM32L432KC_SPI.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_TIM.h"

void initDS1722(void) {
    // Wait 1 ms
    digitalWrite(PA4, PIO_HIGH);
    delay_millis(TIM15, 1);
    // 9 bit resolution
    spiSendReceive((char)0x80);
    spiSendReceive((char)0xE2);
    // hold CE and end write
    delay_millis(TIM15, 1);
    digitalWrite(PA4, PIO_LOW);
    delay_millis(TIM15, 1);
}

int16_t ds1722ReadRaw(void) {
    digitalWrite(PA4, PIO_HIGH);
    delay_millis(TIM15, 1);
    spiSendReceive(0x01);
    // Send two dummy bytes
    uint8_t low = (uint8_t)spiSendReceive(0x00); // Read first byte
    uint8_t high = (uint8_t)spiSendReceive(0x00); // Read second byte
    delay_millis(TIM15, 1);
    digitalWrite(PA4, PIO_LOW);
    delay_millis(TIM15, 1);
    // Combine bytes into a single value
    int16_t temperature = (int16_t)(((uint16_t)high << 8) | low);
    return temperature;
}
