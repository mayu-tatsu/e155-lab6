// main.c
// Mayu Tatsumi; mtatsumi@g.hmc.edu
// 2025-10-22

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

// Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* tempStr =
  "<h1>Temperature</h1><p>Temperature Precision:</p>"
  "<form action=\"prec8\"><input type=\"submit\" value=\"8 bits\"></form>"
  "<form action=\"prec9\"><input type=\"submit\" value=\"9 bits\"></form>"
  "<form action=\"prec10\"><input type=\"submit\" value=\"10 bits\"></form>"
  "<form action=\"prec11\"><input type=\"submit\" value=\"11 bits\"></form>"
  "<form action=\"prec12\"><input type=\"submit\" value=\"12 bits\"></form>";
char* webpageEnd   = "</body></html>";

// Determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL)
    return 1;
  return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

void updateTempPrec(char request[]) {
  int precision = 9;                       // default
  if (inString(request, "prec8") == 1) {
    precision = 8;
  } else if (inString(request, "prec9") == 1) {
    precision = 9;
  } else if (inString(request, "prec10") == 1) {
    precision = 10;
  } else if (inString(request, "prec11") == 1) {
    precision = 11;
  } else if (inString(request, "prec12") == 1) {
    precision = 12;
  }

  setPrecision(precision);

  return;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  pinMode(PA6, GPIO_OUTPUT);                  // LED
  
  enableTIM15Clock();
  initTIM(TIM15);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  initSPI(0b111, 0, 1);                       // SPI clk = fPCLK / 256, CPOL = 0, CPHA = 1
  initTempSensor();

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // TODO: Add SPI code here for reading temperature

    updateTempPrec(request);

    char tempStatusStr[32];
    float temp = getTemp();
    sprintf(tempStatusStr, "Temperature: %.4f", temp);
    
  
    // Update string with current LED state
  
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, tempStr);

    sendString(USART, "<h2>LED Status</h2>");

    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");


    sendString(USART, "<h2>Temp Status</h2>");

    sendString(USART, "<p>");
    sendString(USART, tempStatusStr);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
  }
}