/**************************************************************************/
/*!
    @file     Adafruit_ADXL343.h
    @author   Bryan Siepert and K. Townsend (Adafruit Industries)

    BSD license (see license.txt)

    This is a library for the Adafruit ADS1015 breakout board
    ----> https://www.adafruit.com/products/???

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    v1.0  - First release
*/
/**************************************************************************/
#ifndef _ADAFRUIT_SENSOR_ADXL343_H
#define _ADAFRUIT_SENSOR_ADXL343_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define ADXL343_ADDRESS (0x53) /**< Assumes ALT address pin low */
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL3XX_REG_DEVID (0x00)        /**< Device ID */
#define ADXL3XX_REG_THRESH_TAP (0x1D)   /**< Tap threshold */
#define ADXL3XX_REG_OFSX (0x1E)         /**< X-axis offset */
#define ADXL3XX_REG_OFSY (0x1F)         /**< Y-axis offset */
#define ADXL3XX_REG_OFSZ (0x20)         /**< Z-axis offset */
#define ADXL3XX_REG_DUR (0x21)          /**< Tap duration */
#define ADXL3XX_REG_LATENT (0x22)       /**< Tap latency */
#define ADXL3XX_REG_WINDOW (0x23)       /**< Tap window */
#define ADXL3XX_REG_THRESH_ACT (0x24)   /**< Activity threshold */
#define ADXL3XX_REG_THRESH_INACT (0x25) /**< Inactivity threshold */
#define ADXL3XX_REG_TIME_INACT (0x26)   /**< Inactivity time */
#define ADXL3XX_REG_ACT_INACT_CTL                                              \
  (0x27) /**< Axis enable control for activity and inactivity detection */
#define ADXL3XX_REG_THRESH_FF (0x28) /**< Free-fall threshold */
#define ADXL3XX_REG_TIME_FF (0x29)   /**< Free-fall time */
#define ADXL3XX_REG_TAP_AXES (0x2A)  /**< Axis control for single/double tap */
#define ADXL3XX_REG_ACT_TAP_STATUS (0x2B) /**< Source for single/double tap */
#define ADXL3XX_REG_BW_RATE (0x2C)     /**< Data rate and power mode control */
#define ADXL3XX_REG_POWER_CTL (0x2D)   /**< Power-saving features control */
#define ADXL3XX_REG_INT_ENABLE (0x2E)  /**< Interrupt enable control */
#define ADXL3XX_REG_INT_MAP (0x2F)     /**< Interrupt mapping control */
#define ADXL3XX_REG_INT_SOURCE (0x30)  /**< Source of interrupts */
#define ADXL3XX_REG_DATA_FORMAT (0x31) /**< Data format control */
#define ADXL3XX_REG_DATAX0 (0x32)      /**< X-axis data 0 */
#define ADXL3XX_REG_DATAX1 (0x33)      /**< X-axis data 1 */
#define ADXL3XX_REG_DATAY0 (0x34)      /**< Y-axis data 0 */
#define ADXL3XX_REG_DATAY1 (0x35)      /**< Y-axis data 1 */
#define ADXL3XX_REG_DATAZ0 (0x36)      /**< Z-axis data 0 */
#define ADXL3XX_REG_DATAZ1 (0x37)      /**< Z-axis data 1 */
#define ADXL3XX_REG_FIFO_CTL (0x38)    /**< FIFO control */
#define ADXL3XX_REG_FIFO_STATUS (0x39) /**< FIFO status */
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL343_MG2G_MULTIPLIER (0.004) /**< 4mg per lsb */
/*=========================================================================*/

/** Used with register 0x2C (ADXL3XX_REG_BW_RATE) to set bandwidth */
typedef enum {
  ADXL343_DATARATE_3200_HZ = 0b1111, /**< 3200Hz Bandwidth */
  ADXL343_DATARATE_1600_HZ = 0b1110, /**< 1600Hz Bandwidth */
  ADXL343_DATARATE_800_HZ = 0b1101,  /**<  800Hz Bandwidth */
  ADXL343_DATARATE_400_HZ = 0b1100,  /**<  400Hz Bandwidth */
  ADXL343_DATARATE_200_HZ = 0b1011,  /**<  200Hz Bandwidth */
  ADXL343_DATARATE_100_HZ = 0b1010,  /**<  100Hz Bandwidth */
  ADXL343_DATARATE_50_HZ = 0b1001,   /**<   50Hz Bandwidth */
  ADXL343_DATARATE_25_HZ = 0b1000,   /**<   25Hz Bandwidth */
  ADXL343_DATARATE_12_5_HZ = 0b0111, /**< 12.5Hz Bandwidth */
  ADXL343_DATARATE_6_25HZ = 0b0110,  /**< 6.25Hz Bandwidth */
  ADXL343_DATARATE_3_13_HZ = 0b0101, /**< 3.13Hz Bandwidth */
  ADXL343_DATARATE_1_56_HZ = 0b0100, /**< 1.56Hz Bandwidth */
  ADXL343_DATARATE_0_78_HZ = 0b0011, /**< 0.78Hz Bandwidth */
  ADXL343_DATARATE_0_39_HZ = 0b0010, /**< 0.39Hz Bandwidth */
  ADXL343_DATARATE_0_20_HZ = 0b0001, /**< 0.20Hz Bandwidth */
  ADXL343_DATARATE_0_10_HZ = 0b0000, /**< 0.10Hz Bandwidth (default value) */

  ADXL3XX_DATARATE_3200_HZ = 0b1111, /**< 3200Hz Bandwidth */
  ADXL3XX_DATARATE_1600_HZ = 0b1110, /**< 1600Hz Bandwidth */
  ADXL3XX_DATARATE_800_HZ = 0b1101,  /**<  800Hz Bandwidth */
  ADXL3XX_DATARATE_400_HZ = 0b1100,  /**<  400Hz Bandwidth */
  ADXL3XX_DATARATE_200_HZ = 0b1011,  /**<  200Hz Bandwidth */
  ADXL3XX_DATARATE_100_HZ = 0b1010,  /**<  100Hz Bandwidth */
  ADXL3XX_DATARATE_50_HZ = 0b1001,   /**<   50Hz Bandwidth */
  ADXL3XX_DATARATE_25_HZ = 0b1000,   /**<   25Hz Bandwidth */
  ADXL3XX_DATARATE_12_5_HZ = 0b0111, /**< 12.5Hz Bandwidth */
  ADXL3XX_DATARATE_6_25HZ = 0b0110,  /**< 6.25Hz Bandwidth */
  ADXL3XX_DATARATE_3_13_HZ = 0b0101, /**< 3.13Hz Bandwidth */
  ADXL3XX_DATARATE_1_56_HZ = 0b0100, /**< 1.56Hz Bandwidth */
  ADXL3XX_DATARATE_0_78_HZ = 0b0011, /**< 0.78Hz Bandwidth */
  ADXL3XX_DATARATE_0_39_HZ = 0b0010, /**< 0.39Hz Bandwidth */
  ADXL3XX_DATARATE_0_20_HZ = 0b0001, /**< 0.20Hz Bandwidth */
  ADXL3XX_DATARATE_0_10_HZ = 0b0000  /**< 0.10Hz Bandwidth (default value) */
} adxl3xx_dataRate_t;

