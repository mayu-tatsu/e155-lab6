// STM32L432KC_SPI.c

#include "STM32L432KC.h"

void initSPI(int br, int cpol, int cpha) {
  pinMode(SPI_SDI, GPIO_ALT);
  pinMode(SPI_SDO, GPIO_ALT);
  pinMode(SPI_CS, GPIO_OUTPUT);
  pinMode(SPI_SCK, GPIO_ALT);

  digitalWrite(SPI_CS, PIO_HIGH);

  GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);         // high speed clk for fast gpio/spi
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;               // enable spi clk
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;              // enable gpiob clk

  // specify which alternate fxn
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

  SPI1->CR1 = 0;                                    // clear
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);            // Set baud rate divider
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);        // Set phase
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);        // Set polarity

  SPI1->CR1 |= (SPI_CR1_MSTR);                      // Set as master
  SPI1->CR1 |= (SPI_CR1_SSM);                       // Software slave management
  SPI1->CR1 |= (SPI_CR1_SSI);                       // Internal slave select

  SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);        // Set bit package to 8
  SPI1->CR2 |= (SPI_CR2_FRXTH);                     // Set FIFO full length to 8
  SPI1->CR2 |= (SPI_CR2_SSOE);                      // Enable peripheral select mode

  SPI1->CR1 |= (SPI_CR1_SPE);                       // enable overall SPI
}

char spiSendReceive(char send) {
  while (!(SPI1->SR & SPI_SR_TXE));                     // Wait until TX buffer is empty
  *(volatile char *)(&SPI1->DR) = send;                 // Write data to TX buffer & cast to volatile

  while (!(SPI1->SR & SPI_SR_RXNE));                    // Wait until RX buffer is not empty
  volatile char received = (volatile char)SPI1->DR;     // cast to volatile

  return received;
}