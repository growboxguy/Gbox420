/**************************************************************************/
/*!
    @file     Adafruit_ADXL343.cpp
    @author   Bryan Siepert and K.Townsend (Adafruit Industries)

    BSD License (see license.txt)

    The ADXL343 is a digital accelerometer with 13-bit resolution, capable
    of measuring up to +/-16g.  This driver communicates using I2C.

    This is a library for the Adafruit ADXL343 breakout
    ----> https://www.adafruit.com/product/4097
    or the Adafruit ADXL343 + ADT7410 FeatherWing
    ----> https://www.adafruit.com/product/4147

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    v1.0 - First release
*/
/**************************************************************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <limits.h>

#include "Adafruit_ADXL343.h"

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register

    @param reg The register to write to
    @param value The value to write to the register
*/
/**************************************************************************/

void Adafruit_ADXL343::writeRegister(uint8_t reg, uint8_t value) {
  Adafruit_BusIO_Register reg_obj = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, reg, 1);
  reg_obj.write(value);
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register

    @param reg register to read

    @return The results of the register read request
*/
/**************************************************************************/
uint8_t Adafruit_ADXL343::readRegister(uint8_t reg) {
  Adafruit_BusIO_Register reg_obj = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, reg, 1);
  return ((uint8_t)reg_obj.read());
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified register

    @param reg The register to read two bytes from

    @return The 16-bit value read from the reg starting address
*/
/**************************************************************************/

int16_t Adafruit_ADXL343::read16(uint8_t reg) {
  Adafruit_BusIO_Register reg_obj = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, reg, 2);
  return ((uint16_t)reg_obj.read());
}

/**************************************************************************/
/*!
    @brief  Read the device ID (can be used to check connection)

    @return The 8-bit device ID
*/
/**************************************************************************/
uint8_t Adafruit_ADXL343::getDeviceID(void) {
  // Check device ID register
  return readRegister(ADXL3XX_REG_DEVID);
}

/**************************************************************************/
/*!
    @brief  Enables (1) or disables (0) the interrupts on the specified
            interrupt pin.

    @param cfg The bitfield of the interrupts to enable or disable.

    @return True if the operation was successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_ADXL343::enableInterrupts(int_config cfg) {
  /* Update the INT_ENABLE register with 'config'. */
  writeRegister(ADXL3XX_REG_INT_ENABLE, cfg.value);

  /* ToDo: Add proper error checking! */
  return true;
}

/**************************************************************************/
/*!
    @brief  'Maps' the specific interrupt to either pin INT1 (bit=0),
            of pin INT2 (bit=1).

    @param cfg The bitfield of the interrupts to enable or disable.

    @return True if the operation was successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_ADXL343::mapInterrupts(int_config cfg) {
  /* Update the INT_MAP register with 'config'. */
  writeRegister(ADXL3XX_REG_INT_MAP, cfg.value);

  /* ToDo: Add proper error checking! */
  return true;
}

/**************************************************************************/
/*!
    @brief  Reads the status of the interrupt pins. Reading this register
            also clears or deasserts any currently active interrupt.

    @return The 8-bit content of the INT_SOURCE register.
*/
/**************************************************************************/
uint8_t Adafruit_ADXL343::checkInterrupts(void) {
  return readRegister(ADXL3XX_REG_INT_SOURCE);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent X axis value

    @return The 16-bit signed value for the X axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getX(void) { return read16(ADXL3XX_REG_DATAX0); }

/**************************************************************************/
/*!
    @brief  Gets the most recent Y axis value

    @return The 16-bit signed value for the Y axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getY(void) { return read16(ADXL3XX_REG_DATAY0); }

/**************************************************************************/
/*!
    @brief  Gets the most recent Z axis value

    @return The 16-bit signed value for the Z axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getZ(void) { return read16(ADXL3XX_REG_DATAZ0); }

/**************************************************************************/
/*!
    @brief  Reads 3x16-bits from the x, y, and z data register
    @param x reference to return x acceleration data
    @param y reference to return y acceleration data
    @param z reference to return z acceleration data
    @return True if the operation was successful, otherwise false.
*/
/**************************************************************************/
bool Adafruit_ADXL343::getXYZ(int16_t &x, int16_t &y, int16_t &z) {
  int16_t buffer[] = {0, 0, 0};
  Adafruit_BusIO_Register reg_obj = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_DATAX0, 6);
  if (!reg_obj.read((uint8_t *)&buffer, 6))
    return false;
  x = buffer[0];
  y = buffer[1];
  z = buffer[2];
  return true;
}

/**************************************************************************/
/*!
 *   @brief  Instantiates a new ADXL343 class
 *
 *   @param sensorID  An optional ID # so you can track this sensor, it will
 *                    tag sensorEvents you create.
 */
