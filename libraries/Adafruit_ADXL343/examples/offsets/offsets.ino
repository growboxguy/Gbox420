/* This example shows how to use the trimmer offset registers to account for any error in the sensor 
   and 'zero' out the flat reading to be 0, 0, 1g. note this is unique to each sensor so it'll have
   to be repeated and 'saved' for each ADXL you use!
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

#define ADXL343_SCK 13
#define ADXL343_MISO 12
#define ADXL343_MOSI 11
#define ADXL343_CS 10

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/* NeoTrellis M4, etc.                    */
/* Uncomment following line for Wire1 bus */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(12345, &Wire1);

/* Uncomment for SPI */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(ADXL343_SCK, ADXL343_MISO, ADXL343_MOSI, ADXL343_CS, 12345);

void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Offsets Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);

  /* Display some basic information on this sensor */
  accel.printSensorDetails();
  Serial.println("");

  // init offsets to zero
  accel.setTrimOffsets(0, 0, 0);
  
  Serial.println("Hold accelerometer flat to set offsets to 0, 0, and -1g...");
  delay(1000);
  int16_t x, y, z;
  x = accel.getX();
  y = accel.getY();
  z = accel.getZ();
  Serial.print("Raw X: "); Serial.print(x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(z); Serial.print("  ");Serial.println(" counts");

  // the trim offsets are in 'multiples' of 8, we want to round, so we add 4
  accel.setTrimOffsets(-(x+4)/8, 
                       -(y+4)/8, 
                       -(z-250+4)/8);  // Z should be '250' at 1g (4mg per bit)
  
  int8_t x_offset, y_offset, z_offset;
  accel.getTrimOffsets(&x_offset, &y_offset, &z_offset);
  Serial.print("Current trim offsets: ");
  Serial.print(x_offset);  Serial.print(", ");
  Serial.print(y_offset);  Serial.print(", ");
  Serial.println(z_offset);

  Serial.println();
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");
  Serial.println("m/s^2 ");
  
  Serial.print("Raw X: "); Serial.print(accel.getX()); Serial.print("  ");
  Serial.print("Y: "); Serial.print(accel.getY()); Serial.print("  ");
  Serial.print("Z: "); Serial.print(accel.getZ()); Serial.print("  ");
  Serial.println(" counts");
  Serial.println();
  delay(500);
}