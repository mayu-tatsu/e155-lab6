// DS1722.h
// Mayu Tatsumi; mtatsumi@g.hmc.edu
// 2025-10-22

#ifndef DS1722_H
#define DS1722_H

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
// Function prototypes
/////////////////////////////////////////////////////////////////////////////

// Initializes DS1722 temperature sensor
void initTempSensor(void);

// Reads temperature from DS1722 temperature sensor.
// Return: Temperature in deg.C
float getTemp(int precision);

// Sets the precision of temperature measurements.
// Parameter: prec - desired precision in bits (8, 9, 10, 11, or 12)
void setPrecision(uint8_t prec);

#endif