/**************************************************************************/
Adafruit_ADXL343::Adafruit_ADXL343(int32_t sensorID) {
  _sensorID = sensorID;
  _wire = &Wire;
}

/**************************************************************************/
/*!
 *   @brief  Instantiates a new ADXL343 class
 *
 *   @param sensorID  An optional ID # so you can track this sensor, it will
 *                    tag sensorEvents you create.
 *   @param wireBus   TwoWire instance to use for I2C communication.
 */
/**************************************************************************/
Adafruit_ADXL343::Adafruit_ADXL343(int32_t sensorID, TwoWire *wireBus) {
  _sensorID = sensorID;
  _wire = wireBus;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADXL343 class in software SPI mode

    @param clock The SCK pin
    @param miso The MISO pin
    @param mosi The MOSI pin
    @param cs The CS/SSEL pin
    @param sensorID An optional ID # so you can track this sensor, it will tag
           sensorEvents you create.
*/
/**************************************************************************/
Adafruit_ADXL343::Adafruit_ADXL343(uint8_t clock, uint8_t miso, uint8_t mosi,
                                   uint8_t cs, int32_t sensorID) {
  _sensorID = sensorID;
  _cs = cs;
  _clk = clock;
  _do = mosi;
  _di = miso;
  _wire = NULL;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADXL343 class in hardware SPI mode

    @param cs The CS/SSEL pin
    @param theSPI SPIClass instance to use for SPI communication.
    @param sensorID An optional ID # so you can track this sensor, it will tag
           sensorEvents you create.
*/
/**************************************************************************/
Adafruit_ADXL343::Adafruit_ADXL343(uint8_t cs, SPIClass *theSPI,
                                   int32_t sensorID) {
  _sensorID = sensorID;
  _cs = cs;
  _spi = theSPI;
  _wire = NULL;
}

/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
    @param  i2caddr The 7-bit I2C address to find the ADXL on
    @return True if the sensor was successfully initialised.
*/
/**************************************************************************/
bool Adafruit_ADXL343::begin(uint8_t i2caddr) {

  if (_wire) {
    //-- I2C --------------
    if (i2c_dev) {
      delete i2c_dev; // remove old interface
    }
    i2c_dev = new Adafruit_I2CDevice(i2caddr, _wire);

    if (!i2c_dev->begin()) {
      return false;
    }

  } else {
    //-- SPI --------------
    i2c_dev = NULL;

    if (spi_dev) {
      delete spi_dev; // remove old interface
    }
    if (_spi) {
      // hardware spi
      spi_dev = new Adafruit_SPIDevice(_cs,
                                       1000000,               // frequency
                                       SPI_BITORDER_MSBFIRST, // bit order
                                       SPI_MODE3,             // data mode
                                       _spi);                 // hardware SPI
    } else {
      // software spi
      spi_dev = new Adafruit_SPIDevice(_cs, _clk, _di, _do,
                                       1000000,               // frequency
                                       SPI_BITORDER_MSBFIRST, // bit order
                                       SPI_MODE3);            // data mode
    }

    if (!spi_dev->begin()) {
      return false;
    }
  }

  /* Check connection */
  uint8_t deviceid = getDeviceID();
  if (deviceid != 0xE5) {
    /* No ADXL343 detected ... return false */
    return false;
  }

  _range = ADXL343_RANGE_2_G;
  // Default tap detection level (2G, 31.25ms duration, single tap only)
  // If only the single tap function is in use, the single tap interrupt
  // is triggered when the acceleration goes below the threshold, as
  // long as DUR has not been exceeded.
  writeRegister(ADXL3XX_REG_INT_ENABLE, 0);  // Disable interrupts to start
  writeRegister(ADXL3XX_REG_THRESH_TAP, 20); // 62.5 mg/LSB (so 0xFF = 16 g)
  writeRegister(ADXL3XX_REG_DUR, 50);        // Max tap duration, 625 µs/LSB
  writeRegister(ADXL3XX_REG_LATENT,
                0); // Tap latency, 1.25 ms/LSB, 0=no double tap
  writeRegister(ADXL3XX_REG_WINDOW,
                0); // Waiting period,  1.25 ms/LSB, 0=no double tap
  writeRegister(ADXL3XX_REG_TAP_AXES, 0x7); // Enable the XYZ axis for tap

  // Enable measurements
  writeRegister(ADXL3XX_REG_POWER_CTL, 0x08);

  return true;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer

    @param range The range to set, based on adxl34x_range_t
*/
/**************************************************************************/
void Adafruit_ADXL343::setRange(adxl34x_range_t range) {
  Adafruit_BusIO_Register data_format_reg =
      Adafruit_BusIO_Register(i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC,
                              ADXL3XX_REG_DATA_FORMAT, 1);

  Adafruit_BusIO_RegisterBits range_bits =
      Adafruit_BusIO_RegisterBits(&data_format_reg, 2, 0);

  Adafruit_BusIO_RegisterBits full_range_bit =
      Adafruit_BusIO_RegisterBits(&data_format_reg, 1, 3);

  /* Update the data rate */
  range_bits.write(range);
  /* Make sure that the FULL-RES bit is enabled for range scaling */
  full_range_bit.write(true);

  /* Keep track of the current range (to avoid readbacks) */
  _range = range;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer

    @return The adxl34x_range_t value corresponding to the sensors range
*/
/**************************************************************************/
adxl34x_range_t Adafruit_ADXL343::getRange(void) {
  Adafruit_BusIO_Register data_format_reg =
      Adafruit_BusIO_Register(i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC,
                              ADXL3XX_REG_DATA_FORMAT, 1);

  Adafruit_BusIO_RegisterBits range_bits =
      Adafruit_BusIO_RegisterBits(&data_format_reg, 2, 0);
  return (adxl34x_range_t)range_bits.read();
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL343 (controls power consumption)

    @param dataRate The data rate to set, based on adxl3xx_dataRate_t
*/
/**************************************************************************/
void Adafruit_ADXL343::setDataRate(adxl3xx_dataRate_t dataRate) {
  /* Note: The LOW_POWER bits are currently ignored and we always keep
     the device in 'normal' mode */
  writeRegister(ADXL3XX_REG_BW_RATE, dataRate);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL343 (controls power consumption)

    @return The current data rate, based on adxl3xx_dataRate_t
*/
/**************************************************************************/
adxl3xx_dataRate_t Adafruit_ADXL343::getDataRate(void) {
  Adafruit_BusIO_Register bw_rate_reg = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_BW_RATE, 1);

  Adafruit_BusIO_RegisterBits rate_bits =
      Adafruit_BusIO_RegisterBits(&bw_rate_reg, 4, 0);

  return (adxl3xx_dataRate_t)rate_bits.read();
}

/**************************************************************************/
/*!
    @brief  Retrieves the X Y Z trim offsets, note that they are 4 bits signed
    but we use int8_t to store and 'extend' the sign bit!
    @param x Pointer to the x offset, from -5 to 4 (internally multiplied by 8
   lsb)
    @param y Pointer to the y offset, from -5 to 4 (internally multiplied by 8
   lsb)
    @param z Pointer to the z offset, from -5 to 4 (internally multiplied by 8
   lsb)
*/
/**************************************************************************/
void Adafruit_ADXL343::getTrimOffsets(int8_t *x, int8_t *y, int8_t *z) {
  Adafruit_BusIO_Register x_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSX, 1);
  if (x != NULL)
    *x = x_off.read();
  Adafruit_BusIO_Register y_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSY, 1);
  if (y != NULL)
    *y = y_off.read();
  Adafruit_BusIO_Register z_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSZ, 1);
  if (z != NULL)
    *z = z_off.read();

  return;
}

/**************************************************************************/
/*!
    @brief  Sets the X Y Z trim offsets, note that they are 4 bits signed
    but we use int8_t to store and 'extend' the sign bit!
    @param x The x offset, from -5 to 4 (internally multiplied by 8 lsb)
    @param y The y offset, from -5 to 4 (internally multiplied by 8 lsb)
    @param z The z offset, from -5 to 4 (internally multiplied by 8 lsb)
*/
/**************************************************************************/
void Adafruit_ADXL343::setTrimOffsets(int8_t x, int8_t y, int8_t z) {
  Adafruit_BusIO_Register x_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSX, 1);
  x_off.write(x);

  Adafruit_BusIO_Register y_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSY, 1);
  y_off.write(y);

  Adafruit_BusIO_Register z_off = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSZ, 1);
  z_off.write(z);

  return;
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor event

    @param event Pointer to the sensors_event_t placeholder

    @return True of the read request was successful.
*/
/**************************************************************************/
bool Adafruit_ADXL343::getEvent(sensors_event_t *event) {
  int16_t x, y, z;
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type = SENSOR_TYPE_ACCELEROMETER;
  event->timestamp = millis();
  if (!getXYZ(x, y, z))
    return false;
  event->acceleration.x =
      x * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  event->acceleration.y =
      y * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  event->acceleration.z =
      z * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;

  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data

    @param sensor Pointer to the sensor_t placeholder.
*/
/**************************************************************************/
void Adafruit_ADXL343::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "ADXL343", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_ACCELEROMETER;
  sensor->min_delay = 0;
  sensor->min_value = -156.9064F; /*  -16g = 156.9064 m/s^2  */
  sensor->max_value = 156.9064F;  /* 16g = 156.9064 m/s^2  */
  sensor->resolution = 0.03923F;  /*  4mg = 0.0392266 m/s^2 */
}
