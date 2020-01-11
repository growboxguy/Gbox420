/**************************************************************************/
/*!
    @file     Adafruit_ADXL343.cpp
    @author   K.Townsend (Adafruit Industries)

    BSD License (see license.txt)

    The ADXL343 is a digital accelerometer with 13-bit resolution, capable
    of measuring up to +/-16g.  This driver communicates using I2C.

    This is a library for the Adafruit ADXL343 breakout
    ----> https://www.adafruit.com/products/xxxx

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
    @brief  Abstract away platform differences in Arduino wire library

    @return The read results as an unsigned integer.
*/
/**************************************************************************/
inline uint8_t Adafruit_ADXL343::i2cread(void) {
#if ARDUINO >= 100
  return _wire->read();
#else
  return _wire->receive();
#endif
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library

    @param x The byte to write to the I2C bus.
*/
/**************************************************************************/
inline void Adafruit_ADXL343::i2cwrite(uint8_t x) {
#if ARDUINO >= 100
  _wire->write((uint8_t)x);
#else
  _wire->send(x);
#endif
}

/**************************************************************************/
/*!
    @brief  Abstract away SPI receiver & transmitter

    @param clock The SCK pin
    @param miso The MISO pin
    @param mosi The MOSI pin
    @param data The data to XFER (if any)

    @return The byte received on the MISO line.
*/
/**************************************************************************/
static uint8_t spixfer(uint8_t clock, uint8_t miso, uint8_t mosi,
                       uint8_t data) {
  uint8_t reply = 0;
  for (int i = 7; i >= 0; i--) {
    reply <<= 1;
    digitalWrite(clock, LOW);
    digitalWrite(mosi, data & (1 << i));
    digitalWrite(clock, HIGH);
    if (digitalRead(miso))
      reply |= 1;
  }
  return reply;
}

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register

    @param reg The register to write to
    @param value The value to write to the register
*/
/**************************************************************************/
void Adafruit_ADXL343::writeRegister(uint8_t reg, uint8_t value) {
  if (_i2c) {
    _wire->beginTransmission(_i2caddr);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    _wire->endTransmission();
  } else {
    digitalWrite(_cs, LOW);
    spixfer(_clk, _di, _do, reg);
    spixfer(_clk, _di, _do, value);
    digitalWrite(_cs, HIGH);
  }
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register

    @param reg register to read

    @return The results of the register read request
*/
/**************************************************************************/
uint8_t Adafruit_ADXL343::readRegister(uint8_t reg) {
  if (_i2c) {
    _wire->beginTransmission(_i2caddr);
    i2cwrite(reg);
    _wire->endTransmission();
    _wire->requestFrom(_i2caddr, 1);
    return (i2cread());
  } else {
    reg |= 0x80; // read byte
    digitalWrite(_cs, LOW);
    spixfer(_clk, _di, _do, reg);
    uint8_t reply = spixfer(_clk, _di, _do, 0xFF);
    digitalWrite(_cs, HIGH);
    return reply;
  }
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified register

    @param reg The register to read two bytes from

    @return The 16-bit value read from the reg starting address
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::read16(uint8_t reg) {
  if (_i2c) {
    _wire->beginTransmission(_i2caddr);
    i2cwrite(reg);
    _wire->endTransmission();
    _wire->requestFrom(_i2caddr, 2);
    return (uint16_t)(i2cread() | (i2cread() << 8));
  } else {
    reg |= 0x80 | 0x40; // read byte | multibyte
    digitalWrite(_cs, LOW);
    spixfer(_clk, _di, _do, reg);
    uint16_t reply =
        spixfer(_clk, _di, _do, 0xFF) | (spixfer(_clk, _di, _do, 0xFF) << 8);
    digitalWrite(_cs, HIGH);
    return reply;
  }
}

/**************************************************************************/
/*!
    @brief  Read the device ID (can be used to check connection)

    @return The 8-bit device ID
*/
/**************************************************************************/
uint8_t Adafruit_ADXL343::getDeviceID(void) {
  // Check device ID register
  return readRegister(ADXL343_REG_DEVID);
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
  writeRegister(ADXL343_REG_INT_ENABLE, cfg.value);

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
  writeRegister(ADXL343_REG_INT_MAP, cfg.value);

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
  uint8_t format = readRegister(ADXL343_REG_INT_SOURCE);
  return format;
}

/**************************************************************************/
/*!
    @brief  Gets the most recent X axis value

    @return The 16-bit signed value for the X axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getX(void) { return read16(ADXL343_REG_DATAX0); }

/**************************************************************************/
/*!
    @brief  Gets the most recent Y axis value

    @return The 16-bit signed value for the Y axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getY(void) { return read16(ADXL343_REG_DATAY0); }

/**************************************************************************/
/*!
    @brief  Gets the most recent Z axis value

    @return The 16-bit signed value for the Z axis
*/
/**************************************************************************/
int16_t Adafruit_ADXL343::getZ(void) { return read16(ADXL343_REG_DATAZ0); }

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
  _range = ADXL343_RANGE_2_G;
  _i2c = true;
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
  _range = ADXL343_RANGE_2_G;
  _i2c = true;
  _wire = wireBus;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADXL343 class in SPI mode

    @param clock The SCK pin
    @param miso The MISO pin
    @param mosi The MOSI pin
    @param cs The CS/SSEL pin
    @param sensorID An optional ID # so you can track this sensor, it will tag
           sensoorEvents you create.
*/
/**************************************************************************/
Adafruit_ADXL343::Adafruit_ADXL343(uint8_t clock, uint8_t miso, uint8_t mosi,
                                   uint8_t cs, int32_t sensorID) {
  _sensorID = sensorID;
  _range = ADXL343_RANGE_2_G;
  _cs = cs;
  _clk = clock;
  _do = mosi;
  _di = miso;
  _i2c = false;
}

/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
    @param  i2caddr The 7-bit I2C address to find the ADXL on
    @return True if the sensor was successfully initialised.
*/
/**************************************************************************/
bool Adafruit_ADXL343::begin(uint8_t i2caddr) {

  if (_i2c) {
    _wire->begin();
    _i2caddr = i2caddr;
  } else {
    pinMode(_cs, OUTPUT);
    pinMode(_clk, OUTPUT);
    digitalWrite(_clk, HIGH);
    pinMode(_do, OUTPUT);
    pinMode(_di, INPUT);
  }

  /* Check connection */
  uint8_t deviceid = getDeviceID();
  if (deviceid != 0xE5) {
    /* No ADXL343 detected ... return false */
    return false;
  }

  // Default tap detection level (2G, 31.25ms duration, single tap only)
  // If only the single tap function is in use, the single tap interrupt
  // is triggered when the acceleration goes below the threshold, as
  // long as DUR has not been exceeded.
  writeRegister(ADXL343_REG_INT_ENABLE, 0);  // Disable interrupts to start
  writeRegister(ADXL343_REG_THRESH_TAP, 20); // 62.5 mg/LSB (so 0xFF = 16 g)
  writeRegister(ADXL343_REG_DUR, 50);        // Max tap duration, 625 µs/LSB
  writeRegister(ADXL343_REG_LATENT,
                0); // Tap latency, 1.25 ms/LSB, 0=no double tap
  writeRegister(ADXL343_REG_WINDOW,
                0); // Waiting period,  1.25 ms/LSB, 0=no double tap
  writeRegister(ADXL343_REG_TAP_AXES, 0x7); // Enable the XYZ axis for tap

  // Enable measurements
  writeRegister(ADXL343_REG_POWER_CTL, 0x08);

  return true;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer

    @param range The range to set, based on range_t
*/
/**************************************************************************/
void Adafruit_ADXL343::setRange(range_t range) {
  /* Red the data format register to preserve bits */
  uint8_t format = readRegister(ADXL343_REG_DATA_FORMAT);

  /* Update the data rate */
  format &= ~0x0F;
  format |= range;

  /* Make sure that the FULL-RES bit is enabled for range scaling */
  format |= 0x08;

  /* Write the register back to the IC */
  writeRegister(ADXL343_REG_DATA_FORMAT, format);

  /* Keep track of the current range (to avoid readbacks) */
  _range = range;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer

    @return The range_t value corresponding to the sensors range
*/
/**************************************************************************/
range_t Adafruit_ADXL343::getRange(void) {
  /* Red the data format register to preserve bits */
  return (range_t)(readRegister(ADXL343_REG_DATA_FORMAT) & 0x03);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL343 (controls power consumption)

    @param dataRate The data rate to set, based on dataRate_t
*/
/**************************************************************************/
void Adafruit_ADXL343::setDataRate(dataRate_t dataRate) {
  /* Note: The LOW_POWER bits are currently ignored and we always keep
     the device in 'normal' mode */
  writeRegister(ADXL343_REG_BW_RATE, dataRate);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL343 (controls power consumption)

    @return The current data rate, based on dataRate_t
*/
/**************************************************************************/
dataRate_t Adafruit_ADXL343::getDataRate(void) {
  return (dataRate_t)(readRegister(ADXL343_REG_BW_RATE) & 0x0F);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor event

    @param event Pointer to the sensors_event_t placeholder

    @return True of the read request was successful.
*/
/**************************************************************************/
bool Adafruit_ADXL343::getEvent(sensors_event_t *event) {
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type = SENSOR_TYPE_ACCELEROMETER;
  event->timestamp = millis();
  event->acceleration.x =
      getX() * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  event->acceleration.y =
      getY() * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  event->acceleration.z =
      getZ() * ADXL343_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;

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