/** Used with register 0x31 (ADXL3XX_REG_DATA_FORMAT) to set g range */
typedef enum {
  ADXL343_RANGE_16_G = 0b11, /**< +/- 16g */
  ADXL343_RANGE_8_G = 0b10,  /**< +/- 8g */
  ADXL343_RANGE_4_G = 0b01,  /**< +/- 4g */
  ADXL343_RANGE_2_G = 0b00,  /**< +/- 2g (default value) */

  ADXL34X_RANGE_16_G = 0b11, /**< +/- 16g */
  ADXL34X_RANGE_8_G = 0b10,  /**< +/- 8g */
  ADXL34X_RANGE_4_G = 0b01,  /**< +/- 4g */
  ADXL34X_RANGE_2_G = 0b00   /**< +/- 2g (default value) */
} adxl34x_range_t;

/** Possible interrupts sources on the ADXL343. */
union int_config {
  uint8_t value; /**< Composite 8-bit value of the bitfield.*/
  struct {
    uint8_t overrun : 1;    /**< Bit 0 */
    uint8_t watermark : 1;  /**< Bit 1 */
    uint8_t freefall : 1;   /**< Bit 2 */
    uint8_t inactivity : 1; /**< Bit 3 */
    uint8_t activity : 1;   /**< Bit 4 */
    uint8_t double_tap : 1; /**< Bit 5 */
    uint8_t single_tap : 1; /**< Bit 6 */
    uint8_t data_ready : 1; /**< Bit 7 */
  } bits;                   /**< Individual bits in the bitfield. */
};

/** Possible interrupt pin outputs on the ADXL343. */
typedef enum {
  ADXL343_INT1 = 0,
  ADXL343_INT2 = 1,
  ADXL3XX_INT1 = 0,
  ADXL3XX_INT2 = 1,
} adxl3xx_int_pin;

/**
 * Driver for the Adafruit ADXL343 breakout.
 */
class Adafruit_ADXL343 : public Adafruit_Sensor {
public:
  Adafruit_ADXL343(int32_t sensorID);
  Adafruit_ADXL343(int32_t sensorID, TwoWire *wireBus);
  Adafruit_ADXL343(uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t cs,
                   int32_t sensorID = -1);
  Adafruit_ADXL343(uint8_t cs, SPIClass *theSPI, int32_t sensorID = -1);

  bool begin(uint8_t i2caddr = ADXL343_ADDRESS);
  void setRange(adxl34x_range_t range);
  adxl34x_range_t getRange(void);
  void setDataRate(adxl3xx_dataRate_t dataRate);
  adxl3xx_dataRate_t getDataRate(void);
  bool getEvent(sensors_event_t *);
  void getSensor(sensor_t *);

  uint8_t getDeviceID(void);
  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
  int16_t read16(uint8_t reg);

  bool enableInterrupts(int_config cfg);
  bool mapInterrupts(int_config cfg);
  uint8_t checkInterrupts(void);

  void getTrimOffsets(int8_t *x, int8_t *y, int8_t *z);
  void setTrimOffsets(int8_t x, int8_t y, int8_t z);

  int16_t getX(void);
  int16_t getY(void);
  int16_t getZ(void);
  bool getXYZ(int16_t &x, int16_t &y, int16_t &z);

protected:
  Adafruit_SPIDevice *spi_dev = NULL; ///< BusIO SPI device
  Adafruit_I2CDevice *i2c_dev = NULL; ///< BusIO I2C device

  TwoWire *_wire = NULL;  ///< I2C hardware interface
  SPIClass *_spi = NULL;  ///< SPI hardware interface
  int32_t _sensorID;      ///< User-set sensor identifier
  adxl34x_range_t _range; ///< cache of range
  uint8_t _clk,           ///< SPI software clock
      _do,                ///< SPI software data out
      _di,                ///< SPI software data in
      _cs;                ///< SPI software chip select
};

#endif
