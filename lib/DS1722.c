// DS1722.c

#include "DS1722.h"
#include "STM32L432KC.h"        // includes everything
#include <stdint.h>

// Initializes DS1722 temperature sensor
void initTempSensor() {
  digitalWrite(SPI_CS, PIO_LOW);            // CS pin active low

  spiSendReceive(0x80);                     // write to CSR register
  spiSendReceive(0xEE);                     // Continuous conversion mode

  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin inactive high
  return;
}

// Reads temperature from DS1722 temperature sensor.
// Return: Temperature in deg.C
float getTemp(){
  digitalWrite(SPI_CS, PIO_LOW);            // CS pin active low

  spiSendReceive(0x02);                     // read msb register
  int8_t msb = spiSendReceive(0xFF);        // dummy write for msb
  spiSendReceive(0x01);                     // read lsb register
  uint8_t lsb = spiSendReceive(0xFF);       // dummy write for lsb

  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin inactive high

  float temp = msb + (float) lsb / 256.0;   // calculation
  return temp;
}

void setPrecision(uint8_t prec) {
  digitalWrite(SPI_CS, PIO_LOW);            // CS pin active low

  spiSendReceive(0x80);                     // write to CSR register
  if (prec == 8) {
    spiSendReceive(0xE0);                   // set 8-bit resolution
    delay_millis(TIM15, 75);                // wait for conversion
  } else if (prec == 9) {
    spiSendReceive(0xE2);
    delay_millis(TIM15, 150);
  } else if (prec == 10) {
    spiSendReceive(0xE4);
    delay_millis(TIM15, 300);
  } else if (prec == 11) {
    spiSendReceive(0xE6);
    delay_millis(TIM15, 600);
  } else if (prec == 12) {
    spiSendReceive(0xE8);
    delay_millis(TIM15, 800);
  }

  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin inactive high
  return;
}
