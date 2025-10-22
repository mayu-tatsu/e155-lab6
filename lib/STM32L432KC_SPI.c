// STM32L432KC_SPI.c

#include "STM32L432KC.h"

void initSPI(int br, int cpol, int cpha) {
  RCC->APB2ENR |= _VAL2FLD(RCC_APB2ENR_SPI1EN, 0b1);               // enable spi clk
  RCC->AHB2ENR |= _VAL2FLD(RCC_AHB2ENR_GPIOBEN, 0b1);              // enable gpiob clk

  pinMode(SPI_CS, GPIO_OUTPUT);
  pinMode(SPI_SCK, GPIO_ALT);
  pinMode(SPI_SDI, GPIO_ALT);
  pinMode(SPI_SDO, GPIO_ALT);

  digitalWrite(SPI_CS, PIO_LOW);
  
  GPIOB->OSPEEDR |= _VAL2FLD(GPIO_OSPEEDR_OSPEED3, 0b1);         // high speed clk for fast gpio/spi

  GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3_Msk | GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk);   // clear

  // specify which alternate fxn
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

  SPI1->CR1 &= ~(SPI_CR1_BR_Msk | SPI_CR1_CPHA_Msk | SPI_CR1_CPOL_Msk);     // clear

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);            // Set baud rate divider
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);        // Set phase
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);        // Set polarity

  SPI1->CR1 &= ~(SPI_CR1_LSBFIRST_Msk);             // clear

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 0b1);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 0b1);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0b10);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 0b1);         // Set as master

  SPI1->CR2 &= ~(SPI_CR2_DS_Msk | SPI_CR2_FRXTH_Msk | SPI_CR2_SSOE_Msk);    // clear

  SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);        // Set bit package to 8
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 0b1);        // Set FIFO full length to 8
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0b1);         // Enable peripheral select mode
                    
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 0b1);          // enable overall SPI
}

char spiSendReceive(char send) {
  while (!(SPI1->SR & SPI_SR_TXE));                     // Wait until TX buffer is empty
  *(volatile char *)(&SPI1->DR) = send;                 // Write data to TX buffer & cast to volatile

  while (!(SPI1->SR & SPI_SR_RXNE));                    // Wait until RX buffer is not empty
  volatile char received = (volatile char)SPI1->DR;     // cast to volatile

  return received;
}