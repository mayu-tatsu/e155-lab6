// STM32L432KC_SPI.c
// Mayu Tatsumi; mtatsumi@g.hmc.edu
// 2025-10-22

#include "STM32L432KC.h"

void initSPI(int br, int cpol, int cpha) {
  enableGPIOBClock();
  enableSpi1Clock();

  pinMode(SPI_CS, GPIO_OUTPUT);
  pinMode(SPI_SCK, GPIO_ALT);
  pinMode(SPI_SDI, GPIO_ALT);
  pinMode(SPI_SDO, GPIO_ALT);

  digitalWrite(SPI_CS, PIO_LOW);
  
  GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);             // high speed clk for fast gpio/spi

  // specify which alternate fxn
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);                // Set baud rate divider
  SPI1->CR1 |= (SPI_CR1_MSTR);                          // Set as master

  SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_SSM);     // clear

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);            // Set phase
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);            // Set polarity
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);            // Set bit package to 8

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 0b1);              // software slave management
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 0b1);              // internal slave select

  SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);          // RXNE event is generated when FIFO >= 1/4 full & SS output enable
     
  SPI1->CR1 |= (SPI_CR1_SPE);                           // enable overall SPI
}

char spiSendReceive(char send) {
  while (!(SPI1->SR & SPI_SR_TXE));                     // Wait until TX buffer is empty
  *(volatile char *)(&SPI1->DR) = send;                 // Write data to TX buffer & cast to volatile

  while (!(SPI1->SR & SPI_SR_RXNE));                    // Wait until RX buffer is not empty (data recieved)
  volatile char received = (volatile char)SPI1->DR;     // cast to volatile

  return received;
}