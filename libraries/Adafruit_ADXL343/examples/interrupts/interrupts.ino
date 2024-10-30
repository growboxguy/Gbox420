#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/** The input pins to enable the interrupt on, connected to INT1 and INT2 on the ADXL. */
#define INPUT_PIN_INT1   (5) // Uno = (2)
#define INPUT_PIN_INT2   (6) // Uno = (3)

/**
 * This struct is used to count the number of times that specific interrutps
 * have been fired by the ADXL and detected on the MCU. They will increment
 * by one for each event associated with the specified interrupt 'bit'.
 */
struct adxl_int_stats {
    uint32_t data_ready;
    uint32_t single_tap;
    uint32_t double_tap;
    uint32_t activity;
    uint32_t inactivity;
    uint32_t freefall;
    uint32_t watermark;
    uint32_t overrun;
    uint32_t total;
};

/** Global stats block, incremented inside the interrupt handler(s). */
struct adxl_int_stats g_int_stats = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/** Global counter to track the numbers of unused interrupts fired. */
uint32_t g_ints_fired = 0;

/** Global variable to determine which interrupt(s) are enabled on the ADXL343. */
int_config g_int_config_enabled = { 0 };

/** Global variables to determine which INT pin interrupt(s) are mapped to on the ADXL343. */
int_config g_int_config_map = { 0 };

/** Interrupt service routine for INT1 events. */
void int1_isr(void)
{
    /* By default, this sketch routes the OVERRUN interrupt to INT1. */
    g_int_stats.overrun++;
    g_int_stats.total++;
    g_ints_fired++;

    /* TODO: Toggle an LED! */
}

/** Interrupt service routine for INT2 events. */
void int2_isr(void)
{
    /* By default, this sketch routes the DATA_READY interrupt to INT2. */
    g_int_stats.data_ready++;
    g_int_stats.total++;
    g_ints_fired++;

    /* TODO: Toggle an LED! */
}

/** Configures the HW interrupts on the ADXL343 and the target MCU. */
void config_interrupts(void)
{
  /* NOTE: Once an interrupt fires on the ADXL you can read a register
   *  to know the source of the interrupt, but since this would likely
   *  happen in the 'interrupt context' performing an I2C read is a bad
   *  idea since it will block the device from handling other interrupts
   *  in a timely manner.
   *
   *  The best approach is to try to make use of only two interrupts on
   *  two different interrupt pins, so that when an interrupt fires, based
   *  on the 'isr' function that is called, you already know the int source.
   */

  /* Attach interrupt inputs on the MCU. */
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INPUT_PIN_INT1, INPUT);
  pinMode(INPUT_PIN_INT2, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT1), int1_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT2), int2_isr, RISING);

  /* Enable interrupts on the accelerometer. */
  g_int_config_enabled.bits.overrun    = true;    /* Set the INT1 */
  g_int_config_enabled.bits.watermark  = false;
  g_int_config_enabled.bits.freefall   = false;
  g_int_config_enabled.bits.inactivity = false;
  g_int_config_enabled.bits.activity   = false;
  g_int_config_enabled.bits.double_tap = false;
  g_int_config_enabled.bits.single_tap = false;
  g_int_config_enabled.bits.data_ready = true;    /* Set to INT2 */
  accel.enableInterrupts(g_int_config_enabled);

  /* Map specific interrupts to one of the two INT pins. */
  g_int_config_map.bits.overrun    = ADXL343_INT1;
  g_int_config_map.bits.watermark  = ADXL343_INT1;
  g_int_config_map.bits.freefall   = ADXL343_INT1;
  g_int_config_map.bits.inactivity = ADXL343_INT1;
  g_int_config_map.bits.activity   = ADXL343_INT1;
  g_int_config_map.bits.double_tap = ADXL343_INT1;
  g_int_config_map.bits.single_tap = ADXL343_INT1;
  g_int_config_map.bits.data_ready = ADXL343_INT2;
  accel.mapInterrupts(g_int_config_map);
}

void setup(void)
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("ADXL343 Interrupt Tester"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
  // displaySetRange(ADXL343_RANGE_8_G);
  // displaySetRange(ADXL343_RANGE_4_G);
  // displaySetRange(ADXL343_RANGE_2_G);


  /* Configure the HW interrupts. */
  config_interrupts();

  Serial.println("ADXL343 init complete. Waiting for INT activity.");
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
  delay(10);

  while (g_ints_fired) {
      Serial.println("INT detected!");
      Serial.print("\tOVERRUN Count:    "); Serial.println(g_int_stats.overrun, DEC);
      Serial.print("\tDATA_READY Count: "); Serial.println(g_int_stats.data_ready, DEC);

      /* Decrement the unhandled int counter. */
      g_ints_fired--;
  }
}
