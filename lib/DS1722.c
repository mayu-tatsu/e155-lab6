// DS1722.c
// Mayu Tatsumi; mtatsumi@g.hmc.edu
// 2025-10-22

#include "DS1722.h"
#include "STM32L432KC.h"        // includes everything
#include <stdint.h>

void initTempSensor(void) {
  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin active high

  spiSendReceive(0x80);                     // write to CSR register
  spiSendReceive(0xEE);                     // Continuous conversion mode

  digitalWrite(SPI_CS, PIO_LOW);            // CS pin inactive low
  return;
}

float getTemp(int precision){
  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin active high

  spiSendReceive(0x02);                     // read msb register
  int8_t msb = spiSendReceive(0xFF);        // dummy write for msb
  spiSendReceive(0x01);                     // read lsb register
  uint8_t lsb = spiSendReceive(0xFF);       // dummy write for lsb

  digitalWrite(SPI_CS, PIO_LOW);            // CS pin inactive low
  
  // temperature calculation
  float temp = 0.0f;
  if (precision == 8)
    temp = msb;
  else
    temp = msb + (float) lsb / 256.0;
  return temp;


  /*
  For Testing Negative Values:
  
  -10.125 degC: 1111 0101 1110 0000
  msb = 0b11110101;
  lsb = 0b11100000;
  output: -10.1250 on web

  -25.0625 degC: 1110 0110 1111 0000
  msb = 0b11100110;
  lsb = 0b11110000;
  */
}

void setPrecision(uint8_t prec) {
  digitalWrite(SPI_CS, PIO_HIGH);           // CS pin active high

  spiSendReceive(0x80);                     // write to CSR register
  if (prec == 8) {
    spiSendReceive(0xE0);                   // set 8-bit resolution
    delay_millis(TIM15, 75);                // wait for conversion (based on datasheet)
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

  digitalWrite(SPI_CS, PIO_LOW);            // CS pin inactive low
  return;
}
