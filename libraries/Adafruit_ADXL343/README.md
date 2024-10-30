# Adafruit ADXL343 Accelerometer Driver ![Build Status](https://github.com/adafruit/Adafruit_ADXL343/workflows/Arduino%20Library%20CI/badge.svg)

This driver is for the Adafruit ADXL343 Breakout (https://www.adafruit.com/product/4097), and is based on Adafruit's Unified Sensor Library (Adafruit_Sensor).

## About the ADXL343 ##

The ADXL343 is a digital accelerometer that supports both SPI and I2C mode, with adjustable data rata and 'range' (+/-2/4/8/16g).  The Adafruit_ADXL343 driver takes advantage of I2C mode to reduce the total pin count required to use the sensor.

More information on the ADXL345 can be found in the datasheet: http://www.analog.com/static/imported-files/data_sheets/ADXL343.pdf

## What is the Adafruit Unified Sensor Library? ##

The Adafruit Unified Sensor Library (https://github.com/adafruit/Adafruit_Sensor) provides a common interface and data type for any supported sensor.  It defines some basic information about the sensor (sensor limits, etc.), and returns standard SI units of a specific type and scale for each supported sensor type.

It provides a simple abstraction layer between your application and the actual sensor HW, allowing you to drop in any comparable sensor with only one or two lines of code to change in your project (essentially the constructor since the functions to read sensor data and get information about the sensor are defined in the base Adafruit_Sensor class).

This is imporant useful for two reasons:

1.) You can use the data right away because it's already converted to SI units that you understand and can compare, rather than meaningless values like 0..1023.

2.) Because SI units are standardised in the sensor library, you can also do quick sanity checks working with new sensors, or drop in any comparable sensor if you need better sensitivity or if a lower cost unit becomes available, etc.

Light sensors will always report units in lux, gyroscopes will always report units in rad/s, etc. ... freeing you up to focus on the data, rather than digging through the datasheet to understand what the sensor's raw numbers really mean.

## About this Driver ##

Adafruit invests time and resources providing this open source code.  Please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Kevin (KTOWN) Townsend for Adafruit Industries.
