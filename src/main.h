// main.h
// Mayu Tatsumi; mtatsumi@g.hmc.edu
// 2025-10-22

/**
    Main Header: Contains general defines and selected portions of CMSIS files
    @file main.h
    @author Josh Brake
    @version 1.0 10/7/2020
*/

#ifndef MAIN_H
#define MAIN_H

#include "../lib/STM32L432KC.h"
#include "../lib/DS1722.h"

#define LED_PIN PA6 // LED pin for blinking on Port B pin 3
#define BUFF_LEN 32

#endif // MAIN_H