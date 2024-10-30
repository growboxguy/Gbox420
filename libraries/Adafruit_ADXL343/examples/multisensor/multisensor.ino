/* 
 *  Connect two identical ADXL_343 sensor breakout boards
 *  to a single Arduino. By connecting SDO on one of the  
 *  sensors to Vcc the I2C address of this sensor changes 
 *  from the default (0x53) to the alternative address (0x1D). 
 *  The address is passed during begin().
 *  
 *  This example returns raw sensor output of x, y and z acceleration
 *  for both sensors over serial at 115200 baud.
 *  
 *  Example by Rolf Hut based on sensorTest example
*/


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

/* Create two sensor objects and assign unique IDs */
Adafruit_ADXL343 accel1 = Adafruit_ADXL343(12345);
Adafruit_ADXL343 accel2 = Adafruit_ADXL343(12346);




void setup(void)
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Two Accelerometers on one Arduino"); Serial.println("");

  /* Initialise the first sensors, this uses the default address */
  if(!accel1.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 nr1 detected ... Check your wiring!");
    while(1);
  }
  
  /* Initialise the second sensors, this uses the alternative address */
  /* of 0x1D. On this sensor SDO must be connected to VCC */
  if(!accel2.begin(0x1D))
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 nr2 detected ... Check your wiring!");
    while(1);
  }
  
  /* Set the range and data rate to whatever is appropriate for your project */
  /* See the sensortest example for more details */
  accel1.setRange(ADXL343_RANGE_2_G);
  accel2.setRange(ADXL343_RANGE_2_G);

  accel1.setDataRate(ADXL343_DATARATE_1600_HZ);
  accel2.setDataRate(ADXL343_DATARATE_1600_HZ);

  /* Display some basic information on these sensors */
  accel1.printSensorDetails();
  accel2.printSensorDetails();
  Serial.println("");
}

void loop(void)
{
  /* Get new sensor events */
  sensors_event_t event1;
  sensors_event_t event2;
  accel1.getEvent(&event1);
  accel2.getEvent(&event2);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print(event1.acceleration.x); Serial.print(",");
  Serial.print(event1.acceleration.y); Serial.print(",");
  Serial.print(event1.acceleration.z); Serial.print(",");
  Serial.print(event2.acceleration.x); Serial.print(",");
  Serial.print(event2.acceleration.y); Serial.print(",");
  Serial.println(event2.acceleration.z);
}
